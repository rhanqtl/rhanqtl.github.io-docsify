#include <stdio.h>

int main() {
  int gotcha;
  for (int i = 0; i < 10; i++) {
    int t = gotcha;
    t++;
    gotcha = t;
  }
}
