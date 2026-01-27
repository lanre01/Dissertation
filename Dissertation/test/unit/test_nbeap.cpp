#include <gtest/gtest.h>
#include "../../src/NBeap/NBeap.hpp"
#include <random>
#include <type_traits>

template <typename DimTag>
class NBeapStressTest : public ::testing::Test {
protected:
    static constexpr size_t N = DimTag::value;
    NBeap<int, N> nbeap;
};

using StressDims = ::testing::Types<
    std::integral_constant<size_t, 1>,
    std::integral_constant<size_t, 2>,
    std::integral_constant<size_t, 3>,
    std::integral_constant<size_t, 4>,
    std::integral_constant<size_t, 5>,
    std::integral_constant<size_t, 6>,
    std::integral_constant<size_t, 7>,
    std::integral_constant<size_t, 8>,
    std::integral_constant<size_t, 9>,
    std::integral_constant<size_t, 10>
>;

TYPED_TEST_SUITE(NBeapStressTest, StressDims);

TYPED_TEST(NBeapStressTest, StressTestMultipleDimensions) {
    
    const int STRESS_COUNT = 1000;

    std::mt19937 rng(12345);
    std::uniform_int_distribution<int> dist(0, 999);

    for (int i = 0; i < STRESS_COUNT; ++i) {
        this->nbeap.insert(dist(rng));
    }

    int prev = this->nbeap.extract_min();
    while (this->nbeap.size() > 0) {
        int current = this->nbeap.extract_min();
        EXPECT_GE(current, prev) << "Failed for N=" << TestFixture::N;
        prev = current;
    }
}


class NBeapTest : public ::testing::Test {
protected:
    NBeap<int, 3> nbeap;
};

TEST_F(NBeapTest, ConstructorInitializesCorrectly) {
    EXPECT_EQ(nbeap.size(), 0);
    EXPECT_EQ(nbeap.height(), -1);
    EXPECT_TRUE(nbeap.empty());
}

TEST_F(NBeapTest, InsertSingleElement) {
    nbeap.insert(10);
    EXPECT_EQ(nbeap.size(), 1);
    EXPECT_EQ(nbeap.height(), 0);
}

TEST_F(NBeapTest, InsertMultipleElements) {
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
    EXPECT_EQ(nbeap.height(), 2);
}

TEST_F(NBeapTest, InsertDuplicateValues) {
    nbeap.insert(10);
    nbeap.insert(10);
    nbeap.insert(5);
    nbeap.insert(5);
    nbeap.insert(5);

    EXPECT_EQ(nbeap.size(), 5);
    EXPECT_EQ(nbeap.extract_min(), 5);
}

TEST_F(NBeapTest, PopFromEmptyBeap) {
    EXPECT_THROW(nbeap.extract_min(), std::out_of_range);
}

TEST_F(NBeapTest, PopSingleElement) {
    nbeap.insert(42);
    int value = nbeap.extract_min();

    EXPECT_EQ(value, 42);
    EXPECT_EQ(nbeap.size(), 0);
    EXPECT_EQ(nbeap.height(), -1);
}

TEST_F(NBeapTest, PopMaintainsBeapProperty) {
    nbeap.insert(10);
    nbeap.insert(5);
    nbeap.insert(15);
    nbeap.insert(3);
    nbeap.insert(7);
    nbeap.insert(30);

    EXPECT_EQ(nbeap.extract_min(), 3);
    EXPECT_EQ(nbeap.size(), 5);
    EXPECT_EQ(nbeap.extract_min(), 5);
    EXPECT_EQ(nbeap.extract_min(), 7);
    EXPECT_EQ(nbeap.extract_min(), 10);
}
