#include <iostream>
#include <fstream>
#include <charconv>
#include <benchmark/benchmark.h>
#include <vector>


inline std::vector<int> readRandomData(size_t n)
{
    std::ifstream myfile ("numbers.txt");
    std::vector<int> vec;
    std::string line; 
    size_t i = 0;
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
    size_t i = 0;
    size_t j = 0.975 * count;
    size_t final = count + 0.025 * count;

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


// Benchmark heap construction
static void BM_Construct(benchmark::State& state) {
    size_t count = state.range(0);
    
    for (auto _ : state) {
        std::vector<int> v;
        v.reserve(count);
        benchmark::DoNotOptimize(v);
        /*std::priority_queue<int, std::vector<int>, std::greater<int>> pq(
            std::greater<int>(),
            std::move(v)
        );
        benchmark::DoNotOptimize(pq);*/
    }
}

// Benchmark pushing random data
static void BM_PushRandom(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = readRandomData(count);
    std::vector<int> v;
    v.reserve(count);

    for (auto _ : state) {
        
        //std::make_heap(v.begin(), v.end(), std::greater<>{});
        /*std::priority_queue<int, std::vector<int>, std::greater<int>> pq(
            std::greater<int>(),
            std::move(v)
        );*/

        state.PauseTiming();
        v.clear();
        state.ResumeTiming();

        for (int x : data){
            v.push_back(x);
            std::push_heap(v.begin(), v.end(), std::greater<int>());
        }
        //pq.push(x);
    }
}

// Benchmark push ascending random data
static void BM_PushSortedAsc(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = readRandomData(count);
    std::sort(data.begin(), data.end());
    std::vector<int> v;
    v.reserve(count);

    for (auto _ : state) {
        state.PauseTiming();
        v.clear();
        state.ResumeTiming();
        //std::make_heap(v.begin(), v.end(), std::greater<>{});
        /*std::priority_queue<int, std::vector<int>, std::greater<int>> pq(
            std::greater<int>(), 
            std::move(v)
        );*/

        for (int x : data) 
        {
            v.push_back(x);
            std::push_heap(v.begin(), v.end(), std::greater<int>());
            //pq.push(x);
        }
    }

    state.SetItemsProcessed(state.iterations() * count);
}

// Benchmark push descending random data 
static void BM_PushSortedDesc(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = readRandomData(count);
    std::sort(data.begin(), data.end(), std::greater<int>());
    std::vector<int> v;
    v.reserve(count);

    for (auto _ : state) {
        
        state.PauseTiming();
        v.clear();
        state.ResumeTiming();
        //std::make_heap(v.begin(), v.end(), std::greater<>{});
        /*std::priority_queue<int, std::vector<int>, std::greater<int>> pq(
            std::greater<int>(),
            std::move(v)
        );*/

        for (int x : data) 
        {
            v.push_back(x);
            std::push_heap(v.begin(), v.end(), std::greater<int>());
            //pq.push(x);
        }
    }

    state.SetItemsProcessed(state.iterations() * count);
}

// Benchmark pop
static void BM_Extract(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = readRandomData(count);
    std::vector<int> v;
    v.reserve(count);

    for (auto _ : state) {
        state.PauseTiming();
        
        //std::make_heap(v.begin(), v.end(), std::greater<>{});
        /*std::priority_queue<int, std::vector<int>, std::greater<int>> pq(
            std::greater<int>(),
            std::move(v)
        );*/
        v.clear();
        for (int x : data) 
        {
            //pq.push(x);
            v.push_back(x);
            std::push_heap(v.begin(), v.end());
        }
        state.ResumeTiming();

        for (size_t i = 0; i < count; i++) {
            //pq.pop();
            std::pop_heap(v.begin(), v.end(), std::greater<>{});
            v.pop_back();
        }
    }
}

static void BM_Search(benchmark::State& state)
{
    size_t count = state.range(0);

    auto v = readRandomData(count);
    auto tests = readRandomDataTest(count);
    std::make_heap(v.begin(), v.end(), std::greater<>{});


    for(auto _ : state)
    {
        for (auto q : tests)
            for(size_t i = 0; i < count; i++)
            {
                if(v[i] == q) {
                    auto t = v[i];
                    benchmark::DoNotOptimize(t);
                    break;
                }
            }
    }

    state.SetItemsProcessed(state.iterations() * count);
}


BENCHMARK(BM_Construct)->RangeMultiplier(4)->Range(256, 1<<20);

BENCHMARK(BM_PushRandom)->RangeMultiplier(2)->Range(256, 1<<20);
BENCHMARK(BM_PushSortedAsc)->RangeMultiplier(2)->Range(256, 1<<20);
BENCHMARK(BM_PushSortedDesc)->RangeMultiplier(2)->Range(256, 1<<20);

BENCHMARK(BM_Extract)->RangeMultiplier(2)->Range(256, 1<<20);

BENCHMARK(BM_Search)->RangeMultiplier(4)->Range(256, 1<<20); 

BENCHMARK_MAIN();
