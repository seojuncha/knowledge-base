#include <stdio.h>
#include <signal.h>
 
int main(int argc, char **argv) {
  printf("Hello, World\n");
  raise(SIGSTOP);
  return 0;
}