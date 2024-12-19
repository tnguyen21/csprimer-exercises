#include <unistd.h>

int main() {
  int fds[2];
  pipe(fds);

  if (fork() == 0) {
    // child
    dup2(fds[1], 1);
    close(fds[0]);
    close(fds[1]);
    execlp("ls", "ls", NULL);
  }

  // parent
  dup2(fds[0], 0);
  close(fds[0]);
  close(fds[1]);
  execlp("wc", "wc", NULL);
}
