/*
 *  SPII2C - A program for a SPI to I2C converter device
 *  Copyright (C) 2036  Corey Minyard <corey@minyard.net
 *
 *  SPDX-License-Identifier: GPL-2.0-only
 */

#ifndef I2C_H
#define I2C_H

void i2c_init(void);

bool i2c_transaction(unsigned int i2cnum, unsigned int addr,
		     void *tx, unsigned int tx_size,
		     void *rx, unsigned int rx_size);

#endif /* I2C_H */
