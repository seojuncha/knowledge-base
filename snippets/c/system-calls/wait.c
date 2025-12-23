#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>  // pid_t
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char **arv) {
  int wstatus;
  pid_t cpid, w;

  cpid = fork();
  if (cpid == -1) {
    perror("fork");
    exit(1);
  }

  if (cpid == 0) {
    printf("child pid: %d\n", getpid());
    if (argc == 1) {
      // printf("child pause!\n");
      pause();
    }
    // printf("child would be exited\n");
    exit(EXIT_SUCCESS);
  } else {
    // The child procss is not traced(tracee of ptrace)
    int option = WUNTRACED | WCONTINUED;
    do {
      w = waitpid(cpid, &wstatus, option);
      if (w == -1) {
        perror("waitpid");
        exit(EXIT_FAILURE);
      }

      if (WIFEXITED(wstatus)) {
        printf("Exited normally: %d\n", WEXITSTATUS(wstatus));
      } else if (WIFSIGNALED(wstatus)) {
        printf("Signaled: %d\n", WTERMSIG(wstatus));
      } else if (WIFSTOPPED(wstatus)) {
        printf("Stopped: %d\n", WSTOPSIG(wstatus));
      } else if (WIFCONTINUED(wstatus)) {
        printf("Continued \n");
      }
    } while (!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));
    printf("Done\n");
  }

  return 0;
}