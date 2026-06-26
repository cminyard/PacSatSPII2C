/*
 *  SPII2C - A program for a SPI to I2C converter device
 *  Copyright (C) 2036  Corey Minyard <corey@minyard.net
 *
 *  SPDX-License-Identifier: GPL-2.0-only
 */

#ifndef TOKENS_H
#define TOKENS_H

#include <stdbool.h>

char *first_token(char *str, char **state);
char *next_token(char **state);

/*
 * Convert a token to a uint.  If invalid, it will print "name: <error>"
 * and return false.  Otherwise it puts the value in val and returns true.
 */
bool token_to_uint(const char *tok, unsigned int *val, int base, char *name);

/*
 * Like the above, but gets the token from the token state and validates
 * that it is not NULL.
 */
bool next_token_uint(char **state, unsigned int *val, int base, char *name);

#endif /* TOKENS_H */
