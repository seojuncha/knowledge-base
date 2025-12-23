#include <sys/types.h>
#include <sys/wait.h>
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
  show_maps(getpid(), "all");
  return 0;
}
 
int main(void) {
  pid_t pid = getpid();
  pid_t child_pid = fork();
 
  if (child_pid == 0) {
    return child_func(NULL);
  } 
 
  show_maps(pid, "all");
  if (waitpid(child_pid, NULL, 0) == -1) {
    perror("waitpid");
    return 1;
  }
 
  printf("%d has terminated\n", child_pid);
 
  return 0;
}