
#include <string.h>
#include <ctype.h>

#include "spii2c.h"
#include "commands.h"

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
