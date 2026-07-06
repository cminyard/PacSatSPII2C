/*
 *  SPII2C - A program for a SPI to I2C converter device
 *  Copyright (C) 2026  Corey Minyard <corey@minyard.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-only
 */

#include <pthread.h>
#include <FreeRTOS.h>
#include <task.h>
#include <string.h>

#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>
#include <semaphore.h>

#include "ti_drivers_config.h"

#include "spii2c.h"
#include "console.h"
#include "commands.h"
#include "i2c.h"
#include "spi.h"
#include "gpio.h"

/*
 * The code below is the main code to convert SPI transaction to I2C
 * transaction and to read/write GPIOs.
 */

#define MAX_I2C_MSG_SIZE (SPI_MSG_LEN - 5)

struct spi_to_i2c {
    uint8_t txbuf[MAX_I2C_MSG_SIZE];
    uint8_t addr;
    unsigned int txcount;
    unsigned int rxcount;
    volatile bool inuse;
    volatile bool done;
    struct i2c_transaction t;
    struct spi_tx_msg rspmsg;
};

struct spi_to_i2c spi_to_i2c[CONFIG_I2C_COUNT];

#define NUM_ACP_GPIOS 5
struct gpio_acp {
    unsigned int gpionum;
    volatile bool inuse;
    struct spi_tx_msg rspmsg;
} gpio_acp[NUM_ACP_GPIOS] = {
    {
	.gpionum = CONFIG_GPIO_PC104_8,
    },
    {
	.gpionum = CONFIG_GPIO_ANT_POW,
    },
    {
	.gpionum = CONFIG_GPIO_EXTRA,
    },
    {
	.gpionum = CONFIG_GPIO_ADC_ENABLE,
    },
    {
	.gpionum = CONFIG_GPIO_PC104_7,
    },
};

static sem_t spi_to_i2c_wake;

static void gpio_msg_done(struct spi_tx_msg *msg)
{
    struct gpio_acp *acp = container_of(msg, struct gpio_acp, rspmsg);

    acp->inuse = false;
}
static void handle_spi_recv_msg(unsigned char *msg)
{
    unsigned int i2cnum;
    struct spi_to_i2c *s2i;
    struct gpio_acp *acp;
    struct spi_tx_msg *rsp;

    switch(msg[0]) {
    case ACP_I2C_CMD:
	if (msg[1] >= CONFIG_I2C_COUNT)
	    break;
	i2cnum = msg[1];
	s2i = &spi_to_i2c[i2cnum];
	if (s2i->inuse)
	    break;
	s2i->inuse = true;
	s2i->t.status = -1;

	if (msg[2] >= MAX_I2C_MSG_SIZE)
	    goto return_fail;
	if (msg[3] >= MAX_I2C_MSG_SIZE)
	    goto return_fail;
	s2i->txcount = msg[2];
	s2i->rxcount = msg[3];
	s2i->addr = msg[4];
	memset(s2i->rspmsg.buf, 0, SPI_MSG_LEN);
	memcpy(s2i->txbuf, &msg[5], s2i->txcount);
	if (!i2c_transaction(&s2i->t, i2cnum, s2i->addr,
			     s2i->txbuf, s2i->txcount,
			     &s2i->rspmsg.buf[5], s2i->rxcount)) {
	return_fail:
	    /* Failed, send response now. */
	    s2i->done = true;
	    sem_post(&spi_to_i2c_wake);
	    break;
	}
	break;

    case ACP_SET_GPIO:
	if (msg[1] >= NUM_ACP_GPIOS)
	    break;
	acp = &gpio_acp[msg[1]];

	if (gpio_is_input(acp->gpionum))
	    break;

	GPIO_write(acp->gpionum, msg[2]);
	break;

    case ACP_GET_GPIO:
	if (msg[1] >= NUM_ACP_GPIOS)
	    break;
	acp = &gpio_acp[msg[1]];
	if (acp->inuse)
	    break;
	acp->inuse = true;
	rsp = &acp->rspmsg;
	rsp->done = gpio_msg_done;
	rsp->buf[0] = ACP_GPIO_VALUE;
	rsp->buf[1] = msg[1];
	rsp->buf[2] = gpio_read(acp->gpionum, false);
	spi_send(rsp);
	break;

    default: /* Ignore everything else. */
	break;
    }
}

static void spi_rsp_done(struct spi_tx_msg *msg)
{
    struct spi_to_i2c *s2i = container_of(msg, struct spi_to_i2c, rspmsg);

    s2i->inuse = false;
}

static void spi_i2c_transaction_done(struct i2c_transaction *t)
{
    struct spi_to_i2c *s2i = container_of(t, struct spi_to_i2c, t);

    s2i->done = true;
    sem_post(&spi_to_i2c_wake);
}

static void *
spiI2cThread(void *arg0)
{
    unsigned int i;
    uint8_t *msg;

    while (true) {
	sem_wait(&spi_to_i2c_wake);

	for (i = 0; i < CONFIG_I2C_COUNT; i++) {
	    struct spi_to_i2c *s2i = &spi_to_i2c[i];

	    if (!s2i->done)
		continue;
	    msg = s2i->rspmsg.buf;
	    msg[0] = ACP_I2C_RSP;
	    msg[1] = i;
	    msg[2] = s2i->t.status;
	    msg[3] = s2i->rxcount;
	    msg[4] = s2i->addr;
	    /* Message data is already in the buffer. */
	    s2i->done = false;
	    spi_send(&s2i->rspmsg);
	}
    }
}

void *
mainThread(void *arg0)
{
    pthread_t thread;
    pthread_attr_t task_attrs;
    struct sched_param task_params;
    int rv;
    unsigned int i;

    for (i = 0; i < CONFIG_I2C_COUNT; i++) {
	spi_to_i2c[i].t.i2cnum = i;
	spi_to_i2c[i].t.done = spi_i2c_transaction_done;
	spi_to_i2c[i].rspmsg.done = spi_rsp_done;
	dlist_link_init(&spi_to_i2c[i].rspmsg.link);
    }

    console_init(handle_command);

    rv = sem_init(&spi_to_i2c_wake, 0, 0);
    if (rv) {
        printf("Error creating spi_to_i2c_wake: %d\n", rv);
        while (true) {}
    }

    GPIO_init();
    i2c_init();
    spi_recv_msg_handler = handle_spi_recv_msg;
    spi_init();
    command_init();
    adc_init();

    GPIO_setConfig(CONFIG_GPIO_ADC_ENABLE,
		   GPIO_CFG_OUT_STD | CONFIG_GPIO_ADC_ENABLE_IOMUX |
		   GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_GPIO_ANT_POW,
		   GPIO_CFG_OUT_STD | CONFIG_GPIO_ANT_POW_IOMUX |
		   GPIO_CFG_OUT_HIGH);
    GPIO_setConfig(CONFIG_GPIO_EXTRA,
		   GPIO_CFG_IN_PU | CONFIG_GPIO_EXTRA_IOMUX);
    GPIO_setConfig(CONFIG_GPIO_PC104_7,
		   GPIO_CFG_IN_PU | CONFIG_GPIO_PC104_7_IOMUX);
    GPIO_setConfig(CONFIG_GPIO_PC104_8,
		   GPIO_CFG_IN_PU | CONFIG_GPIO_PC104_8_IOMUX);

    printf("SPI I2C Converter\n");

    pthread_attr_init(&task_attrs);
    task_params.sched_priority = 1;
    rv = pthread_attr_setschedparam(&task_attrs, &task_params);
    rv |= pthread_attr_setdetachstate(&task_attrs, PTHREAD_CREATE_DETACHED);
    rv |= pthread_attr_setstacksize(&task_attrs, SPI_THREAD_STACK_SIZE);
    if (rv != 0) {
        /* failed to set attributes */
        while (1) {
        }
    }

    rv = pthread_create(&thread, &task_attrs, spiThread, NULL);
    if (rv != 0) {
        while (1) {
        }
    }

    rv = pthread_create(&thread, &task_attrs, spiI2cThread, NULL);
    if (rv != 0) {
        while (1) {
        }
    }
#if 1
    rv = pthread_create(&thread, &task_attrs, adcThread, NULL);
    if (rv != 0) {
        while (1) {
        }
    }
#endif
    /* We use this thread for the console thread. */
    consoleThread(arg0);
    return NULL;
}
