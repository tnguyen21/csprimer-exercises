#define _GNU_SOURCE
#include <sched.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <sys/mount.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define STACK_SIZE 65536

struct child_config {
  int argc;
  char **argv;
};

/* Entry point for child after `clone` */
int child(void *arg) {
  struct child_config *config = arg;

  // todo mkdir oldroot if doesnt exist
  if (mount(NULL, "/", NULL, MS_REC | MS_PRIVATE, NULL)) {
    perror("mount /");
    exit(1);
  }
  if (mount("myroot", "/tmp/myroot", NULL, MS_BIND, NULL)) {
    perror("mount myroot");
    exit(1);
  }
  if (syscall(SYS_pivot_root, "/tmp/myroot", "/tmp/myroot/oldroot")) {
    perror("SYS_pivot_root");
    exit(1);
  }

  chdir("/");
  umount2("oldroot", MNT_DETACH);
  // rm oldroot

  if (execvpe(config->argv[0], config->argv, NULL)) {
    fprintf(stderr, "execvpe failed %m.\n");
    return -1;
  }
  return 0;
}


int main(int argc, char**argv) {
  struct child_config config = {0};
  int flags = CLONE_NEWIPC | CLONE_NEWUTS | CLONE_NEWPID; // CLONE_NEWNET breaks on WSL??
  pid_t child_pid = 0;

  // Prepare child configuration
  config.argc = argc - 1;
  config.argv = &argv[1];

  // Allocate stack for child
  char *stack = 0;
  if (!(stack = malloc(STACK_SIZE))) {
    fprintf(stderr, "Malloc failed");
    exit(1);
  }

  mkdir("/sys/fs/cgroup/cg1", 0755);
  FILE *f;
  if ((f = fopen("/sys/fs/cgroup/cg1/cgroup.procs", "w")) == NULL) {
    fprintf(stderr, "failed to fopen cgroup.procs: %m\n");
    exit(1);
  }
  fprintf(f, "%d\n", getpid());
  fclose(f);

  if ((f = fopen("/sys/fs/cgroup/cg1/pids.max", "w")) == NULL) {
    fprintf(stderr, "failed to fopen cgroup.procs: %m\n");
    exit(1);
  }
  fprintf(f, "3");
  fclose(f);

  // Clone parent, enter child code
  if ((child_pid = clone(child, stack + STACK_SIZE, flags | SIGCHLD, &config)) == -1) {
    fprintf(stderr, "Clone failed");
    exit(2);
  }

  wait(NULL);

  rmdir("/sys/fs/cgroup/cg1");

  return 0;
}
