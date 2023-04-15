#include <stdio.h>

int main(int argc, char *argv[], char *envp[]) {
  printf("location of argv: %p\n", argv);
  for (char **p = argv; *p; p++) {
    printf("\targv[%ld] = %s\n", p - argv, *p);
  }

  printf("location of envp: %p\n", envp);
  for (char **p = envp; *p; p++) {
    printf("\tenvp[%ld] = %s\n", p - envp, *p);
  }

  return 0;
}
