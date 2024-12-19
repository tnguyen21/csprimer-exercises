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
#define MAX_JOBS 8
#define PROMPT "> "
#define MAX_PROMPT 1024

#define ST_DEFAULT 0
#define ST_STOPPED 1
#define ST_FG 2
#define ST_BG 3

char* state_names[4] = {"DEFAULT", "STOPPED", "FG", "BG"};

struct job_t {
  pid_t pgid;
  char state;
  char prompt[4096];
};

struct job_t jobs[MAX_JOBS] = {0};

pid_t getfgpg(void) {
  for (int jid = 0; jid < MAX_JOBS; jid++) {
    if (jobs[jid].state == ST_FG) return jobs[jid].pgid;
  }

  return 0;
}

void sigint_handler(int sig) {
  pid_t fgpg = getfgpg();
  if (!fgpg) return;
  if (killpg(fgpg, SIGINT) < 0) perror("error sending SIGINT to child\n");

  return;
}

void sigtstp_handler(int sig) {
  pid_t fgpg = getfgpg();
  if (!fgpg) return;
  if (killpg(fgpg, SIGTSTP) < 0) perror("sigtstp_handler");

  return;
}

void sigchild_handler(int sig) {
  int status, childpid, jid;
  while ((childpid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {
    if  (WIFEXITED(status) || WIFSIGNALED(status)) {
      for (jid = 0; jid <= MAX_JOBS; jid++) {
        if (jobs[jid].pgid == childpid) jobs[jid].state = ST_DEFAULT;
      }
    } else if (WIFSTOPPED(status)) {
      for (jid = 0; jid <= MAX_JOBS; jid++) {
        if (jobs[jid].pgid == childpid) jobs[jid].state = ST_STOPPED;
      }
    }
  }
  return;
}

int main() {
  char buffer[BUFFER_SIZE], prompt_copy[MAX_PROMPT];
  int argi, cmdi;
  char *cmds[MAX_CMDS][MAX_ARGV];
  char *tok;
  const char *delimiters = " \t\n";

  // handle signals
  signal(SIGINT, sigint_handler);
  signal(SIGTSTP, sigtstp_handler);
  signal(SIGCHLD, sigchild_handler);

  while (1) {
    printf(PROMPT);
    if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) break; // Exit on EOF or error

    buffer[strcspn(buffer, "\n")] = '\0';

    // tokenize
    argi = 0;
    cmdi = 0;
    strncpy(prompt_copy, buffer, MAX_PROMPT);
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
    if (0 == strcmp(cmds[0][0], "jobs")) {
      for (int jid = 0; jid < MAX_JOBS; jid++) {
        if (jobs[jid].state == ST_DEFAULT) continue;
        printf("[%d]\t%s\t%s\n", jid, state_names[jobs[jid].state], jobs[jid].prompt);
      }
      continue;
    }
    if (0 == strcmp(cmds[0][0], "bg")) {
      int jid = atoi(cmds[0][1]);
      if (jid < 0 || jid >= MAX_JOBS || jobs[jid].state == ST_DEFAULT) {
        printf("invalid job id: %d\n", jid);
        continue;
      }
      if (killpg(jobs[jid].pgid, SIGCONT) < 0) {
        perror("error sending SIGCONT");
        continue;
      }
      jobs[jid].state = ST_BG;
      continue;
    }
    if (0 == strcmp(cmds[0][0], "fg")) {
      int jid = atoi(cmds[0][1]);
      if (jid < 0 || jid >= MAX_JOBS || jobs[jid].state == ST_DEFAULT) {
        printf("invalid job id: %d\n", jid);
        continue;
      }
      if (killpg(jobs[jid].pgid, SIGCONT) < 0) {
        perror("error sending SIGCONT");
        continue;
      }
      jobs[jid].state = ST_FG;
      while (jobs[jid].state == ST_FG) sleep(1);
      continue;
    }
    // TODO
    // clean up dup2 and close; open pipes
    // clean up after failed fork/exec
    // guard for more cmds than > MAX_CMDS
    int fds[2];
    int infd = 0;
    int childpids[cmdi + 1];
    int jid;
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
      if (i == 0) {
        for (jid = 0; jid <= MAX_JOBS; jid++) {
          if (jobs[jid].state == ST_DEFAULT) {
            jobs[jid].state = ST_FG;
            jobs[jid].pgid = childpids[0];
            strncpy(jobs[jid].prompt, prompt_copy, MAX_PROMPT);
            break;
          }
        }
      }
      setpgid(childpids[i], childpids[0]);
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

    while (jobs[jid].state == ST_FG) sleep(1);

  }
  return EXIT_SUCCESS;
}
