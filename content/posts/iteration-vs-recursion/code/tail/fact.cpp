#include <benchmark/benchmark.h>

#include <cstdint>
#include <iostream>
#include <unordered_map>

static uintmax_t fact_tail_rec(const size_t n, uintmax_t acc) {
  if (n == 0) {
    return acc;
  }
  return fact_tail_rec(n - 1, acc * n);
}

static uintmax_t fact_r(size_t n) { return fact_tail_rec(n, 1); }

static uintmax_t fact_i(size_t n) {
  uintmax_t ans = 1;
  for (size_t i = 1; i <= n; i++) {
    ans *= i;
  }
  return ans;
}

// fact(20) < 2^64-1 < fact(21)
static void benchmark_fact_r(benchmark::State &state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(fact_r(20));
  }
}

BENCHMARK(benchmark_fact_r);

static void benchmark_fact_i(benchmark::State &state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(fact_i(20));
  }
}

BENCHMARK(benchmark_fact_i);

BENCHMARK_MAIN();
