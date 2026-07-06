/*
 *  SPII2C - A program for a SPI to I2C converter device
 *  Copyright (C) 2026  Corey Minyard <corey@minyard.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-only
 */

#ifndef I2C_H
#define I2C_H

#include <ti/drivers/I2C.h>

/* Status codes, translated from the one from TI. */
#define I2C_ACP_STATUS_SUCCESS 0

/* Unknown error */
#define I2C_ACP_STATUS_ERROR 1

/* Unknown command code */
#define I2C_ACP_STATUS_UNDEFINEDCMD 2

#define I2C_ACP_STATUS_TIMEOUT 3
#define I2C_ACP_STATUS_CLOCK_TIMEOUT 4
#define I2C_ACP_STATUS_ADDR_NACK 5
#define I2C_ACP_STATUS_DATA_NACK 6
#define I2C_ACP_STATUS_ARB_LOST 7
#define I2C_ACP_STATUS_INCOMPLETE 8
#define I2C_ACP_STATUS_BUS_BUSY 9
#define I2C_ACP_STATUS_CANCEL 10
#define I2C_ACP_STATUS_INVALID_TRANS 11

struct i2c_transaction {
    I2C_Transaction t;
    unsigned int i2cnum;

    int status;
    void (*done)(struct i2c_transaction *transaction);
    void *user_data;
};

void i2c_init(void);

bool i2c_transaction(struct i2c_transaction *t,
		     unsigned int i2cnum, unsigned int addr,
		     void *tx, unsigned int tx_size,
		     void *rx, unsigned int rx_size);

#endif /* I2C_H */
