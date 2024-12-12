#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/ioctl.h>

volatile sig_atomic_t resized = 0;

void handle(int sig) { resized = 1; }

int main() {
  struct sigaction sa;
  struct winsize ws;

  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_handler = handle;

  if (sigaction(SIGWINCH, &sa, NULL) == -1) { exit(EXIT_FAILURE); }

  for (;;) {
    if (resized) {
      ioctl(0, TIOCGWINSZ, &ws);
      printf("c: %d r: %d\n", ws.ws_col, ws.ws_row);
      resized = 0;
    }
  }
}
