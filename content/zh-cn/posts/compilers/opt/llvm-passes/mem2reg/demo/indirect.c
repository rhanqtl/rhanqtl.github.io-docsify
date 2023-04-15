#include <stdio.h>

void foo() {
  int a = 42;
  int *p = &a;
}

void bar() {
  static int baz = 42;
  int *p, *q = p;
  printf("%p %p\n", p, q);
  p = &baz;
  q = p;
  // printf("%p\n", &baz);
}

void qux() {
  // union {
  //   float fval;
  //   int ival;
  // } u;
  // u.ival = 0x12345678;
  // printf("%g\n", u.fval);
  float fval = 3.14F;
  int ival = *(int *)(&fval);
}

int main() {
  bar();
  // qux();
  // int *p = NULL;
  // *p = 42;
  // int i = ~0x80000000;
  // printf("%d\n", i + 1);
}
