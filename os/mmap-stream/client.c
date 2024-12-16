#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "ringbuffer.h"

#define SIZE (1 << 20) // Test with this many bytes of data

int main () {
  int checksum, max, n, fd;
  struct timespec start, end;
  void *p;  

  max = SIZE / sizeof(int);
  
  // create shm object
  if ((fd = shm_open("/stream", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) == -1) {
    perror("shm_open");
    exit(-1);
  }

  // mmap shm obj
  if ((p = mmap(NULL, sizeof(struct RingBuffer), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
    perror("mmap");
    exit(-1);
  }

  // receive a bunch of data
  clock_gettime(CLOCK_MONOTONIC, &start);
  struct RingBuffer *rb = (struct RingBuffer *)p;
  checksum = 0;
  for (int i = 0; i < max; i++) {
    n = rb_get(rb);
    checksum ^= n;
  }
  clock_gettime(CLOCK_MONOTONIC, &end);

  float secs =
      (float)(end.tv_nsec - start.tv_nsec) / 1e9 + (end.tv_sec - start.tv_sec);
  float mibs = (float)SIZE / secs / (1 << 20);

  printf("Received at %.3f MiB/s. Checksum: %d\n", mibs, checksum);
}
