

#include <iostream>

#include "Heap/Heap.hpp"
#include "Beap/Beap.hpp"
#include "nBeap/nBeap.hpp"
#include <string>

int main()
{
    Beap beap = Beap();

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
