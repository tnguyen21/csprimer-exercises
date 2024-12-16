#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_ITEMS (1 << 12)
typedef int item;

struct RingBuffer {
    item buffer[NUM_ITEMS];
    int capacity;
    int size;  // number of current items (<= capacity)
    int wp;  // locations at which to write
    int rp;  // ... and read
    pthread_mutex_t lock;
    pthread_cond_t readable;
    pthread_cond_t writeable;
};

struct RingBuffer* rb_init(int capacity) {
    struct RingBuffer *rb = malloc(sizeof(struct RingBuffer));
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
    return rb;
}

void rb_free(struct RingBuffer *rb) {
    free(rb->buffer);
    free(rb);
    pthread_mutex_destroy(&rb->lock);
    pthread_cond_destroy(&rb->readable);
    pthread_cond_destroy(&rb->writeable);
}

void rb_add(struct RingBuffer*rb, item item) {
    pthread_mutex_lock(&rb->lock);
    while (rb->size == rb->capacity) {
        pthread_cond_wait(&rb->writeable, &rb->lock);
    }
    rb->buffer[rb->wp] = item;
    rb->wp = (rb->wp + 1) % rb->capacity;
    rb->size++;
    pthread_cond_signal(&rb->readable);
    pthread_mutex_unlock(&rb->lock);
}

item rb_get(struct RingBuffer*rb) {
    pthread_mutex_lock(&rb->lock);
    while (rb->size == 0) {
        pthread_cond_wait(&rb->readable, &rb->lock);
    }
    item item = rb->buffer[rb->rp];
    rb->rp = (rb->rp + 1) % rb->capacity;
    rb->size--;
    pthread_cond_signal(&rb->writeable);
    pthread_mutex_unlock(&rb->lock);
    return item;
}
