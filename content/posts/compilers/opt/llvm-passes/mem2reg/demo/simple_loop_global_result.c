static int acc = 0;

void foo(const int begin, const int end) {
  int t = 0;
  for (int i = begin; i < end; i++) {
    t += i;
  }
  acc += t;
}
