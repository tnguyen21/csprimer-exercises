// implicit free list implementation
// TODO external free list
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/mman.h>

struct header {
  unsigned int size;
  int free;
};

void *heap_start = NULL;
void *heap_end;

// TODO ask for more memory from mmap if heap grows >1MB
// TODO split blocks; large free blocks should be split if small malloc is made
// TODO coallesce adjacent free blocks
void* myalloc(int size) {
  struct header *h;

  if (heap_start == NULL) {
    heap_start = mmap(NULL, 1<<20, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    heap_end = heap_start;
  }

  void *p = heap_start;
  while (p < heap_end) {
    h = (struct header*)p;
    if (h->free && h->size >= size) {
      h->free = 0;
      return (void*)(h+1);
    }
    p += sizeof(struct header) + h->size;
  }

  h = (struct header*)heap_end;
  h->size = size;
  h->free = 0;
  heap_end = (void*)(h + 1) + size; // TODO dont extend beyond heap

  return (void*)(h+1);
}


void myfree(void *ptr) {
  if (!ptr) return;

  struct header *h = (struct header*)ptr - 1;
  h->free = 1;
}

int main() {
  int *p = (int*)myalloc(4);
  char *q = (char*)myalloc(1000);

  *p = 42;
  *q = 'A';

  assert (*p == 42);
  assert (*q == 'A');
  myfree(p);

  int *r = (int*)myalloc(4);
  *r = 15;

  assert (*r == 15);
  assert (p == r);
  
  printf("%p, %p, %p\nok\n", p, q, r);
  return 0;
}
