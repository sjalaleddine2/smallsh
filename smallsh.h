#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/types.h>
#include <signal.h>
#include <stdint.h>
#include <sys/wait.h>
#include <fcntl.h>

char *str_gsub(char *restrict *restrict haystack, char const *restrict needle, char const *restrict sub);
void free_mem(char **array, int count_words, char* input, char* output);
void manage_background(pid_t pidchildBG, int pidbgStatus);
void expand_words(char *words[513], int* exitFG, int* pidBG, int count_words);
void parse_words(char *words[513], bool* isBackground, char *input_file, char *output_file, int count_words, int i);