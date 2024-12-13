#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

volatile uint64_t handled = 0;

void handle(int sig) {
  handled |= (1 << sig);
  printf("Caught %d: %s (%d total)\n", sig, sys_siglist[sig],
         __builtin_popcount(handled));
  
  if (sig == SIGINT || sig == SIGTERM || sig == SIGQUIT)
    exit(0);

}

int main(int argc, char* argv[]) {
    // Register all valid signals
    for (int i = 0; i < NSIG; i++) {
        signal(i, handle);
    }

    // sigfpe?
    volatile int zero = 0;
    printf("%d", 1 / zero);

    // spin
    for (;;)
      sleep(1);
}
