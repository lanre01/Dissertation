

#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <algorithm>
#include <string>
#include <queue>  
#include <chrono>

#include "Beap/Beap.hpp"
#include "NBeap/NBeap.hpp"

static std::mt19937 rng(42);

std::vector<int> generateRandomData(size_t n) {
    std::uniform_int_distribution<int> dist(1, 1'000'000);
    std::vector<int> v(n);
    for (auto& x : v) x = dist(rng);
    return v;
}





int main()
{
    int MAX_NUMBER = 35;

    NBeap<int, 4> nbeap;
    auto data = generateRandomData(MAX_NUMBER);

    //nbeap.insert(54);
    
    
    const int STRESS_COUNT = 35;
    

    for (int i = 0; i < STRESS_COUNT; ++i) {
        nbeap.insert(rand() % STRESS_COUNT);
    }

    nbeap.printState("After insertion");

    int prev = nbeap.extract_min();
    while (nbeap.size() > 0) {
        int current = nbeap.extract_min();
        if(current < prev)
        {
            std::cout << "Beap property violated, current=" << current << " previous=" << prev << std::endl;
            nbeap.printState("After violation");
        } // Should be non-decreasing
        prev = current;
    }


    std::cout << "end" << std::endl;

    return 0;
}
