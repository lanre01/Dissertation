#include <iostream>
#include <fstream>
#include <benchmark/benchmark.h>
#include <charconv>
#include "../../src/beap/beap.hpp"  



/*static std::mt19937 rng(42);

std::vector<int> generateRandomData(size_t n) {
    std::uniform_int_distribution<int> dist(1, 1'000'000);
    std::vector<int> v(n);
    for (auto& x : v) x = dist(rng);
    return v;
}*/

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
        Beap<int> b;
        b.reserve(count);
        benchmark::DoNotOptimize(b);
    }
}

static void BM_BulkInsert(benchmark::State& state)
{
    size_t count = state.range(0);

    for(auto _ : state)
    {
        state.PauseTiming();
        auto data = readRandomData(count);
        state.ResumeTiming();
        Beap<int> b = Beap<int>(data);
        benchmark::DoNotOptimize(b);
    }
}

// Benchmark pushing random data
static void BM_PushRandom(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = readRandomData(count);
    Beap<int> b;
    b.reserve(count);

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
    Beap<int> b;
    b.reserve(count);

    for (auto _ : state) {
        state.PauseTiming();
        b.clear();
        state.ResumeTiming();

        for (int x : data)
            b.insert(x);
    }

    ////state.SetItemsProcessed(state.iterations() * count);

}


// Benchmark insert descending random data 
static void BM_PushSortedDesc(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = readRandomData(count);
    std::sort(data.begin(), data.end(), std::greater<int>());
    Beap<int> b;
    b.reserve(count);

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
    auto data = readRandomData(count);
    Beap<int> b;
    b.reserve(count);

    for (auto _ : state) {
        state.PauseTiming();
        b.clear();
        for(auto d : data)
            b.insert(d);
        state.ResumeTiming();
        
        for (size_t i = 0; i < count; i++) {
            benchmark::DoNotOptimize(b.extract());
        }
    }
}

// Benchmark search 
static void BM_Search(benchmark::State& state) {
    size_t count = state.range(0);
    
    Beap<int> b;
    b.reserve(count);
    auto data = readRandomData(count);
    for (int x : data) b.insert(x);

    auto tests = readRandomDataTest(count);

    
    for (auto _ : state) {
        for(auto t : tests) {
            benchmark::DoNotOptimize(b.search(t));
        }
    }
}




// Benchmark remove(value)
static void BM_Remove(benchmark::State& state) {
    size_t count = state.range(0);
    Beap<int> b;
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
}



BENCHMARK(BM_Construct)->RangeMultiplier(4)->Range(256, 1<<20); 

BENCHMARK(BM_BulkInsert)->RangeMultiplier(2)->Range(256, 1<<20);
BENCHMARK(BM_PushRandom)->RangeMultiplier(2)->Range(256, 1<<20); 
BENCHMARK(BM_PushSortedAsc)->RangeMultiplier(2)->Range(256, 1<<20); 
BENCHMARK(BM_PushSortedDesc)->RangeMultiplier(2)->Range(256, 1<<20); 
BENCHMARK(BM_Extract)->RangeMultiplier(2)->Range(256, 1<<20); 
BENCHMARK(BM_Search)->RangeMultiplier(2)->Range(256, 1<<20); 
BENCHMARK(BM_Remove)->RangeMultiplier(2)->Range(256, 1<<20); 



BENCHMARK_MAIN();
