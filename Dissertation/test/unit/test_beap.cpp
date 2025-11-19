#include <gtest/gtest.h>
#include "../../src/Beap/Beap.hpp"  

class BeapTest : public ::testing::Test {
protected:
    Beap beap;
    
    void SetUp() override {
        beap = Beap();
    }
};


TEST_F(BeapTest, ConstructorInitializesCorrectly) {
    EXPECT_EQ(beap.size, 0);
    EXPECT_EQ(beap.height, 0);
    EXPECT_TRUE(beap.container.empty());
}


TEST_F(BeapTest, InsertFirstElement) {
    beap.push(10);
    
    EXPECT_EQ(beap.size, 1);
    EXPECT_EQ(beap.height, 1);
    EXPECT_EQ(beap.container.size(), 1);
    EXPECT_EQ(beap.container[0], 10);
}

TEST_F(BeapTest, InsertMultipleElementsMaintainsOrder) {
    beap.push(10);
    beap.push(5);
    beap.push(15);
    beap.push(3);
    
    EXPECT_EQ(beap.size, 4);
    EXPECT_EQ(beap.container[0], 3);
}

TEST_F(BeapTest, InsertDuplicateValues) {
    beap.push(10);
    beap.push(10);
    beap.push(5);
    
    EXPECT_EQ(beap.size, 3);
    EXPECT_EQ(beap.container[0], 5); 
}


TEST_F(BeapTest, PopFromEmptyBeap) {
    EXPECT_THROW(beap.pop(), std::out_of_range);
}

TEST_F(BeapTest, PopSingleElement) {
    beap.push(42);
    int value = beap.pop();
    
    EXPECT_EQ(value, 42);
    EXPECT_EQ(beap.size, 0);
    EXPECT_EQ(beap.height, 0);
}

TEST_F(BeapTest, PopMaintainsBeapProperty) {
    beap.push(10);
    beap.push(5);
    beap.push(15);
    beap.push(3);
    beap.push(7);
    beap.push(30);
    
    int min = beap.pop();
   
    EXPECT_EQ(min, 3);
    EXPECT_EQ(beap.size, 5);
    EXPECT_EQ(beap.container[0], 5);
}

TEST_F(BeapTest, SearchInEmptyBeap) {
    auto result = beap.search(10);
    EXPECT_EQ(result.first, -1);
    EXPECT_EQ(result.second, -1);
}

TEST_F(BeapTest, SearchExistingElement) {
    beap.push(10);
    beap.push(5);
    beap.push(15);
    
    std::pair<int, int> result = beap.search(5);
    EXPECT_NE(result.first, -1); 
    EXPECT_EQ(beap.container[result.first], 5); 
}

TEST_F(BeapTest, SearchNonExistingElement) {
    beap.push(10);
    beap.push(5);
    
    auto result = beap.search(99);
    EXPECT_EQ(result.first, -1);
    EXPECT_EQ(result.second, -1);
}


TEST_F(BeapTest, RemoveFromEmptyBeap) {
    EXPECT_NO_THROW(beap.remove(10)); 
}

TEST_F(BeapTest, RemoveExistingElement) {
    beap.push(10);
    beap.push(5);
    beap.push(15);
    beap.push(3);
    beap.push(7);
    int initialSize = beap.size;
    beap.remove(5);
    EXPECT_EQ(beap.size, initialSize - 1);
    EXPECT_EQ(beap.container[0], 3); 
    EXPECT_EQ(beap.container[1], 10);
    beap.remove(7);
    EXPECT_EQ(beap.container[2], 15);
}

TEST_F(BeapTest, RemoveNonExistingElement) {
    beap.push(10);
    beap.push(5);
    
    int initialSize = beap.size;
    beap.remove(99);
    EXPECT_EQ(beap.size, initialSize); 
}


// Complex Operations Tests
TEST_F(BeapTest, MultipleOperationsSequence) {
    beap.push(20);
    beap.push(10);
    beap.push(30);
    
    EXPECT_EQ(beap.pop(), 10);
    
    beap.push(5);
    beap.push(25);
    
    EXPECT_EQ(beap.pop(), 5);
    
    beap.remove(20);
    
    // Final state should be valid
    EXPECT_EQ(beap.size, 2);
    EXPECT_EQ(beap.container[0], 25);
}

TEST_F(BeapTest, LargeNumberOfInsertions) {
    const int NUM_ELEMENTS = 100;
    
    for (int i = NUM_ELEMENTS; i > 0; --i) {
        beap.push(i);
    }
    
    EXPECT_EQ(beap.size, NUM_ELEMENTS);
    
    // Should pop in sorted order (min first)
    for (int i = 1; i <= NUM_ELEMENTS; ++i) {
        EXPECT_EQ(beap.pop(), i);
    }
}

// Edge Cases
TEST_F(BeapTest, NegativeNumbers) {
    beap.push(-5);
    beap.push(-10);
    beap.push(0);
    
    EXPECT_EQ(beap.pop(), -10);
    EXPECT_EQ(beap.pop(), -5);
    EXPECT_EQ(beap.pop(), 0);
}

TEST_F(BeapTest, LargeNumbers) {
    beap.push(1000000);
    beap.push(500000);
    beap.push(1500000);
    
    EXPECT_EQ(beap.pop(), 500000);
}


// Performance-oriented Tests (can be disabled for quick runs)
TEST_F(BeapTest, StressTest) {
    const int STRESS_COUNT = 1000;
    
    for (int i = 0; i < STRESS_COUNT; ++i) {
        beap.push(rand() % 1000);
    }
    
    int prev = beap.pop();
    while (beap.size > 0) {
        int current = beap.pop();
        EXPECT_GE(current, prev); // Should be non-decreasing
        prev = current;
    }
}

TEST_F(BeapTest, SearchFailsOnIncompleteLastLevel) {
    beap.push(5);
    beap.push(10);
    beap.push(3);
    beap.push(7);
    // Typically creates an incomplete bottom row for height=2

    auto res = beap.search(7);   // Should terminate
    EXPECT_NE(res.first, -1);    // Your code often loops or returns wrong
}

TEST_F(BeapTest, SearchGreaterThanAllElements) {
    beap.push(1);
    beap.push(2);
    beap.push(3);
    beap.push(4);

    auto res = beap.search(999);
    EXPECT_EQ(res.first, -1);
    EXPECT_EQ(res.second, -1);  // Your implementation hangs here
}

TEST_F(BeapTest, SearchBetweenTwoLevelsCausesCycle) {
    beap.push(10);
    beap.push(20);
    beap.push(5);
    beap.push(15);
    beap.push(12);    // inserted between levels

    // 12 sits “between” levels, causing oscillation
    auto res = beap.search(12);
    EXPECT_NE(res.first, -1);   // often loops
}

TEST_F(BeapTest, SearchLeftMoveDoesNotAdjustHeight) {
    beap.push(50);
    beap.push(40);
    beap.push(30);
    beap.push(20);
    beap.push(10);

    // Searching for "1" forces search to left edge improperly
    auto res = beap.search(1);
    EXPECT_EQ(res.first, -1);   // Should terminate, often loops
}

TEST_F(BeapTest, SearchWithInvalidParentsGenerated) {
    beap.push(100);
    beap.push(60);
    beap.push(80);
    beap.push(20);
    beap.push(40);
    beap.push(10); // now parents mismatch

    auto res = beap.search(40);
    EXPECT_NE(res.first, -1);   // Your implementation sometimes throws or loops
}

TEST_F(BeapTest, SearchRootIncorrectDownwardMove) {
    beap.push(10);
    beap.push(20);
    beap.push(30);
    beap.push(40);

    auto res = beap.search(10);
    EXPECT_EQ(res.first, 0);  // root should be found instantly
}

TEST_F(BeapTest, SearchForMaximumFails) {
    beap.push(5);
    beap.push(10);
    beap.push(15);
    beap.push(20);
    beap.push(25);

    auto res = beap.search(25);
    EXPECT_NE(res.first, -1); 
}

TEST_F(BeapTest, SearchRandomisedStructureStability) {
    for (int i = 0; i < 200; i++) {
        beap.push(rand() % 1000);
    }
    for (int i = 0; i < 200; i++) {
        int target = rand() % 1000;
        auto res = beap.search(target);
        // Should terminate always
        SUCCEED();
    }
}

TEST_F(BeapTest, SearchMiddleValueCausesOscillation) {
    beap.push(50);
    beap.push(10);
    beap.push(40);
    beap.push(20);
    beap.push(30);
    beap.push(60);

    // Search for 35 which is not present
    // causes oscillation in incorrect BEAP geometry
    auto res = beap.search(35);
    EXPECT_EQ(res.first, -1);
}

TEST_F(BeapTest, SearchLevelBoundaryMisalignment) {
    beap.push(8);
    beap.push(3);
    beap.push(6);
    beap.push(2);
    beap.push(7);
    beap.push(4);

    // Searching for something between levels
    auto res = beap.search(5);
    EXPECT_EQ(res.first, -1);   // Should terminate
}

TEST_F(BeapTest, MassiveRandomSearchStability) {
    const int N = 5000;
    for (int i = 0; i < N; ++i) {
        beap.push(rand() % 100000);
    }
    for (int i = 0; i < 5 * N; ++i) {
        int target = rand() % 100000;
        auto res = beap.search(target);
        // We do not assert found/not-found here, just that it terminates.
        SUCCEED();
    }
}

