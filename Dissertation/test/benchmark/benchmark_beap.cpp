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


// Benchmark beap construction
static void BM_Baseline(benchmark::State& state) {
    size_t count = state.range(0);
    for (auto _ : state) {
        Beap<int> b;
        b.reserve(count);
        benchmark::DoNotOptimize(b);
    }
}

// Benchmark pushing random data
static void BM_PushRandom(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = generateRandomData(count);

    for (auto _ : state) {
        Beap<int> b;
        b.reserve(count);
        for (int x : data)
            b.push(x);
    }
    
    state.SetItemsProcessed(state.iterations() * count);
}


// Benchmark push ascending random data
static void BM_PushSortedAsc(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = generateRandomData(count);
    std::sort(data.begin(), data.end());

    for (auto _ : state) {
        Beap<int> b;
        b.reserve(count);

        for (int x : data)
            b.push(x);
    }

    state.SetItemsProcessed(state.iterations() * count);

}


// Benchmark push descending random data 
static void BM_PushSortedDesc(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = generateRandomData(count);
    std::sort(data.begin(), data.end(), std::greater<int>());

    for (auto _ : state) {
        Beap<int> b;
        b.reserve(count);

        for (int x : data)
            b.push(x);
    }

    state.SetItemsProcessed(state.iterations() * count);
}

// Benchmark Pop
static void BM_Pop(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = generateRandomData(count);
    

    for (auto _ : state) {
        state.PauseTiming();
        Beap<int> b;
        b.reserve(count);
        for(auto d : data)
            b.push(d);
        state.ResumeTiming();
        
        for (size_t i = 0; i < count; i++) {
            auto t = b.pop();
            benchmark::DoNotOptimize(t);
        }
    }

    state.SetItemsProcessed(state.iterations() * count);
}

// Benchmark search 
static void BM_Search(benchmark::State& state) {
    size_t count = state.range(0);

    Beap<int> b;
    b.reserve(count);
    auto data = generateRandomData(count);
    for (int x : data) b.push(x);

    std::uniform_int_distribution<int> dist(1, 1'000'000);

    for (auto _ : state) {
        int q = dist(rng);
        auto result = b.search(q);
        benchmark::DoNotOptimize(result);
    }

    state.SetItemsProcessed(state.iterations() * count);
}



// Benchmark remove(value)
static void BM_RemoveValue(benchmark::State& state) {
    size_t count = state.range(0);
    Beap<int> b;
    b.reserve(count);
    
    for (auto _ : state) {
        state.PauseTiming();
        auto data = generateRandomData(count);
        for (auto x : data) b.push(x);
        state.ResumeTiming();

        for (auto x : data)
            b.remove(x);
    }
    

    state.SetItemsProcessed(state.iterations() * count);
}



BENCHMARK(BM_Baseline)->RangeMultiplier(4)->Range(256, 1<<20); 

BENCHMARK(BM_PushRandom)->RangeMultiplier(2)->Range(256, 1<<20); 
BENCHMARK(BM_PushSortedAsc)->RangeMultiplier(2)->Range(256, 1<<20); 
BENCHMARK(BM_PushSortedDesc)->RangeMultiplier(2)->Range(256, 1<<20); 

BENCHMARK(BM_Pop)->RangeMultiplier(2)->Range(256, 1<<20); 

BENCHMARK(BM_Search)->RangeMultiplier(4)->Range(256, 1<<20); 

BENCHMARK(BM_RemoveValue)->RangeMultiplier(4)->Range(256, 1<<20); 


BENCHMARK_MAIN();
