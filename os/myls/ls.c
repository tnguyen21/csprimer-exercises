#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>


int main(int argc, char *argv[]) {
  DIR *dirp;
  struct dirent *dent;

  dirp = opendir(argc > 1 ? argv[1] : ".");

  while((dent = readdir(dirp)) != NULL)
    printf("%24lu %24hu %24s\n", dent->d_ino, dent->d_reclen, dent->d_name);
}
