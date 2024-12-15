#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "cputime.h"

void merge(int *arr, int n, int mid) {
  int left = 0, right = mid, i;
  int *x = malloc(n * sizeof(int));
  // copy the ith item from either the left or right part
  for (i = 0; i < n; i++) {
    if (right == n)
      x[i] = arr[left++];
    else if (left == mid)
      x[i] = arr[right++];
    else if (arr[right] < arr[left])
      x[i] = arr[right++];
    else
      x[i] = arr[left++];
  }
  // transfer from temporary array back to given one
  for (i = 0; i < n; i++)
    arr[i] = x[i];
  free(x);
}

void msort(int *arr, int n) {
  if (n < 2)
    return;
  int mid = n / 2;

  msort(arr, mid);
  msort(arr + mid, n - mid);
  merge(arr, n, mid);
}

struct msortargs {
  int *arr;
  int n;
};

void *start(void *args) {
  int *arr = ((struct msortargs *)args)->arr;
  int n = ((struct msortargs *)args)->n;
  
  msort(arr, n);
  return NULL;
}

void thread_msort(int *arr, int n) {
  /* strategy
  * - create 2 threads
  * - find mid
  * - one thread sorts LHS, other thread sorts RHS
  * - wait for threads to finish, then one final merge at end
  *
  *
  * future exercise -- this strategy can scale to N threads
  * - split arr into len(arr)/N sized chunks
  * - create N threads, and msort len(arr)/N sized chunks in each thread
  * - wait for all threads to finish, then intelligent do an N chunk join of final arr
  */
  pthread_t t1, t2;

  int mid = n / 2;
  struct msortargs args1 = {arr, mid};
  struct msortargs args2 = {arr+mid, n-mid};

  // LHS and RHS
  pthread_create(&t1, NULL, start, (void *)&args1);
  pthread_create(&t2, NULL, start, (void *)&args2);
  
  pthread_join(t1, NULL);
  pthread_join(t1, NULL);

  merge(arr, n, mid);
}

int main () {
  int n = 1 << 24;
  int *arr = malloc(n * sizeof(int));
  // populate array with n many random integers
  srand(1234);
  for (int i = 0; i < n; i++)
    arr[i] = rand();

  fprintf(stderr, "Sorting %d random integers\n", n);

  // actually sort, and time cpu use
  struct profile_times t;
  profile_start(&t);
  thread_msort(arr, n);
  profile_log(&t);

  // assert that the output is sorted
  for (int i = 0; i < n - 1; i++)
    if (arr[i] > arr[i + 1]) {
      printf("error: arr[%d] = %d > arr[%d] = %d", i, arr[i], i + 1,
             arr[i + 1]);
      return 1;
    }
    return 0;
}
