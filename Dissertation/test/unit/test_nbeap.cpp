#include <gtest/gtest.h>
#include "../../src/nBeap/nBeap.hpp"  
#include <random>
#include <algorithm>


class nBeapTest : public ::testing::Test {
protected:
    nBeap<int, 3> nbeap;
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

    EXPECT_EQ(nbeap.size(), 4);
    EXPECT_EQ(nbeap.height(), 2);

}


