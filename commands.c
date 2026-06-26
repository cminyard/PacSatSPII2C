/*
 *  SPII2C - A program for a SPI to I2C converter device
 *  Copyright (C) 2036  Corey Minyard <corey@minyard.net
 *
 *  SPDX-License-Identifier: GPL-2.0-only
 */

#include <string.h>

#include "spii2c.h"
#include "tokens.h"
#include "commands.h"

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

    for (i = 0; commands[i].cmd; i++) {
	if (strcmp(commands[i].cmd, cmd) == 0) {
	    commands[i].handler(&tokst);
	    return;
	}
    }
    printf("Unknown command: %s\n", cmd);
}
