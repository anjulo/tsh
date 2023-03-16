# Tux Shell (tsh)

Tux Shell (tsh) is a simplistic shell that allows users to execute commands in a Unix-like environment. It provides a basic command-line interface where users can enter commands to interact with the operating system.

## Features

- Basic command-line interface
- Supports execution of Unix-like commands
- Simple to use and lightweight

## Getting Started

### Prerequisites

To use Tux Shell, you need to have a Unix-like operating system installed on your machine.

### Installation

1. Clone the Tux Shell repository from GitHub:
  
      ```bash
      git clone git@github.com:anjulo/tsh.git
      ```
2. Navigate to the Tux Shell directory:
  
      ```bash
      cd tsh
      ```
3. Compile the source code:
  
      ```bash
      make
      ```
4. Run the executable:
  
      ```bash
      ./tsh
      ```
5. Enter commands to interact with the operating system (e.g. `ls`)
6. Exit the Tux Shell by entering `exit`

## Usage
The tux shell provides a command-line interface that allows users to interact with their operating system. It provides a limited set of built-in commands and can execute external programs. It also supports input and output redirection, multiple commands, and background execution, subshell execution, and piping.

### Built-in Commands
The built-in commands supported by the tux shell can be listed by entering `help` at the command prompt. Some of the supported built-in commands are listed below.
command | description
--- | ---
`help` | Displays a list of built-in commands
`cd` | Changes the current working directory
`our_pwd` | Displays the current working directory
`exit` | Exits the tux shell

### External Commands
The tux shell can execute external programs. To execute an external program, enter the name of the program at the command prompt. For example, to execute the `ls` command, enter `ls` at the command prompt. The tux shell will execute the `ls` command and display the output. If the program requires any arguments, they can be passed to the program by entering them after the program name. For example, to execute the `ls` command with the `-l` argument, enter `ls -l` at the command prompt. The tux shell will execute the `ls` command with the `-l` argument and display the output. 

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

## License

