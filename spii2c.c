
#include "console.h"
#include "commands.h"

void
mainThread(void *arg0)
{
    console_init(handle_command);

    printf("SPI I2C Converter\n");
    consoleThread(arg0);
}
