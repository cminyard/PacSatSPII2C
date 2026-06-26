/*
 *  SPII2C - A program for a SPI to I2C converter device
 *  Copyright (C) 2036  Corey Minyard <corey@minyard.net
 *
 *  SPDX-License-Identifier: GPL-2.0-only
 */

#ifndef SPII2C_H
#define SPII2C_H

#include <stddef.h>
#include <stdint.h>

#include <FreeRTOS.h>
#include <task.h>

#define printf iprintf
int iprintf(const char *fmt, ...);

#endif /* SPII2C_H */
