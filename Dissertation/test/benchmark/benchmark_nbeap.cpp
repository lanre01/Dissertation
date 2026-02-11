#include <benchmark/benchmark.h>
#include <random>
#include "../../src/NBeap/NBeap.hpp"    

static std::mt19937 rng(42);

template<typename T>
std::vector<T> generateRandomData(size_t size) {
    std::uniform_int_distribution<T> dist(1, 1'000'000);
    std::vector<T> v(size);
    for (auto& x : v) x = dist(rng);
    return v;
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
    auto data = generateRandomData<T>(count);
    
    for (auto _ : state) {
        state.PauseTiming();
        NBeap<T, N> b;  
        b.reserve(count);
        state.ResumeTiming();
        
        for (auto& x : data)
            b.insert(x);

        //benchmark::DoNotOptimize(b);
    }
}

// Sorted input - best case for minNBeap
template<typename T, int N>
static void BM_PushSorted(benchmark::State& state) {
    const size_t count = state.range(0);
    auto data = generateRandomData<T>(count);
    std::sort(data.begin(), data.end());

    for (auto _ : state) {
        state.PauseTiming();
        NBeap<T, N> b;
        b.reserve(count);
        state.ResumeTiming();

        for (auto& x : data)
            b.insert(x);

        //benchmark::DoNotOptimize(b);
    }
}

template<typename T, int N>
static void BM_PushSortedDesc(benchmark::State& state) {
    const size_t count = state.range(0);
    auto data = generateRandomData<T>(count);
    std::sort(data.begin(), data.end(), std::greater<int>());

    for (auto _ : state) {
        state.PauseTiming();
        NBeap<T, N> b;
        b.reserve(count);
        state.ResumeTiming();

        for (auto& x : data)
            b.insert(x);

        //benchmark::DoNotOptimize(b);
    }
}

// Benchmark extract()
template<typename T, int N>
static void BM_Extract(benchmark::State& state) {
    const size_t count = state.range(0);
    auto data = generateRandomData<T>(count);

    for (auto _ : state) {
        state.PauseTiming();
        NBeap<T, N> b;
        b.reserve(count);
        for (auto& x : data) b.insert(x);
        state.ResumeTiming();

        for (size_t i = 0; i < count; ++i) {
            b.extract();
        }

        //benchmark::DoNotOptimize(b);
    }
}

// Benchmark search()
template<typename T, int N>
static void BM_Search(benchmark::State& state) {
    const size_t count = state.range(0);
    auto data = generateRandomData<T>(count);

    NBeap<T, N> b;
    for (auto& x : data) b.insert(x);

    std::uniform_int_distribution<T> dist(1, 1'000'000);

    for (auto _ : state) {
        T query = dist(rng);
        benchmark::DoNotOptimize(b.search(query));
    }
}



#define REGISTER_BEAP_BENCH(T, N) \
BENCHMARK_TEMPLATE(BM_Construct, T, N)->RangeMultiplier(4)->Range(256, 1<<20);\
BENCHMARK_TEMPLATE(BM_PushRandom, T, N)->RangeMultiplier(2)->Range(256, 1<<20); \
BENCHMARK_TEMPLATE(BM_PushSorted, T, N)->RangeMultiplier(2)->Range(256, 1<<20); \
BENCHMARK_TEMPLATE(BM_Extract, T, N)->RangeMultiplier(2)->Range(256, 1<<20); \
BENCHMARK_TEMPLATE(BM_Search, T, N)->RangeMultiplier(2)->Range(256, 1<<20);

#define REGISTER_SHORTER_BEAP_BENCH(T, N) \
BENCHMARK_TEMPLATE(BM_Construct, T, N)->RangeMultiplier(4)->Range(256, 1<<20);\
BENCHMARK_TEMPLATE(BM_PushRandom, T, N)->RangeMultiplier(4)->Range(256, 1<<20); \
BENCHMARK_TEMPLATE(BM_PushSorted, T, N)->RangeMultiplier(4)->Range(256, 1<<20); \
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
