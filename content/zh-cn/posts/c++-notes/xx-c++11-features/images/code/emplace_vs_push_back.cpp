#include <benchmark/benchmark.h>

#include <array>
#include <cstdlib>
#include <vector>

constexpr size_t ARRAY_SIZE = 4096 / sizeof(int);

struct Huge {
  std::array<int, ARRAY_SIZE> data;
  Huge() {
    int x = std::rand();
    data.fill(x);
  }
};

static void BM_push_back(benchmark::State &state) {
  std::vector<Huge> v;
  for (auto _ : state) {
    v.push_back(Huge{});
    benchmark::DoNotOptimize(v);
  }
}
BENCHMARK(BM_push_back);

static void BM_emplace_back(benchmark::State &state) {
  std::vector<Huge> v;
  for (auto _ : state) {
    v.emplace_back();
    benchmark::DoNotOptimize(v);
  }
}
BENCHMARK(BM_emplace_back);

BENCHMARK_MAIN();
