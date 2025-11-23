#include <benchmark/benchmark.h>
#include <random>
#include "../../src/nBeap/nBeap.hpp"    

static std::mt19937 rng(42);

template<typename T>
std::vector<T> generateRandomData(size_t size) {
    std::uniform_int_distribution<T> dist(1, 1'000'000);
    std::vector<T> v(size);
    for (auto& x : v) x = dist(rng);
    return v;
}

// ----------------------------------------
// Benchmark Fixture
// ----------------------------------------

template<typename T, int N>
class BeapFixture : public benchmark::Fixture {
public:
    nBeap<T, N> beap;

    void SetUp(const ::benchmark::State&) override {
    
        beap = nBeap<T, N>();
    }

    void TearDown(const ::benchmark::State&) override { }
};


//  Benchmark constructor
template<typename T, int N>
static void BM_Construct(benchmark::State& state) {
    for (auto _ : state) {
        nBeap<T, N> b;
        benchmark::DoNotOptimize(b);
    }
}

// Benchmark push() – random input
template<typename T, int N>
static void BM_PushRandom(benchmark::State& state) {
    const size_t count = state.range(0);
    auto data = generateRandomData<T>(count);

    for (auto _ : state) {
        state.PauseTiming();
        nBeap<T, N> b;   // fresh beap
        state.ResumeTiming();

        for (auto& x : data)
            b.push(x);

        benchmark::DoNotOptimize(b);
    }
}

// Benchmark push() – sorted input (worst-case?)
template<typename T, int N>
static void BM_PushSorted(benchmark::State& state) {
    const size_t count = state.range(0);
    auto data = generateRandomData<T>(count);
    std::sort(data.begin(), data.end());

    for (auto _ : state) {
        state.PauseTiming();
        nBeap<T, N> b;
        state.ResumeTiming();

        for (auto& x : data)
            b.push(x);

        benchmark::DoNotOptimize(b);
    }
}

// Benchmark remove_min()
template<typename T, int N>
static void BM_RemoveMin(benchmark::State& state) {
    const size_t count = state.range(0);
    auto data = generateRandomData<T>(count);

    for (auto _ : state) {
        state.PauseTiming();
        nBeap<T, N> b;
        for (auto& x : data) b.push(x);
        state.ResumeTiming();

        for (size_t i = 0; i < count; ++i) {
            b.remove_min();
        }

        benchmark::DoNotOptimize(b);
    }
}

// Benchmark search()
template<typename T, int N>
static void BM_Search(benchmark::State& state) {
    const size_t count = state.range(0);
    auto data = generateRandomData<T>(count);

    nBeap<T, N> b;
    for (auto& x : data) b.push(x);

    std::uniform_int_distribution<T> dist(1, 1'000'000);

    for (auto _ : state) {
        T query = dist(rng);
        benchmark::DoNotOptimize(b.search(query));
    }
}

// Mixed workload benchmark - push + search + remove_min
template<typename T, int N>
static void BM_MixedWorkload(benchmark::State& state) {
    const size_t operations = state.range(0);
    nBeap<T, N> b;

    std::uniform_int_distribution<int> opdist(0, 2);
    std::uniform_int_distribution<T> valdist(1, 1'000'000);

    for (auto _ : state) {
        int op = opdist(rng);

        if (op == 0) {
            // push
            b.push(valdist(rng));
        } else if (op == 1) {
            // search
            benchmark::DoNotOptimize(b.search(valdist(rng)));
        } else if (op == 2 && b.size > 0) {
            // remove_min
            b.remove_min();
        }
    }
}

#define REGISTER_BEAP_BENCH(T, N) \
BENCHMARK_TEMPLATE(BM_Construct, T, N); \
BENCHMARK_TEMPLATE(BM_PushRandom, T, N)->Arg(1000)->Arg(10000)->Arg(50000); \
BENCHMARK_TEMPLATE(BM_PushSorted, T, N)->Arg(1000)->Arg(10000)->Arg(50000); \
BENCHMARK_TEMPLATE(BM_RemoveMin, T, N)->Arg(1000)->Arg(10000); \
BENCHMARK_TEMPLATE(BM_Search, T, N)->Arg(10000); \
BENCHMARK_TEMPLATE(BM_MixedWorkload, T, N)->Arg(50000);


// Benchmark for int and N=2,3 (you can add more)
REGISTER_BEAP_BENCH(int, 2)
REGISTER_BEAP_BENCH(int, 3)
REGISTER_BEAP_BENCH(int, 4)


BENCHMARK_MAIN();
