#include <gtest/gtest.h>
#include "../../src/Beap/Beap.hpp"  
#include "test_utils.hpp"


constexpr size_t INVALID_INDEX = static_cast<size_t>(-1);

class BeapTest : public ::testing::Test {
protected:
    Beap<int> beap;
};


TEST_F(BeapTest, ConstructorInitializesCorrectly) {
    EXPECT_EQ(beap.size(), 0);
    EXPECT_EQ(beap.height(), -1);
    EXPECT_TRUE(beap.empty());
}


TEST_F(BeapTest, InsertFirstElement) {
    beap.insert(10);
    
    EXPECT_EQ(beap.size(), 1);
    EXPECT_EQ(beap.height(), 0);
    EXPECT_EQ(beap.size(), 1);
    EXPECT_EQ(beap.extract(), 10);
}

TEST_F(BeapTest, InsertMultipleElementsMaintainsOrder) {
    beap.insert(10);
    beap.insert(5);
    beap.insert(15);
    beap.insert(3);
    
    EXPECT_EQ(beap.size(), 4);
    EXPECT_EQ(beap.extract(), 3);
}

TEST_F(BeapTest, InsertDuplicateValues) {
    beap.insert(10);
    beap.insert(10);
    beap.insert(5);
    
    EXPECT_EQ(beap.size(), 3);
    EXPECT_EQ(beap.extract(), 5); 
}


TEST_F(BeapTest, PopFromEmptyBeap) {
    EXPECT_THROW(beap.extract(), std::out_of_range);
}

TEST_F(BeapTest, PopSingleElement) {
    beap.insert(42);
    int value = beap.extract();
    
    EXPECT_EQ(value, 42);
    EXPECT_EQ(beap.size(), 0);
    EXPECT_EQ(beap.height(), -1);

    beap.insert(3);
    EXPECT_EQ(beap.size(), 1);
    EXPECT_EQ(beap.height(), 0);
}

TEST_F(BeapTest, PopMaintainsBeapProperty) {
    beap.insert(10);
    beap.insert(5);
    beap.insert(15);
    beap.insert(3);
    beap.insert(7);
    beap.insert(30);
    
    int min = beap.extract();

    EXPECT_EQ(min, 3);
    EXPECT_EQ(beap.size(), 5);
    EXPECT_EQ(beap.extract(), 5);
}

TEST_F(BeapTest, SearchInEmptyBeap) {
    EXPECT_FALSE(beap.search(10));
}

TEST_F(BeapTest, SearchExistingElement) {
    beap.insert(10);
    beap.insert(5);
    beap.insert(15);
    beap.insert(3);
    beap.insert(7);
    
    auto result = beap.search(5);
    EXPECT_TRUE(beap.search(5)); 
}

TEST_F(BeapTest, SearchNonExistingElement) {
    beap.insert(10);
    beap.insert(5);
    
    EXPECT_FALSE(beap.search(99));
}

TEST_F(BeapTest, SearchManyExistingElement)
{
    size_t MAX_NUMBER = 1000;
    for(size_t i = 1; i <= MAX_NUMBER; i++)
    {
        beap.insert(i);
    }

    for(size_t i = MAX_NUMBER; i > 0; i--)
    {
        EXPECT_TRUE(beap.search(i));
    }
    
}


TEST_F(BeapTest, RemoveFromEmptyBeap) {
    EXPECT_NO_THROW(beap.remove(10)); 
}

TEST_F(BeapTest, RemoveExistingElement) {
    size_t MAX_NUMBER = 10000;
    auto data = generateRandomData(MAX_NUMBER);
    for (int x : data) beap.insert(x);

    
    //b.printState("Pushes");
    for (int x : data) {
        beap.remove(x);
    }

    EXPECT_EQ(beap.empty(), true);
}

TEST_F(BeapTest, RemoveNonExistingElement) {
    beap.insert(10);
    beap.insert(5);
    
    int initialSize = beap.size();
    beap.remove(99);
    EXPECT_EQ(beap.size(), initialSize); 
}


// Complex Operations Tests
TEST_F(BeapTest, MultipleOperationsSequence) {
    beap.insert(20);
    beap.insert(10);
    beap.insert(30);
    
    EXPECT_EQ(beap.extract(), 10);
    
    beap.insert(5);
    beap.insert(25);
    
    EXPECT_EQ(beap.extract(), 5);
    
    beap.remove(20);
    
    // Final state should be valid
    EXPECT_EQ(beap.size(), 2);
    EXPECT_EQ(beap.getContainer()[0], 25);
}

TEST_F(BeapTest, LargeNumberOfInsertions) {
    const int NUM_ELEMENTS = 10000;
    
    for (int i = NUM_ELEMENTS; i > 0; --i) {
        beap.insert(i);
    }
    
    EXPECT_EQ(beap.size(), NUM_ELEMENTS);

    //beap.printState("Before pop");
    // Should pop in sorted order (min first)
    for (int i = 1; i <= NUM_ELEMENTS; ++i) {
        EXPECT_EQ(beap.extract(), i);
        //beap.printState("pop");
    }
}

// Edge Cases
TEST_F(BeapTest, NegativeNumbers) {
    beap.insert(-5);
    beap.insert(-10);
    beap.insert(0);
    
    EXPECT_EQ(beap.extract(), -10);
    EXPECT_EQ(beap.extract(), -5);
    EXPECT_EQ(beap.extract(), 0);
}

TEST_F(BeapTest, LargeNumbers) {
    beap.insert(1000000);
    beap.insert(500000);
    beap.insert(1500000);
    
    EXPECT_EQ(beap.extract(), 500000);
}


// Performance-oriented Tests (can be disabled for quick runs)
TEST_F(BeapTest, StressTest) {
    const int STRESS_COUNT = 1000;
    
    for (int i = 0; i < STRESS_COUNT; ++i) {
        beap.insert(rand() % 1000);
    }
    
    int prev = beap.extract();
    while (beap.size() > 0) {
        int current = beap.extract();
        EXPECT_GE(current, prev); // Should be non-decreasing
        prev = current;
    }
}

TEST_F(BeapTest, SearchGreaterThanAllElements) {
    beap.insert(1);
    beap.insert(2);
    beap.insert(3);
    beap.insert(4);

    EXPECT_FALSE(beap.search(999));
}

TEST_F(BeapTest, SearchixtureOfInputs) {
    int MAX_NUMBER = 1000;
    auto data = generateRandomData(MAX_NUMBER);
    auto tests = generateRandomData(MAX_NUMBER/4);

    for (auto d : data) {
        beap.insert(rand() % 1000);
    }
    
    for (auto t : tests) {
        auto res = beap.search(t);
        SUCCEED();
    }
}


