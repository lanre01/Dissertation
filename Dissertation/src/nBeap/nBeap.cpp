#include "nBeap.hpp"

template<Comparable Type, int N>
nBeap<Type, N>::nBeap()
{
	height = 0;
	size = 0;
}

template<Comparable Type, int N>
nBeap<Type, N>::~nBeap()
{
}

template<Comparable Type, int N>
Type nBeap<Type, N>::remove_min()
{
	if(size <= 0)
    {
        throw std::out_of_range("cannot pop empty beap");
    }
    Type minValue = container[0];
    std::swap(container[0], container[size-1]);
    size--;
    std::pair<int, int> currentLevel = span(height);
    if (size-1 < currentLevel.first)
    {
        height--;
    }
    
    container.resize(size);
    siftUp(0, 1);
    return minValue;
}

template<Comparable Type, int N>
void nBeap<Type, N>::push(Type val)
{
	size++;
    std::pair<int,int> currentLevel = span(height);
    if (size - 1 > currentLevel.second)
    {
        height++;
    }
    container.push_back(val);
    siftDown(0, size - 1, height);
}

template<Comparable Type, int N>
void nBeap<Type, N>::remove(Type val)
{
	std::pair<int,int> indexAndHeight = _search(val);
    int index = indexAndHeight.first;

    if (index == -1) {
        return;
    }

    std::swap(container[index], container[size-1]);
    size--;
    std::pair<int,int> lastLevel = span(height);
    if (lastLevel.first >= size)
    {
        height--;
    }
    container.resize(size);
    if (index+1 == size)
    {
        // removing the last element in the vector.
        return;
    }
    siftUp(index, indexAndHeight.second);
}

template<Comparable Type, int N>
bool nBeap<Type, N>::search(Type val)
{
	return _search(val).first == -1 ? false : true;
}

template<Comparable Type, int N>
std::pair<int, int> nBeap<Type, N>::_search(Type val)
{
	return {};
}


template<Comparable Type, int N>
std::pair<int, int> nBeap<Type, N>::span(int height)
{
	return std::pair<int, int>();
}

template<Comparable Type, int N>
void nBeap<Type, N>::siftDown(const int startPos, const int index, const int childHeight)
{
	if (size <= 1)
    {
        return;
    }
    Type newItem = container[index];
    int currentPos = index;
    int currentHeight = childHeight;

    while (currentPos > startPos)
    {
        auto parents = getParents(currentHeight, currentPos);
        int maxParentPos = getLargestParent(parents);
        if (maxParentPos == -1 || newItem >= container[maxParentPos]) {
            break;
        }

        container[currentPos] = container[maxParentPos];
        currentPos = maxParentPos;
        currentHeight--;
    }

    container[currentPos] = newItem;
}

template<Comparable Type, int N>
void nBeap<Type, N>::siftUp(const int index, const int h)
{
	if (container.empty())
    {
        return;
    }

    int currentPos = index;
    int currentHeight = h;
    int newItem = container[index];

    while (true)
    {
        auto children = getChildren(currentHeight, currentPos);
		int minChildPos = getSmallestChildIndex(children);

        if (minChildPos == -1 ) {
            break;
        }

        // Move the smaller child up
        container[currentPos] = container[minChildPos];
        currentPos = minChildPos;
        currentHeight++;
    }

    container[currentPos] = newItem;
    siftDown(index, currentPos, currentHeight);
}

template<Comparable Type, int N>
std::pair<int, int> nBeap<Type, N>::getChildren(int parenIndex, int parentHeight)
{
	return {};
}

template<Comparable Type, int N>
std::pair<int, int> nBeap<Type, N>::getParents(int childIndex, int childHeight)
{
	return {};
}

template<Comparable Type, int N>
int nBeap<Type, N>::getSmallestChildIndex(std::pair<int, int>& children)
{
	return Type();
}
template<Comparable Type, int N>
int nBeap<Type, N>::getLargestParentIndex(std::pair<int, int>& parents)
{
	return Type();
}