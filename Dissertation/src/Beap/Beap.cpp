#include "Beap.hpp"

Beap& Beap::operator=(Beap const& rhs)
{
    if (this == &rhs)
    {
        return *this;
    }

    container = rhs.container;
    height = rhs.height;
    size = rhs.size;
    return *this;
}

void Beap::siftUp(const int pos, const int h)
{
    if (container.empty())
    {
        return;
    }

    int currentPos = pos;
    int currentHeight = h;
    int newItem = container[pos];

    while (true)
    {
        std::pair<int, int> children = getChildren(currentHeight, currentPos);
        int leftChild = children.first;
        int rightChild = children.second;
        //std::cout << "leftChild is " << container[leftChild] << "and rightChild is " << container[rightChild] << std::endl;
        // Find the smallest child
        int minChildPos = -1;
        if (leftChild < size) {
            minChildPos = leftChild;
            if (rightChild < size && container[rightChild] < container[leftChild]) {
                minChildPos = rightChild;
            }
        }

        //std::cout << "min child is " << container[minChildPos] << std::endl;

        // If no children or children are larger, we're done
        if (minChildPos == -1 /* || newItem <= container[minChildPos]*/) {
            break;
        }

        // Move the smaller child up
        container[currentPos] = container[minChildPos];
        currentPos = minChildPos;
        currentHeight++;
    }

    container[currentPos] = newItem;
    siftDown(pos, currentPos, currentHeight);
}

void Beap::siftDown(const int startPos, const int pos, const int childHeight) 
{
    if (container.empty())
    {
        return;
    }
    // siftDown(0, size-1, height);
    //std::cout << "pushing " << container[pos] << " from " << pos << " to " << startPos << std::endl;
    int newItem = container[pos];
    int currentPos = pos;
    int currentHeight = childHeight;

    while (currentPos > startPos)
    {
        std::pair<int, int> parents = getParents(currentHeight, currentPos);
        //std::cout << parents.first << ',' << parents.second << " at height " << childHeight << std::endl;
        int maxParentPos = getMaxIndexIncontainer(parents.first, parents.second);
        if (maxParentPos == -1 || newItem >= container[maxParentPos]) {
            break;
        }

        container[currentPos] = container[maxParentPos];
        currentPos = maxParentPos;
        currentHeight--;
    }
    //std::cout << newItem << std::endl;
    container[currentPos] = newItem;
}

int Beap::getMaxIndexIncontainer(int index1, int index2)
{
    if (index2 == -1) return index1;
    if (index1 == -1) return index2;

    return container[index1] > container[index2] ? index1 : index2;
}

std::pair<int, int> Beap::search(int value)
{
    if(size <= 0 || value < container[0])
    {
        return {-1, -1};
    }

    int h = height;
    std::pair<int, int> lastLevel = span(h);
    int end = lastLevel.first;
    
    if(value == container[end])
    {
        return {end, h};
    }
    int start = lastLevel.second;

    if(start >= size)
    {
        h--;
        start = span(h).second;
    }

    int idx = start;
    
    while(true)
    {
        /*std::cout 
            << "\n[SEARCH STEP]"
            << " idx=" << idx
            << " h=" << h
            << " value=" << value
            << " current=" << (idx >= 0 && idx < size ? std::to_string(container[idx]) : "OUT_OF_RANGE")
            << " | level_span=" << span(h).first << "-" << span(h).second
            << " | height=" << height
            << " | size=" << size
            << std::endl;*/
        
        #ifdef DEBUG
        auto row = span(h);
        if (idx < row.first || idx > row.second || idx < 0 || idx >= size) {
            std::cerr << "[INVARIANT BROKEN] idx=" << idx
                    << " h=" << h
                    << " span=" << row.first << "-" << row.second
                    << " size=" << size << std::endl;
            throw std::logic_error("search left beap geometry");
        }
        #endif

        if(idx == end)
        {
            return {-1, -1};
        }

        if(value < container[idx])
        {
            std::pair<int, int> parents = getParents(h, idx);
            if(parents.first <= -1)
            {
                return {-1, -1};
            }
            idx = parents.first;
            h--;
        }
        else if(value > container[idx])
        {
            auto children = getChildren(h, idx);
            if(children.first >= size)
            {
                idx--;
            }
            else{
                idx = children.first;
                h++;
            }

        }
        else{
            return {idx, h};
        }
    }
}

int Beap::pop()
{
    if(size <= 0)
    {
        throw std::out_of_range("cannot pop empty beap");
    }
    int minValue = container[0];
    std::swap(container[0], container[size-1]);
    size--;
    std::pair<int, int> currentLevel = Beap::span(height);
    if (size-1 < currentLevel.first)
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
    std::pair<int,int> currentLevel = span(height);
    //std::cout << currentLevel.first << "," << currentLevel.second << std::endl;
    if (size - 1 > currentLevel.second)
    {
        height++;
    }
    container.push_back(value);
    siftDown(0, size - 1, height);
} 

void Beap::remove(int value)
{
    std::pair<int,int> indexAndHeight = search(value);
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

inline std::pair<int, int> Beap::span(int h)
{
    int start = ((h * (h - 1)) / 2) + 1;
    int end = (h * (h + 1)) / 2;
    return {start-1, end-1};
    /*
    0 - 0*(0-1) / 2 + 1 = 1. 0*(0+1) / 2 = 0 - (1,0) - (0, -1)
    1 - 1*(1-1)/2 + 1 = 1, 1*(1+1)/2 = 1 = (1,1) - (0, 0)
    2 - 2*(2-1)/2 + 1 = 2, 2*(2+1)/2 = 3 = (2,3) - (1, 2)
    */
}

std::pair<int, int> Beap::getParents(const int childHeight, const int childIndex) {
    
    int parentHeight = childHeight - 1;
    std::pair<int, int> parentLevel = Beap::span(parentHeight);
    std::pair<int, int> childLevel = Beap::span(childHeight);
    int numberOfElementInTheLevel = parentLevel.second - parentLevel.first + 1;

    if (childIndex == childLevel.first)
    {
        return {-1 ,  parentLevel.first};
    }

    if (childIndex == childLevel.second)
    {
        return {parentLevel.second, -1};
    }

    int secondParent = childIndex - numberOfElementInTheLevel;

    return { secondParent - 1, secondParent};
}

std::pair<int, int> Beap::getChildren(int parentHeight, int index)
{
    std::pair<int,int> currentLevel = Beap::span(parentHeight);
    int numberOfElementsInTheLevel = currentLevel.second - currentLevel.first + 1;

    return { index + numberOfElementsInTheLevel, index + numberOfElementsInTheLevel + 1 };
}