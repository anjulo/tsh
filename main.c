/*
 * Skeleton code for tux shell
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "cmdparse.h"
#include "cmdrun.h"

/*
 * Main function for shell.
 */
int
main(int argc, char *argv[])
{
	int quiet = 0, parseonly = 0;
	char input[BUFSIZ];
	int i = 0, r = 0;

	// Check for '-q', '-p' option:
	//    -q: be quiet -- print no prompts
	//    -p: parse only -- do not run the cmd
	for (i = 1; i < argc; i++) {
		if(strcmp(argv[i], "-q") == 0) {
			quiet = 1;
		} else if(strcmp(argv[i], "-p") == 0) {
			parseonly = 1;
		}
	}

	while (!feof(stdin)) {
    parsestate_t parsestate;
		command_t *cmdlist; // linked list for a command
		// Print the prompt
		if (!quiet) {
			printf("tsh$ ");
			fflush(stdout);
		}

		// Read a string, checking for error or EOF
		if (fgets(input, BUFSIZ, stdin) == NULL) {  
			if (ferror(stdin) && errno == EINTR) { // errno - global variable for error status
                                             // EINTR - interrupt
                                             // ferror - checks if error has occurred in a file stream
				cmd_line_exec(0);
				continue;
			}
			if (ferror(stdin))
				// This function prints a description of the
				// error, preceded by 'tsh: '.
				perror("tsh");
			break;
		}

		// build the command list
		parse_init(&parsestate, input);

		cmdlist = cmd_line_parse(&parsestate, 0);
		if (!cmdlist) {
			printf("Syntax error\n");
			continue;
		}

		// print the command list
		if (!quiet) {
			cmd_print(cmdlist, 0);
			// why do we need to do this?
			fflush(stdout);
		}

    // this actually invokes the command list.
    // you will fill in cmd_line_exec.
		if (!parseonly && cmdlist)
			cmd_line_exec(cmdlist);
    if (cmdlist)
      cmd_free(cmdlist);

	}

	return 0;
}
