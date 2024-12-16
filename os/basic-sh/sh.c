#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>


#define BUFFER_SIZE 1024
#define MAX_ARGV 256
#define PROMPT "> "

volatile pid_t childpid = 0;

void sigint_handler(int sig) {
  if (!childpid) return;
  if (kill(childpid, SIGINT) < 0) perror("error sending SIGINT to child\n");

  return;
}

int main() {
  char buffer[BUFFER_SIZE];
  int argc;
  char *argv[MAX_ARGV];
  const char *delimiters = " \t\n";

  // handle signals
  signal(SIGINT, sigint_handler);

  while (1) {
    printf(PROMPT);
    if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) break; // Exit on EOF or error

    buffer[strcspn(buffer, "\n")] = '\0';

    // tokenize
    argc = 0;
    argv[argc] = strtok(buffer, delimiters);

    while (argv[argc] != NULL) {
      argv[++argc] = strtok(NULL, delimiters);
    }

    printf("%d args:\n", argc);
    for (int i=0; i<argc; i++) {
      printf("%s ", argv[i]);
    }
    printf("\n");

    // eval
    if (argc == 0) continue;
    if (0 == strcmp(argv[0], "quit")) exit(0);

    if ((childpid = fork()) < 0) {
      perror("fork error\n");
      exit(1);
    }
    if (childpid == 0) {
      // in child proc
      if (execvp(argv[0], argv) < 0) {
        perror("execvp error\n");
        exit(1);
      }
      // should never get here
      exit(1);
    }

    // in parent proc
    int status;
    waitpid(childpid, &status, 0);
    childpid = 0;
  }

  return EXIT_SUCCESS;
}
