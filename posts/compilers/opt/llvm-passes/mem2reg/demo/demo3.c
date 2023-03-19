#include <stdio.h>

int f(int x, int y) {
  if (x > 42) {
    y += 1;
  } else {
    y += 2;
  }
  return y;
}

int main() {
  int x, y;
  scanf("%d %d", &x, &y);
  printf("%d\n", f(x, y));
}
