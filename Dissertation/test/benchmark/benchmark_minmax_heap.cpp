#include <iostream>
#include <fstream>
#include <charconv>
#include <algorithm>
#include <cmath>
#include <vector>
#include <benchmark/benchmark.h>
#include "minmax_heap.hpp"
#include "../../src/n_beap/n_beap.hpp"    

inline std::vector<int> readRandomData(size_t n)
{
    std::ifstream file("numbers.bin", std::ios::binary);
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

    size_t startIndex = static_cast<size_t>(0.975 * count);
    size_t endIndex   = static_cast<size_t>(count + 0.025 * count);
    size_t sampleSize = endIndex - startIndex;

    std::vector<int> vec(sampleSize);

    file.seekg(startIndex * sizeof(int), std::ios::beg);

    file.read(reinterpret_cast<char*>(vec.data()),
              sampleSize * sizeof(int));

    return vec;
}

template<typename T>
static void BM_Construct(benchmark::State& state)
{
    const size_t count = state.range(0);
    for (auto _ : state)
    {
        state.PauseTiming();
        auto data = readRandomData(count);
        state.ResumeTiming();

        make_minmax_heap(data.begin(), data.end());
        benchmark::DoNotOptimize(data);
    }
}

template<typename T>
static void BM_PushRandom(benchmark::State& state)
{
    const size_t count = state.range(0);
    auto data = readRandomData(count);
    std::vector<T> heap;
    heap.reserve(count);

    for (auto _ : state)
    {
        state.PauseTiming();
        heap.clear();
        state.ResumeTiming();

        for (auto x : data)
        {
            heap.push_back(x);
            push_minmax_heap(heap.begin(), heap.end());
        }
    }
}

template<typename T>
static void BM_PushSortedAsc(benchmark::State& state)
{
    const size_t count = state.range(0);
    auto data = readRandomData(count);
    std::sort(data.begin(), data.end());
    std::vector<T> heap;
    heap.reserve(count);

    for (auto _ : state)
    {
        state.PauseTiming();
        heap.clear();
        state.ResumeTiming();

        for (auto x : data)
        {
            heap.push_back(x);
            push_minmax_heap(heap.begin(), heap.end());
        }
    }
}

template<typename T>
static void BM_PushSortedDesc(benchmark::State& state)
{
    const size_t count = state.range(0);
    auto data = readRandomData(count);
    std::sort(data.begin(), data.end(), std::greater<int>());
    std::vector<T> heap;
    heap.reserve(count);

    for (auto _ : state)
    {
        state.PauseTiming();
        heap.clear();
        state.ResumeTiming();

        for (auto x : data)
        {
            heap.push_back(x);
            push_minmax_heap(heap.begin(), heap.end());
        }
    }
}

template<typename T>
static void BM_Extract(benchmark::State& state)
{
    const size_t count = state.range(0);

    for (auto _ : state)
    {
        state.PauseTiming();
        auto heap = readRandomData(count);
        make_minmax_heap(heap.begin(), heap.end());
        state.ResumeTiming();

        for (size_t i = 0; i < count; i++)
        {
            pop_minmax_heap_min(heap.begin(), heap.end());
            T min_value = heap.back();
            heap.pop_back();
            benchmark::DoNotOptimize(min_value);
        }
    }
}

template<typename T>
static void BM_SearchM(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = readRandomData(count);
    auto test = readRandomDataTest(count);
    std::vector<int> vec;
    vec.reserve(count);
    
    for (auto _ : state) {
        state.PauseTiming();
        vec.clear();
        state.ResumeTiming();
        for (int x : data) 
        {
            vec.push_back(x);
            push_minmax_heap(vec.begin(), vec.end());
        }

        NBeap<int, 2> beap(vec);

        for(int x : test)
        {
            auto res = beap.search(x);
            benchmark::DoNotOptimize(res);
        }

        // convert back to a min-max heap
        vec = beap.getContainer();
        make_minmax_heap(vec.begin(), vec.end());
    }
}



static double percentile(std::vector<double> v, double p)
{
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
    ->ComputeStatistics("p99", [](const std::vector<double>& v){ return percentile(v, 99.0); })

#define REGISTER_MINMAX_HEAP_BENCH(T) \
BENCHMARK_TEMPLATE(BM_Construct, T)->RangeMultiplier(4)->Range(256, 1<<20) \
    ADD_STATS(); \
BENCHMARK_TEMPLATE(BM_PushRandom, T)->RangeMultiplier(2)->Range(256, 1<<20) \
    ADD_STATS(); \
BENCHMARK_TEMPLATE(BM_PushSortedAsc, T)->RangeMultiplier(2)->Range(256, 1<<20) \
    ADD_STATS(); \
BENCHMARK_TEMPLATE(BM_PushSortedDesc, T)->RangeMultiplier(2)->Range(256, 1<<20) \
    ADD_STATS(); \
BENCHMARK_TEMPLATE(BM_Extract, T)->RangeMultiplier(2)->Range(256, 1<<20) \
    ADD_STATS(); \
BENCHMARK_TEMPLATE(BM_SearchM, T)->RangeMultiplier(2)->Range(256, 1<<20) \
    ADD_STATS();

REGISTER_MINMAX_HEAP_BENCH(int);

BENCHMARK_MAIN();
