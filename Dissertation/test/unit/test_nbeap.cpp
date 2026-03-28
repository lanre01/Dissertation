#include <gtest/gtest.h>
#include "../../src/n_beap/n_beap.hpp"
#include "test_utils.hpp"
#include <type_traits>


template <typename DimTag>
class NBeapTestMultipleDim : public ::testing::Test {
protected:
    static constexpr size_t N = DimTag::value;
    NBeap<int, N> nbeap;
};

using DIMS = ::testing::Types<
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

TYPED_TEST_SUITE(NBeapTestMultipleDim, DIMS);

TYPED_TEST(NBeapTestMultipleDim, SearchInMultipleDimensions) {
    
    const int MAX_NUMBER = 1000;

    auto data = generateRandomData(MAX_NUMBER);

    for(auto x : data) this->nbeap.insert(x);

    for(auto x : data)
    {
        EXPECT_TRUE(this->nbeap.search(x));
    }
}

TYPED_TEST(NBeapTestMultipleDim, SearchSuccedesInMultipleDimensions) {
    
    const int MAX_NUMBER = 1000;

    auto data = generateRandomData(MAX_NUMBER);
    auto tests = generateRandomData(MAX_NUMBER/4);
    for(auto x : data) this->nbeap.insert(x);

    for(auto x : tests)
    {
        this->nbeap.search(x);
        SUCCEED();
    }
}

TYPED_TEST(NBeapTestMultipleDim, MultipleInsertionAndExtraction) {
    
    const int MAX_NUMBER = 1000;

    std::uniform_int_distribution<int> dist(0, 999);

    for (int i = 0; i < MAX_NUMBER; ++i) {
        this->nbeap.insert(dist(rng));
    }

    int prev = this->nbeap.extract();
    while (this->nbeap.size() > 0) {
        int current = this->nbeap.extract();
        EXPECT_GE(current, prev) << "Failed for N=" << TestFixture::N;
        prev = current;
    }
}

TYPED_TEST(NBeapTestMultipleDim, MultipleRemoveOperations)
{
    const int MAX_NUMBER = 1000;
    auto data = generateRandomData(MAX_NUMBER);
    
    for(auto d : data)
    {
        this->nbeap.insert(d);
    }

    for(auto d : data)
    {
        this->nbeap.remove(d);
    }

    EXPECT_EQ(0, this->nbeap.size());
}

TYPED_TEST(NBeapTestMultipleDim, MultipleRemoveOperationsSucced)
{
    const int MAX_NUMBER = 1000;
    auto data = generateRandomData(MAX_NUMBER);
    auto tests = generateRandomData(MAX_NUMBER/4);
    for(auto d : data)
    {
        this->nbeap.insert(d);
    }

    for(auto d : tests)
    {
        this->nbeap.remove(d);
        SUCCEED();
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
    EXPECT_EQ(nbeap.extract(), 5);
}

TEST_F(NBeapTest, PopFromEmptyBeap) {
    EXPECT_THROW(nbeap.extract(), std::out_of_range);
}

TEST_F(NBeapTest, PopSingleElement) {
    nbeap.insert(42);
    int value = nbeap.extract();

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

    EXPECT_EQ(nbeap.extract(), 3);
    EXPECT_EQ(nbeap.size(), 5);
    EXPECT_EQ(nbeap.extract(), 5);
    EXPECT_EQ(nbeap.extract(), 7);
    EXPECT_EQ(nbeap.extract(), 10);
}

TEST_F(NBeapTest, SearchInEmptyBeap) {
    auto result = nbeap.search(10);
    EXPECT_FALSE(result);
}

TEST_F(NBeapTest, SearchExistingElement) {
    nbeap.insert(10);
    nbeap.insert(5);
    nbeap.insert(15);
    nbeap.insert(3);
    nbeap.insert(7);
    
    auto result = nbeap.search(5);
    EXPECT_TRUE(result); 
}

TEST_F(NBeapTest, SearchNonExistingElement) {
    nbeap.insert(10);
    nbeap.insert(5);
    
    auto result = nbeap.search(99);
    EXPECT_FALSE(result);
}

TEST_F(NBeapTest, SearchManyExistingElement)
{
    size_t MAX_NUMBER = 1000;
    for(size_t i = 1; i <= MAX_NUMBER; i++)
    {
        nbeap.insert(i);
    }

    //nbeap.printState("After Insertion");
    for(size_t i = MAX_NUMBER; i > 0; i--)
    {
        EXPECT_TRUE(nbeap.search(i));
    }
    
}

TEST_F(NBeapTest, NegativeNumbers) {
    nbeap.insert(-5);
    nbeap.insert(-10);
    nbeap.insert(0);
    
    EXPECT_EQ(nbeap.extract(), -10);
    EXPECT_EQ(nbeap.extract(), -5);
    EXPECT_EQ(nbeap.extract(), 0);
}