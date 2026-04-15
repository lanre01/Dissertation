#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <algorithm>
#include <string>
#include <queue>  
#include <chrono>

#include "beap/beap.hpp"
#include "n_beap/n_beap.hpp"

static std::mt19937 rng(42);

std::vector<int> generateRandomData(size_t n) {
    std::uniform_int_distribution<int> dist(1, 1'000'000);
    std::vector<int> v(n);
    for (auto& x : v) x = dist(rng);
    return v;
}



void createFilesWithNumber()
{
    std::uniform_int_distribution<int> dist(1, 1'000'000'000);
    dist(rng);

    std::ofstream myfile("numbers.bin", std::ios::binary);

    for (int i = 0; i < 4'000'000; i++) {
        int x = dist(rng);
        myfile.write(reinterpret_cast<char*>(&x), sizeof(x));
    }

    myfile.close();

}

int main()
{
    NBeap<int, 3> nbeap;
    std::vector<size_t> vec;
    

    // nbeap.insert(10);

    /*int MAX_NUMBER = 10;
    auto data = generateRandomData(MAX_NUMBER);
    Beap<int> beap = Beap<int>(data);

    int prev = beap.extract();
    while (beap.size() > 0) {
        int current = beap.extract();
        if (current <  prev)
        {
            std::cout << "Failed: " << current << "is less than" << prev << std::endl;
        }
        prev = current;
    }*/

    // size_t MAX_NUMBER = 10000;
    // auto data = generateRandomData(MAX_NUMBER);
    // for(auto x : data)
    // {
    //     nbeap.insert(x);
    //     //nbeap.printState("Insertion");
    // }
    // nbeap.printState("After Insertion");

    // int prev = nbeap.extract(); 
    // while (nbeap.size() > 0) {
    //     //nbeap.printState("before failure");
    //     int current = nbeap.extract();
    //     if (current <  prev)
    //     {
    //         nbeap.printState("After failure");
    //         std::cout << "Failed: " << current << " is less than" << prev << std::endl;
    //     }
    //     prev = current;
    // }

    // nbeap.printState("After extraction");

    // Testing span and height function
    // auto res = nbeap.getSpanAndHeight(100, 3);
    // std::cout << "params = 100, 3: " << res[0] << "," << res[1] << "," << res[2] << std::endl;

    // res = nbeap.getSpanAndHeight(0, 3);
    // std::cout << "params = 0, 3: " << res[0] << "," << res[1] << "," << res[2] << std::endl;

    // res = nbeap.getSpanAndHeight(0, 10);
    // std::cout << "params = 0, 10: " << res[0] << "," << res[1] << "," << res[2] << std::endl;
    // res = nbeap.getSpanAndHeight(5, 9);
    // std::cout << "params = 5, 9: " << res[0] << "," << res[1] << "," << res[2] << std::endl;
    // nbeap.search(30);



    createFilesWithNumber();

    std::cout << "end" << std::endl;

    return 0;
}
