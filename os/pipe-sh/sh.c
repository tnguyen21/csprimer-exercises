#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>


#define BUFFER_SIZE 1024
#define MAX_ARGV 256
#define MAX_CMDS 8
#define PROMPT "> "

// TODO
volatile pid_t childpid = 0;

void sigint_handler(int sig) {
  if (!childpid) return;
  if (kill(childpid, SIGINT) < 0) perror("error sending SIGINT to child\n");

  return;
}

int main() {
  char buffer[BUFFER_SIZE];
  int argi, cmdi;
  char *cmds[MAX_CMDS][MAX_ARGV];
  char *tok;
  const char *delimiters = " \t\n";

  // handle signals
  signal(SIGINT, sigint_handler);

  while (1) {
    printf(PROMPT);
    if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) break; // Exit on EOF or error

    buffer[strcspn(buffer, "\n")] = '\0';

    // tokenize
    argi = 0;
    cmdi = 0;
    tok = strtok(buffer, delimiters);
    while (1) {
      cmds[cmdi][argi++] = tok;
      if (tok == NULL) break;
      tok = strtok(NULL, delimiters);

      if (tok && strcmp(tok, "|") == 0) {
        cmds[cmdi++][argi] = NULL;
        argi = 0;
        tok = strtok(NULL, delimiters);
      }
    }

    // eval
    if (cmds[0][0] == 0) continue;
    if (0 == strcmp(cmds[0][0], "quit")) exit(0);


    // TODO
    // clean up dup2 and close; open pipes
    // clean up after failed fork/exec
    // guard for more cmds than > MAX_CMDS
    int fds[2];
    int infd = 0;
    int childpids[cmdi + 1];
    for (int i = 0; i <= cmdi; i++) {
      if (i != cmdi) pipe(fds);

      if ((childpids[i] = fork()) < 0) {
        perror("fork error\n");
        exit(1);
      }
      if (childpids[i] == 0) { // in child proc
        if (i != cmdi) {
          dup2(fds[1], 1);
          close(fds[1]);
          close(fds[0]);
        }
        dup2(infd, 0);

        if (execvp(cmds[i][0], cmds[i]) < 0) {
          perror("execvp error\n");
          exit(1);
        }
        // should never get here
        exit(1);
      }
      // in parent
      if (i != cmdi) {
        if (infd != 0)
          close(infd);
        close(fds[1]);
        infd = fds[0];
      }
    }

    // in parent proc
    if (infd != 0)
      close(infd);

    int status;
    for (int i = 0; i <= cmdi; i++) {
      waitpid(childpids[i], &status, 0);
    }
  }
  return EXIT_SUCCESS;
}
