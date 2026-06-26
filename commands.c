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
