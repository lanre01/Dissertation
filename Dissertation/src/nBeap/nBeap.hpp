#include <stdexcept>
#include <concepts>
#include <vector>
#include <utility>
#include <functional>
#include <algorithm>
#include <optional>

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
    

    nBeap();
    ~nBeap();
    
    T extract_min() {
        return T();
    }

    void insert(T val)
    {
        container.push_back(val);
        auto res = span(_height);

        if(res.second > _size)
        {
            _height++;
        }
        bubbleUp(_size, _height);
        _size++;
    }

    void remove(T val)
    {

    }

    bool search(T val) { 
        return _search(val).has_value(); 
    };

    uint64_t size() {return _size;}
    size_t height() {return _height - 1;}
    bool empty() {return _size == 0; }

private:
    size_t _height;
    uint64_t _size;
    std::vector<T> container;


    std::optional<std::pair<uint64_t, size_t>> _search(T val); // returns the index and height
    std::pair<uint64_t, uint64_t> span(size_t h);
    void siftDown(const uint64_t startPos, const uint64_t index, const size_t childHeight);
    void bubbleUp(uint64_t index, size_t h);
    std::pair<uint64_t, uint64_t> getChildren(uint64_t parentIndex, size_t parentHeight);
    std::pair<uint64_t, uint64_t> getParents(uint64_t childIndex, size_t childHeight);
    uint64_t getSmallestChildIndex(std::pair<uint64_t, uint64_t> children);
    uint64_t getLargestParentIndex(std::pair<uint64_t, uint64_t> parents);
};

template<Comparable T, size_t N, typename Compare>
nBeap<T, N, Compare>::nBeap() : _height(0), _size(0) {}

template<Comparable T, size_t N, typename Compare>
nBeap<T, N, Compare>::~nBeap() = default;

template<Comparable T, size_t N, typename Compare>
std::optional<std::pair<uint64_t, size_t>> _search(T val)
{
    return std::nullopt; 
}

template<Comparable T, size_t N, typename Compare>
std::pair<uint64_t, uint64_t> nBeap<T, N, Compare>::span(size_t height)
{
    return {0, 0}; 
}

template<Comparable T, size_t N, typename Compare>
void nBeap<T, N, Compare>::siftDown(const uint64_t startPos, const uint64_t index, const size_t childHeight)
{
    
}

template<Comparable T, size_t N, typename Compare>
void nBeap<T, N, Compare>::bubbleUp(uint64_t index, size_t h)
{
    auto currentPos = index;
    auto currentHeight = h;
    T val = container[index];

    for(;;;)
    {
        // get the parents 
        // get the largest/smallest parent
        // compare this parent to val
        // for min-nbeap break if it is less than val and vice versa
        // move the selected parent to the currentPos 
        // currentPos = new index
        // height--

    }

    container[currentPos] = std::move(val);
}

template<Comparable T, size_t N, typename Compare>
std::pair<uint64_t, uint64_t> getChildren(uint64_t parentIndex, size_t parentHeight)
{
    return {0, 0}; 
}

template<Comparable T, size_t N, typename Compare>
std::pair<uint64_t, uint64_t> nBeap<T, N, Compare>::getParents(uint64_t childIndex, size_t childHeight)
{
    return {0, 0}; 
}

template<Comparable T, size_t N, typename Compare>
uint64_t nBeap<T, N, Compare>::getSmallestChildIndex(std::pair<uint64_t, uint64_t> children)
{
    return children.first; 
}

template<Comparable T, size_t N, typename Compare>
uint64_t nBeap<T, N, Compare>::getLargestParentIndex(std::pair<uint64_t, uint64_t> parents)
{
    return parents.first; 
}
