#include "smallsh.h"

void does_nothing(int signo) {
}

void manage_background(pid_t pidchildBG, int pidbgStatus) {
  while ((pidchildBG = waitpid(0, &pidbgStatus, WNOHANG | WUNTRACED)) > 0) {
    if (WIFEXITED(pidbgStatus)) {
      fprintf(stderr,"Child process %jd done. Exit status %d.\n",(intmax_t) pidchildBG,WEXITSTATUS(pidbgStatus));
    }
    else if (WIFSIGNALED(pidbgStatus)) {
      fprintf(stderr,"Child process %jd done. Signaled %d.\n",(intmax_t) pidchildBG, WTERMSIG(pidbgStatus));
    }
    else if (WIFSTOPPED(pidbgStatus)) {
      kill(pidchildBG,SIGCONT);
      fprintf(stderr,"Child process %jd stopped. Continuing.\n",(intmax_t) pidchildBG);
    }
  }
}

void expand_words(char *words[513], int* exitFG, int* pidBG, int count_words) {
  char *shellProcess = NULL;
  asprintf(&shellProcess, "%d", getpid());

  char *exitFGstr = NULL;
  char *pidBGstr = NULL;

  if (exitFG == 0) {
    asprintf(&exitFGstr, "0");
  }
  else {
    asprintf(&exitFGstr, "%jd", (intmax_t) *exitFG);
  }

  if (pidBG == 0) {
    asprintf(&pidBGstr, "");
  }
  else {
    asprintf(&pidBGstr, "%jd", (intmax_t) *pidBG);
  }

  for (int i = 0; i < count_words; ++i) 
  {
    str_gsub(&words[i], "$$", shellProcess);
    str_gsub(&words[i], "$?", exitFGstr);
    str_gsub(&words[i], "$!", pidBGstr);
    if (strncmp(words[i], "~/", 2) == 0) {
      str_gsub(&words[i], "~", getenv("HOME"));
    } 
  }

  free(shellProcess);
  free(exitFGstr);
  free(pidBGstr);

}

void parse_words(char *words[513], bool* isBackground, char *input_file, char *output_file, int count_words, int i) {
  if (words[i] != NULL && strcmp(words[i], "#") == 0) {
    free(words[i]);
    words[i] = NULL;
  }
  
  i -= 1;

  if (i >= 0 && strcmp(words[i], "&") == 0) {
    *isBackground = true;
    free(words[i]);
    words[i] = NULL;
    i -= 1;
  }
  
  if (i >= 1 && (strcmp(words[i-1], "<") == 0 || strcmp(words[i-1],">") == 0)) {
    if (strcmp(words[i-1], "<") == 0) {
      input_file = strdup(words[i]);
      free(words[i]);
      free(words[i-1]);
      words[i] = NULL;
      words[i-1] = NULL;
      i -= 2;
    }

    else {
      output_file = strdup(words[i]);
      free(words[i]);
      free(words[i-1]);
      words[i] = NULL;
      words[i-1] = NULL;
      i -= 2;
    }
  }

  if (i >= 1 && (strcmp(words[i-1], "<") == 0 || strcmp(words[i-1],">") == 0)) {
    if (strcmp(words[i-1], "<") == 0) {
      input_file = strdup(words[i]);
      free(words[i]);
      free(words[i-1]);
      words[i] = NULL;
      words[i-1] = NULL;
      i -= 2;
    }

    else {
      output_file = strdup(words[i]);
      free(words[i]);
      free(words[i-1]);
      words[i] = NULL;
      words[i-1] = NULL;
      i -= 2;
    }
  }
}

void free_mem(char **array, int count_words, char* input, char* output)
{
    for (int i = 0; i < count_words; ++i) 
    {
      free(array[i]);
      array[i] = NULL;
      free(input);
      input = NULL;
      free(output);
      output = NULL;
    }
}


char *str_gsub(char *restrict *restrict haystack, char const *restrict needle, char const *restrict sub) 
{
  char *str = *haystack;
  size_t haystack_len = strlen(str);
  size_t const needle_len = strlen(needle),
                sub_len = strlen(sub);

  for (; (str = strstr(str, needle)) ;) {
      ptrdiff_t off = str - *haystack;
      if (sub_len > needle_len) {
        str = realloc(*haystack, sizeof **haystack * (haystack_len + sub_len - needle_len + 1));
        if (!str) goto exit;
        *haystack = str;
        str = *haystack + off;
      }
      memmove(str + sub_len, str + needle_len, haystack_len + 1 - off - needle_len);
      memcpy(str, sub, sub_len);
      haystack_len = haystack_len + sub_len - needle_len;
      str += sub_len;
      if (strcmp(needle, "~") == 0) goto exit;
  }
  str = *haystack;
  if (sub_len < needle_len) {
    str = realloc(*haystack, sizeof **haystack * (haystack_len + 1));
    if (!str) goto exit;
    *haystack = str;
  }
exit:
  return str;
}

int main(int argc, char *argv[])
{
  struct sigaction ignore_action = {0}, nothing_action = {0}; 
  struct sigaction old_action1 = {0}, old_action2 = {0};
  ignore_action.sa_handler = SIG_IGN;
  nothing_action.sa_handler = does_nothing;
  sigaction(SIGTSTP, &ignore_action, &old_action1);
  sigaction(SIGINT, &ignore_action, &old_action2);
  char* pPS1 = NULL;
  char* pIFS = NULL;
  char *line = NULL;
  pid_t exitFG = 0;
  pid_t pidBG = 0;
  pid_t pidchildBG = 0;
  int pidbgStatus;
  size_t n = 0;

  /*Get environment variables*/
  pPS1 = getenv("PS1");
  pIFS = getenv("IFS");

  if (pPS1 == NULL) {
    pPS1 = "";
  }

  if (pIFS == NULL) {
    pIFS = " \t\n";
  }

  /*Refresh command line*/
  for (;;) {
   input:

    /*Manage background processes, prints out if background process was stopped/exitted*/
    manage_background(pidchildBG, pidbgStatus);

    /*Reset signals and retrieve user input*/
    fprintf(stderr, "%s", pPS1);
    sigaction(SIGINT, &nothing_action, NULL);
    getline(&line, &n, stdin);
    sigaction(SIGINT, &ignore_action, NULL);
    if (feof(stdin)) {
      clearerr(stdin);
      fprintf(stderr, "\nexit\n");
      exit(exitFG);
    }

    char *words[513] = {NULL};
    char *cur_word = NULL;
    int  count_words = 0;
    bool isBackground = false;
    char* input_file = NULL;
    char* output_file = NULL;
    cur_word = strtok(line, pIFS);
   
    /*Split words*/
    if (cur_word != NULL) {
      words[0] = strdup(cur_word);
      count_words += 1;

      for (int i = 1;;++i) 
      {
        cur_word = strtok(NULL, pIFS);
        if (cur_word == NULL) {
          break;
        }
        words[i] = strdup(cur_word);
        count_words += 1;
      }
    }

    /*Expand tokens $$, $?, $! int process ID, exit status of last FG command, and pid of most recent BG process*/
    expand_words(words, &exitFG, &pidBG, count_words);

    /*Parse words into tokens (background, input/outfile files) if user inputs more than 1 word, otherwise return to beginning*/
    int i = 0;
    while (words[i] != NULL && strcmp(words[i], "#") != 0) 
    {
      i += 1;
    }

    if (count_words > 0) {
      parse_words(words, &isBackground, input_file, output_file, count_words, i);
    }

    else {
      free_mem(words, count_words, input_file, output_file);
      goto input;
    }


    pid_t exit_arg = 0;
    int   chdir_exit = 0;

    /*Built in commands exit and cd*/
    if (strcmp(words[0], "exit") == 0) {
      if (words[2] != NULL) {
        fprintf(stderr, "Invalid number of arguments\n");
        free_mem(words, count_words, input_file, output_file);
        goto input;
      }

      if (count_words == 1) {
        exit_arg = exitFG;
      }
      
      else {
        for (int i = 0; i < strlen(words[1]); ++i)
        {
          if (isdigit(words[1][i]) == 0) {
            fprintf(stderr, "Argument not an integer\n");
            free_mem(words, count_words, input_file, output_file);
            goto input;
          }
        }
        exit_arg = atoi(words[1]);
      }

      fprintf(stderr, "\nexit\n");
      kill(0, SIGINT);
      free_mem(words, count_words, input_file, output_file);
      exit(exit_arg);

    }

    else if (strcmp(words[0], "cd") == 0) {
      if (count_words >= 3) {
        fprintf(stderr, "Invalid number of arguments");
        free_mem(words, count_words, input_file, output_file);
        goto input;
      }

      else if (count_words == 1) {
        chdir_exit = chdir(getenv("HOME"));
        if (chdir_exit != 0) {
          fprintf(stderr, "Failure to change directory");
          free_mem(words, count_words, input_file, output_file);
          goto input;
        }
      }

      else {
        chdir_exit = chdir(words[1]);
        if (chdir_exit != 0) {
          fprintf(stderr, "Failure to change directory");
          free_mem(words, count_words, input_file, output_file);
          goto input;
        }
      }
    }

    /*Non-built-in commands, fork new child processes when executing*/
    else {
      int childStatus;
      int status;

      pid_t childPid = fork();
      switch (childPid){
      case -1:
        fprintf(stderr,"Forking has failed");
        free_mem(words, count_words, input_file, output_file);
        exit(1);
        break;
      case 0:
        sigaction(SIGTSTP, &old_action1, NULL);
        sigaction(SIGINT, &old_action2, NULL);
        if (input_file != NULL) {
          int sourceFD = open(input_file, O_RDONLY);
          if (sourceFD == -1) {
            fprintf(stderr,"Failed to open file to read");
            free_mem(words, count_words, input_file, output_file);
            exit(2);
          }

          status = dup2(sourceFD, 0);
          close(sourceFD);
          if (status == -1) {
            fprintf(stderr,"Failed to redirect stdin to input file");
            free_mem(words, count_words, input_file, output_file);
            exit(3);
            }
        }

        if (output_file != NULL) {
          int targetFD = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0777);
          if (targetFD == -1) {
            fprintf(stderr,"Failed to open file to write");
            free_mem(words, count_words, input_file, output_file);
            exit(2);
          }

          status = dup2(targetFD, 1);
          close(targetFD);
          if (status == -1) {
            fprintf(stderr,"Failed to redirect stdout to output file");
            free_mem(words, count_words, input_file, output_file);
            exit(3);
          }
        }
        execvp(words[0], words);
        fprintf(stderr,"Failed to execute");
        free_mem(words, count_words, input_file, output_file);
        exit(4);
        break;
      default:
        if (isBackground == false) {
          childPid = waitpid(childPid, &childStatus, WUNTRACED);
          if (WIFEXITED(childStatus)) {
            exitFG = WEXITSTATUS(childStatus);
          }
          else if (WIFSIGNALED(childStatus)) {
            exitFG = WTERMSIG(childStatus) + 128;
          }
          else if (WIFSTOPPED(childStatus)) {
            kill(childPid, SIGCONT);
            fprintf(stderr, "Child process %jd stopped. Continuing.\n", (intmax_t) childPid);
            pidBG = childPid;
          }
        }
        else {
          pidBG = childPid;
        }
        break;
      }
    }

    free_mem(words, count_words, input_file, output_file);
  }
}

