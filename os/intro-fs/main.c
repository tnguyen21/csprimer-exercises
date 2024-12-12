#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
  struct stat fs;
  int fd = open("./foo", O_WRONLY | O_TRUNC | O_CREAT);

  int prior_blocks = -1;
  for (int i=0; i < (1<<20); i++) {
    write(fd, ".", 1);
    fstat(fd, &fs);
    if (fs.st_blocks != prior_blocks) {
      printf("size: %ld, blocks: %ld, on disk: %ld\n", fs.st_size, fs.st_blocks, fs.st_blocks*512);
      prior_blocks = fs.st_blocks;
    }
  }
}
