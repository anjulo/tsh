/*
 * Skeleton code for tux shell processing
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
#include <limits.h> // for PATH_MAX
#include <stdbool.h> 
#include <sys/stat.h>
#include <sys/wait.h>
#include "cmdparse.h"
#include "cmdrun.h"

#ifndef PATH_MAX // define PATH_MAX if it's not already defined
#define PATH_MAX 4096
#endif

void redirect_io(command_t *cmd);
int cd_exec(command_t *cmd, bool verbose);
int exit_exec(command_t *cmd, bool verbose);
int our_pwd_exec(command_t *cmd, bool verbose);
int help_exec(command_t *cmd);

/**
  * Executes a single command.
  * @cmd: The command to execute.
  * @pass_pipefd: If this command is the right-hand side of a pipe, this
  *   is the file descriptor for the pipe. Otherwise, it's STDIN_FILENO.
  *
  * @return the process ID of the child process, or -1 on if some syscall fails. 
*/
static pid_t
cmd_exec(command_t *cmd, int *pass_pipefd)
{
  (void)pass_pipefd;      // get rid of unused warning
	pid_t cpid = -1;	
	int pipefd[2];		// file descriptors for this process's pipe

	if (cmd->controlop == CMD_PIPE) {
    if(pipe(pipefd) == -1){
      perror("pipe");
      return -1;
    }
	}

  // fork
  cpid =  fork();
  if(cpid == -1){
    perror("fork");
    return -1;
  }

  // child
  else if(cpid == 0){
    
    //pipe
    // setup stdout to this command's pipe
    if(cmd->controlop == CMD_PIPE){  
      if(close(pipefd[0]) == -1){ // close read end of pipe
        perror("close");
        return -1;
      }
      if(dup2(pipefd[1], STDOUT_FILENO) == -1){ // redirect stdout to write end of pipe
        perror("dup2");
        return -1;
      }
      if(close(pipefd[1]) == -1){ // close write end of pipe
        perror("close");
        return -1;
      }
    }
    // setup stdin to previous command's pipe
    if(*pass_pipefd != STDIN_FILENO){
      if(dup2(*pass_pipefd, STDIN_FILENO) == -1){ // redirect stdin to read end of previous pipe
        perror("dup2");
        return -1;
      }
      pass_pipefd = NULL; // set to NULL so that it doesn't get closed in the parent
    }

    // io redirection
    redirect_io(cmd);
    
    // sub-shell
    if(cmd->subshell){
      int ss_status = cmd_line_exec(cmd->subshell);
      if(ss_status) // non- zero status
        exit(5);
      exit(0);
    }

    
    // builtin commands for the child
    // cd
    if(strcmp(cmd->argv[0], "cd") == 0){
      if(!cd_exec(cmd, true))
        exit(0);
      exit(1);
    }
    // exit
    if(strcmp(cmd->argv[0], "exit") == 0){
        if(exit_exec(cmd, true))
          exit(1);
    }
    // our_pwd
    if(strcmp(cmd->argv[0], "our_pwd") == 0){
      if(!our_pwd_exec(cmd, true))
        exit(0);
      exit(1);
    }
    // help
    if(strcmp(cmd->argv[0], "help") == 0){
      exit(0);
    }

    
    // execute non-builtin commands
    execvp(cmd->argv[0], cmd->argv);
    perror("execvp");
    return -1;
  }

  // parent
  else{ 
    // close pipes
    if(*pass_pipefd != STDIN_FILENO){
      if(close(*pass_pipefd) == -1){ // close read end of previous pipe
        perror("close");
        return -1;
      }
      *pass_pipefd = STDIN_FILENO; // set to STDIN_FILENO
    }
    if(cmd->controlop == CMD_PIPE){
      if(close(pipefd[1]) == -1){ // close write end of this pipe
        perror("close");
        return -1;
      }
      *pass_pipefd = pipefd[0]; // set pass_pipefd to read end of this pipe
    }

    if(cmd->subshell)
      return cpid;
    
    // built-in commands in the parent process
    // cd
    if(strcmp(cmd->argv[0], "cd") == 0){ 
      if(cd_exec(cmd, false) == -1) // syscall error
        return -1;
    }
    // exit
    if(strcmp(cmd->argv[0], "exit") == 0){
      exit_exec(cmd, false);
    }
    // our_pwd
    if(strcmp(cmd->argv[0], "our_pwd") == 0){
      if(our_pwd_exec(cmd, false) == -1)
        return -1;
    } 

    // help
    if(strcmp(cmd->argv[0], "help") == 0){
      help_exec(cmd);
    }

    return cpid;
  }
}


/**
 * Executes a command line.
 * 
 * @cmdline: The command line to execute.
 * @return the exit status of the last command executed, or -1 if an error occurs.
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
    pid_t cmd_pid;	 
    switch (cmdlist->controlop)
    {
      case CMD_END:
      case CMD_SEMICOLON:
        cmd_pid = cmd_exec(cmdlist, &pipefd);
        waitpid(cmd_pid, &wp_status, 0);
        cmd_status = WEXITSTATUS(wp_status);
        break;
      
      case CMD_AND:
        cmd_pid = cmd_exec(cmdlist, &pipefd);
        waitpid(cmd_pid, &wp_status, 0);
        cmd_status = WEXITSTATUS(wp_status);
        if(cmd_status != 0) // if the last command exited with non-zero status
          goto done;
        break;
      case CMD_OR:
        cmd_pid = cmd_exec(cmdlist, &pipefd);
        waitpid(cmd_pid, &wp_status, 0);
        cmd_status = WEXITSTATUS(wp_status);
        if(WEXITSTATUS(wp_status) == 0)
          goto done;
        break;
        
      case CMD_BACKGROUND:
      case CMD_PIPE:
        cmd_exec(cmdlist, &pipefd);
        cmd_status = 0;
        break;
      
      default:
        break;
    }
    
		cmdlist = cmdlist->next;
	}

        while (waitpid(0, 0, WNOHANG) > 0);  // reap any zombies

done:
	return cmd_status;
}


/**
 * Redirect I/O for a command.
 * 
 * @param cmd The command to redirect I/O for.
 */
void redirect_io(command_t *cmd){
  int fd;
  for (int i = 0; i <= 2; i++){
    if (cmd->redirect_filename[i]){
      if(i == 0)
        fd = open(cmd->redirect_filename[i], O_RDONLY);
      else
        fd = open(cmd->redirect_filename[i], O_CREAT | O_TRUNC |  O_WRONLY, 0666); //0666 is default, anyways!
      if(fd == -1){
        perror("open");
        abort();
      }
      if(dup2(fd,i) == -1){ // duplicate file descriptor
        perror("dup2");
        abort();
      }
      if(close(fd) == -1){
        perror("close");
        abort();
      }
    }
  }
}
/**
 * Determine if a string contains non-numeric characters.
 * 
 * @param str The string to check.
 * @return 1 if the string contains non-numeric characters, 0 otherwise.
 */
/**
  * cd_exec - Execute a cd.
  * @cmd: The command to execute.
  * @verbose: If true, print error messages.
  *
  * @return 0 on success, 1 on syntax error, -1 on syscall error.
*/
int cd_exec(command_t *cmd, bool verbose){ 
    if(!cmd->argv[1] || cmd->argv[2]){
      if(verbose){
        char buf[100];
        sprintf(buf, "cd: Syntax error! Wrong number of arguments! \n");
        write(STDERR_FILENO, buf, strlen(buf));
      }
      return 1;
    }
    // replace ~ and $HOME with getenv("HOME")
    if(cmd->argv[1][0] == '~'){
      char *home = getenv("HOME");
      if(!home){
        if(verbose){
          char buf[100];
          sprintf(buf, "cd: HOME not set! \n");
          write(STDERR_FILENO, buf, strlen(buf));
        }
        return 1;
      }
      char *new = malloc(strlen(home) + strlen(cmd->argv[1]) + 1);  
      strcpy(new, home);
      strcat(new, cmd->argv[1] + 1); // +1 to skip the ~
      strcpy(cmd->argv[1], new);
      free(new);
    }
    // replace $HOME with getenv("HOME")
    char *dollar_home = "$HOME";
    if(strncmp(cmd->argv[1], dollar_home, strlen(dollar_home)) == 0){
      char *home = getenv("HOME");
      if(!home){
        if(verbose){
          char buf[100];
          sprintf(buf, "cd: HOME not set! \n");
          write(STDERR_FILENO, buf, strlen(buf));
        }
        return 1;
      }
      char *new = malloc(strlen(home) + strlen(cmd->argv[1]) + 1);  
      strcpy(new, home);
      strcat(new, cmd->argv[1] + strlen(dollar_home)); // +strlen(dollar_home) to skip the $HOME
      strcpy(cmd->argv[1], new);
      free(new);
    }

    // execute cd
    if(chdir(cmd->argv[1]) == -1){
      if(verbose)
        perror("cd");
      return -1;
    }
    return 0;
}
int containsNonNumeric(const char *str){
  size_t i;
  for(i = 0; i < strlen(str); i++){
    if(!isdigit(str[i]))
      return 1;
  }
  return 0;
}
/**
  * exit_exec - Execute an exit.
  * @cmd: The command to execute.
  * @verbose: If true, print error messages.
  *
  * Returns 1 on syntax error, -1 on syscall error and exits on success.
*/
int exit_exec(command_t *cmd, bool verbose){
  if(!cmd->argv[1]){
    exit(0);
  }
  else if(cmd->argv[2]){
    if(verbose){
      char buf[100];
      sprintf(buf, "exit: Syntax error! Wrong number of arguments! \n");
      write(STDERR_FILENO, buf, strlen(buf));
    }
    return 1;
  }
  else{
    if(containsNonNumeric(cmd->argv[1]))
      exit(2);
    else  
      exit(atoi(cmd->argv[1]));
  }
}
/**
  * our_pwd_exec - Execute our_pwd.
  * @cmd: The command to execute.
  * @verbose: If true, print error messages.
  *
  * Returns 0 on success, 1 on syntax error, -1 on syscall error.
*/
int our_pwd_exec(command_t *cmd, bool verbose){
  if(cmd->argv[1]){
    if(verbose){
      char buf[100];
      sprintf(buf, "pwd: Syntax error! Wrong number of arguments! \n");
      write(STDERR_FILENO, buf, strlen(buf));
    }
    return 1;
  }
  char buf[PATH_MAX];
  if(!getcwd(buf, PATH_MAX)){
    if(verbose)
      perror("our_pwd");
    return -1;
  }
  // append newline to buf
  if(verbose){
    strcat(buf, "\n");
    write(STDOUT_FILENO, buf, strlen(buf));
  }
  return 0;
}

/**
  * help_exec - Execute help.
  * @cmd: The command to execute.
  *
  * Returns 0 on success, 1 on syntax error, -1 on syscall error.
*/
int help_exec(command_t *cmd){
  // // TODO: use flags to determine which help message to print
  // if(cmd->argv[1]){
  //   char buf[100];
  //   sprintf(buf, "help: Syntax error! Wrong number of arguments! \n");
  //   write(STDERR_FILENO, buf, strlen(buf));
  //   return 1;
  // }
  char buf[1000];
  // TODO: Add help message for each command
  sprintf(buf, "Tux shell - a simple shell. \nThese shell commands are defined internally. Type 'help' to see this list. \n");
  write(STDOUT_FILENO, buf, strlen(buf));
  sprintf(buf, "help - print this help message. \n");
  write(STDOUT_FILENO, buf, strlen(buf));
  sprintf(buf, "cd [dir] - change directory to dir. If dir is not specified, change to $HOME. \n");
  write(STDOUT_FILENO, buf, strlen(buf));
  sprintf(buf, "exit [n] - exit the shell. If n is not specified, exit with status 0. \n");
  write(STDOUT_FILENO, buf, strlen(buf));
  sprintf(buf, "our_pwd - print the current working directory. \n");
  write(STDOUT_FILENO, buf, strlen(buf));
  return 0;
}

