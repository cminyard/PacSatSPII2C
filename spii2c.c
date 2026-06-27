/*
 *  SPII2C - A program for a SPI to I2C converter device
 *  Copyright (C) 2036  Corey Minyard <corey@minyard.net
 *
 *  SPDX-License-Identifier: GPL-2.0-only
 */

#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>

#include "ti_drivers_config.h"

#include "spii2c.h"
#include "console.h"
#include "commands.h"
#include "i2c.h"

void
mainThread(void *arg0)
{
    console_init(handle_command);
    GPIO_init();
    i2c_init();

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

    /* We use this thread for the console thread. */
    consoleThread(arg0);
}
