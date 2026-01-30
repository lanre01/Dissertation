

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
    NBeap<int, 2> nbeap;
    /*int MAX_NUMBER = 35;

    
    auto data = generateRandomData(MAX_NUMBER);

    //nbeap.insert(54);
    
    
    const int STRESS_COUNT = 35;
    

    for (int i = 0; i < STRESS_COUNT; ++i) {
        nbeap.insert(rand() % STRESS_COUNT);
    }

    nbeap.printState("After insertion");

    int prev = nbeap.extract();
    while (nbeap.size() > 0) {
        int current = nbeap.extract();
        if(current < prev)
        {
            std::cout << "Beap property violated, current=" << current << " previous=" << prev << std::endl;
            nbeap.printState("After violation");
        } // Should be non-decreasing
        prev = current;
    }*/

    size_t MAX_NUMBER = 35;
    for(size_t i = 1; i <= MAX_NUMBER; i++)
    {
        nbeap.insert(i);
    }

    nbeap.printState("After Insertion");
    for(size_t i = MAX_NUMBER; i > 0; i--)
    {
        if(!nbeap.search(i))
        {
            std::cout << "Val is: " << i << std::endl;
        }
    }


    std::cout << "end" << std::endl;

    return 0;
}
