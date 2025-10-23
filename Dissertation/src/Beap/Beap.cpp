#include "Beap.hpp"

Beap& Beap::operator=(Beap const& rhs)
{
    

    


}

void Beap::siftUp(int pos, int h)
{
    int startPos = pos;
    int endPos = size;
    int newItem = container[pos];

    std::pair<int, int> childPair = getChildren(h, pos);
    int minChildPos = childPair.first;

    while (minChildPos < endPos)
    {
        if ((childPair.second < endPos) && container[childPair.second] < container[minChildPos])
        {
            minChildPos = childPair.second;
        }

        container[pos] = container[minChildPos];
        pos = minChildPos;
        h++;
        childPair = getChildren(h, pos);
        minChildPos = childPair.first;
    }

    container[pos] = newItem;
    siftDown(startPos, pos, h);
    
}

void Beap::siftDown(int startPos, int pos, int startHeight) 
{
    int newItem = container[pos];

    while (pos > startPos)
    {
        std::pair<int, int> parents = getParents(startHeight, pos);
        int minParentPos = getMinIndexIncontainer(parents.first, parents.second);
        if (newItem < container[minParentPos])
        {
            container[pos] = container[minParentPos];
            pos = minParentPos;
            continue;
        }
        break;
    }

    container[pos] = newItem;
}

int Beap::getMinIndexIncontainer(int index1, int index2)
{
    if (index2 == -1) return index1;
    if (index1 == -1) return index2;

    return container[index1] < container[index2] ? index1 : index2;
}

std::pair<int, int> Beap::search(int value)
{
    auto lastLevel = span(height);

    int start = lastLevel.first, end = lastLevel.second;
    int idx = start;
    int h = height;

    while (true)
    {
        if (value > container[idx])
        {
            if (idx == end)
            {
                return { -1,-1 };
            }

            auto diff = idx - start;
            h--;
            lastLevel = span(h);
            start = lastLevel.first, end = lastLevel.second;
            idx = start + diff;
            continue;
        }
        else if (value < container[idx])
        {
            if ((unsigned int)idx == (size - 1))
            {
                auto diff = idx - start;
                h--;
                lastLevel = span(h);
                start = lastLevel.first, end = lastLevel.second;
                idx = start + diff;
                continue;
            }

            auto diff = idx - start;
            lastLevel = span(h + 1);
            auto newStart = lastLevel.first, newEnd = lastLevel.second;
            auto newIdx = newStart + diff + 1;
            if ((unsigned int) newIdx < size)
            {
                h++;
                start = newStart;
                end = newEnd;
                idx = newIdx;
                continue;
            }

            if (idx == end)
            {
                return { -1,-1 };
            }

            idx++;
            continue;
        }
        else
        {
            return { idx, h };
        }
    }
}

int Beap::pop()
{
    int minValue = container[0];
    std::swap(container[1], container[size]);
    size--;
    std::pair<int,int> currentLevel = Beap::span(height);
    if (size < (unsigned int)currentLevel.first)
    {
        height--;
    }
    
    container.resize(size);
    siftUp(0, 1);
    return minValue;
}

void Beap::push(int value)
{
    size++;
    std::pair<int, int> currentLevel = span(height);
    if (size > (unsigned int)currentLevel.second)
    {
        height++;
    }
    container.push_back(value);
    siftDown(0, size, 1);
} 

void Beap::remove(int value)
{
    auto indexAndHeight = search(value);
    int index = indexAndHeight.first;

    if (index == -1) {
        return;
    }

    auto level = span(indexAndHeight.second);
    int start = level.first, end = level.second;

    std::swap(container[index], container[size]);
    if (size - 1 == (unsigned int)start)
    {
        height--;
    }
    size--;
    container.resize(size);
    if ((unsigned int)index == size)
    {
        // removing the last element in the vector.
        return;
    }
    siftUp(index, height);
}

std::pair<int, int> Beap::span(int h)
{
    int start = (h * (h - 1)) / 2 + 1;
    int end = (h * (h + 1)) / 2;
    return {start, end};
}

std::pair<int, int> Beap::getParents(int childHeight, int childIndex) {
    int parentHeight = childHeight--;
    auto parentLevel = Beap::span(parentHeight);
    auto childLevel = Beap::span(childHeight);
    auto numberOfElementInTheLevel = parentLevel.second - parentLevel.first + 1;

    if (childIndex == childLevel.first)
    {
        return { parentLevel.first, -1 };
    }

    if (childIndex == childLevel.second)
    {
        return { -1, parentLevel.second };
    }

    auto firstParent = childIndex - numberOfElementInTheLevel;

    return { firstParent, firstParent++ };
}

std::pair<int, int> Beap::getChildren(int parentheight, int index)
{
    auto currentLevel = Beap::span(parentheight);
    auto numberOfElementsInTheLevel = currentLevel.second - currentLevel.first;

    return { index + numberOfElementsInTheLevel, index + numberOfElementsInTheLevel + 1 };
}