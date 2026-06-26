/*
 * Copyright (c) 2021, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_drivers_config.h =============
 *  Configured MSPM0 Driver module declarations
 *
 */
#ifndef ti_drivers_config_h
#define ti_drivers_config_h

#define CONFIG_LP_MSPM0L1228

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#define POWER_STARTUP_DELAY                                                (16)

/* UART */
#include <ti/drivers/UART.h>
#include <ti/drivers/uart/UARTMSPM0.h>

#define CONFIG_UART_COUNT 1
#define CONFIG_UART_BUFFER_LENGTH 1
#define CONFIG_Display_COUNT 1

#define CONFIG_UART_0_INST UART0
#define CONFIG_UART_BAUD_RATE 9600

extern const uint_least8_t CONFIG_UART_0;
extern const uint_least8_t UART_count;

uint8_t rxBuffer[CONFIG_UART_BUFFER_LENGTH];
uint8_t txBuffer[CONFIG_UART_BUFFER_LENGTH];

/* clang-format on */
void UART0_IRQHandler(void);

/* GPIO */
#include <ti/driverlib/dl_gpio.h>

/* Power on/off the ADC. */
extern const uint_least8_t CONFIG_GPIO_ADC_ENABLE_CONST;
#define CONFIG_GPIO_ADC_ENABLE 25  //PA25
#define CONFIG_GPIO_ADC_ENABLE_IOMUX IOMUX_PINCM69

/* Power on/off the external antenna controller power. */
extern const uint_least8_t CONFIG_GPIO_ANT_POW_CONST;
#define CONFIG_GPIO_ANT_POW 21  //PA21
#define CONFIG_GPIO_ANT_POW_IOMUX IOMUX_PINCM56

/* Extra pin to the antenna control connector pin 10. */
extern const uint_least8_t CONFIG_GPIO_EXTRA_CONST;
#define CONFIG_GPIO_EXTRA 23  //PA23
#define CONFIG_GPIO_EXTRA_IOMUX IOMUX_PINCM67

/* PC104 GPIO7. */
extern const uint_least8_t CONFIG_GPIO_PC104_7_CONST;
#define CONFIG_GPIO_PC104_7 26  //PA26
#define CONFIG_GPIO_PC104_7_IOMUX IOMUX_PINCM73

/* PC104 GPIO8. */
extern const uint_least8_t CONFIG_GPIO_PC104_8_CONST;
#define CONFIG_GPIO_PC104_8 16  //PA16
#define CONFIG_GPIO_PC104_8_IOMUX IOMUX_PINCM42

/* The range of pins available on this device */
extern const uint_least8_t GPIO_pinLowerBound;
extern const uint_least8_t GPIO_pinUpperBound;

/* Negative Logic */
#define CONFIG_ADC_ON 0
#define CONFIG_ADC_OFF 1

/* Negative Logic */
#define CONFIG_ANT_POW_ON 0
#define CONFIG_ANT_POW_OFF 1

#endif /* ti_drivers_config_h */
