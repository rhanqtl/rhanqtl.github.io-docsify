#include <benchmark/benchmark.h>

#include <cstdint>
#include <iostream>
#include <unordered_map>

static std::unordered_map<uint64_t, uint64_t> memo = {{0, 1}, {1, 1}};

static uintmax_t fib_r(size_t n) {
  auto it = memo.find(n);
  if (it != memo.end()) {
    return it->second;
  }
  uintmax_t ans = fib_r(n - 1) + fib_r(n - 2);
  memo[n] = ans;
  return ans;
}

__attribute__((noinline)) static uintmax_t fib_i(size_t n) {
  uintmax_t a = 1, b = 1;
  // n cannot be UINT64_MAX, because It's impossible to represent
  // fib(UINT64_MAX), so we don't need to worry about infinite loops.
  for (size_t i = 2; i <= n; i++) {
    uintmax_t c = a + b;
    a = b;
    b = c;
  }
  return a;
}

// fib(93) < 2^64-1 < fib(94)
static void benchmark_fib_r(benchmark::State &state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(fib_r(93));
    benchmark::DoNotOptimize(fib_r(93));
    benchmark::DoNotOptimize(fib_r(93));
    benchmark::DoNotOptimize(fib_r(93));
    benchmark::DoNotOptimize(fib_r(93));
    benchmark::DoNotOptimize(fib_r(93));
    benchmark::DoNotOptimize(fib_r(93));
    benchmark::DoNotOptimize(fib_r(93));
    benchmark::DoNotOptimize(fib_r(93));
    benchmark::DoNotOptimize(fib_r(93));
  }
}

BENCHMARK(benchmark_fib_r);

static void benchmark_fib_i(benchmark::State &state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(fib_i(93));
    benchmark::DoNotOptimize(fib_i(93));
    benchmark::DoNotOptimize(fib_i(93));
    benchmark::DoNotOptimize(fib_i(93));
    benchmark::DoNotOptimize(fib_i(93));
    benchmark::DoNotOptimize(fib_i(93));
    benchmark::DoNotOptimize(fib_i(93));
    benchmark::DoNotOptimize(fib_i(93));
    benchmark::DoNotOptimize(fib_i(93));
    benchmark::DoNotOptimize(fib_i(93));
  }
}

BENCHMARK(benchmark_fib_i);

BENCHMARK_MAIN();
