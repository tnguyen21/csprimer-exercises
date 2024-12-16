#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "ringbuffer.h"

#define SIZE (1 << 20) // Test with this many bytes of data

int main () {
  int checksum, max, n, fd;
  void *p;
  srand(0x1234);
  max = SIZE / sizeof(int);

  // create shm object
  if ((fd = shm_open("/stream", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) == -1) {
    perror("shm_open");
    exit(-1);
  }
  ftruncate(fd, sizeof(struct RingBuffer));

  // mmap shm obj
  if ((p = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
    perror("mmap");
    exit(-1);
  }

  struct RingBuffer *rb = (struct RingBuffer *)p;
  rb->capacity = NUM_ITEMS;
  rb->size = 0;
  rb->wp = 0;
  rb->rp = 0;

  pthread_mutexattr_t mattr;
  pthread_condattr_t cattr;
  pthread_mutexattr_init(&mattr);  
  pthread_condattr_init(&cattr);  
  pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
  pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED);

  pthread_mutex_init(&rb->lock, &mattr);
  pthread_cond_init(&rb->readable, &cattr);
  pthread_cond_init(&rb->writeable, &cattr);

  // stream it a bunch of random integers
  checksum = 0;
  for (int i = 0; i < max; i++) {
    n = rand();
    checksum ^= n;
    rb_add(rb, n);
  }
  printf("Wrote %d random ints to client, checksum %d\n", max, checksum);

  for (;;) {};
}
