#include <iostream>
#include <fstream>
#include <charconv>
#include <algorithm>
#include <cmath>
#include <vector>
#include <benchmark/benchmark.h>
#include "../../src/n_beap/n_beap.hpp"    


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



template<typename T, int N>
class BeapFixture : public benchmark::Fixture {
public:
    // NBeap<T, N> beap;

    void SetUp(const ::benchmark::State&) override {
    
        // beap = NBeap<T, N>();
    }

    void TearDown(const ::benchmark::State&) override { }
};


//  Benchmark constructor
template<typename T, int N>
static void BM_Construct(benchmark::State& state) {
    const size_t count = state.range(0);
    for (auto _ : state) {
        state.PauseTiming();
        auto data = readRandomData(count);
        state.ResumeTiming();

        NBeap<T, N> b(data);
        benchmark::DoNotOptimize(b);
    }
}

// Random input
template<typename T, int N>
static void BM_PushRandom(benchmark::State& state) {
    const size_t count = state.range(0);
    auto data = readRandomData(count);
    std::vector<int> vec;
    vec.reserve(count);
    NBeap<T, N> b(vec);  

    for (auto _ : state) {
        state.PauseTiming();
        b.clear();
        state.ResumeTiming();
        
        for (auto x : data)
             b.insert(x);
    }
}

// Benchmark insert ascending random data
template<typename T, int N>
static void BM_PushSortedAsc(benchmark::State& state) {
    const size_t count = state.range(0);
    auto data = readRandomData(count);
    std::sort(data.begin(), data.end());
    std::vector<int> vec;
    vec.reserve(count);
    NBeap<T, N> b(vec);  

    for (auto _ : state) {
        state.PauseTiming();
        b.clear();
        state.ResumeTiming();

        for (int x : data)
            b.insert(x);

    }
}

template<typename T, int N>
static void BM_PushSortedDesc(benchmark::State& state) {
    const size_t count = state.range(0);
    auto data = readRandomData(count);
    std::sort(data.begin(), data.end(), std::greater<int>());
    std::vector<int> vec;
    vec.reserve(count);
    NBeap<T, N> b(vec);  

    for (auto _ : state) {
        state.PauseTiming();
        b.clear();
        state.ResumeTiming();

        for (auto x : data)
            b.insert(x);

    }
}

template<typename T, int N>
static void BM_Extract(benchmark::State& state) {
    const size_t count = state.range(0);
    
    for (auto _ : state) {
        state.PauseTiming();
        auto data = readRandomData(count);
        NBeap<T, N> b(data);  
        state.ResumeTiming();

        for (size_t i = 0; i < count; i++) {
            benchmark::DoNotOptimize(b.extract());
        }
    }
}

template<typename T, int N>
static void BM_Search(benchmark::State& state) {
    const size_t count = state.range(0);
    auto data = readRandomData(count);
    auto tests = readRandomDataTest(count);

    NBeap<T, N> b(data);

    for (auto _ : state) {
        for(auto t : tests) {
            benchmark::DoNotOptimize(b.search(t));
        }
    }
}

template<typename T, int N>
static void BM_SSearch(benchmark::State& state) {
    const size_t count = state.range(0);
    auto data = readRandomData(count);
    std::vector<int>tests(data.begin(), data.begin() + 0.05 * data.size());

    NBeap<T, N> b(data);

    for (auto _ : state) {
        for(auto t : tests) {
            benchmark::DoNotOptimize(b.search(t));
        }
    }
}


template<typename T, int N>
static void BM_Remove(benchmark::State& state) {
    size_t count = state.range(0);
    auto tests = readRandomDataTest(count);

    for (auto _ : state) {
        state.PauseTiming();
        auto data = readRandomData(count);
        NBeap<T, N> b(data);
        state.ResumeTiming();

        for (auto t : tests)
            b.remove(t);
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

#define ADD_SSTATS() \
    ->Repetitions(5) \
    ->ComputeStatistics("p90", [](const std::vector<double>& v){ return percentile(v, 90.0); }) \
    ->ComputeStatistics("p95", [](const std::vector<double>& v){ return percentile(v, 95.0); }) \
    ->ComputeStatistics("p99", [](const std::vector<double>& v){ return percentile(v, 99.0); });

#define REGISTER_BEAP_BENCH(T, N) \
BENCHMARK_TEMPLATE(BM_Construct, T, N)->RangeMultiplier(4)->Range(256, 1<<20)\
    ADD_STATS();\
BENCHMARK_TEMPLATE(BM_PushRandom, T, N)->RangeMultiplier(2)->Range(256, 1<<20) \
    ADD_STATS(); \
BENCHMARK_TEMPLATE(BM_PushSortedAsc, T, N)->RangeMultiplier(2)->Range(256, 1<<20) \
    ADD_STATS(); \
BENCHMARK_TEMPLATE(BM_PushSortedDesc, T, N)->RangeMultiplier(2)->Range(256, 1<<20) \
    ADD_STATS(); \
BENCHMARK_TEMPLATE(BM_Extract, T, N)->RangeMultiplier(2)->Range(256, 1<<20) \
    ADD_STATS(); \
BENCHMARK_TEMPLATE(BM_Remove, T, N)->RangeMultiplier(2)->Range(256, 1<<20) \
    ADD_SSTATS(); \
BENCHMARK_TEMPLATE(BM_Search, T, N)->RangeMultiplier(2)->Range(256, 1<<20) \
    ADD_SSTATS(); \
BENCHMARK_TEMPLATE(BM_SSearch, T, N)->RangeMultiplier(2)->Range(256, 1<<20) \
    ADD_SSTATS();




#define REGISTER_SHORTER_BEAP_BENCH(T, N) \
BENCHMARK_TEMPLATE(BM_Construct, T, N)->RangeMultiplier(4)->Range(256, 1<<18)\
    ADD_SSTATS(); \
BENCHMARK_TEMPLATE(BM_PushRandom, T, N)->RangeMultiplier(4)->Range(256, 1<<18)\
    ADD_SSTATS(); \
BENCHMARK_TEMPLATE(BM_PushSortedAsc, T, N)->RangeMultiplier(4)->Range(256, 1<<18)\
    ADD_SSTATS(); \
BENCHMARK_TEMPLATE(BM_PushSortedDesc, T, N)->RangeMultiplier(2)->Range(256, 1<<18)\
    ADD_SSTATS(); \
BENCHMARK_TEMPLATE(BM_Extract, T, N)->RangeMultiplier(4)->Range(256, 1<<18)\
    ADD_SSTATS(); \
BENCHMARK_TEMPLATE(BM_Remove, T, N)->RangeMultiplier(4)->Range(256, 1<<18)\
    ADD_SSTATS(); \
BENCHMARK_TEMPLATE(BM_Search, T, N)->RangeMultiplier(4)->Range(256, 1<<18)\
    ADD_SSTATS(); \
BENCHMARK_TEMPLATE(BM_SSearch, T, N)->RangeMultiplier(4)->Range(256, 1<<18)\
    ADD_SSTATS();


// REGISTER_SHORTER_BEAP_BENCH(int, 1)
REGISTER_BEAP_BENCH(int, 1)
REGISTER_BEAP_BENCH(int, 2)
REGISTER_BEAP_BENCH(int, 3)
REGISTER_BEAP_BENCH(int, 4)
REGISTER_BEAP_BENCH(int, 5)
REGISTER_BEAP_BENCH(int, 6)
REGISTER_BEAP_BENCH(int, 7)
REGISTER_BEAP_BENCH(int, 8)
REGISTER_BEAP_BENCH(int, 9)
REGISTER_BEAP_BENCH(int, 10)


BENCHMARK_MAIN();
