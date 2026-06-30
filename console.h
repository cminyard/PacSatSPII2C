/*
 *  SPII2C - A program for a SPI to I2C converter device
 *  Copyright (C) 2036  Corey Minyard <corey@minyard.net
 *
 *  SPDX-License-Identifier: GPL-2.0-only
 */

#ifndef CONSOLE_H
#define CONSOLE_H

#include "spii2c.h"

/* Maximum size of an input line. */
#define CONSOLE_MAX_INPUT_SIZE 128

/* Must be called before consoleThread or any use of printf. */
void console_init(void (*handler)(char *cmd));

/* The thread handling the console. */
void *consoleThread(void *arg0);


#endif /* CONSOLE_H */
