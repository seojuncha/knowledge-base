#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

extern char **environ;

static void printenv() {
  char **env = environ;
  pid_t pid = getpid();
  while (*env) {
    printf("[%d] %s\n", pid, *env);
    env++;
  }
}

/**
 * int execv(const char *pathname, char *const argv[]);
 *  : execv(path, argv[])
 */
static int use_vec(void) {
  char *const vec[] = {"ls", "-l", "/", NULL};
  execv("/usr/bin/ls", vec);
  perror("execv");
  return 1;
}

static int use_vec_p(void) {
  char *const vec[] = {"ls", "-l", "/", NULL};

  printf("PATH: %s\n", getenv("PATH"));
  setenv("PATH", "/", 1);
  execvp("ls", vec);
  perror("execvp");
  return 1;
}

static int use_vec_env(void) {
  char *const vec[] = {"ls", "-l", "/", NULL};
  char *const env[] = {
    "VAR=value",
    NULL
  };
  execve("/usr/bin/ls", vec, env);
  perror("execve");
  return 1;
}

/**
 * int execl(const char *pathname, const char *arg, ... , (char *) NULL);
 *  : execl(path, arg0, arg1, ..., NULL)
 */
static int use_list(void) {
  execl("/usr/bin/ls", "ls", "-l", "/", NULL);
  perror("execl");
  return 1;
}


int main(void) {
  int w, wstatus;
  pid_t pid;

  printf("shell: %s\n", getenv("SHELL"));

  pid = fork();

  if (pid == 0) {
    // return use_list();
    // return use_vec();
    // return use_vec_p();
    return use_vec_env();
  } else if (pid > 0) {
    w = waitpid(pid, NULL, 0);
    if (WIFEXITED(wstatus)) {
      printf("Exited normally[%d]\n", w);
    }
  }
  return 0;
}
