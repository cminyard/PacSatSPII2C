/*
 *  SPII2C - A program for a SPI to I2C converter device
 *  Copyright (C) 2036  Corey Minyard <corey@minyard.net
 *
 *  SPDX-License-Identifier: GPL-2.0-only
 */

#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#include "ti_drivers_config.h"
#include "spii2c.h"
#include "spi.h"

static sem_t CS_sem;
static sem_t SPI_done_sem;

static struct dlist tx_queue;
static sem_t queue_sem;

void (*spi_recv_msg_handler)(unsigned char *msg);

void
spi_init(void)
{
    int rv;

    dlist_init(&tx_queue);

    rv = sem_init(&queue_sem, 0, 1);
    if (rv) {
        printf("Error creating queue_sem: %d\n", rv);
        while (true) {}
    }

    rv = sem_init(&CS_sem, 0, 0);
    if (rv) {
        printf("Error creating CS_sem: %d\n", rv);
        while (true) {}
    }

    rv = sem_init(&SPI_done_sem, 0, 0);
    if (rv) {
        printf("Error creating SPI_done_sem: %d\n", rv);
        while (true) {}
    }

    SPI_init();
}

void gpio_CS(uint_least8_t index)
{
    sem_post(&CS_sem);
}

void spi_transfer_done(SPI_Handle handle, SPI_Transaction *transaction)
{
    sem_post(&SPI_done_sem);
}

int
spi_send(struct spi_tx_msg *msg)
{
    sem_wait(&queue_sem);
    dlist_add_tail(&tx_queue, &msg->link);
    sem_post(&queue_sem);
    sem_post(&CS_sem);

    return 0;
}

void *
spiThread(void *arg0)
{
    SPI_Handle handle;
    SPI_Params params;
    SPI_Transaction transaction;
    static unsigned char dummy_tx_buf[SPI_MSG_LEN] = {0xff};
    static unsigned char rx_buf[SPI_MSG_LEN];
    struct timespec timeout;
    int rv;
    struct spi_tx_msg *tx_msg;
    
    GPIO_setConfig(CONFIG_GPIO_ANT_IRQ,
		   GPIO_CFG_OUT_STD | CONFIG_GPIO_ANT_IRQ_IOMUX);
    GPIO_write(CONFIG_GPIO_ANT_IRQ, CONFIG_ANT_IRQ_OFF);

    while (true) {
	/* Grab CS from the SPI controller so we can monitor it. */
	GPIO_setConfig(GPIO_SPI0_CS0_PIN,
		       (GPIO_CFG_INPUT | GPIO_CFG_IN_INT_FALLING
			| GPIO_SPI0_IOMUX_CS0));
	GPIO_setCallback(GPIO_SPI0_CS0_PIN, gpio_CS);
	GPIO_enableInt(GPIO_SPI0_CS0_PIN);

	sem_wait(&queue_sem);
	while (GPIO_read(GPIO_SPI0_CS0_PIN) == CONFIG_ANT_CS_OFF
	       && !dlist_empty(&tx_queue)) {
	    sem_post(&queue_sem);
	    sem_wait(&CS_sem);
	    sem_wait(&queue_sem);
	}

	tx_msg = dlist_get_head_container(&tx_queue, struct spi_tx_msg, link);

	sem_post(&queue_sem);

	/* Now put the CS line back to SPI use. */
	GPIO_disableInt(GPIO_SPI0_CS0_PIN);
	GPIO_setConfigAndMux(GPIO_SPI0_CS0_PIN,
			     (GPIO_CFG_INPUT | GPIO_SPI0_IOMUX_CS0),
			     GPIO_SPI0_IOMUX_CS0_FUNC);

	/* Set up for a SPI transfer. */
	SPI_Params_init(&params);
	params.frameFormat = SPI_MOTO4_POL0_PHA0;
	params.mode = SPI_PERIPHERAL;
	params.transferCallbackFxn = spi_transfer_done;
	params.transferMode = SPI_MODE_CALLBACK;
	params.bitRate = 500000;
	handle = SPI_open(ACP_HOST_SPI, &params);
	if (handle == NULL) {
	    printf("Error initializing SPI peripheral\n");
	    while (true) {}
	}

	transaction.count = SPI_MSG_LEN;
	if (tx_msg)
	    transaction.txBuf = tx_msg->buf;
	else
	    transaction.txBuf = dummy_tx_buf;
	transaction.rxBuf = rx_buf;
	if (!SPI_transfer(handle, &transaction)) {
	    printf("Error from SPI_Transfer\n");
	    while (true) {}
	}

	/* Signal the other side that we are ready. */
	GPIO_write(CONFIG_GPIO_ANT_IRQ, CONFIG_ANT_IRQ_ON);

	/* Wait for transaction to complete. */
	if (clock_gettime(CLOCK_REALTIME, &timeout) == -1) {
	    printf("Error from clock_gettime\n");
	    while (true) {}
	}
	timeout.tv_sec += 1; /* One second, maybe too much? */
	rv = sem_timedwait(&SPI_done_sem, &timeout);

	/* We are no longer ready for a transfer. */
	GPIO_write(CONFIG_GPIO_ANT_IRQ, CONFIG_ANT_IRQ_ON);

	if (rv == -1) {
	    printf("Timeout waiting for SPI transaction\n");
	} else {
	    if (spi_recv_msg_handler)
		spi_recv_msg_handler(rx_buf);

	    if (tx_msg) {
		sem_wait(&queue_sem);
		dlist_remove_link(&tx_queue, &tx_msg->link);
		sem_post(&queue_sem);
		if (tx_msg->done)
		    tx_msg->done(tx_msg);
	    }
	}

	/* SPI transfer is done, disable SPI. */
	SPI_close(handle);
    }

    return NULL;
}

