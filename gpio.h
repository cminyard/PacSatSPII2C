/*
 *  SPII2C - A program for a SPI to I2C converter device
 *  Copyright (C) 2026  Corey Minyard <corey@minyard.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-only
 */

#ifndef GPIO_H
#define GPIO_H

bool gpio_is_input(unsigned int idx);

unsigned int gpio_read(unsigned int idx, bool nlogic);

#endif /* GPIO_H */
