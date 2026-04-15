#include <iostream>
#include <fstream>
#include <charconv>
#include <benchmark/benchmark.h>
#include <random>
#include <algorithm>
#include <set>
#include <vector>

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

using MinSet = std::set<int, std::less<int>>;

static void BM_Construct(benchmark::State& state) {
    const size_t count = state.range(0);

    for (auto _ : state) {
        state.PauseTiming();
        auto data = readRandomData(count);
        state.ResumeTiming();
        MinSet s(data.begin(), data.end());
        benchmark::DoNotOptimize(s);
    }
}

static void BM_PushRandom(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = readRandomData(count);
    MinSet s;

    for (auto _ : state) {
        state.PauseTiming();
        s.clear();
        state.ResumeTiming();
        for (int x : data)
            s.insert(x);
    }
}

static void BM_PushSortedAsc(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = readRandomData(count);
    std::sort(data.begin(), data.end());
    MinSet s;

    for (auto _ : state) {
        state.PauseTiming();
        s.clear();
        state.ResumeTiming();
        for (int x : data)
            s.insert(x);
    }
}

static void BM_PushSortedDesc(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = readRandomData(count);
    std::sort(data.begin(), data.end(), std::greater<int>());
    MinSet s;

    for (auto _ : state) {
        state.PauseTiming();
        s.clear();
        state.ResumeTiming();
        for (int x : data)
            s.insert(x);
    }
}

static void BM_Extract(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = readRandomData(count);
    MinSet s;

    for (auto _ : state) {
        state.PauseTiming();
        s.clear();
        s.insert(data.begin(), data.end());
        state.ResumeTiming();

        while (!s.empty()) {
            benchmark::DoNotOptimize(*s.begin());
            s.erase(s.begin());
        }
    }
}

static void BM_Search(benchmark::State& state) {
    size_t count = state.range(0);
    MinSet s;
    auto data = readRandomData(count);
    s.insert(data.begin(), data.end());
    auto tests = readRandomDataTest(count);

    for (auto _ : state) {
        for (auto t : tests)
            benchmark::DoNotOptimize(s.find(t));
    }
}

static void BM_Remove(benchmark::State& state) {
    size_t count = state.range(0);
    MinSet s;
    auto data = readRandomData(count);
    auto tests = readRandomDataTest(count);

    for (auto _ : state) {
        state.PauseTiming();
        s.clear();
        s.insert(data.begin(), data.end());
        state.ResumeTiming();

        for (auto t : tests)
            benchmark::DoNotOptimize(s.erase(t));
    }
}

static void BM_SSearch(benchmark::State& state) {
    const size_t count = state.range(0);
    auto data = readRandomData(count);
    MinSet s;
    std::vector<int>tests(data.begin(), data.begin() + 0.05 * data.size());

    for (auto _ : state) {
        for(auto t : tests) {
            benchmark::DoNotOptimize(s.find(t));
        }
    }
}

static void BM_SearchM(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = readRandomData(count);
    auto test = readRandomDataTest(count);
    MinSet s;
    
    for (auto _ : state) {
        state.PauseTiming();
        s.clear();
        state.ResumeTiming();
        for (int x : data) 
        {
            s.insert(x);
        }

        for(int x : test)
        {
            auto res = s.find(x);
            benchmark::DoNotOptimize(res);
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

BENCHMARK(BM_Construct)->RangeMultiplier(4)->Range(256, 1<<20)
    ADD_STATS();
BENCHMARK(BM_PushRandom)->RangeMultiplier(2)->Range(256, 1<<20)
    ADD_STATS();
BENCHMARK(BM_PushSortedAsc)->RangeMultiplier(2)->Range(256, 1<<20)
    ADD_STATS();
BENCHMARK(BM_PushSortedDesc)->RangeMultiplier(2)->Range(256, 1<<20)
    ADD_STATS();
BENCHMARK(BM_Extract)->RangeMultiplier(2)->Range(256, 1<<20)
    ADD_STATS();
BENCHMARK(BM_Search)->RangeMultiplier(4)->Range(256, 1<<20)
    ADD_STATS();
BENCHMARK(BM_SSearch)->RangeMultiplier(4)->Range(256, 1<<20)
    ADD_STATS();
BENCHMARK(BM_Remove)->RangeMultiplier(4)->Range(256, 1<<20)
    ADD_STATS(); \
BENCHMARK(BM_SearchM)->RangeMultiplier(4)->Range(256, 1<<20)
    ADD_STATS();

BENCHMARK_MAIN();