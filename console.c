/*
 *  SPII2C - A program for a SPI to I2C converter device
 *  Copyright (C) 2026  Corey Minyard <corey@minyard.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-only
 */

#include <ti/display/Display.h>
#include <ti/display/DisplayUart.h>

#include "ti_drivers_config.h"

#include "spii2c.h"
#include "console.h"

static Display_Handle display;
static void (*command_handler)(char *cmd);

void
console_init(void (*handler)(char *cmd))
{
    command_handler = handler;

    Display_init();

    display = Display_open(Display_Type_UART, NULL);
    if (!display) {
	while (1) {
	}
    }
}
    
void *
consoleThread(void *arg0)
{
    char c;
    UART_Handle uart;
    DisplayUart_Object *uart_object;
    size_t len;
    uint32_t status;
    static char input[CONSOLE_MAX_INPUT_SIZE];
    unsigned int input_pos = 0;

    printf("> ");

    uart_object = display->object;
    uart = uart_object->hUart;

    while (1) {
        len = 0;
        while (len == 0) {
            status = UART_read(uart, &c, 1, &len);

            if (status != UART_STATUS_SUCCESS) {
                /* UART_read() failed */
                while (1) {
                }
            }
        }
	switch(c) {
	case '\n':
	case '\r':
	    printf("\n");
	    input[input_pos] = '\0';
	    command_handler(input);
	    input_pos = 0;
	    printf("> ");
	    break;

	case '\b':
	case 0x7f: /* Delete */
	    if (input_pos > 0) {
		input_pos--;
		printf("\b \b");
	    }
	    break;

	default:
	    if (input_pos < CONSOLE_MAX_INPUT_SIZE - 1) {
		input[input_pos++] = c;
		printf("%c", c);
	    }
	    break;
	}
    }
}

#undef iprintf
int iprintf(const char *fmt, ...)
{
    va_list va;

    va_start(va, fmt);
    Display_vprintf(display, 0, 0, fmt, va);
    va_end(va);

    return 0;
}
