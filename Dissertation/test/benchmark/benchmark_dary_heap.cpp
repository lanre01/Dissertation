#include <iostream>
#include <fstream>
#include <charconv>
#include <algorithm>
#include <cmath>
#include <vector>
#include <benchmark/benchmark.h>
#include "minmax_heap.hpp"

inline std::vector<int> readRandomData(size_t n)
{
    std::ifstream file("numbers.bin", std::ios::binary);
    std::vector<int> buffer(n);

    file.read(reinterpret_cast<char*>(buffer.data()),
                     buffer.size() * sizeof(int));
    return buffer;
}

template<int D>
static void BM_Construct(benchmark::State& state)
{
    const size_t count = state.range(0);

    for (auto _ : state)
    {
        state.PauseTiming();
        auto data = readRandomData(count);
        state.ResumeTiming();

        make_dary_heap<D>(data.begin(), data.end());
        benchmark::DoNotOptimize(data);
    }
}

template<int D>
static void BM_PushRandom(benchmark::State& state)
{
    const size_t count = state.range(0);
    auto data = readRandomData(count);
    std::vector<int> heap;
    heap.reserve(count);

    for (auto _ : state)
    {
        state.PauseTiming();
        heap.clear();
        state.ResumeTiming();

        for (auto x : data)
        {
            heap.push_back(x);
            push_dary_heap<D>(heap.begin(), heap.end());
        }
    }
}

template<int D>
static void BM_PushSortedAsc(benchmark::State& state)
{
    const size_t count = state.range(0);
    auto data = readRandomData(count);
    std::sort(data.begin(), data.end());

    std::vector<int> heap;
    heap.reserve(count);

    for (auto _ : state)
    {
        state.PauseTiming();
        heap.clear();
        state.ResumeTiming();

        for (auto x : data)
        {
            heap.push_back(x);
            push_dary_heap<D>(heap.begin(), heap.end());
        }
    }
}

template<int D>
static void BM_PushSortedDesc(benchmark::State& state)
{
    const size_t count = state.range(0);
    auto data = readRandomData(count);
    std::sort(data.begin(), data.end(), std::greater<int>());

    std::vector<int> heap;
    heap.reserve(count);

    for (auto _ : state)
    {
        state.PauseTiming();
        heap.clear();
        state.ResumeTiming();

        for (auto x : data)
        {
            heap.push_back(x);
            push_dary_heap<D>(heap.begin(), heap.end());
        }
    }
}

template<int D>
static void BM_Extract(benchmark::State& state)
{
    const size_t count = state.range(0);

    for (auto _ : state)
    {
        state.PauseTiming();
        auto data = readRandomData(count);
        make_dary_heap<D>(data.begin(), data.end());
        state.ResumeTiming();

        while (!data.empty())
        {
            pop_dary_heap<D>(data.begin(), data.end());
            benchmark::DoNotOptimize(data.back());
            data.pop_back();
        }
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
    ->ComputeStatistics("p99", [](const std::vector<double>& v){ return percentile(v, 99.0); });

#define REGISTER_DARY_HEAP_BENCH(D) \
BENCHMARK_TEMPLATE(BM_Construct, D)->RangeMultiplier(4)->Range(256, 1<<20) \
    ADD_STATS(); \
BENCHMARK_TEMPLATE(BM_PushRandom, D)->RangeMultiplier(2)->Range(256, 1<<20) \
    ADD_STATS(); \
BENCHMARK_TEMPLATE(BM_PushSortedAsc, D)->RangeMultiplier(2)->Range(256, 1<<20) \
    ADD_STATS(); \
BENCHMARK_TEMPLATE(BM_PushSortedDesc, D)->RangeMultiplier(2)->Range(256, 1<<20) \
    ADD_STATS(); \
BENCHMARK_TEMPLATE(BM_Extract, D)->RangeMultiplier(2)->Range(256, 1<<20) \
    ADD_STATS();

REGISTER_DARY_HEAP_BENCH(2)
REGISTER_DARY_HEAP_BENCH(3)
REGISTER_DARY_HEAP_BENCH(4)
REGISTER_DARY_HEAP_BENCH(5)
REGISTER_DARY_HEAP_BENCH(6)
REGISTER_DARY_HEAP_BENCH(7)
REGISTER_DARY_HEAP_BENCH(8)
REGISTER_DARY_HEAP_BENCH(9)
REGISTER_DARY_HEAP_BENCH(10)

BENCHMARK_MAIN();
