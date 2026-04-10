#include <iostream>
#include <fstream>
#include <benchmark/benchmark.h>
#include <charconv>
#include "../../src/beap/beap.hpp"  

inline std::vector<int> readRandomData(size_t n)
{
    std::ifstream myfile ("numbers.txt");
    std::vector<int> vec;
    std::string line; 
    int i = 0;
    while (getline(myfile, line) && i < n)
    {
        int result;
        auto [prt, ec] = std::from_chars(line.data(), line.data() + line.size(), result);
        if(ec != std::errc::invalid_argument)
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
    std::ifstream myfile ("numbers.txt");
    std::vector<int> vec;
    std::string line; 
    int i = 0;
    int j = 0.975 * count;
    int final = count + 0.025 * count;

    while (getline(myfile, line) && i < j) i++;

    while(getline(myfile, line) && j < final )
    {
        int result;
        auto [prt, ec] = std::from_chars(line.data(), line.data() + line.size(), result);
        if(ec != std::errc::invalid_argument)
        {
            j++;
            vec.push_back(result);
        }
    }

    myfile.close();
    return vec;
}


// Benchmark beap construction
static void BM_Construct(benchmark::State& state) {
    size_t count = state.range(0);

    for (auto _ : state) {
        state.PauseTiming();
        auto data = readRandomData(count);
        state.ResumeTiming();

        Beap<int> b(data);
        benchmark::DoNotOptimize(b);
    }
}

// Benchmark pushing random data
static void BM_PushRandom(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = readRandomData(count);
    std::vector<int> vec;
    vec.reserve(count);
    Beap<int> b(vec);
    

    for (auto _ : state) {
        state.PauseTiming();
        b.clear();
        state.ResumeTiming();
        
        for (int x : data)
            b.insert(x);
    }
}


// Benchmark insert ascending random data
static void BM_PushSortedAsc(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = readRandomData(count);
    std::sort(data.begin(), data.end());
    std::vector<int> vec;
    vec.reserve(count);
    Beap<int> b(vec);

    for (auto _ : state) {
        state.PauseTiming();
        b.clear();
        state.ResumeTiming();

        for (int x : data)
            b.insert(x);
    }
}


// Benchmark insert descending random data 
static void BM_PushSortedDesc(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = readRandomData(count);
    std::sort(data.begin(), data.end(), std::greater<int>());
    std::vector<int> vec;
    vec.reserve(count);
    Beap<int> b(vec);

    for (auto _ : state) {
        state.PauseTiming();
        b.clear();
        state.ResumeTiming();

        for (int x : data)
            b.insert(x);
    }
}

// Benchmark Extract
static void BM_Extract(benchmark::State& state) {
    size_t count = state.range(0);
    
    

    for (auto _ : state) {
        state.PauseTiming();
        auto data = readRandomData(count);
        Beap<int> b(data);
        state.ResumeTiming();
        
        for (size_t i = 0; i < count; i++) {
            benchmark::DoNotOptimize(b.extract());
        }
    }
}

static void BM_Search(benchmark::State& state) {
    size_t count = state.range(0);
    
    auto data = readRandomData(count);
    Beap<int> b(data);
    auto tests = readRandomDataTest(count);

    
    for (auto _ : state) {
        for(auto t : tests) {
            benchmark::DoNotOptimize(b.search(t));
        }
    }
}



static void BM_Remove(benchmark::State& state) {
    size_t count = state.range(0);
   
    auto tests = readRandomDataTest(count);

    for (auto _ : state) {
        state.PauseTiming();
        auto data = readRandomData(count);
        Beap<int> b(data);
        state.ResumeTiming();

        for (auto t : tests)
            b.remove(t);
    }
}

static void BM_SSearch(benchmark::State& state) {
    const size_t count = state.range(0);
    auto data = readRandomData(count);
    std::vector<int>tests(data.begin(), data.begin() + 0.05 * data.size());

    Beap<int> b(data);

    for (auto _ : state) {
        for(auto t : tests) {
            benchmark::DoNotOptimize(b.search(t));
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

BENCHMARK(BM_Construct)->RangeMultiplier(4)->Range(256, 1<<20) \
    ADD_STATS(); 

BENCHMARK(BM_PushRandom)->RangeMultiplier(2)->Range(256, 1<<20) \
    ADD_STATS(); 
BENCHMARK(BM_PushSortedAsc)->RangeMultiplier(2)->Range(256, 1<<20) \
    ADD_STATS(); 
BENCHMARK(BM_PushSortedDesc)->RangeMultiplier(2)->Range(256, 1<<20) \
    ADD_STATS(); 
BENCHMARK(BM_Extract)->RangeMultiplier(2)->Range(256, 1<<20) \
    ADD_STATS(); 
BENCHMARK(BM_Search)->RangeMultiplier(2)->Range(256, 1<<20) \
    ADD_STATS(); 
BENCHMARK(BM_SSearch)->RangeMultiplier(2)->Range(256, 1<<20) \
ADD_STATS(); 
BENCHMARK(BM_Remove)->RangeMultiplier(2)->Range(256, 1<<20) \
    ADD_STATS(); 



BENCHMARK_MAIN();
