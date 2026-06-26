/*
 *  SPII2C - A program for a SPI to I2C converter device
 *  Copyright (C) 2036  Corey Minyard <corey@minyard.net
 *
 *  SPDX-License-Identifier: GPL-2.0-only
 */

#include "console.h"
#include "commands.h"

void
mainThread(void *arg0)
{
    console_init(handle_command);

    printf("SPI I2C Converter\n");
    consoleThread(arg0);
}
