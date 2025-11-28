#include <benchmark/benchmark.h>
#include <random>
#include <algorithm>
#include <queue>       
#include <vector>


static std::mt19937 rng(42);

std::vector<int> generateRandomData(size_t n) {
    std::uniform_int_distribution<int> dist(1, 1'000'000);
    std::vector<int> v(n);
    for (auto& x : v) x = dist(rng);
    return v;
}

// Benchmark heap construction
static void BM_Construct(benchmark::State& state) {
    size_t count = state.range(0);
    
    for (auto _ : state) {
        std::vector<int> v;
        v.reserve(count);
        benchmark::DoNotOptimize(v);
        std::priority_queue<int, std::vector<int>, std::greater<int>> pq(
            std::greater<int>(),
            std::move(v)
        );
        benchmark::DoNotOptimize(pq);
    }
}

// Benchmark pushing random data
static void BM_PushRandom(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = generateRandomData(count);

    for (auto _ : state) {
        std::vector<int> v;
        v.reserve(count);
        std::priority_queue<int, std::vector<int>, std::greater<int>> pq(
            std::greater<int>(),
            std::move(v)
        );

        for (int x : data)
            pq.push(x);
    }

    state.SetItemsProcessed(state.iterations() * count);
}

// Benchmark push ascending random data
static void BM_PushSortedAsc(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = generateRandomData(count);
    std::sort(data.begin(), data.end());

    for (auto _ : state) {
        std::vector<int> v;
        v.reserve(count);
        std::priority_queue<int, std::vector<int>, std::greater<int>> pq(
            std::greater<int>(), 
            std::move(v)
        );

        for (int x : data) 
            pq.push(x);
    }

    state.SetItemsProcessed(state.iterations() * count);
}

// Benchmark push descending random data 
static void BM_PushSortedDesc(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = generateRandomData(count);
    std::sort(data.begin(), data.end(), std::greater<int>());

    for (auto _ : state) {
        std::vector<int> v;
        v.reserve(count);
        std::priority_queue<int, std::vector<int>, std::greater<int>> pq(
            std::greater<int>(),
            std::move(v)
        );

        for (int x : data) 
            pq.push(x);
    }

    state.SetItemsProcessed(state.iterations() * count);
}

// Benchmark pop
static void BM_Pop(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = generateRandomData(count);

    for (auto _ : state) {
        state.PauseTiming();
        std::vector<int> v;
        v.reserve(count);
        std::priority_queue<int, std::vector<int>, std::greater<int>> pq(
            std::greater<int>(),
            std::move(v)
        );
        for (int x : data) pq.push(x);
        state.ResumeTiming();

        for (size_t i = 0; i < count; i++) {
            pq.pop();
        }
    }

    state.SetItemsProcessed(state.iterations() * count);
}



BENCHMARK(BM_Construct)->RangeMultiplier(4)->Range(256, 1<<20);

BENCHMARK(BM_PushRandom)->RangeMultiplier(2)->Range(256, 1<<20);
BENCHMARK(BM_PushSortedAsc)->RangeMultiplier(2)->Range(256, 1<<20);
BENCHMARK(BM_PushSortedDesc)->RangeMultiplier(2)->Range(256, 1<<20);

BENCHMARK(BM_Pop)->RangeMultiplier(2)->Range(256, 1<<20);

BENCHMARK_MAIN();
