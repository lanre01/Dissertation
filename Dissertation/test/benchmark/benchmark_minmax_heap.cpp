#include <iostream>
#include <fstream>
#include <charconv>
#include <benchmark/benchmark.h>
#include <vector>
#include "minmax_heap.hpp"

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