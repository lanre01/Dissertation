#include <gtest/gtest.h>
#include "../../src/nBeap/nBeap.hpp"  
#include <random>
#include <algorithm>


class nBeapTest : public ::testing::Test {
protected:
    nBeap<int, 2> nbeap;
};


TEST_F(nBeapTest, ConstructorInitializesCorrectly) {
    EXPECT_EQ(nbeap.size(), 0);
    EXPECT_EQ(nbeap.height(), -1);
    EXPECT_TRUE(nbeap.empty());
}


TEST_F(nBeapTest, InsertSingleElement)
{
    nbeap.insert(10);

    EXPECT_EQ(nbeap.size(), 1);
    EXPECT_EQ(nbeap.height(), 0);
}

TEST_F(nBeapTest, InsertMultipleElements)
{
    nbeap.insert(10);
    nbeap.insert(15);
    nbeap.insert(19);
    nbeap.insert(130);
    nbeap.insert(5);
    nbeap.insert(1);
    nbeap.insert(3);
    nbeap.insert(6);
    nbeap.insert(7);


    EXPECT_EQ(nbeap.size(), 9);
    EXPECT_EQ(nbeap.height(), 3);

    //nbeap.printState("After insertion");
}

TEST_F(nBeapTest, InsertDuplicateValues) {
    nbeap.insert(10);
    nbeap.insert(10);
    nbeap.insert(5);
    
    EXPECT_EQ(nbeap.size(), 3);
    EXPECT_EQ(nbeap.extract_min(), 5); 
}

TEST_F(nBeapTest, PopFromEmptyBeap) {
    EXPECT_THROW(nbeap.extract_min(), std::out_of_range);
}


TEST_F(nBeapTest, PopSingleElement) {
    nbeap.insert(42);
    int value = nbeap.extract_min();
    
    EXPECT_EQ(value, 42);
    EXPECT_EQ(nbeap.size(), 0);
    EXPECT_EQ(nbeap.height(), -1);

    nbeap.insert(3);
    EXPECT_EQ(nbeap.size(), 1);
    EXPECT_EQ(nbeap.height(), 0);
}

TEST_F(nBeapTest, PopMaintainsBeapProperty) {
    nbeap.insert(10);
    nbeap.insert(5);
    nbeap.insert(15);
    nbeap.insert(3);
    nbeap.insert(7);
    nbeap.insert(30);

    //nbeap.printState("Before extracting 3");
    EXPECT_EQ(nbeap.extract_min(), 3);
    //nbeap.printState("After extracting 3");
    EXPECT_EQ(nbeap.size(), 5);
    EXPECT_EQ(nbeap.extract_min(), 5);
    //nbeap.printState("After extracting 5");
    EXPECT_EQ(nbeap.extract_min(), 7);
    EXPECT_EQ(nbeap.extract_min(), 10);
    
}





TEST_F(nBeapTest, StressTest) {
    const int STRESS_COUNT = 10;
    
    for (int i = 0; i < STRESS_COUNT; ++i) {
        nbeap.insert(rand() % 1000);
    }
    
    int prev = nbeap.extract_min();
    while (nbeap.size() > 0) {
        int current = nbeap.extract_min();
        EXPECT_GE(current, prev); // Should be non-decreasing
        prev = current;
    }
}



