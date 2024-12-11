#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>

#define SLEEP_SEC 3
#define NUM_MULS 100000000
#define NUM_MALLOCS 100000
#define MALLOC_SIZE 1000

#define TOTAL_USEC(tv) (tv).tv_sec * 1000000 + (tv).tv_usec
#define TOTAL_NSEC(tv) (tv).tv_sec * 1000000000 + (tv).tv_nsec

struct profile_times {
  uint64_t real_ns;
  uint64_t user_ms;
  uint64_t sys_ms;
};

void profile_start(struct profile_times *t) {
  struct timespec ts;
  struct rusage usage;
  // get real time
  clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
  t->real_ns = TOTAL_NSEC(ts);

  // get user and sys time
  getrusage(RUSAGE_SELF, &usage);
  t->user_ms = TOTAL_USEC(usage.ru_utime);
  t->sys_ms = TOTAL_USEC(usage.ru_stime);
}

// TODO given starting information, compute and log differences to now
void profile_log(struct profile_times *t) {
  struct timespec ts;
  struct rusage usage;

  clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
  getrusage(RUSAGE_SELF, &usage);

  uint64_t diff_real = TOTAL_NSEC(ts) - t->real_ns;
  uint64_t diff_user = TOTAL_USEC(usage.ru_utime) - t->user_ms;
  uint64_t diff_sys = TOTAL_USEC(usage.ru_stime) - t->sys_ms;

  printf("[pid %d]\n", getpid());
  printf("real: %0.03f user: %0.03f sys %0.03f\n",
    diff_real / 1000000000.0,
    diff_user / 1000000.0,
    diff_sys / 1000000.0
  );
}

int main(int argc, char *argv[]) {
  struct profile_times t;
  struct rusage usage;

  float x = 1.0;
  profile_start(&t);
  for (int i = 0; i < NUM_MULS; i++)
    x *= 1.1;
  profile_log(&t);

  profile_start(&t);
  void *p;
  for (int i = 0; i < NUM_MALLOCS; i++)
    p = malloc(MALLOC_SIZE);
  profile_log(&t);

  profile_start(&t);
  sleep(SLEEP_SEC);
  profile_log(&t);

  printf("ok\n");
}
