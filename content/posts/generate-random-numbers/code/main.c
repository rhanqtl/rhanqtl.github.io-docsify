#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define WIDTH 1024
#define N_ITERS 1000000
typedef struct {
  clock_t time_elapsed;
  int stats[WIDTH];
} report_t;

void report_init(report_t *r) {
  r->time_elapsed = 0;
  memset(r->stats, 0, WIDTH * sizeof(int));
}

void report_print(const report_t *r) {
  printf("time elapsed: %.03gs\n", (double)r->time_elapsed / CLOCKS_PER_SEC);
  // printf("distribution:\n");
  // for (int i = 0; i < WIDTH; i++) {
  //   printf("%4d | ", i);
  //   for (int j = 0; j < r->stats[i]; j += 10) {
  //     printf("*");
  //   }
  //   putchar('\n');
  // }

  // Standard deviation
  double sum = 0.0;
  for (int i = 0; i < WIDTH; i++) {
    sum += r->stats[i];
  }
  double mean = sum / WIDTH;
  double sd = 0.0;
  for (int i = 0; i < WIDTH; i++) {
    sd += pow(r->stats[i] - mean, 2);
  }
  sd = sqrt(sd / WIDTH);
  printf("Standard deviation: %.03g\n", sd);
}

void print_usage(const char *prog);
void bench_float(const int width, report_t *r);
void bench_int(const int width, report_t *r);

int main(int argc, char *argv[]) {
  if (argc != 1 && argc != 2) {
    print_usage(argv[0]);
    exit(EXIT_FAILURE);
  }

  bool use_float = false;
  if (argc == 2 && strcmp(argv[1], "-f") == 0) {
    use_float = true;
  }

  report_t r;
  report_init(&r);
  if (use_float) {
    printf("=== Benchmark of Floating Point ===\n");
    bench_float(WIDTH, &r);
  } else {
    printf("=== Benchmark of Integer ===\n");
    bench_int(WIDTH, &r);
  }
  report_print(&r);
}

void bench_float(const int width, report_t *r) {
#define GET_RAND() ((int)(width * (rand() / (RAND_MAX + 1.0))))
  for (int i = 0; i < N_ITERS; i++) {
    clock_t begin = clock();
    int v = GET_RAND();
    clock_t end = clock();
    r->time_elapsed += end - begin;
    r->stats[v]++;
  }
#undef GET_RAND
}

void bench_int(const int width, report_t *r) {
#define GET_RAND() (rand() % width)
  for (int i = 0; i < N_ITERS; i++) {
    clock_t begin = clock();
    int v = GET_RAND();
    clock_t end = clock();
    r->time_elapsed += end - begin;
    r->stats[v]++;
  }
#undef GET_RAND
}

void print_usage(const char *prog) {
  fprintf(stderr,
          "usage: %s [-f]\n"
          "  -f  use floating point\n",
          prog);
}
