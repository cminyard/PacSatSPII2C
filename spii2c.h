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
/*
 *__attribute__((__format__(printf, 1, 2)))
 *   This should work, why doesn't it?
 */ 
int iprintf(const char *fmt, ...);

#define MAIN_THREAD_STACK_SIZE configMINIMAL_STACK_SIZE * 6
#define SPI_THREAD_STACK_SIZE configMINIMAL_STACK_SIZE * 5

#define container_of(ptr, type, member)          \
    ((type *)(((char *) ptr) - offsetof(type, member)))

#endif /* SPII2C_H */
