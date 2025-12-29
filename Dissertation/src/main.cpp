

#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <algorithm>
#include <string>
#include <queue>  
#include <chrono>

#include "Beap/Beap.hpp"
#include "nBeap/nBeap.hpp"

static std::mt19937 rng(42);

std::vector<int> generateRandomData(size_t n) {
    std::uniform_int_distribution<int> dist(1, 1'000'000);
    std::vector<int> v(n);
    for (auto& x : v) x = dist(rng);
    return v;
}





int main()
{
    int MAX_NUMBER = 100000;


    Beap<int> b;
    nBeap<int, 3> nb;
    b.reserve(MAX_NUMBER);
    auto data = generateRandomData(MAX_NUMBER);
    for (int x : data) b.push(x);

    for (int x : data) {
        b.remove(x);
        
    }

    return 0;
}
