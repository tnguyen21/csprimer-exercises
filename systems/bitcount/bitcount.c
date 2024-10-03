#include <assert.h>
#include <stdio.h>

int bitcount(int n) {
  int cnt = 0;
  for (int i = sizeof(n) * 8 - 1; i >= 0; i--) {
    if (n & (1 << i)) {
      cnt++;
    }
  }

  return cnt;
}

int main() {
  assert(bitcount(0) == 0);
  assert(bitcount(1) == 1);
  assert(bitcount(3) == 2);
  assert(bitcount(8) == 1);
  // harder case:
  assert(bitcount(0xffffffff) == 32);
  printf("OK\n");
}
