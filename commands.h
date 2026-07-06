/*
 *  SPII2C - A program for a SPI to I2C converter device
 *  Copyright (C) 2026  Corey Minyard <corey@minyard.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-only
 */

#ifndef COMMANDS_H
#define COMMANDS_H

void command_init(void);

void handle_command(char *cmd);

#endif /* COMMANDS_H */
