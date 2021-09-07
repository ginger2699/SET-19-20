#ifndef LIB_BASH_H
  #define LIB_BASH_H

#define _DEFAULT_SOURCE
#include <string.h>
#include <unistd.h> /*lib for getcwd*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#define COMMANDSIZE 100
#define ARG_MAX 100

  extern char** argumentTokenizer(char* command);
  extern char** commandTokenizer(char* firstCommand, char* restOfCommands,int* numberOfCommands);
  extern void command_parser(char* input_line);
  extern void command_cd(char* input);
  extern void multiple_commands(char* token, char* commands);
  extern void single_command(char* command);
#endif
