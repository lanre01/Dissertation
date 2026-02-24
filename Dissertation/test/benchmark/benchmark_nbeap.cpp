#include <iostream>
#include <fstream>
#include <charconv>
#include <benchmark/benchmark.h>
#include "../../src/NBeap/NBeap.hpp"    


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
    int j = 0.75 * count;
    int final = count + 0.25 * count;

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



template<typename T, int N>
class BeapFixture : public benchmark::Fixture {
public:
    NBeap<T, N> beap;

    void SetUp(const ::benchmark::State&) override {
    
        beap = NBeap<T, N>();
    }

    void TearDown(const ::benchmark::State&) override { }
};


//  Benchmark constructor
template<typename T, int N>
static void BM_Construct(benchmark::State& state) {
    const size_t count = state.range(0);
    for (auto _ : state) {
        NBeap<T, N> b;
        b.reserve(count);
        benchmark::DoNotOptimize(b);
    }
}

// Random input
template<typename T, int N>
static void BM_PushRandom(benchmark::State& state) {
    const size_t count = state.range(0);
    auto data = readRandomData(count);
    NBeap<T, N> b;  
    b.reserve(count);

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
    NBeap<T, N> b;
    b.reserve(count);

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
    NBeap<T, N> b;
    b.reserve(count);

    for (auto _ : state) {
        state.PauseTiming();
        b.clear();
        state.ResumeTiming();

        for (auto x : data)
            b.insert(x);

    }
}

// Benchmark extract()
template<typename T, int N>
static void BM_Extract(benchmark::State& state) {
    const size_t count = state.range(0);
    auto data = readRandomData(count);
    NBeap<T, N> b;
    b.reserve(count);

    for (auto _ : state) {
        state.PauseTiming();
        b.clear();
        for (auto x : data) b.insert(x);
        state.ResumeTiming();

        for (size_t i = 0; i < count; i++) {
            benchmark::DoNotOptimize(b.extract());
        }
    }
}

// Benchmark search()
template<typename T, int N>
static void BM_Search(benchmark::State& state) {
    const size_t count = state.range(0);
    auto data = readRandomData(count);
    auto tests = readRandomDataTest(count);

    NBeap<T, N> b;
    b.reserve(count);
    for (auto x : data) b.insert(x);

    for (auto _ : state) {
        for(auto t : tests) {
            benchmark::DoNotOptimize(b.search(t));
        }
    }
}

// Benchmark remove(value)
template<typename T, int N>
static void BM_Remove(benchmark::State& state) {
    size_t count = state.range(0);
    NBeap<int> b;
    b.reserve(count);
    auto tests = readRandomDataTest(count);
    auto data = readRandomData(count);

    for (auto _ : state) {
        state.PauseTiming();
        b.clear();
        for (auto x : data) b.insert(x);
        state.ResumeTiming();

        for (auto t : tests)
            b.remove(t);
    }
    

    //state.SetItemsProcessed(state.iterations() * count);
}



#define REGISTER_BEAP_BENCH(T, N) \
BENCHMARK_TEMPLATE(BM_Construct, T, N)->RangeMultiplier(4)->Range(256, 1<<20);\
BENCHMARK_TEMPLATE(BM_PushRandom, T, N)->RangeMultiplier(2)->Range(256, 1<<20); \
BENCHMARK_TEMPLATE(BM_PushSortedAsc, T, N)->RangeMultiplier(2)->Range(256, 1<<20); \
BENCHMARK_TEMPLATE(BM_PushSortedDesc, T, N)->RangeMultiplier(2)->Range(256, 1<<20); \
BENCHMARK_TEMPLATE(BM_Extract, T, N)->RangeMultiplier(2)->Range(256, 1<<20); \
BENCHMARK_TEMPLATE(BM_Search, T, N)->RangeMultiplier(2)->Range(256, 1<<20);

#define REGISTER_SHORTER_BEAP_BENCH(T, N) \
BENCHMARK_TEMPLATE(BM_Construct, T, N)->RangeMultiplier(4)->Range(256, 1<<20);\
BENCHMARK_TEMPLATE(BM_PushRandom, T, N)->RangeMultiplier(4)->Range(256, 1<<20); \
BENCHMARK_TEMPLATE(BM_PushSortedAsc, T, N)->RangeMultiplier(4)->Range(256, 1<<20); \
BENCHMARK_TEMPLATE(BM_PushSortedDesc, T, N)->RangeMultiplier(2)->Range(256, 1<<20); \
BENCHMARK_TEMPLATE(BM_Extract, T, N)->RangeMultiplier(4)->Range(256, 1<<20); \
BENCHMARK_TEMPLATE(BM_Search, T, N)->RangeMultiplier(4)->Range(256, 1<<20);


REGISTER_SHORTER_BEAP_BENCH(int, 1)
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
