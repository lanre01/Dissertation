#include <iostream>
#include <fstream>
#include <charconv>
#include <benchmark/benchmark.h>
#include <random>
#include <algorithm>
#include <map>       
#include <vector>

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


static void BM_Construct(benchmark::State& state) {
   
    for (auto _ : state) {
        std::map<int, int> m;
        benchmark::DoNotOptimize(m);
    }
}

// Benchmark pushing random data
static void BM_PushRandom(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = readRandomData(count);
    std::map<int, int> m;

    for (auto _ : state) {    
        state.PauseTiming();
        m.clear();
        state.ResumeTiming();
        for (int x : data) 
            m[x] = x;
    }
    
}

// Benchmark push ascending random data
static void BM_PushSortedAsc(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = readRandomData(count);
    std::sort(data.begin(), data.end());
    std::map<int, int> m;
    for (auto _ : state) {
        state.PauseTiming();
        m.clear();
        state.ResumeTiming();
        
        for (int x : data) 
            m[x] = x;
    }
}

// Benchmark push descending random data 
static void BM_PushSortedDesc(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = readRandomData(count);
    std::sort(data.begin(), data.end(), std::greater<int>());

    std::map<int, int> m;

    for (auto _ : state) {
        state.PauseTiming();
        m.clear();
        state.ResumeTiming();    
        
        for (int x : data) 
            m[x] = x;
    }
}

// Benchmark minimum
static void BM_Extract(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = readRandomData(count);
    std::map<int, int> m;

    for (auto _ : state) {
        state.PauseTiming();
        m.clear();
        for (int x : data) 
            m[x] = x;
        state.ResumeTiming();
        
        while(!m.empty()) {
            benchmark::DoNotOptimize(m.erase(m.begin()));
        }
    }
}

static void BM_Search(benchmark::State& state)
{
    size_t count = state.range(0);
    std::map<int, int> m;
    auto data = readRandomData(count);
    for (int x : data) 
        m[x] = x;

    auto tests = readRandomDataTest(count);

    for(auto _ : state)
    {
        for(auto t : tests){
            benchmark::DoNotOptimize(m.find(t));
        }
    }
}


// Benchmark remove(value)
static void BM_Remove(benchmark::State& state) {
    size_t count = state.range(0);
    std::map<int, int> m;    
    auto data = readRandomData(count);
    auto tests = readRandomDataTest(count);

    for (auto _ : state) {
        state.PauseTiming();
        m.clear();
        for (auto x : data) 
            m[x] = x;
        state.ResumeTiming();

        for (auto t : tests){
            benchmark::DoNotOptimize(m.erase(t));
        }
    }
}


BENCHMARK(BM_Construct)->RangeMultiplier(4)->Range(256, 1<<20);

BENCHMARK(BM_PushRandom)->RangeMultiplier(2)->Range(256, 1<<20); 
BENCHMARK(BM_PushSortedAsc)->RangeMultiplier(2)->Range(256, 1<<20); 
BENCHMARK(BM_PushSortedDesc)->RangeMultiplier(2)->Range(256, 1<<20); 

BENCHMARK(BM_Extract)->RangeMultiplier(2)->Range(256, 1<<20); 

BENCHMARK(BM_Search)->RangeMultiplier(4)->Range(256, 1<<20); 

BENCHMARK(BM_Remove)->RangeMultiplier(4)->Range(256, 1<<20); 


BENCHMARK_MAIN();