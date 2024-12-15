#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef int job;

struct JobQueue {
    job *buffer;
    int capacity;
    int size;  // number of current items (<= capacity)
    int wp;  // locations at which to write
    int rp;  // ... and read
    pthread_mutex_t lock;
    pthread_cond_t readable;
    pthread_cond_t writeable;
};

struct JobQueue* JQ_init(int capacity) {
    struct JobQueue *jq = malloc(sizeof(struct JobQueue));
    jq->buffer = malloc(capacity * sizeof(job));
    jq->capacity = capacity;
    jq->size = 0;
    jq->wp = 0;
    jq->rp = 0;
    pthread_mutex_init(&jq->lock, NULL);
    pthread_cond_init(&jq->readable, NULL);
    pthread_cond_init(&jq->writeable, NULL);
    return jq;
}

void JQ_free(struct JobQueue *jq) {
    free(jq->buffer);
    free(jq);
    pthread_mutex_destroy(&jq->lock);
    pthread_cond_destroy(&jq->readable);
    pthread_cond_destroy(&jq->writeable);
}

void JQ_add(struct JobQueue*jq, job item) {
    pthread_mutex_lock(&jq->lock);
    while (jq->size == jq->capacity) {
        pthread_cond_wait(&jq->writeable, &jq->lock);
    }
    jq->buffer[jq->wp] = item;
    printf("%2d -> [%d]        %p\n", item, jq->wp, pthread_self());
    jq->wp = (jq->wp + 1) % jq->capacity;
    jq->size++;
    pthread_cond_signal(&jq->readable);
    pthread_mutex_unlock(&jq->lock);
}

job JQ_get(struct JobQueue*jq) {
    pthread_mutex_lock(&jq->lock);
    while (jq->size == 0) {
        pthread_cond_wait(&jq->readable, &jq->lock);
    }
    job item = jq->buffer[jq->rp];
    printf("      [%d] -> %2d  %p\n", jq->rp, item, pthread_self());
    jq->rp = (jq->rp + 1) % jq->capacity;
    jq->size--;
    pthread_cond_signal(&jq->writeable);
    pthread_mutex_unlock(&jq->lock);
    return item;
}

void *producer(void *arg) {
  struct JobQueue *jq = (struct JobQueue *)arg;
  int i = 0;
  while (1)
    JQ_add(jq, i++ % 100);
}

void *consumer(void *arg) {
  struct JobQueue *jq = (struct JobQueue *)arg;
  while (1)
      JQ_get(jq);
}

int main () {
  printf("Starting basic test\n");
  struct JobQueue *jq = JQ_init(8);
  int i;
  for (i = 0; i < 5; i++) { JQ_add(jq, i); }
  for (i = 0; i < 5; i++) { JQ_get(jq); }
  for (i = 0; i < 5; i++) { JQ_add(jq, i); }
  for (i = 0; i < 5; i++) { JQ_get(jq); }
  JQ_free(jq);
  printf("Starting concurrent test\n");
  // start n producers, m consumers in threads
  // producer just write incrementing integers to jq indefinitely
  // consumers just read/print them
  int n = 2;
  int m = 3;
  pthread_t prod[n], cons[m];
  jq = JQ_init(4);
  for (i = 0; i < n; i++)
    pthread_create(&prod[i], NULL, producer, jq);
  for (i = 0; i < m; i++)
    pthread_create(&cons[i], NULL, consumer, jq);
  for (i = 0; i < n; i++)
    pthread_join(prod[i], NULL);
  for (i = 0; i < m; i++)
    pthread_join(cons[i], NULL);
  JQ_free(jq);
  printf("OK\n");
}
