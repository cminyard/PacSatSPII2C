#ifndef CONSOLE_H
#define CONSOLE_H

#include "spii2c.h"

#define CONSOLE_MAX_INPUT_SIZE 128

void console_init(void (*handler)(char *cmd));
void consoleThread(void *arg0);


#endif /* CONSOLE_H */
