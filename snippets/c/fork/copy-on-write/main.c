#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
 
/* read PFN in /proc/{pid}/pagemap from VA */
static long get_pfn(pid_t pid, unsigned long va) {
  char fmt[] = "/proc/%d/pagemap";
  char path[64]; 
  unsigned long entry;
  int fd;
  unsigned long offset = (va / 4096) * 8;
  long pfn;
 
  sprintf(path, fmt, pid);
  printf("pagemap path: %s\n", path);
  
  fd = open(path, O_RDONLY);
  if (fd == -1) {
    perror("open");
    return -1;
  }
 
  pread(fd, &entry, 8, offset);
  close(fd);
 
  if (!(entry & (1ULL << 63))) {
    fprintf(stderr, "no entry\n");
    return -1;
  }
  
  pfn = entry & ((1ULL << 55) - 1);
  printf("VA=0x%lx, PFN=%ld\n", va, pfn);
 
  return pfn;
}
 

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
 
int global = 0;
 
static int child_func(void *arg) {
  int *child_var = arg;
  pid_t pid = getpid();
 
  printf("\nBEFORE WRITE >> address of child_var: %p\n", child_var);
  get_pfn(pid, (unsigned long)child_var);
  printf("var in child 1: %d\n", *child_var);
 
  *child_var += 10;
 
  printf("\nsleep...\n");
  sleep(1);
 
  printf("\nAFTER WRITE >> address of child_var: %p\n", child_var);
  get_pfn(pid, (unsigned long)child_var);
  printf("var in child 2: %d\n", *child_var);
  return 0;
}
 
int main(void) {
  int wstatus;
  int parent_var = 5;
  pid_t pid = getpid();
  pid_t child_pid;
 
  /*show_maps(pid, "all");*/
 
  printf("address of parent_var: %p\n", &parent_var);
  get_pfn(pid, (unsigned long)&parent_var);
 
  child_pid = fork();
  if (child_pid == 0) {
    return 0;
    /*return child_func(&parent_var);*/
  } 
 
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
 
  printf("\nlast var in parent: %d\n", parent_var);
  get_pfn(pid, (unsigned long)&parent_var);
 
  printf("%d has terminated\n", child_pid);
 
  return 0;
}