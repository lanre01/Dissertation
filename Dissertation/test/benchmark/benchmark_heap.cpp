#include <iostream>
#include <fstream>
#include <charconv>
#include <algorithm>
#include <cmath>
#include <vector>
#include <benchmark/benchmark.h>
#include <vector>
#include <queue>


inline std::vector<int> readRandomData(size_t n)
{
    std::ifstream file("numbers.bin", std::ios::binary);
    int value;
    std::vector<int> buffer(n);

    file.read(reinterpret_cast<char*>(buffer.data()),
                     buffer.size() * sizeof(int));
    return buffer;
}

inline std::vector<int> readRandomDataTest(size_t count)
{
    std::ifstream file("numbers.bin", std::ios::binary);

    if (!file)
        throw std::runtime_error("Could not open numbers.bin");

    // Compute the sampling window
    size_t startIndex = static_cast<size_t>(0.975 * count);
    size_t endIndex   = static_cast<size_t>(count + 0.025 * count);
    size_t sampleSize = endIndex - startIndex;

    std::vector<int> vec(sampleSize);

    file.seekg(startIndex * sizeof(int), std::ios::beg);

    file.read(reinterpret_cast<char*>(vec.data()),
              sampleSize * sizeof(int));

    return vec;
}



static void BM_Construct(benchmark::State& state) {
    size_t count = state.range(0);

    for(auto _ : state)
    {
        state.PauseTiming();
        auto data = readRandomData(count);
        state.ResumeTiming();

        std::priority_queue<int, std::vector<int>, std::greater<int>> pq(
            std::greater<int>(),
            std::move(data)
        );
        benchmark::DoNotOptimize(pq);
    }
}


static void BM_PushRandom(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = readRandomData(count);
    

    for (auto _ : state) {
        
        //std::make_heap(v.begin(), v.end(), std::greater<>{});
        state.PauseTiming();
        std::vector<int> v;
        v.reserve(count);
        std::priority_queue<int, std::vector<int>, std::greater<int>> pq(
            std::greater<int>(),
            std::move(v)
        );
        state.ResumeTiming();

        for (int x : data){
            pq.push(x);
        }

    }
}


static void BM_PushSortedAsc(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = readRandomData(count);
    std::sort(data.begin(), data.end());
    

    for (auto _ : state) {
        state.PauseTiming();
        std::vector<int> v;
        v.reserve(count);
        std::priority_queue<int, std::vector<int>, std::greater<int>> pq(
            std::greater<int>(), 
            std::move(v)
        );
        state.ResumeTiming();
        

        for (int x : data) 
        {
            pq.push(x);
        }
    }
}


static void BM_PushSortedDesc(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = readRandomData(count);
    std::sort(data.begin(), data.end(), std::greater<int>());
    

    for (auto _ : state) {
        
        state.PauseTiming();
        std::vector<int> v;
        v.reserve(count);
        std::priority_queue<int, std::vector<int>, std::greater<int>> pq(
            std::greater<int>(),
            std::move(v)
        );
        state.ResumeTiming();
        

        for (int x : data) 
        {
            pq.push(x);
        }
    }
}


static void BM_Extract(benchmark::State& state) {
    size_t count = state.range(0);

    for (auto _ : state) {
        state.PauseTiming();
        auto data = readRandomData(count);
        std::priority_queue<int, std::vector<int>, std::greater<int>> pq(
            std::greater<int>(),
            std::move(data)
        );
        state.ResumeTiming();

        for (size_t i = 0; i < count; i++) {
            pq.pop();
        }
    }
}


static double percentile(std::vector<double> v, double p) {
    if (v.empty()) return 0.0;
    std::sort(v.begin(), v.end());
    const double idx = (p / 100.0) * (v.size() - 1);
    const std::size_t lo = static_cast<std::size_t>(std::floor(idx));
    const std::size_t hi = static_cast<std::size_t>(std::ceil(idx));
    const double frac = idx - lo;
    return v[lo] + (v[hi] - v[lo]) * frac;
}


#define ADD_STATS() \
    ->Repetitions(10) \
    ->ComputeStatistics("p90", [](const std::vector<double>& v){ return percentile(v, 90.0); }) \
    ->ComputeStatistics("p95", [](const std::vector<double>& v){ return percentile(v, 95.0); }) \
    ->ComputeStatistics("p99", [](const std::vector<double>& v){ return percentile(v, 99.0); });



BENCHMARK(BM_Construct)->RangeMultiplier(4)->Range(256, 1<<20)\
    ADD_STATS();

BENCHMARK(BM_PushRandom)->RangeMultiplier(2)->Range(256, 1<<20)\
    ADD_STATS();
BENCHMARK(BM_PushSortedAsc)->RangeMultiplier(2)->Range(256, 1<<20)\
    ADD_STATS();
BENCHMARK(BM_PushSortedDesc)->RangeMultiplier(2)->Range(256, 1<<20)\
    ADD_STATS();

BENCHMARK(BM_Extract)->RangeMultiplier(2)->Range(256, 1<<20)\
    ADD_STATS();

BENCHMARK_MAIN();
