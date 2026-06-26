/*
 * Copyright (c) 2021, Texas Instruments Incorporated
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
 *  ============ ti_drivers_config.c =============
 *  Configured TI-Drivers module definitions
 */

#include "ti_drivers_config.h"

/*
 *  =============================== UART ===============================
 */
#include <ti/drivers/uart/UARTMSPM0.h>
#include <ti/display/Display.h>
#include <ti/display/DisplayUart.h>

const uint_least8_t CONFIG_UART_0 = 0;
const uint_least8_t UART_count    = CONFIG_UART_COUNT;

#define Display_UARTBUFFERSIZE 128
static char displayUARTBuffer[Display_UARTBUFFERSIZE];

DisplayUart_Object displayUartObject;

const DisplayUart_HWAttrs displayUartHWAttrs = {
    .uartIdx = CONFIG_UART_0,
    .baudRate = CONFIG_UART_BAUD_RATE,
    .mutexTimeout = (unsigned int) (-1),
    .strBuf = displayUARTBuffer,
    .strBufLen = Display_UARTBUFFERSIZE
};

const Display_Config Display_config[CONFIG_Display_COUNT] = {
    /* CONFIG_Display_0 */
    {
	.fxnTablePtr = &DisplayUartMin_fxnTable,
        .object   = &displayUartObject,
        .hwAttrs  = &displayUartHWAttrs
    },
};

const uint_least8_t Display_count = CONFIG_Display_COUNT;

static const UARTMSP_HWAttrs UARTMSPHWAttrs[CONFIG_UART_COUNT] = {
    /* UART0 */
    {
        .regs          = UART0,
        .irq           = UART0_INT_IRQn,
        .rxPin         = IOMUX_PINCM2, /* PA1 */
        .rxPinFunction = IOMUX_PINCM2_PF_UART0_RX,
        .txPin         = IOMUX_PINCM1, /* PA0 */
        .txPinFunction = IOMUX_PINCM1_PF_UART0_TX,
        .mode          = DL_UART_MODE_NORMAL,
        .direction     = DL_UART_DIRECTION_TX_RX,
        .flowControl   = DL_UART_FLOW_CONTROL_NONE,
        .clockSource   = DL_UART_CLOCK_BUSCLK,
        .clockDivider  = DL_UART_CLOCK_DIVIDE_RATIO_4,
        .rxIntFifoThr  = DL_UART_RX_FIFO_LEVEL_ONE_ENTRY,
        .txIntFifoThr  = DL_UART_TX_FIFO_LEVEL_EMPTY,
    },
};

UART_Data_Object UARTObject[CONFIG_UART_COUNT] = {
    {
        .object =
            {
                .supportFxns        = &UARTMSPSupportFxns,
                .buffersSupported   = true,
                .eventsSupported    = false,
                .callbacksSupported = false,
                .dmaSupported       = false,
            },
        .buffersObject =
            {
                .rxBufPtr  = rxBuffer,
                .txBufPtr  = txBuffer,
                .rxBufSize = sizeof(rxBuffer),
                .txBufSize = sizeof(txBuffer),
            },
    },
};

const UART_Config UART_config[CONFIG_UART_COUNT] = {
    {
        &UARTObject[CONFIG_UART_0],
        &UARTMSPHWAttrs[CONFIG_UART_0],
    },
};

void UART0_IRQHandler(void)
{
    UARTMSP_interruptHandler((UART_Handle) &UART_config[0]);
}

void UARTMSP_eventCallback(
    UART_Handle handle, uint32_t event, uint32_t data, void *userArg)
{
}

/*
 *  =============================== GPIO ===============================
 */

#include <ti/drivers/GPIO.h>
#include <ti/drivers/gpio/GPIOMSPM0.h>

/* The range of pins available on this device */
const uint_least8_t GPIO_pinLowerBound = 0;
const uint_least8_t GPIO_pinUpperBound = 27;

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>
/*
 *  ======== gpioPinConfigs ========
 *  Array of Pin configurations
 */
GPIO_PinConfig gpioPinConfigs[27] = {
    GPIO_DO_NOT_CONFIG, /* PA0 */
    GPIO_DO_NOT_CONFIG, /* PA1 */
    GPIO_DO_NOT_CONFIG, /* PA2 */
    GPIO_DO_NOT_CONFIG, /* PA3 */
    GPIO_DO_NOT_CONFIG, /* PA4 */
    GPIO_DO_NOT_CONFIG, /* PA5 */
    GPIO_DO_NOT_CONFIG, /* PA6 */
    GPIO_DO_NOT_CONFIG, /* PA7 */
    GPIO_DO_NOT_CONFIG, /* PA8 */
    GPIO_DO_NOT_CONFIG, /* PA9 */
    GPIO_DO_NOT_CONFIG, /* PA10 */
    GPIO_DO_NOT_CONFIG, /* PA11 */
    GPIO_DO_NOT_CONFIG, /* PA12 */
    GPIO_DO_NOT_CONFIG, /* PA13 */
    GPIO_DO_NOT_CONFIG, /* PA14 */
    GPIO_DO_NOT_CONFIG, /* PA15 */
    GPIO_DO_NOT_CONFIG, /* PA16 */
    GPIO_DO_NOT_CONFIG, /* PA17 */
    GPIO_DO_NOT_CONFIG, /* PA18 */
    GPIO_DO_NOT_CONFIG, /* PA19 */
    GPIO_DO_NOT_CONFIG, /* PA20 */
    GPIO_DO_NOT_CONFIG, /* PA21 */
    GPIO_DO_NOT_CONFIG, /* PA22 */
    GPIO_DO_NOT_CONFIG, /* PA23 */
    GPIO_DO_NOT_CONFIG, /* PA24 */
    GPIO_DO_NOT_CONFIG, /* PA25 */
    GPIO_DO_NOT_CONFIG, /* PA26 */
};

/*
 *  ======== gpioCallbackFunctions ========
 *  Array of callback function pointers
 *  Change at runtime with GPIO_setCallback()
 */
GPIO_CallbackFxn gpioCallbackFunctions[27] = {0};

/*
 *  ======== gpioUserArgs ========
 *  Array of user argument pointers
 *  Change at runtime with GPIO_setUserArg()
 *  Get values with GPIO_getUserArg()
 */
void *gpioUserArgs[27];

const GPIO_Config GPIO_config = {
    .configs = (GPIO_PinConfig *) gpioPinConfigs,
    .callbacks = (GPIO_CallbackFxn *) gpioCallbackFunctions,
    .userArgs = gpioUserArgs,
    .intPriority = (~0)
};

const uint_least8_t CONFIG_GPIO_ADC_ENABLE_CONST = CONFIG_GPIO_ADC_ENABLE;
