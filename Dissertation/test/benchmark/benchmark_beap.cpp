#include <benchmark/benchmark.h>
#include <random>
#include <algorithm>
#include "../../src/Beap/Beap.hpp"  

static std::mt19937 rng(42);

std::vector<int> generateRandomData(size_t n) {
    std::uniform_int_distribution<int> dist(1, 1'000'000);
    std::vector<int> v(n);
    for (auto& x : v) x = dist(rng);
    return v;
}

void SetUp(const ::benchmark::State& s)  {
        Beap b = Beap();
}

void TearDown(const ::benchmark::State&) {}

// Benchmark beap construction
static void BM_Construct(benchmark::State& state) {
    for (auto _ : state) {
        Beap b;
        benchmark::DoNotOptimize(b);
    }
}

// Benchmark pushing random data
static void BM_PushRandom(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = generateRandomData(count);

    for (auto _ : state) {
        state.PauseTiming();
        Beap b;
        b.container.reserve(count);
        state.ResumeTiming();

        for (int x : data)
            b.push(x);
    }
}


// Benchmark push ascending random data
static void BM_PushSortedAsc(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = generateRandomData(count);
    std::sort(data.begin(), data.end());

    for (auto _ : state) {
        state.PauseTiming();
        Beap b;
        state.ResumeTiming();

        for (int x : data)
            b.push(x);
    }
}


// Benchmark push descending random data 
static void BM_PushSortedDesc(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = generateRandomData(count);
    std::sort(data.begin(), data.end(), std::greater<int>());

    for (auto _ : state) {
        state.PauseTiming();
        Beap b;
        state.ResumeTiming();

        for (int x : data)
            b.push(x);
    }
}

// Benchmark Pop
static void BM_Pop(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = generateRandomData(count);

    for (auto _ : state) {
        Beap b;
        for (int x : data) b.push(x);

        for (size_t i = 0; i < count; i++) {
            b.pop();
        }
    }
}

// Benchmark search 
static void BM_Search(benchmark::State& state) {
    size_t count = state.range(0);

    Beap b;
    auto data = generateRandomData(count);
    for (int x : data) b.push(x);

    std::uniform_int_distribution<int> dist(1, 1'000'000);

    for (auto _ : state) {
        int q = dist(rng);
        auto result = b.search(q);
        benchmark::DoNotOptimize(result);
    }
}



// Benchmark remove(value)
static void BM_RemoveValue(benchmark::State& state) {
    size_t count = state.range(0);

    for (auto _ : state) {
        state.PauseTiming();
        Beap b;
        auto data = generateRandomData(count);
        for (int x : data) b.push(x);
        state.ResumeTiming();

        for (int x : data)
            b.remove(x);
    }
}

// Benchmark Mixed workload: 40% push, 40% search, 20% pop
static void BM_MixedWorkload(benchmark::State& state) {
    Beap b;

    std::uniform_int_distribution<int> valdist(1, 1'000'000);
    std::uniform_int_distribution<int> opdist(1, 100);

    for (auto _ : state) {
        int op = opdist(rng);

        if (op <= 40) {
            b.push(valdist(rng));
        }
        else if (op <= 80) {
            benchmark::DoNotOptimize(b.search(valdist(rng)));
        }
        else {
            if (b.size > 0) b.pop();
        }
    }
}


BENCHMARK(BM_Construct);

BENCHMARK(BM_PushRandom)->Arg(1000)->Arg(5000)->Arg(10000);
BENCHMARK(BM_PushSortedAsc)->Arg(1000)->Arg(5000)->Arg(10000);
BENCHMARK(BM_PushSortedDesc)->Arg(1000)->Arg(5000)->Arg(10000);

BENCHMARK(BM_Pop)->Arg(1000)->Arg(5000)->Arg(10000);

BENCHMARK(BM_Search)->Arg(5000)->Arg(10000);

BENCHMARK(BM_RemoveValue)->Arg(2000)->Arg(5000);

BENCHMARK(BM_MixedWorkload)->Arg(50000);

BENCHMARK_MAIN();
