/*
 *  SPII2C - A program for a SPI to I2C converter device
 *  Copyright (C) 2036  Corey Minyard <corey@minyard.net
 *
 *  SPDX-License-Identifier: GPL-2.0-only
 */

#include <pthread.h>
#include <FreeRTOS.h>
#include <task.h>

#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>

#include "ti_drivers_config.h"

#include "spii2c.h"
#include "console.h"
#include "commands.h"
#include "i2c.h"
#include "spi.h"

void *
mainThread(void *arg0)
{
    pthread_t thread;
    pthread_attr_t task_attrs;
    struct sched_param task_params;
    int rv;

    console_init(handle_command);
    GPIO_init();
    i2c_init();
    spi_init();

    GPIO_setConfig(CONFIG_GPIO_ADC_ENABLE,
		   GPIO_CFG_OUT_STD | CONFIG_GPIO_ADC_ENABLE_IOMUX);
    GPIO_setConfig(CONFIG_GPIO_ANT_POW,
		   GPIO_CFG_OUT_STD | CONFIG_GPIO_ANT_POW_IOMUX);
    GPIO_setConfig(CONFIG_GPIO_EXTRA,
		   GPIO_CFG_IN_PU | CONFIG_GPIO_EXTRA_IOMUX);
    GPIO_setConfig(CONFIG_GPIO_PC104_7,
		   GPIO_CFG_IN_PU | CONFIG_GPIO_PC104_7_IOMUX);
    GPIO_setConfig(CONFIG_GPIO_PC104_8,
		   GPIO_CFG_IN_PU | CONFIG_GPIO_PC104_8_IOMUX);

    GPIO_write(CONFIG_GPIO_ADC_ENABLE, CONFIG_ADC_ON);
    GPIO_write(CONFIG_GPIO_ANT_POW, CONFIG_ANT_POW_OFF);

    printf("SPI I2C Converter\n");

    pthread_attr_init(&task_attrs);
    task_params.sched_priority = 1;
    rv = pthread_attr_setschedparam(&task_attrs, &task_params);
    rv |= pthread_attr_setdetachstate(&task_attrs, PTHREAD_CREATE_DETACHED);
    rv |= pthread_attr_setstacksize(&task_attrs, SPI_THREAD_STACK_SIZE);
    if (rv != 0) {
        /* failed to set attributes */
        while (1) {
        }
    }

    rv = pthread_create(&thread, &task_attrs, spiThread, NULL);
    if (rv != 0) {
        while (1) {
        }
    }

    /* We use this thread for the console thread. */
    consoleThread(arg0);
    return NULL;
}
