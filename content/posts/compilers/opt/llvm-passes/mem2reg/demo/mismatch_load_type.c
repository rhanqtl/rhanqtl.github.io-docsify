#include <stdio.h>

struct A {
  int ival;
  double fval;
};

int main() {
  struct A a = {.ival = 42, .fval = 3.14};
  printf("A { ival = %d, fval = %g }\n", a.ival, a.fval);

  int nums[3] = {1, 2, 3};
  printf("%d\n", nums[1]);
}
