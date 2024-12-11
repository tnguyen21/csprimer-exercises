#include <unistd.h>
#include <stdio.h>
#include <sys/resource.h>

void rst(int depth, long int btm) {
  if (depth % 10000 == 0) {
    struct rlimit rl;
    printf("[pid %d] frame %d %ld (%p)\n", getpid(), depth, btm - (long)&depth, &depth);
    getrlimit(RLIMIT_STACK, &rl);
    printf("cur: %lu max: %lu\n", rl.rlim_cur, rl.rlim_max);
  }
  rst(depth+1, btm);
}

void fst() {
  int depth = 0;
  rst(depth, (long)&depth);
}

int main(int argc, char *argv[]) {
  struct rlimit rl;
  getrlimit(RLIMIT_STACK, &rl);
  printf("cur: %lu max: %lu\n", rl.rlim_cur, rl.rlim_max);
  rl.rlim_cur = rl.rlim_cur*2;
  setrlimit(RLIMIT_STACK, &rl);
  fst();
}
