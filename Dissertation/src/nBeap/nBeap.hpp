#include <stdexcept>
#include <concepts>
#include <vector>
#include <utility>
#include <functional>
#include <algorithm>
#include <optional>
#include <bits/stdc++.h>

template<typename T>
concept Comparable = requires(T a, T b) {
    { a < b } -> std::convertible_to<bool>;
    { a > b } -> std::convertible_to<bool>;
//    { a == b } -> std::convertible_to<bool>;
};

template<Comparable T, size_t N, typename Compare = std::greater<T>>
class nBeap
{
public:
    nBeap(Compare c) 
    : _height(0)
    , _size(0)
    , compare(c)
    , interval(0, 0)
    , levelSize(0)
    {}

    nBeap() 
    : _height(0)
    , _size(0)
    , compare(Compare())
    , interval(0,0)
    , levelSize(0)
    {}

    ~nBeap() = default;


    
    T extract_min() {

        // check we can actually extract
        if(_size <= 0)
        {
            throw std::out_of_range("cannot pop empty nbeap");
        }

        
        _size--;
        auto minElem = std::exchange(container[0], container[_size]);
        container.resize(_size);

        // if we just extracted the last element
        if(_size == 0)
        {
            levelSize = 0;
            _height = 0;
            interval = {0, 0};
            return;
        }

        // in case the last element on a level is removed 
        if(_size - 1 < interval.first)
        {
            levelSize = getNumOfElemInPrevLevel(_height, levelSize);
            interval.second = interval.first  - 1;
            interval.first -= levelSize;
            _height--;
        }

        siftDown(0, 1);

        return minElem;
    }

    void insert(T val)
    {
        container.push_back(val);

        if(_size == 0)
        {
            _size++;
            _height++;
            interval = {0,0};
            levelSize++;
            return;
        }


        if(interval.second > _size)
        {
            levelSize = getNumberOfElementInNextLevel(_height, levelSize);
            interval.first = interval.second + 1;
            interval.first = interval.second + levelSize;
            _height++;
        }

        bubbleUp(0, _size, _height);
        _size++;
    }

    void remove(T val)
    {

    }

    bool search(T val) { 
        return _search(val).has_value(); 
    };

    uint64_t size() {return _size;}
    unsigned int height() {return _height - 1;}
    bool empty() {return _size == 0; }

private:
    unsigned int _height;
    uint64_t _size;
    std::vector<T> container;
    Compare compare;
    std::pair<uint64_t, uint64_t> interval;
    uint64_t levelSize;
    const size_t SECOND_LAYER_START = 3;

    struct Vec { std::array<unsigned int, N> data; };

    std::optional<std::pair<uint64_t, unsigned int>> _search(T val); // returns the index and height
    std::pair<uint64_t, uint64_t> span(unsigned int h);
    
    void siftDown(uint64_t startPos, unsigned int h, std::pair<uint64_t, uint64_t> levelInterval = {0,0});
    void bubbleUp(uint64_t index, uint64_t endIndex, unsigned int h);

    std::pair<uint64_t, uint64_t> getChildren(uint64_t parentIndex, unsigned int parentHeight);
    std::pair<uint64_t, uint64_t> getParents(uint64_t childIndex, unsigned int childHeight);
    std::optional<uint64_t> getMinOrMaxParentIndex(uint64_t currentPos, unsigned int childHeight, std::pair<uint64_t, uint64_t> levelInterval);
    std::optional<uint64_t> getMinOrMaxChildIndex(uint64_t currentPos, unsigned int childHeight, std::pair<uint64_t, uint64_t> levelInterval);
    inline uint64_t getNumberOfElementInNextLevel(unsigned int currHeight, uint64_t numOfElemCurrLevel)
    {
       // __int128 temp = numOfElemCurrLevel * (currHeight + N - 1 );

        return  (numOfElemCurrLevel * (currHeight + N - 1 )) / currHeight;
    }

    inline uint64_t getNumOfElemInPrevLevel(unsigned int currHeight, uint64_t numOfElemCurreLevel)
    {
        return  (numOfElemCurreLevel * (currHeight - 1)) / (currHeight + N - 2);
    }

    inline uint64_t getNumOfElemInFirstInnerLayer(unsigned int currHeight, uint64_t numOfElemCurrLevle)
    {
        if((int)(currHeight - 3) <= 0)
            return 0;
        
        uint64_t temp = (currHeight + N - 2) * (currHeight + N - 3) * (currHeight + N - 4);

        return temp / (currHeight - 3);
    }
};


template<Comparable T, size_t N, typename Compare>
std::optional<std::pair<uint64_t, unsigned int>> _search(T val)
{
    return std::nullopt; 
}

template<Comparable T, size_t N, typename Compare>
std::pair<uint64_t, uint64_t> nBeap<T, N, Compare>::span(unsigned int height)
{
    return {0, 0}; 
}

template<Comparable T, size_t N, typename Compare>
void nBeap<T, N, Compare>::siftDown(uint64_t startPos, unsigned int h, std::pair<uint64_t, uint64_t> levelInterval)
{
    T val = std::move(container[startPos]);
    auto currPos = startPos;

    while(h < _height)
    {
        std::optional<uint64_t> selected_child_index = getMinOrMaxChildIndex(currPos, h, levelInterval.second - levelInterval.first + 1);

        if(!selected_child_index.has_value() || compare(selected_child_index.value(), val))
        {
            break;
        }

        std::swap(container[selected_child_index], container[currPos]);
        currPos = selected_child_index.value();
        levelInterval = getNumberOfElementInNextLevel(h, levelInterval.second - levelInterval.first + 1);
        h++;
    }

    container[currPos] = std::move(val);

}

template <Comparable T, size_t N, typename Compare>
void nBeap<T, N, Compare>::bubbleUp(uint64_t toIndex, uint64_t fromIndex, unsigned int h)
{
    auto currentPos = fromIndex;
    auto currentHeight = h;
    T val = std::move(container[fromIndex]);

    while (currentPos > toIndex)
    {
        // get the parents 
        // get the largest/smallest parent
        // compare this parent to val
        // for min-nbeap break if it is less than val and vice versa
        // move the selected parent to the currentPos 
        // currentPos = new index
        // height--

        std::optional<uint64_t> selected_parent_index = getMinOrMaxParentIndex(currentPos, currentHeight);

        if(!selected_parent_index.has_value() || compare(val, container[selected_parent_index.value()]))
        {
            break;
        }
        container[currentPos] = std::move(container[selected_parent_index]);
        currentPos = selected_parent_index.value();
        currentHeight--;
    }

    container[currentPos] = std::move(val);
}

template<Comparable T, size_t N, typename Compare>
inline std::pair<uint64_t, uint64_t> getChildren(uint64_t parentIndex, unsigned int parentHeight)
{
    return {0, 0}; 
}

template<Comparable T, size_t N, typename Compare>
inline std::pair<uint64_t, uint64_t> nBeap<T, N, Compare>::getParents(uint64_t childIndex, unsigned int childHeight)
{
    return {0, 0}; 
}

template <Comparable T, size_t N, typename Compare>
inline std::optional<uint64_t> nBeap<T, N, Compare>::getMinOrMaxParentIndex(
    uint64_t currentPos, 
    unsigned int childHeight,
    std::pair<uint64_t, uint64_t> levelInterval
)
{
    // calcualate the span to get the range of indices in the level
    // get the max/min of the parents
    return std::nullopt;
}

template <Comparable T, size_t N, typename Compare>
inline std::optional<uint64_t> nBeap<T, N, Compare>::getMinOrMaxChildIndex(
    uint64_t currentPos, 
    unsigned int childHeight, 
    std::pair<uint64_t, uint64_t> levelInterval
)
{
    // calcualate the span to get the range of indices in the level
    // get the max/min of the children
    return std::nullopt;
}


// need to then write an implementation for getParents/Children
// need to then write an implementation for the convertion from coordinates
// to indices


/*
array<d> - d is the dimension of the beap

X - max value for any index in the coordinate
L - number of elements in that layer
I - index of the element in that layer

C[i] = C[i-1] + f(I, i)

C[i] = f(I, i)
i    1   2   3   4    5    6    7   8   9   
1    3   2   1   0    0    0    0   1   2
2    0   1   2   3    2    1    0   0   0 
3    0   0   0   0    1    2    3   2   1
*/