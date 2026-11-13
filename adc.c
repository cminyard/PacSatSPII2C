/*
 *  SPII2C - A program for a SPI to I2C converter device
 *  Copyright (C) 2026  Corey Minyard <corey@minyard.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-only
 */

#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#include "ti_drivers_config.h"
#include <ti/drivers/spi/SPIMSPM0.h>
#include "spii2c.h"
#include "spi.h"
#include "i2c.h"

/*
 * ADC conversion reading for a pair of ADC1015-Q1 converters.
 */

/* Posted when a SPI transaction completes. */
static sem_t adc_done_sem;
volatile bool adc_transfer_waiting;

void
adc_init(void)
{
    int rv;

    rv = sem_init(&adc_done_sem, 0, 0);
    if (rv) {
        printf("Error creating adc_done_sem: %d\n", rv);
        while (true) {}
    }
}

static void
adc_i2c_done(struct i2c_transaction *transaction)
{
    if (adc_transfer_waiting)
	sem_post(&adc_done_sem);
}

static bool
wait_for_sem(sem_t *sem, unsigned int timeout_ms)
{
    struct timespec timeout;
    int rv;

    if (clock_gettime(CLOCK_REALTIME, &timeout) == -1)
	return false;
    timeout.tv_nsec += timeout_ms * 1000000;
    while (timeout.tv_nsec >= 1000000000) {
	timeout.tv_nsec -= 1000000000;
	timeout.tv_sec += 1;
    }
    rv = sem_timedwait(sem, &timeout);
    if (rv)
	return false;
    return true;
}

static void
wait_ms(unsigned int timeout_ms)
{
    struct timespec timeout = { 0, 0 };

    timeout.tv_nsec += timeout_ms * 1000000;
    while (timeout.tv_nsec >= 1000000000) {
	timeout.tv_nsec -= 1000000000;
	timeout.tv_sec += 1;
    }
    clock_nanosleep(CLOCK_MONOTONIC, 0, &timeout, NULL);
}

static void
read_adc_value(uint8_t addr, int muxnum, int bitnum,
	       uint8_t *val, uint8_t *flag)
{
    uint8_t adc_tx[3];
    uint8_t adc_rx[2];
    struct i2c_transaction t;
    unsigned int retries = 10;

    t.done = adc_i2c_done;
    adc_transfer_waiting = true;
    adc_tx[0] = 1; /* Config register. */
    adc_tx[1] = 0x83; /* bits 1000_0011 */
    adc_tx[1] |= (muxnum + 4) << 4; /* Set which ADC to read. */
    adc_tx[2] = 0x03;
    if (!i2c_transaction(&t, 2, addr, adc_tx, 3, adc_rx, 0)) {
	printf("Error from I2C transaction, status = %d  0x%x:%d\n",
	       t.status, addr, muxnum);
	goto out;
    }
    if (!wait_for_sem(&adc_done_sem, 100)) {
	printf("Timeout from I2C ADC transaction1 0x%x:%d\n", addr, muxnum);
	goto out;
    }
    if (t.status) {
	printf("Error status from I2C transaction = %d  0x%x:%d\n",
	       t.status, addr, muxnum);
	goto out;
    }

    while (true) {
	/* Set the config to start the conversion. */
	adc_tx[0] = 1;
	if (!i2c_transaction(&t, 2, addr, adc_tx, 1, adc_rx, 2))
	    goto out;
	if (!wait_for_sem(&adc_done_sem, 100)) {
	    printf("Timeout from I2C ADC transaction2 0x%x:%d\n", addr, muxnum);
	    goto out;
	}
	if (t.status) {
	    printf("Error status from I2C transaction = %d  0x%x:%d\n",
		   t.status, addr, muxnum);
	    goto out;
	}

	wait_ms(10); /* Wait for the conversion to complete. */

	if (adc_rx[0] & 0x80) {
	    /* Conversion complete, get value. */
	    adc_tx[0] = 0;
	    if (!i2c_transaction(&t, 2, addr, adc_tx, 1, adc_rx, 2))
		goto out;
	    if (!wait_for_sem(&adc_done_sem, 100)) {
		printf("Timeout from I2C ADC transaction3 0x%x:%d\n",
		       addr, muxnum);
		goto out;
	    }
	    if (t.status) {
		printf("Error status from I2C transaction = %d  0x%x:%d\n",
		       t.status, addr, muxnum);
		goto out;
	    }
	    val[0] = adc_rx[0] >> 4;
	    if (val[0] & 0x8)
		val[0] |= 0xf0; /* Extend the sign bit. */
	    val[1] = (adc_rx[0] << 4) | (adc_rx[1] >> 4);
	    break;
	}

	/* Conversion not complete yet, try again. */
	if (--retries == 0) {
	    printf("I2C ADC never came ready transaction 0x%x:%d\n",
		   t.status, addr);
	    goto out;
	}
    }
    *flag |= 1 << bitnum;
 out:
    adc_transfer_waiting = false;
}

/*
 * ADC inputs don't map cleanly.  Make it easier to use by mapping
 * in a sane manner to pins on the ADC connector.
 */
static uint8_t map_adc[8] = {
    3, /* Pin 6 */
    2, /* Pin 7 */
    0, /* Pin 8 */
    1, /* Pin 9 */
    4, /* Pin 10 */
    5, /* USB 5V */
    6, /* USB 3.3V */
    7  /* spare */
};

static void
read_adc_map(unsigned int adcnum, uint8_t *rval, uint8_t *flag)
{
    uint8_t addr = 0x48;
    /* Use the bottom 2 bits to address the ADC on the chip. */
    uint8_t num = map_adc[adcnum];

    if (num >= 4) /* Second ADC. */
	addr = 0x49;
    num %= 4;
    read_adc_value(addr, num, adcnum, rval, flag);
}

bool
read_adc(unsigned int adcnum, int16_t *rval)
{
    uint8_t flag = 0;
    uint8_t val[2];

    if (adcnum >= 8)
	return false;
    read_adc_map(adcnum, &val, &flag);
    if (flag)
	*rval = (((int16_t) val[0]) << 8) | val[1];
    return !!flag;
}

volatile bool adc_spi_inuse;
bool send_adc_vals = true;

static void
adc_spi_done(struct spi_tx_msg *msg)
{
    adc_spi_inuse = false; 
}

void *
adcThread(void *arg0)
{
    struct spi_tx_msg msg;
    unsigned int i;

    memset(&msg, 0, sizeof(msg));
    dlist_link_init(&msg.link);
    msg.done = adc_spi_done;
    while (true) {
	struct timespec timeout = { 1, 0 };

	clock_nanosleep(CLOCK_REALTIME, 0, &timeout, NULL);
	if (!send_adc_vals)
	    continue;

	if (adc_spi_inuse) {
	    printf("ADC SPI message still in use.\n");
	    continue;
	}
	adc_spi_inuse = true;

	msg.buf[0] = ACP_ADC_REPORT;
	msg.buf[1] = 0;
	for (i = 0; i < 8; i++)
	    read_adc_map(i, &msg.buf[i * 2 + 2], &msg.buf[1]);
	spi_send(&msg);
    }

    return NULL;
}
