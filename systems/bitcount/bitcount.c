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

int bitcount_v2(unsigned n) {
  int cnt = 0;
  while (n) {
    cnt += n & 0x01;
    n >>= 1;
  }
  return cnt;
}

// deletes right most 1s bit
// x &= (x-1)
//
// x       = 1110
// x - 1   = 1101
// x&(x-1) = 1100
//
// x - 1   = 1011
// x&(x-1) = 1000

int bitcount_v3(unsigned n) {
  int cnt = 0;
  while (n) {
    cnt++;
    n &= (n-1);
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
  
  assert(bitcount_v2(0) == 0);
  assert(bitcount_v2(1) == 1);
  assert(bitcount_v2(3) == 2);
  assert(bitcount_v2(8) == 1);
  // harder case:
  assert(bitcount_v2(0xffffffff) == 32);
  
  assert(bitcount_v3(0) == 0);
  assert(bitcount_v3(1) == 1);
  assert(bitcount_v3(3) == 2);
  assert(bitcount_v3(8) == 1);
  // harder case:
  assert(bitcount_v3(0xffffffff) == 32);
  
  printf("OK\n");
}
