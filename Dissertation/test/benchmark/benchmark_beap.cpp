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
        Beap b;
        b.container.reserve(count);
        benchmark::DoNotOptimize(b);
    }
}

// Benchmark pushing random data
static void BM_PushRandom(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = generateRandomData(count);

    for (auto _ : state) {
        Beap b;
        b.container.reserve(count);
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
        Beap b;
        b.container.reserve(count);

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
        Beap b;
        b.container.reserve(count);

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
        Beap b;
        b.container.reserve(count);
        for(auto d : data)
            b.push(d);
        state.ResumeTiming();
        
        for (size_t i = 0; i < count; i++) {
            b.pop();
        }
    }

    state.SetItemsProcessed(state.iterations() * count);
}

// Benchmark search 
static void BM_Search(benchmark::State& state) {
    size_t count = state.range(0);

    Beap b;
    b.container.reserve(count);
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
    Beap b;
    b.container.reserve(count);
    auto data = generateRandomData(count);
    for (int x : data) b.push(x);

    for (auto _ : state) {
        for (int x : data)
            b.remove(x);
    }

    state.SetItemsProcessed(state.iterations() * count);
}

// Benchmark Mixed workload: 40% push, 40% search, 20% pop
/*static void BM_MixedWorkload(benchmark::State& state) {
    Beap b;
    b.container.reserve(state.range(0));


    std::uniform_int_distribution<int> valdist(1, 1'000'000);
    std::uniform_int_distribution<int> opdist(1, 100);

    for (auto _ : state) {
        state.PauseTiming();
        int op = opdist(rng);
        state.ResumeTiming();

        if (op <= 40) {
            b.push(valdist(rng));
        }
        else if (op <= 80) {
            benchmark::DoNotOptimize(b.search(valdist(rng)));
        }
        else {
            if (b.size > 0) benchmark::DoNotOptimize(b.pop());
        }
    }
}*/


BENCHMARK(BM_Baseline)->RangeMultiplier(4)->Range(256, 1<<20); 

BENCHMARK(BM_PushRandom)->RangeMultiplier(2)->Range(256, 1<<20); 
BENCHMARK(BM_PushSortedAsc)->RangeMultiplier(2)->Range(256, 1<<20); 
BENCHMARK(BM_PushSortedDesc)->RangeMultiplier(2)->Range(256, 1<<20); 

BENCHMARK(BM_Pop)->RangeMultiplier(2)->Range(256, 1<<20); 

BENCHMARK(BM_Search)->RangeMultiplier(4)->Range(256, 1<<18); 

BENCHMARK(BM_RemoveValue)->RangeMultiplier(4)->Range(256, 1<<18); 

//BENCHMARK(BM_MixedWorkload)->Arg(1000)->Arg(5000)->Arg(10000);

BENCHMARK_MAIN();
