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
#include <ti/drivers/GPIO.h>

/* Power on/off the ADC. */
#define CONFIG_GPIO_ADC_ENABLE 25  //PA25
#define CONFIG_GPIO_ADC_ENABLE_IOMUX IOMUX_PINCM69
/* Negative Logic */
#define CONFIG_ADC_ON 0
#define CONFIG_ADC_OFF 1

/* Power on/off the external antenna controller power. */
#define CONFIG_GPIO_ANT_POW 21  //PA21
#define CONFIG_GPIO_ANT_POW_IOMUX IOMUX_PINCM56
/* Negative Logic */
#define CONFIG_ANT_POW_ON 0
#define CONFIG_ANT_POW_OFF 1

/* Extra pin to the antenna control connector pin 10. */
#define CONFIG_GPIO_EXTRA 23  //PA23
#define CONFIG_GPIO_EXTRA_IOMUX IOMUX_PINCM67

/* PC104 GPIO7. */
#define CONFIG_GPIO_PC104_7 26  //PA26
#define CONFIG_GPIO_PC104_7_IOMUX IOMUX_PINCM73

/* PC104 GPIO8. */
#define CONFIG_GPIO_PC104_8 16  //PA16
#define CONFIG_GPIO_PC104_8_IOMUX IOMUX_PINCM42

/* IRQ from the antenna control processor to the main processor. */
#define CONFIG_GPIO_ANT_IRQ 3  //PA3
#define CONFIG_GPIO_ANT_IRQ_IOMUX IOMUX_PINCM8
/* Negative Logic */
#define CONFIG_ANT_IRQ_ON 0
#define CONFIG_ANT_IRQ_OFF 1

/* The range of pins available on this device */
extern const uint_least8_t GPIO_pinLowerBound;
extern const uint_least8_t GPIO_pinUpperBound;

/* I2C */
//extern const uint_least8_t CONFIG_I2C_CONTROLLER_CONST;

/* ======== I2C Addresses and Speeds ======== */
#include <ti/drivers/I2C.h>

/* ---- CONFIG_I2C_CONTROLLER I2C bus components ---- */

#define I2C_A 0
#define I2C_B 1
#define I2C_ADC 2

#define I2C_CLOCK_MHZ 32

/* CONFIG_I2C_TMP max speed (supported by all components) */
#define CONFIG_I2C_CONTROLLER_MAXSPEED (100U) /* kbps */
#define CONFIG_I2C_CONTROLLER_MAXBITRATE ((I2C_BitRate) I2C_100kHz)

#define CONFIG_I2C_COUNT 3

#define I2C_A_INST I2C0
#define I2C_A_IRQHandler I2C0_IRQHandler
#define I2C_A_INT_IRQN I2C0_INT_IRQn

#define GPIO_I2C_A_SDA_PIN 10
#define GPIO_I2C_A_IOMUX_SDA IOMUX_PINCM25
#define GPIO_I2C_A_IOMUX_SDA_FUNC IOMUX_PINCM25_PF_I2C0_SDA

#define GPIO_I2C_A_SCL_PIN 22
#define GPIO_I2C_A_IOMUX_SCL IOMUX_PINCM57
#define GPIO_I2C_A_IOMUX_SCL_FUNC IOMUX_PINCM57_PF_I2C0_SCL

#define I2C_B_INST I2C2
#define I2C_B_IRQHandler I2C2_IRQHandler
#define I2C_B_INT_IRQN I2C2_INT_IRQn

#define GPIO_I2C_B_SDA_PIN 24
#define GPIO_I2C_B_IOMUX_SDA IOMUX_PINCM68
#define GPIO_I2C_B_IOMUX_SDA_FUNC IOMUX_PINCM68_PF_I2C2_SDA

#define GPIO_I2C_B_SCL_PIN 15
#define GPIO_I2C_B_IOMUX_SCL IOMUX_PINCM41
#define GPIO_I2C_B_IOMUX_SCL_FUNC IOMUX_PINCM41_PF_I2C2_SCL

#define I2C_ADC_INST I2C1
#define I2C_ADC_IRQHandler I2C1_IRQHandler
#define I2C_ADC_INT_IRQN I2C1_INT_IRQn

#define GPIO_I2C_ADC_SDA_PIN 18
#define GPIO_I2C_ADC_IOMUX_SDA IOMUX_PINCM50
#define GPIO_I2C_ADC_IOMUX_SDA_FUNC IOMUX_PINCM50_PF_I2C1_SDA

#define GPIO_I2C_ADC_SCL_PIN 17
#define GPIO_I2C_ADC_IOMUX_SCL IOMUX_PINCM49
#define GPIO_I2C_ADC_IOMUX_SCL_FUNC IOMUX_PINCM49_PF_I2C1_SCL

/* DMA */
#define CONFIG_DMA_COUNT 1
#define CONFIG_DMA_CH_COUNT 2
#define DEFAULT_DMA_PRIORITY 31

#define DMA_SPI0_TX_CHAN_ID 0
#define SPI0_TX_INST_DMA_TRIGGER_0 DMA_SPI0_TX_TRIG
#define DMA_SPI0_RX_CHAN_ID 1
#define SPI0_RX_INST_DMA_TRIGGER_1 DMA_SPI0_RX_TRIG

/* SPI */
#include <ti/drivers/SPI.h>

#define ACP_HOST_SPI 0

#define SPI0_INST SPI0
#define SPI0_INST_IRQHandler SPI0_IRQHandler
#define SPI0_INST_INT_IRQN SPI0_INT_IRQn

#define GPIO_SPI0_PICO_PIN 9 /* PA8 */
#define GPIO_SPI0_IOMUX_PICO IOMUX_PINCM20
#define GPIO_SPI0_IOMUX_PICO_FUNC IOMUX_PINCM20_PF_SPI0_PICO

#define GPIO_SPI0_POCI_PIN 4 /* PA4 */
#define GPIO_SPI0_IOMUX_POCI IOMUX_PINCM9
#define GPIO_SPI0_IOMUX_POCI_FUNC IOMUX_PINCM9_PF_SPI0_POCI

#define GPIO_SPI0_SCLK_PIN 11 /* PA11 */
#define GPIO_SPI0_IOMUX_SCLK IOMUX_PINCM26
#define GPIO_SPI0_IOMUX_SCLK_FUNC IOMUX_PINCM26_PF_SPI0_SCLK

#define GPIO_SPI0_CS0_PIN 2 /* PA2 */
#define GPIO_SPI0_IOMUX_CS0 IOMUX_PINCM7
#define GPIO_SPI0_IOMUX_CS0_FUNC IOMUX_PINCM7_PF_SPI0_CS0
#define CONFIG_ANT_CS_ON 0
#define CONFIG_ANT_CS_OFF 1

#endif /* ti_drivers_config_h */
