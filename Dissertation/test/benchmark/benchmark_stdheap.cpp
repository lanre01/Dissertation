#include <benchmark/benchmark.h>
#include <random>
#include <algorithm>
#include <queue>       
#include <vector>

// Same RNG as Beap benchmark
static std::mt19937 rng(42);

// Same helper used in beap version
std::vector<int> generateRandomData(size_t n) {
    std::uniform_int_distribution<int> dist(1, 1'000'000);
    std::vector<int> v(n);
    for (auto& x : v) x = dist(rng);
    return v;
}

// Benchmark heap construction
static void BM_Construct(benchmark::State& state) {
    for (auto _ : state) {
        std::priority_queue<int> pq;
        benchmark::DoNotOptimize(pq);
    }
}

// Benchmark pushing random data
static void BM_PushRandom(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = generateRandomData(count);

    for (auto _ : state) {
        state.PauseTiming();
        std::vector<int> v;
        v.reserve(count);
        std::priority_queue<int> pq(v.begin(), v.end());
        state.ResumeTiming();

        for (int x : data)
            pq.push(x);
    }
}

// Benchmark push ascending random data
static void BM_PushSortedAsc(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = generateRandomData(count);
    std::sort(data.begin(), data.end());

    for (auto _ : state) {
        state.PauseTiming();
        std::vector<int> v;
        v.reserve(count);
        std::priority_queue<int, std::vector<int>> pq(
            std::less<int>(), 
            std::move(v)
        );
        state.ResumeTiming();

        for (int x : data) 
            pq.push(x);
    }
}

// Benchmark push descending random data 
static void BM_PushSortedDesc(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = generateRandomData(count);
    std::sort(data.begin(), data.end(), std::greater<int>());

    for (auto _ : state) {
        state.PauseTiming();
        std::vector<int> v;
        v.reserve(count);
        std::priority_queue<int, std::vector<int>> pq(
            std::less<int>(), 
            std::move(v)
        );
        state.ResumeTiming();

        for (int x : data) 
            pq.push(x);
    }
}

// Benchmark pop
static void BM_Pop(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = generateRandomData(count);

    for (auto _ : state) {
        std::priority_queue<int> pq;
        for (int x : data) pq.push(x);

        for (size_t i = 0; i < count; i++) {
            pq.pop();
        }
    }
}



BENCHMARK(BM_Construct);

BENCHMARK(BM_PushRandom)->Arg(1000)->Arg(5000)->Arg(10000);
BENCHMARK(BM_PushSortedAsc)->Arg(1000)->Arg(5000)->Arg(10000);
BENCHMARK(BM_PushSortedDesc)->Arg(1000)->Arg(5000)->Arg(10000);

BENCHMARK(BM_Pop)->Arg(1000)->Arg(5000)->Arg(10000);

BENCHMARK_MAIN();
