int foo(int n) {
  int sum = 0;
  for (int i = 0; i < n; i++) {
    sum += i;
  }
  return sum;
}

void bar(int n, int *sum) {
  *sum = 0;
  for (int i = 0; i < n; i++) {
    *sum += i;
  }
}
