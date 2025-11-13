

#include <iostream>

#include "Heap/Heap.hpp"
#include "Beap/Beap.hpp"
#include "nBeap/nBeap.hpp"
#include <string>

int main()
{
    Beap beap = Beap();
    const int NUM_ELEMENTS = 100;

    beap.push(1000000);
    beap.push(500000);
    beap.push(1500000);
    beap.printState("push 1500000");
}
