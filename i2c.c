/*
 *  SPII2C - A program for a SPI to I2C converter device
 *  Copyright (C) 2026  Corey Minyard <corey@minyard.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-only
 */

#include <semaphore.h>

#include "ti_drivers_config.h"
#include "spii2c.h"
#include "i2c.h"

static I2C_Handle i2c_handles[CONFIG_I2C_COUNT];
static sem_t i2c_sem[CONFIG_I2C_COUNT];
static volatile bool i2c_inuse[CONFIG_I2C_COUNT];

static void
i2c_transfer_done(I2C_Handle handle, I2C_Transaction *transaction,
		  bool transferStatus)
{
    struct i2c_transaction *t = container_of(transaction,
					     struct i2c_transaction, t);

    i2c_inuse[t->i2cnum] = false;
    t->status = -transaction->status;

    if (t->done)
	t->done(t);
}

void
i2c_init(void)
{
    unsigned int i;
    I2C_Params i2cParams;

    I2C_init();

    for (i = 0; i < CONFIG_I2C_COUNT; i++) {
	I2C_Params_init(&i2cParams);
	i2cParams.bitRate = I2C_100kHz;
	i2cParams.transferMode = I2C_MODE_CALLBACK;
	i2cParams.transferCallbackFxn = i2c_transfer_done;
	i2c_handles[i] = I2C_open(i, &i2cParams);
	if (!i2c_handles[i]) {
	    while (1) {}
	}
	if (sem_init(&i2c_sem[i], 0, 1)) {
	    while (1) {}
	}
    }
}

bool
i2c_transaction(struct i2c_transaction *t,
		unsigned int i2cnum, unsigned int addr,
		void *tx, unsigned int tx_size,
		void *rx, unsigned int rx_size)
{
    int err;
    bool rv = true;

    sem_wait(&i2c_sem[i2cnum]);
    if (i2c_inuse[i2cnum]) {
	printf("I2C %d is in use.\n", i2cnum);
	sem_post(&i2c_sem[i2cnum]);
	return false;
    }
    i2c_inuse[i2cnum] = true;
    sem_post(&i2c_sem[i2cnum]);

    t->status = I2C_ACP_STATUS_ERROR;
    t->i2cnum = i2cnum;
    t->t.targetAddress = addr;
    t->t.writeBuf = tx;
    t->t.writeCount = tx_size;
    t->t.readBuf = rx;
    t->t.readCount = rx_size;

    /*
     * Start an I2C transaction.  i2c_transfer_done() will be called
     * when it's complete.
     *
     * Note that this function will hang if the I2C clock line is hung
     * by some external mechanism (or missing a pull-up, as I found
     * out by experience).  There's not much that can be done about it
     * without re-writing TI's code here.
     */
    err = I2C_transferTimeout(i2c_handles[i2cnum], &t->t, 200);
    if (err != I2C_STATUS_SUCCESS) {
	printf("Error from I2C: %d\n", rv);
	return false;
    }

    return true;
}
