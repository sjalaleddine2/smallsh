# smallsh

## Overview

`smallsh` is a custom shell program implemented in C, designed to emulate fundamental behaviors of standard Unix shells like Bash. It provides a command-line interface where users can execute commands, manage processes, and handle input/output redirection.

## Features

- **Interactive Command Prompt**: Displays a prompt for user input and processes commands accordingly.
- **Command Parsing**: Interprets user commands, supporting arguments and various operators.
- **Parameter Expansion**:
  - `$$`: Expands to the process ID of `smallsh`.
  - `$?`: Expands to the exit status of the last foreground command.
  - `$!`: Expands to the process ID of the most recent background command.
- **Tilde Expansion**: Recognizes the `~` symbol to represent the user's home directory.
- **Built-in Commands**:
  - `cd [directory]`: Changes the current working directory. Defaults to the home directory if no argument is provided.
  - `exit`: Terminates the shell, ensuring all child processes are properly terminated.
- **External Command Execution**: Executes non-built-in commands using appropriate `exec` functions.
  - Supports input (`<`) and output (`>`) redirection.
  - Allows background execution of commands using the `&` operator.
- **Signal Handling**:
  - Custom behavior for `SIGINT` (interrupt signal) and `SIGTSTP` (stop signal) to manage process control effectively.

## Compilation Instructions

To compile `smallsh`, follow these steps:

1. **Clone the Repository**:

   ```bash
   git clone https://github.com/sjalaleddine2/smallsh.git
   cd smallsh
   ```

2. **Compile the Program**:

   Utilize the provided `makefile` by executing:

   ```bash
   make
   ```

   This will generate an executable named `smallsh`.

## Usage

After compilation, start the shell by running:

```bash
./smallsh
```

Once initiated, `smallsh` will display a prompt (`: `) awaiting user input. You can then execute commands as you would in a standard shell.

## Example Commands

- **Change Directory**:

  ```bash
  cd /path/to/directory
  ```

- **List Files**:

  ```bash
  ls -la
  ```

- **Run a Command in the Background**:

  ```bash
  sleep 10 &
  ```

- **Redirect Output to a File**:

  ```bash
  ls > output.txt
  ```

- **Check Status of Last Command**:

  ```bash
  status
  ```

## Signal Handling

- **`SIGINT` (`Ctrl+C`)**: Custom behavior to handle interruption of foreground processes without terminating `smallsh`.
- **`SIGTSTP` (`Ctrl+Z`)**: Toggles between allowing and disallowing background execution of processes.

## Exit the Shell

To exit `smallsh`, simply type:

```bash
exit
```

This command will terminate the shell after ensuring all spawned processes are appropriately handled.

## License

This project is licensed under the MIT License.
