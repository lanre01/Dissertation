#include <benchmark/benchmark.h>
#include <random>
#include <algorithm>
#include <map>       
#include <vector>


static std::mt19937 rng(42);

std::vector<int> generateRandomData(size_t n) {
    std::uniform_int_distribution<int> dist(1, 1'000'000);
    std::vector<int> v(n);
    for (auto& x : v) x = dist(rng);
    return v;
}

static void BM_Baseline(benchmark::State& state) {
   
    for (auto _ : state) {
        std::map<int, int> m;
        benchmark::DoNotOptimize(m);
    }
}

// Benchmark pushing random data
static void BM_PushRandom(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = generateRandomData(count);

    for (auto _ : state) {
        std::map<int, int> m;
        for (int x : data) 
            m[x] = x;
    }
    
    state.SetItemsProcessed(state.iterations() * count);
}

// Benchmark push ascending random data
static void BM_PushSortedAsc(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = generateRandomData(count);
    std::sort(data.begin(), data.end());

    for (auto _ : state) {
        std::map<int, int> m;
        
        for (int x : data) 
            m[x] = x;
    }

    state.SetItemsProcessed(state.iterations() * count);

}

// Benchmark push descending random data 
static void BM_PushSortedDesc(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = generateRandomData(count);
    std::sort(data.begin(), data.end(), std::greater<int>());

    for (auto _ : state) {
        std::map<int, int> m;
        
        for (int x : data) 
            m[x] = x;
    }

    state.SetItemsProcessed(state.iterations() * count);
}

// Benchmark minimum
static void BM_Pop(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = generateRandomData(count);
    

    for (auto _ : state) {
        state.PauseTiming();
        std::map<int, int> m;
        
        for (int x : data) 
            m[x] = x;
        state.ResumeTiming();
        
        while(!m.empty()) {
            benchmark::DoNotOptimize(m.erase(m.begin()));
        }
    }

    state.SetItemsProcessed(state.iterations() * count);
}

static void BM_Search(benchmark::State& state)
{
    size_t count = state.range(0);
    std::map<int, int> m;
    auto data = generateRandomData(count);
    for (int x : data) 
        m[x] = x;

    std::uniform_int_distribution<int> dist(1, 1'000'000);

    for(auto _ : state)
    {
        int q = dist(rng);
        auto result = m.find(q);
        benchmark::DoNotOptimize(result);
    }

    state.SetItemsProcessed(state.iterations() * count);
}


// Benchmark remove(value)
static void BM_RemoveValue(benchmark::State& state) {
    size_t count = state.range(0);
    std::map<int, int> m;    
    auto data = generateRandomData(count);

    for (auto _ : state) {
        state.PauseTiming();
        for (auto x : data) 
            m[x] = x;
        state.ResumeTiming();

        for (auto x : data){
            auto t = m.erase(x);
            benchmark::DoNotOptimize(t);
    
        }
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