#include <stdexcept>
#include <concepts>
#include <vector>
#include <utility>
#include <functional>
#include <algorithm>

template<typename T>
concept Comparable = requires(T a, T b) {
    { a < b } -> std::convertible_to<bool>;
    { a > b } -> std::convertible_to<bool>;
    { a == b } -> std::convertible_to<bool>;
};

template<Comparable T, int N, typename Compare = std::greater<T>>
class nBeap
{
public:
    int height;
    int size;
    std::vector<T> container;

    nBeap();
    ~nBeap();
    T remove_min();
    void push(T val);
    void remove(T val);
    bool search(T val);

private:
    std::pair<int, int> _search(T val);
    std::pair<int, int> span(int height);
    void siftDown(const int startPos, const int index, const int childHeight);
    void siftUp(const int index, const int h);
    std::pair<int, int> getChildren(int parentIndex, int parentHeight);
    std::pair<int, int> getParents(int childIndex, int childHeight);
    int getSmallestChildIndex(std::pair<int, int>& children);
    int getLargestParentIndex(std::pair<int, int>& parents);
};

template<Comparable T, int N, typename Compare>
nBeap<T, N, Compare>::nBeap() : height(0), size(0) {}

template<Comparable T, int N, typename Compare>
nBeap<T, N, Compare>::~nBeap() = default;

template<Comparable T, int N, typename Compare>
bool nBeap<T, N, Compare>::search(T val)
{
    return _search(val).first != -1;
}

template<Comparable T, int N, typename Compare>
std::pair<int, int> nBeap<T, N, Compare>::_search(T val)
{
    return {-1, -1}; // stub
}

template<Comparable T, int N, typename Compare>
std::pair<int, int> nBeap<T, N, Compare>::span(int height)
{
    return {0, -1}; // stub
}

template<Comparable T, int N, typename Compare>
void nBeap<T, N, Compare>::siftDown(const int startPos, const int index, const int childHeight)
{
    if (size <= 1) return;

    T newItem = container[index];
    int currentPos = index;
    int currentHeight = childHeight;

    while (currentPos > startPos)
    {
        auto parents = getParents(currentPos, currentHeight);
        int maxParentPos = getLargestParentIndex(parents);

        if (maxParentPos == -1) break;
        // use only > or < to respect your concept (or change the concept)
        if (!(container[maxParentPos] > newItem)) break;

        container[currentPos] = container[maxParentPos];
        currentPos = maxParentPos;
        currentHeight--;
    }

    container[currentPos] = newItem;
}

template<Comparable T, int N, typename Compare>
void nBeap<T, N, Compare>::siftUp(const int index, const int h)
{
    if (container.empty()) return;

    int currentPos = index;
    int currentHeight = h;
    T newItem = container[index];

    while (true)
    {
        auto children = getChildren(currentPos, currentHeight);
        int minChildPos = getSmallestChildIndex(children);
        if (minChildPos == -1) break;

        container[currentPos] = container[minChildPos];
        currentPos = minChildPos;
        currentHeight++;
    }

    container[currentPos] = newItem;
    siftDown(index, currentPos, currentHeight);
}

template<Comparable T, int N, typename Compare>
std::pair<int, int> nBeap<T, N, Compare>::getChildren(int parentIndex, int parentHeight)
{
    return {-1, -1}; // stub
}

template<Comparable T, int N, typename Compare>
std::pair<int, int> nBeap<T, N, Compare>::getParents(int childIndex, int childHeight)
{
    return {-1, -1}; // stub
}

template<Comparable T, int N, typename Compare>
int nBeap<T, N, Compare>::getSmallestChildIndex(std::pair<int, int>& children)
{
    return -1; // stub
}

template<Comparable T, int N, typename Compare>
int nBeap<T, N, Compare>::getLargestParentIndex(std::pair<int, int>& parents)
{
    return -1; // stub
}
