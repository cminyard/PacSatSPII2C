/*
 *  SPII2C - A program for a SPI to I2C converter device
 *  Copyright (C) 2036  Corey Minyard <corey@minyard.net
 *
 *  SPDX-License-Identifier: GPL-2.0-only
 */

#include <string.h>

#include "ti_drivers_config.h"

#include "spii2c.h"
#include "tokens.h"
#include "commands.h"


#include <ti/drivers/GPIO.h>

/* GPIO */

static bool
gpio_is_input(unsigned int idx)
{
    GPIO_PinConfig cfg;

    /* Low bit of IO is set on an input. */
    GPIO_getConfig(idx, &cfg);
    cfg = (cfg & GPIO_CFG_IO_MASK) >> GPIO_CFG_IO_LSB;

    return cfg & 1;
}

static unsigned int
gpio_read(unsigned int idx, bool nlogic)
{
    unsigned int val;

    if (gpio_is_input(idx)) {
	val = GPIO_read(idx);
    } else {
	/*
	 * You cannot read the value off an output GPIO pin, and
	 * there's no API get the value from the register.  So just
	 * pull what it's configured to.
	 */
	GPIO_PinConfig cfg;

	GPIO_getConfig(idx, &cfg);
	val = !!(cfg & GPIO_CFG_OUT_HIGH);
    }
    if (nlogic)
	val = !val;
    return val;
}

static void
pr_gpio(const char *name, unsigned int idx, bool nlogic)
{
    printf("%s(%s %s): %d\n", name,
	   gpio_is_input(idx) ? "in" : "out", nlogic ? "n" : "p",
	   gpio_read(idx, nlogic));
}

static void
gpio_command(char **tokst)
{
    char *tok = next_token(tokst);
    char *vtok = next_token(tokst);
    unsigned int val, gpionum;
    bool nlogic = false;

    if (!tok) {
	pr_gpio("adc", CONFIG_GPIO_ADC_ENABLE, true);
	pr_gpio("ant", CONFIG_GPIO_ANT_POW, true);
	pr_gpio("ext", CONFIG_GPIO_EXTRA, false);
	pr_gpio("pc104_7", CONFIG_GPIO_PC104_7, false);
	pr_gpio("pc104_8", CONFIG_GPIO_PC104_8, false);
	return;
    }

    if (strcmp(tok, "adc") == 0) {
	gpionum = CONFIG_GPIO_ADC_ENABLE;
	nlogic = true;
    } else if (strcmp(tok, "ant") == 0) {
	gpionum = CONFIG_GPIO_ANT_POW;
	nlogic = true;
    } else if (strcmp(tok, "ext") == 0) {
	gpionum = CONFIG_GPIO_EXTRA;
    } else if (strcmp(tok, "pc104_7") == 0) {
	gpionum = CONFIG_GPIO_PC104_7;
    } else if (strcmp(tok, "pc104_8") == 0) {
	gpionum = CONFIG_GPIO_PC104_8;
    } else {
	printf("Unknown gpio name: %s\n", tok);
	return;
    }

    if (vtok) {
	if (gpio_is_input(gpionum)) {
	    printf("gpio %s is an input, cannot set\n", tok);
	    return;
	}
	if (!token_to_uint(vtok, &val, 0, "gpio value"))
	    return;
	if (nlogic)
	    val = !val;
	GPIO_write(gpionum, val);
    }

    printf("%s: %d\n", tok, gpio_read(gpionum, nlogic));
}

/* I2C */

static void
i2c_command(char **tokst)
{
    unsigned int i2cnum;
    unsigned int tgtaddr;
    unsigned int rx_count;
    uint8_t rx_data[32];
    unsigned int tx_count = 0;
    uint8_t tx_data[32];
    char *tok;
    unsigned int i, v;
    
    if (!next_token_uint(tokst, &i2cnum, 0, "i2cnum"))
	return;
    if (!next_token_uint(tokst, &tgtaddr, 0, "tgtaddr"))
	return;
    if (!next_token_uint(tokst, &rx_count, 0, "rx count"))
	return;
    if (i2cnum >= CONFIG_I2C_COUNT) {
	printf("i2cnum out of range, ranges from 0-%d\n",
	       CONFIG_I2C_COUNT - 1);
	return;
    }
    if (tgtaddr > 0xff) {
	printf("tgtaddr out of range, ranges from 0-255\n");
	return;
    }
    if (rx_count > sizeof(rx_data)) {
	printf("rx count too large, max is %d\n", sizeof(rx_data));
	return;
    }

    tok = next_token(tokst);
    while (tok) {
	if (tx_count >= sizeof(tx_data)) {
	    printf("Too many tx bytes, limit is %d\n", sizeof(tx_data));
	    return;
	}
	if (!token_to_uint(tok, &v, 0, "txval"))
	    return;
	tx_data[tx_count] = v;
	tx_count++;
	tok = next_token(tokst);
    }

    if (!i2c_transaction(i2cnum, tgtaddr, tx_data, tx_count, rx_data,
			 rx_count)) {
	printf("I2C transaction failed\n");
    } else {
	printf("Read: ");
	for (i = 0; i < rx_count; i++) {
	    if (i % 16 == 0)
		printf("\n ");
	    printf(" %2.2x", rx_data[i]);
	}
	printf("\n");
    }
}

/* Main command structure. */

static void help_command(char **tokst);

const static struct command {
    const char *cmd;
    void (*handler)(char **tokst);
    const char *example;
    const char *help;
} commands[] = {
    {
	"help", help_command,
	"[substring]",
	"Print help for all commands or commands with given substring."
    },
    {
	"gpio", gpio_command,
	"[gpioname [newvalue]]",
	"Print and optionally set the value of a gpio."
    },
    {
	"i2c", i2c_command,
	"<i2cnum> <tgtaddr> <rx count> [tx1 [tx2 [....]]]",
	"Run an I2C transaction write the tx byes and read rx count bytes."
    },
    {}
};

static void
help_command(char **tokst)
{
    unsigned int i;
    char *pattern = next_token(tokst);

    for (i = 0; commands[i].cmd; i++) {
	if (!pattern || strstr(commands[i].cmd, pattern))
	    printf("%s %s\n    %s\n\n", commands[i].cmd, commands[i].example,
		   commands[i].help);
    }
}

void
handle_command(char *incmd)
{
    unsigned int i;
    char *tokst;
    char *cmd = first_token(incmd, &tokst);

    if (!cmd)
	return; /* Ignore blank lines. */

    for (i = 0; commands[i].cmd; i++) {
	if (strcmp(commands[i].cmd, cmd) == 0) {
	    commands[i].handler(&tokst);
	    return;
	}
    }
    printf("Unknown command: %s\n", cmd);
}
