/*
 *  SPII2C - A program for a SPI to I2C converter device
 *  Copyright (C) 2026  Corey Minyard <corey@minyard.net>
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
#define SPI_THREAD_STACK_SIZE configMINIMAL_STACK_SIZE * 6

#define container_of(ptr, type, member)          \
    ((type *)(((char *) ptr) - offsetof(type, member)))

void adc_init(void);
void *adcThread(void *arg0);

/*
 * Message numbers, see README.md for details.
 */
#define ACP_I2C_CMD 1
#define ACP_I2C_RSP 2
#define ACP_SET_GPIO 3
#define ACP_GET_GPIO 4
#define ACP_GPIO_VALUE 5
#define ACP_ADC_REPORT 6

#endif /* SPII2C_H */
