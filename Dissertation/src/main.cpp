

#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <algorithm>
#include <string>
#include <queue>  
#include <chrono>

#include "Heap/Heap.hpp"
#include "Beap/Beap.hpp"
#include "nBeap/nBeap.hpp"

static std::mt19937 rng(42);

std::vector<int> generateRandomData(size_t n) {
    std::uniform_int_distribution<int> dist(1, 1'000'000);
    std::vector<int> v(n);
    for (auto& x : v) x = dist(rng);
    return v;
}

void measure_push_cost_per_operation()
{
    /*size_t MAX_ELEMENT = 1000;
    Beap<int> beap;
    beap.container.reserve(MAX_ELEMENT);
    std::vector<int> v;
    v.reserve(MAX_ELEMENT);
    std::priority_queue<int, std::vector<int>> pq(
        std::less<int>(), 
        std::move(v)
    );
    

    
    std::vector<long long> timings_beap(MAX_ELEMENT);
    std::vector<long long> timings_heap(MAX_ELEMENT);
    auto data = generateRandomData(MAX_ELEMENT);

    for (size_t i = 0; i < MAX_ELEMENT; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        beap.push(data[i]);
        auto end = std::chrono::high_resolution_clock::now();
        if(i <= 0)
        {
            timings_beap[i] = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        }
        else {
            timings_beap[i] = timings_beap[i-1] + std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        }
    }

    for (size_t i = 0; i < MAX_ELEMENT; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        pq.push(data[i]);
        auto end = std::chrono::high_resolution_clock::now();
        if(i <= 0)
        {
            timings_heap[i] = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        }
        else {
            timings_heap[i] = timings_heap[i-1] + std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        }
    }

    std::string filename = "push.csv";
    std::ofstream csvFile(filename);

    csvFile << "beap,heap" << std::endl;
    for(size_t i = 0; i < MAX_ELEMENT; i++)
    {
        csvFile << timings_beap[i] << ',' << timings_heap[i] << std::endl;
    }

    csvFile.close();
    std::cout << "done" << std::endl;*/
}

void measure_remove_min_cost_per_operation()
{
    /*size_t MAX_ELEMENT = 1000;
    Beap<int> beap;
    beap.container.reserve(MAX_ELEMENT);
    std::vector<int> v;
    v.reserve(MAX_ELEMENT);
    std::priority_queue<int, std::vector<int>> pq(
        std::less<int>(), 
        std::move(v)
    );

    auto data = generateRandomData(MAX_ELEMENT);

    for (size_t i = 0; i < MAX_ELEMENT; i++) {
        beap.push(data[i]);    
        pq.push(data[i]);
    }
    
    std::vector<long long> timings_beap(MAX_ELEMENT);
    std::vector<long long> timings_heap(MAX_ELEMENT);

    int i = 0;
    while(beap.size > 0)
    {
        auto start = std::chrono::high_resolution_clock::now();
        beap.pop();
        auto end = std::chrono::high_resolution_clock::now();   
        
        if(i <= 0)
        {
            timings_beap[i] = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        }
        else {
            timings_beap[i] = timings_beap[i-1] + std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        }
        i++;
    }

    i = 0;
    while(!pq.empty())
    {
        auto start = std::chrono::high_resolution_clock::now();
        pq.pop();
        auto end = std::chrono::high_resolution_clock::now();   
        
        if(i <= 0)
        {
            timings_heap[i] = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        }
        else {
            timings_heap[i] = timings_heap[i-1] + std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        }
        i++;
    }


    std::string filename = "remove_min_cost.csv";
    std::ofstream csvFile(filename);

    csvFile << "beap,heap" << std::endl;
    for(size_t i = 0; i < MAX_ELEMENT; i++)
    {
        csvFile << timings_beap[i] << ',' << timings_heap[i] << std::endl;
    }

    csvFile.close();
    std::cout << "done" << std::endl;    */
}


int main()
{
    //measure_push_cost_per_operation();
    //measure_remove_min_cost_per_operation();
    
    Beap<int> beap = Beap<int>();

    for (int i = 0; i < 100; i++) {
        beap.push(rand() % 1000);
    }
    std::cout << "started" << std::endl;
    for (int i = 0; i < 100; i++) {
        int target = rand() % 1000;
        auto res = beap.search(target);
        std::cout << "Successfully searched target: " << target << " and result is " 
        << res.first << ", " << res.second << std::endl; 
    }
}
