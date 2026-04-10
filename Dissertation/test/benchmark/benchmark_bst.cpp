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
    std::ifstream myfile("numbers.txt");
    std::vector<int> vec;
    std::string line;
    int i = 0;
    while (getline(myfile, line) && i < n)
    {
        int result;
        auto [prt, ec] = std::from_chars(line.data(), line.data() + line.size(), result);
        if (ec != std::errc::invalid_argument)
        {
            i++;
            vec.push_back(result);
        }
    }
    myfile.close();
    return vec;
}

inline std::vector<int> readRandomDataTest(size_t count)
{
    std::ifstream myfile("numbers.txt");
    std::vector<int> vec;
    std::string line;
    int i = 0;
    int j = 0.975 * count;
    int final_count = count + 0.025 * count;

    while (getline(myfile, line) && i < j) i++;

    while (getline(myfile, line) && j < final_count)
    {
        int result;
        auto [prt, ec] = std::from_chars(line.data(), line.data() + line.size(), result);
        if (ec != std::errc::invalid_argument)
        {
            j++;
            vec.push_back(result);
        }
    }
    myfile.close();
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