#include <benchmark/benchmark.h>
#include <random>
#include <algorithm>
#include <queue>       // for std::priority_queue
#include <vector>

// Same RNG as Beap benchmark
static std::mt19937 rng(42);

// Same helper used in beap version
std::vector<int> generateRandomData(size_t n) {
    std::uniform_int_distribution<int> dist(1, 1'000'000);
    std::vector<int> v(n);
    for (auto& x : v) x = dist(rng);
    return v;
}

// --------------------------------------------------------
// 1. Constructor Benchmark
// --------------------------------------------------------

static void BM_Construct(benchmark::State& state) {
    for (auto _ : state) {
        std::priority_queue<int> pq;
        benchmark::DoNotOptimize(pq);
    }
}

// --------------------------------------------------------
// 2. Benchmark push() with random data
// --------------------------------------------------------

static void BM_PushRandom(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = generateRandomData(count);

    for (auto _ : state) {
        state.PauseTiming();
        std::priority_queue<int> pq;
        state.ResumeTiming();

        for (int x : data)
            pq.push(x);
    }
}

// --------------------------------------------------------
// 3. Benchmark push() with sorted ascending input
// --------------------------------------------------------

static void BM_PushSortedAsc(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = generateRandomData(count);
    std::sort(data.begin(), data.end());

    for (auto _ : state) {
        state.PauseTiming();
        std::priority_queue<int> pq;
        state.ResumeTiming();

        for (int x : data) 
            pq.push(x);
    }
}

// --------------------------------------------------------
// 4. Benchmark push() with sorted descending input
// --------------------------------------------------------

static void BM_PushSortedDesc(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = generateRandomData(count);
    std::sort(data.begin(), data.end(), std::greater<int>());

    for (auto _ : state) {
        state.PauseTiming();
        std::priority_queue<int> pq;
        state.ResumeTiming();

        for (int x : data) 
            pq.push(x);
    }
}

// --------------------------------------------------------
// 5. Benchmark pop()
// --------------------------------------------------------

static void BM_Pop(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = generateRandomData(count);

    for (auto _ : state) {
        std::priority_queue<int> pq;
        for (int x : data) pq.push(x);

        for (size_t i = 0; i < count; i++) {
            pq.pop();
        }
    }
}

// --------------------------------------------------------
// 6. Benchmark search() (simulated)
// Priority queue cannot search efficiently.
// We simulate the cost: scan a vector copy.
// --------------------------------------------------------

static void BM_Search(benchmark::State& state) {
    size_t count = state.range(0);
    auto data = generateRandomData(count);

    std::priority_queue<int> pq;
    for (int x : data) pq.push(x);

    std::uniform_int_distribution<int> dist(1, 1'000'000);

    for (auto _ : state) {
        int q = dist(rng);

        // simulate linear search (closest comparable operation)
        bool found = false;
        auto tmp = data; // simulate scanning container
        for (int x : tmp)
            if (x == q) { found = true; break; }

        benchmark::DoNotOptimize(found);
    }
}

// --------------------------------------------------------
// 7. Benchmark remove(value)
// Priority queue has no fast remove(value).
// Simulate by rebuilding a heap without that value.
// --------------------------------------------------------

static void BM_RemoveValue(benchmark::State& state) {
    size_t count = state.range(0);
    
    for (auto _ : state) {
        auto data = generateRandomData(count);

        state.PauseTiming();
        std::priority_queue<int> pq;
        for (int x : data) pq.push(x);
        state.ResumeTiming();

        // remove each value by rebuilding heap (closest comparable cost)
        for (int val : data) {
            std::vector<int> tmp;
            tmp.reserve(pq.size());

            // extract all into tmp
            while (!pq.empty()) {
                int top = pq.top();
                pq.pop();
                if (top != val)
                    tmp.push_back(top);
            }

            // rebuild heap
            for (int x : tmp) pq.push(x);
        }
    }
}

// --------------------------------------------------------
// 8. Mixed workload
// --------------------------------------------------------

static void BM_MixedWorkload(benchmark::State& state) {
    std::priority_queue<int> pq;

    std::uniform_int_distribution<int> valdist(1, 1'000'000);
    std::uniform_int_distribution<int> opdist(1, 100);

    for (auto _ : state) {
        int op = opdist(rng);

        if (op <= 40) {
            pq.push(valdist(rng));
        }
        else if (op <= 80) {
            if (!pq.empty())
                benchmark::DoNotOptimize(pq.top());
        }
        else {
            if (!pq.empty()) pq.pop();
        }
    }
}

// --------------------------------------------------------
// Register benchmarks (EXACT SAME ARGS AS Beap)
// --------------------------------------------------------

BENCHMARK(BM_Construct);

BENCHMARK(BM_PushRandom)->Arg(1000)->Arg(5000)->Arg(10000);
BENCHMARK(BM_PushSortedAsc)->Arg(1000)->Arg(5000)->Arg(10000);
BENCHMARK(BM_PushSortedDesc)->Arg(1000)->Arg(5000)->Arg(10000);

BENCHMARK(BM_Pop)->Arg(1000)->Arg(5000)->Arg(10000);

// Comparable but commented out to match your Beap setup
//BENCHMARK(BM_Search)->Arg(10000);
//BENCHMARK(BM_RemoveValue)->Arg(2000)->Arg(5000);
//BENCHMARK(BM_MixedWorkload)->Arg(50000);

BENCHMARK_MAIN();
