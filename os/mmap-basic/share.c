#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>


int main () {
  int stat;
  srand(time(NULL));

  int *np = (int*)mmap(NULL, sizeof(int), PROT_WRITE | PROT_READ, MAP_ANONYMOUS | MAP_SHARED, -1, 0);

  if (fork() == 0) {
    // as the child, write a random number to shared memory (TODO!)
    *np = rand();
    printf("Child has written %d to address %p\n", *np, np);
    exit(0);
  } else {
    // as the parent, wait for the child and read out its number
    wait(&stat);
    printf("Parent reads %d from address %p\n", *np, np);
  }
}
