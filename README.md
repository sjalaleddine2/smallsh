# Smallsh

## Shell written in C, that implements a command interface similar to well-known shells such as bash

The program performs the following actions:
- Print an interactive input prompt
- Parse command line input into semantic tokens
- Implement parameter expansion
- Shell special parameters $$, $?, and $!
- Tilde (~) expansion
- Implement two shell built-in commands: exit and cd
- Execute non-built-in commands using the the appropriate EXEC(3) function.
  - Implement redirection operators ‘<’ and ‘>’
  - Implement the ‘&’ operator to run commands in the background
- Implement custom behavior for SIGINT and SIGTSTP signals

## Instructions to compile
- Download makefile and type make in command interface
