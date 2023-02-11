/*
 * Skeleton code for Lab 2 - Shell processing
 * This file contains skeleton code for executing parsed commands.
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


void cmd_redirect(command_t *cmd){
  int ifd, ofd, efd ; // input, output and error file descriptors
  if(cmd->redirect_filename[0]){
        ifd = open(cmd->redirect_filename[0], O_RDONLY); //0666 is default, anyways!
        if(ifd == -1){
          perror("open");
          abort();
        }
  if(dup2(fd,i) == -1){ // duplicate file descriptor
    perror("dup2");
    abort();
  }
        if(close(ifd) == -1){
          perror("close");
          abort();
        }
      }
  if(cmd->redirect_filename[1]){
        ofd = open(cmd->redirect_filename[1], O_CREAT | O_WRONLY, 0666); //0666 is default, anyways!
        if(ofd == -1){
          perror("open");
          abort();
        }
        if(dup2(ofd,STDOUT_FILENO) == -1){ // duplicate file descriptor
          perror("dup2");
          abort();
        }
        if(close(ofd) == -1){
          perror("close");
          abort();
        }
      }
  if(cmd->redirect_filename[2]){
        efd = open(cmd->redirect_filename[2], O_CREAT | O_WRONLY, 0666); //0666 is default, anyways!
        if(efd == -1){
          perror("open");
          abort();
        }
        if(dup2(efd,STDOUT_FILENO) == -1){ // duplicate file descriptor
          perror("dup2");
          abort();
        }
        if(close(efd) == -1){
          perror("close");
          abort();
        }
      }
 
}
/* cmd_exec(cmd, pass_pipefd)
 *
 *   Execute the single command specified in the 'cmd' command structure.
 *
 *   The 'pass_pipefd' argument is used for pipes.
 *
 *     On input, '*pass_pipefd' is the file descriptor that the
 *     current command should use to read the output of the previous
 *     command. That is, it's the "read end" of the previous
 *     pipe, if there was a previous pipe; if there was not, then
 *     *pass_pipefd will equal STDIN_FILENO.
 *
 *     On output, cmd_exec should set '*pass_pipefd' to the file descriptor
 *     used for reading from THIS command's pipe (so that the next command
 *     can use it). If this command didn't have a pipe -- that is,
 *     if cmd->controlop != PIPE -- then this function should set
 *     '*pass_pipefd = STDIN_FILENO'.
 *
 *   Returns the process ID of the forked child, or < 0 if some system call
 *   fails.
 *
 *   Besides handling normal commands, redirection, and pipes, you must also
 *   handle three internal commands: "cd", "exit", and "our_pwd".
 *   (Why must "cd", "exit", and "our_pwd" (a version of "pwd") be implemented
 *   by the shell, versus simply exec()ing to handle them?)
 *
 *   Note that these special commands still have a status!
 *   For example, "cd DIR" should return status 0 if we successfully change
 *   to the DIR directory, and status 1 otherwise.
 *   Thus, "cd /tmp && echo /tmp exists" should print "/tmp exists" to stdout
 *      if and only if the /tmp directory exists.
 *   Not only this, but redirections should work too!
 *   For example, "cd /tmp > foo" should create an empty file named 'foo';
 *   and "cd /tmp 2> foo" should print any error messages to 'foo'.
 *
 *   Some specifications:
 *
 *       --subshells:
 *         the exit status should be either 0 (if the last
 *         command would have returned 0) or 5 (if the last command
 *         would have returned something non-zero). This is not the
 *         behavior of bash.
 *
 *       --cd:
 *
 *          this builtin takes exactly one argument besides itself (this
 *          is also not bash's behavior). if it is given fewer
 *          ("cd") or more ("cd foo bar"), that is a syntax error.  Any
 *          error (syntax or system call) should result in a non-zero
 *          exit status. Here is the specification for output:
 *
 *                ----If there is a syntax error, your shell should
 *                display the following message verbatim:
 *                   "cd: Syntax error! Wrong number of arguments!"
 *
 *                ----If there is a system call error, your shell should
 *                invoke perror("cd")
 *
 *       --our_pwd:
 *
 *          This stands for "our pwd", which prints the working
 *          directory to stdout, and has exit status 0 if successful and
 *          non-zero otherwise. this builtin takes no arguments besides
 *          itself. Handle errors in analogy with cd. Here, the syntax
 *          error message should be:
 *
 *              "pwd: Syntax error! Wrong number of arguments!"
 *
 *       --exit:
 *
 *          As noted in the lab, exit can take 0 or 1 arguments. If it
 *          is given zero arguments (besides itself), then the shell
 *          exits with command status 0. If it is given one numerical
 *          argument, the shell exits with that numerical argument. If it
 *          is given one non-numerical argument, do something sensible.
 *          If it is given more than one argument, print an error message,
 *          and do not exit.
 *
 *
 *   Implementation hints are given in the function body.
 */
static pid_t
cmd_exec(command_t *cmd, int *pass_pipefd)
{
        (void)pass_pipefd;      // get rid of unused warning
	pid_t pid = -1;		// process ID for child
	int pipefd[2];		// file descriptors for this process's pipe

	/* EXERCISE 4: Complete this function!
	 * We've written some of the skeleton for you, but feel free to
	 * change it.
	 */

	// Create a pipe, if this command is the left-hand side of a pipe.
	// Return -1 if the pipe fails.
	if (cmd->controlop == CMD_PIPE) {
		/* Your code here*/
	}
  pid =  fork();
  if(pid == -1){
    perror("fork");
    abort();
  }
  else if(pid == 0){ // child
    if(cmd->redirect_filename){ // redirection
      if(cmd->redirect_filename[0]) 
        cmd_redirect(cmd,STDIN_FILENO);
      if(cmd->redirect_filename[1])
        cmd_redirect(cmd,STDOUT_FILENO);
      if(cmd->redirect_filename[2])
        cmd_redirect(cmd,STDERR_FILENO);
    }
    execvp(cmd->argv[0], cmd->argv);
    perror("execvp");
    abort();
  }
  else{
    //return pid;
  }
  
	// return the child process ID
	return pid;
}


/* cmd_line_exec(cmdlist)
 *
 *   Execute the command list.
 *
 *   Execute each individual command with 'cmd_exec'.
 *   String commands together depending on the 'cmdlist->controlop' operators.
 *   Returns the exit status of the entire command list, which equals the
 *   exit status of the last completed command.
 *
 *   The operators have the following behavior:
 *
 *      CMD_END, CMD_SEMICOLON
 *                        Wait for command to exit.  Proceed to next command
 *                        regardless of status.
 *      CMD_AND           Wait for command to exit.  Proceed to next command
 *                        only if this command exited with status 0.  Otherwise
 *                        exit the whole command line.
 *      CMD_OR            Wait for command to exit.  Proceed to next command
 *                        only if this command exited with status != 0.
 *                        Otherwise exit the whole command line.
 *      CMD_BACKGROUND, CMD_PIPE
 *                        Do not wait for this command to exit.  Pretend it
 *                        had status 0, for the purpose of returning a value
 *                        from cmd_line_exec.
 */
int
cmd_line_exec(command_t *cmdlist)
{
	int cmd_status = 0;	    // status of last command executed
	int pipefd = STDIN_FILENO;  // read end of last pipe

	while (cmdlist) {
		int wp_status;	    // Use for waitpid's status argument!
				    // Read the manual page for waitpid() to
				    // see how to get the command's exit
				    // status (cmd_status) from this value.

		// EXERCISE 4: Fill out this function!
		// If an error occurs in cmd_exec, feel free to abort().

		/* Your code here */
    switch (cmdlist->controlop)
    {
    case CMD_END:
    case CMD_SEMICOLON:
      cmd_status = cmd_exec(cmdlist, &pipefd);
      waitpid(cmd_status, &wp_status, 0);
      break;
    
    default:
      break;
    }

		cmdlist = cmdlist->next;
	}

        while (waitpid(0, 0, WNOHANG) > 0);

done:
	return cmd_status;
}
