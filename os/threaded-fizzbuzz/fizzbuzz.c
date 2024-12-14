#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

volatile int n = 1, f = 0, b = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;

void *fizz(void *arg) {
  while (1) {
    pthread_mutex_lock(&lock);
    if (n % 3 == 0 && f == 0) {
      f = 1;
      printf("\tfizz");
    } else {
      pthread_cond_wait(&c, &lock);
    }
    pthread_mutex_unlock(&lock);
  }
}

void *buzz(void *arg) {
  while (1) {
    pthread_mutex_lock(&lock);
    if (n % 5 == 0 && b == 0) {
      b = 1;
      printf("\tbuzz");
    } else {
      pthread_cond_wait(&c, &lock);
    }
    pthread_mutex_unlock(&lock);
  }
}

int main () {
  // start two threads, running fizz and buzz respectively
  pthread_t t1, t2;
  pthread_create(&t1, NULL, fizz, NULL);
  pthread_create(&t2, NULL, buzz, NULL);
  // every 100ms, update n randomly from the range [0, 16), indefinitely
  while (1) {
    pthread_mutex_lock(&lock);
    n = rand() & 0xf;
    f = b = 0;
    printf("\n%d:", n);
    pthread_cond_broadcast(&c);
    pthread_mutex_unlock(&lock);
    usleep(100000);
  }
}
