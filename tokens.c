/*
 *  SPII2C - A program for a SPI to I2C converter device
 *  Copyright (C) 2036  Corey Minyard <corey@minyard.net
 *
 *  SPDX-License-Identifier: GPL-2.0-only
 */

#include <stdlib.h>
#include <ctype.h>

#include "spii2c.h"
#include "tokens.h"

static char *i_next_token(char *str, char **state)
{
    char *rv;

    if (!str)
	str = *state;

    while (isspace(*str))
	str++;
    if (!*str)
	return NULL;

    rv = str;

    while (*str && !isspace(*str))
	str++;

    if (*str) {
	*str = '\0';
	str++;
    }
    *state = str;

    return rv;
}

char *
first_token(char *str, char **state)
{
    return i_next_token(str, state);
}

char *
next_token(char **state)
{
    return i_next_token(NULL, state);
}

bool
token_to_uint(const char *tok, unsigned int *val, int base, char *name)
{
    char *end;

    *val = strtoul(tok, &end, base);
    if (*end != '\0') {
	printf("%s: Invalid value '%s'\n", name, tok);
	return false;
    }
    return true;
}

bool
next_token_uint(char **state, unsigned int *val, int base, char *name)
{
    char *tok = next_token(state);

    if (!tok) {
	printf("%s: Value is missing\n", name);
	return false;
    }

    return token_to_uint(tok, val, base, name);
}

bool
token_to_uint8(const char *tok, uint8_t *val, int base, char *name)
{
    unsigned int ival;

    if (!token_to_uint(tok, &ival, base, name))
	return false;
    if (ival > 255) {
	printf("%s: Value too large for byte\n", name);
	return false;
    }
    *val = ival;
    return true;
}

bool
next_token_uint8(char **state, uint8_t *val, int base, char *name)
{
    char *tok = next_token(state);

    if (!tok) {
	printf("%s: Value is missing\n", name);
	return false;
    }

    return token_to_uint8(tok, val, base, name);
}
