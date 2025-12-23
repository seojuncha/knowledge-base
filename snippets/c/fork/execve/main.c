#define _POSIX_SOURCE
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
 
static int show_maps(pid_t pid, const char *find) {
  FILE *f;
  char fmt[] = "/proc/%ld/maps";
  char path[64];
  char buf[256];
 
  sprintf(path, fmt, pid);
  printf("<< %s >>\n", path);

  f = fopen(path, "r");
  if (!f) {
    perror("fopen");
    return -1;
  }
 
  while (fgets(buf, sizeof(buf), f)) {
    if (!strcmp(find, "all")) {
      fputs(buf, stdout);
    } else {
      if (strstr(buf, find) != NULL) {
        printf("%s\n", buf);
        break;
      }
    }
  }
  return 0;
}
 
static int child_func(void *arg) {
  char *argv[] = {"child", NULL};
  show_maps(getpid(), "all");
  execve("child", argv, NULL);
  return 0;
}
 
int main(void) {
  int wstatus;
  pid_t pid = getpid();
  pid_t child_pid = fork();
 
  if (child_pid == 0) {
    return child_func(NULL);
  } 
 
  sleep(1);
 
  show_maps(pid, "all");
 
  do {
    if (waitpid(child_pid, &wstatus, WUNTRACED) == -1) {
      perror("waitpid");
      return 1;
    }
    if (WIFSTOPPED(wstatus)) {
      printf("===== After execve ====\n");
      show_maps(child_pid, "all");
      kill(child_pid, SIGCONT);
    }
  } while (!WIFEXITED(wstatus));
 
  printf("%d has terminated\n", child_pid);
 
  return 0;
}