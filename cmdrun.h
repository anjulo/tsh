#ifndef TSH_OSPSH_H
#define TSH_OSPSH_H

/*
 * Header file for tux shell
 * This file contains the definitions required for executing commands
 * parsed by the code in cmdparse.c
 */

#include "cmdparse.h"

/* Execute the command list. */
int cmd_line_exec(command_t *);

#endif
