#define _GNU_SOURCE
// #define _POSIX_C_SOURCE 199309L
// #define _POSIX_C_SOURCE 1
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void my_sa_handler(int signo) { printf("signo: %d\n", signo); }

void sigaction_handler(int signo, siginfo_t *info, void *context) {
  printf("signo: %d\n", signo);
  if (!info) {
    fprintf(stderr, "not siginfo\n");
    exit(1);
  }
}

int main(void) {
  struct sigaction act = {0};

  act.sa_flags = SA_SIGINFO;
  act.sa_sigaction = &sigaction_handler;
  // act.sa_handler = &my_sa_handler;
  if (sigaction(SIGSEGV, &act, NULL) < 0) {
    perror("sigaction");
    exit(1);
  }

  raise(SIGSEGV);
  // raise(SIGINT);
  // raise(SIGKILL);

  return 0;
}
