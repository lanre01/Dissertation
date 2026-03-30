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

TYPED_TEST(NBeapTestMultipleDim, InsertingNegativeNumbers)
{

    this->nbeap.insert(-5);
    this->nbeap.insert(-10);
    this->nbeap.insert(0);
    
    EXPECT_EQ(this->nbeap.extract(), -10);
    EXPECT_EQ(this->nbeap.extract(), -5);
    EXPECT_EQ(this->nbeap.extract(), 0);
}
