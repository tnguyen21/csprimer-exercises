#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


//bool ispangram(char *s) {
//  // bool arr[26] = {0};
//  int bs = 0;
//
//  for (;*s != '\0'; s++) {
//    if (*s >= 'A' && *s <= 'Z') {
//      // arr[(int)*s - 'A'] = 1;
//      bs |= 1 << (*s - 'A');
//    } else if (*s >= 'a' && *s <= 'z') {
//      // arr[(int)*s - 'a'] = 1;
//      bs |= 1 << (*s - 'a');
//    }
//  }
//  
//  // for (int i = 0; i < 26; i++) { if (!arr[i]) return false;}  
//  // return true;
//  return bs == 0x03ffffff;
//}

#define MASK 0x07fffffe

bool ispangram(char *s) {
  int bs = 0;
  char c;
  while ((c = *s++) != '\0') {
    if (c < '@') continue; // ignore chars in ascii table that arent relevant
    
    bs |= 1 << (c & 0x1f);
  }

  return (bs & MASK) == MASK;
}

int main() {
  size_t len;
  ssize_t read;
  char *line = NULL;
  while ((read = getline(&line, &len, stdin)) != -1) {
    if (ispangram(line))
      printf("%s", line);
  }

  if (ferror(stdin))
    fprintf(stderr, "Error reading from stdin");

  free(line);
  fprintf(stderr, "ok\n");
}
