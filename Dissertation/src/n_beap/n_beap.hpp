// An N-Dimensional beap implementation based on J. Ian Munro and Hendra Suwanda's 
// bi-parental heap (see https://www.sciencedirect.com/science/article/pii/0022000080900379)
// Copyright Lawal Alongbija 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See http://www.boost.org/LICENSE_1_0.txt)

#include <stdexcept>
#include <concepts>
#include <vector>
#include <utility>
#include <functional>
#include <algorithm>
//#include <bits/stdc++.h>

template<typename T>
concept Comparable = requires(T a, T b) {
    { a < b } -> std::convertible_to<bool>;
    { a > b } -> std::convertible_to<bool>;
//    { a == b } -> std::convertible_to<bool>;
};

template<Comparable T, size_t N, typename Compare = std::greater<T>>
class NBeap
{
public:
    NBeap(Compare c) 
    : _height(0)
    , _size(0)
    , compare(c)
    , _interval(0, 0)
    , _levelSize(0)
    {}

    NBeap() 
    : _height(0)
    , _size(0)
    , compare(Compare())
    , _interval(0,0)
    , _levelSize(0)
    {}

    ~NBeap() = default;


    NBeap& operator=(const NBeap&) = default;
    void reserve(size_t count)
    {
        _container.reserve(count);
    }

    T extract() {

        // check we can actually extract
        if(_size <= 0)
        {
            throw std::out_of_range("cannot pop empty nbeap");
        }

        _size--;
        auto minElem = std::exchange(_container[0], _container[_size]);
        _container.resize(_size);

        // if we just extracted the last element
        if(_size == 0)
        {
            _levelSize = 0;
            _height = 0;
            _interval = {0, 0};
            return minElem;
        }

        // in case the last element on a level is removed 
        if(_size - 1 < _interval.first)
        {
            if constexpr (N == 1)
            {
                _height--;
                _interval.first -= 1;
                _interval.second -= 1;
                
            }

            else if constexpr (N == 2)
            {
                _levelSize--;
                _height--;
                _interval.second = _interval.first - 1;
                _interval.first -= _levelSize;
            }
            else {
                _interval = getPreviousLevelInterval(_height, _interval);
                _levelSize = _interval.second - _interval.first + 1; 
                _height--;
            }
            
        }

        siftDown(0, 1);

        return minElem;
    }

    void insert(T val)
    {
        _container.push_back(std::move(val));

        if(_size == 0)
        {
            _size++;
            _height++;
            _interval = {0,0};
            _levelSize++;
            return;
        }

        if constexpr (N == 1)
        {
            _height++;
            _interval.first++;
            _interval.second++;
        }
        else {
            if (_interval.second < _size)
            {
                if constexpr(N == 2) 
                {
                    _height++;
                    _levelSize++;
                    _interval.first = _interval.second + 1; 
                    _interval.second += _levelSize;
                    
                }
                else {
                    _interval = getNextLevelInterval(_height, _interval);
                    _levelSize = _interval.second - _interval.first + 1;
                    _height++;
                }
                
            }
        }

       

        bubbleUp(0, _size, _height, _interval);

        _size++;
    }

    void remove(T val)
    {
        std::pair<uint64_t, unsigned int> out{-1,-1};

        if(!_searchHelper(val, out))
        {
            return;
        }
        
        std::swap(_container[out.first], _container[_size - 1]);

        _size--;
        _container.resize(_size);

        if(_size == 0)
        {
            _levelSize = 0;
            _height = 0;
            _interval = {0, 0};
            return;
        }

        if(_size - 1 < _interval.first)
        {
            _levelSize = getNumOfElemInPrevLevel(_height, _levelSize);
            _interval.second = _interval.first  - 1;
            _interval.first -= _levelSize;
            _height--;
        }

        auto li = span(out.second, _interval, _height);


        if(compare(val, _container[out.first]))
        {
            bubbleUp(0, out.first, out.second, li);
        }
        

        siftDown(out.first, out.second, li);

    }

    bool search(T val) { 
        
        if(_size <= 0 || compare(_container[0], val))
        {
            return false;
        }

        std::pair<uint64_t, unsigned int> out;

        if constexpr (N == 1)
        {
            return _search1D(val, out);
        }
        else if constexpr (N == 2)
        {
            return _search2D(val, out);
        }
        else {
            return _searchND(val, _interval.first, _height, _interval, out);
        }

    };

    uint64_t size() {return _size;}
    unsigned int height() {return _height - 1;}
    bool empty() {return _size == 0; }
    void clear()
    {
        _height = 0;
        _size = 0;
        _container.clear();
        _interval = {0, 0};
        _levelSize = 0;
    }

    void printState(const std::string& operation) {
    	std::cout << "After " << operation << ": " << std::endl;
        uint64_t i = 0;
        unsigned int h = 1;
        uint64_t n = 1;
        while(i < _size)
        {
            for (uint64_t k = 0; k < n; k++)
            {
                if(i+k >= _size)
                {
                    break;
                }
                std::cout << _container[i+k] << " ";
            }
            std::cout << std::endl;

            i+=n;
            n = getNumberOfElementInNextLevel(h, n);
            h++;
        }
		std::cout << " | size=" << _size << " height=" << _height << " levelSize=" << _levelSize << std::endl;
        std::cout << " | intervals=" << _interval.first << "," << _interval.second << std::endl;
		std::cout.flush();
	}


private:
    unsigned int _height;
    uint64_t _size;
    std::vector<T> _container;
    Compare compare;
    std::pair<uint64_t, uint64_t> _interval;
    uint64_t _levelSize;

    static constexpr uint64_t INVALID_INDEX = static_cast<size_t>(-1);


    std::optional<std::pair<uint64_t, unsigned int>> _search(
        T val, uint64_t from, unsigned int height, std::pair<uint64_t, uint64_t> levelInterval
    ); // returns the index and height
    std::pair<uint64_t, uint64_t> span(
        unsigned int preferredHeight, 
        std::pair<uint64_t, uint64_t> knownHeighInterval, 
        unsigned int knowHeight
    );
    
    void siftDown(uint64_t startPos, unsigned int h, std::pair<uint64_t, uint64_t> levelInterval = {0,0});
    void bubbleUp(uint64_t index, uint64_t endIndex, unsigned int h, std::pair<uint64_t, uint64_t> levelInterval);
    
    uint64_t getBestParentIndex(
        uint64_t currentPos, unsigned int childHeight, std::pair<uint64_t, uint64_t> levelInterval
    );

    uint64_t getBestChild(
        uint64_t currentPos, unsigned int childHeight, std::pair<uint64_t, uint64_t> levelInterval
    );

    std::pair<uint64_t, uint64_t> getNMinusOffsetLevel(
        uint64_t levelIndex, std::pair<uint64_t, uint64_t> levelInterval, uint64_t offset = 1
    );

    inline uint64_t getNumberOfElementInNextLevel(
        unsigned int currHeight, uint64_t numOfElemCurrLevel, int offsetDim
    );

    bool _search1D(T val, std::pair<uint64_t, unsigned int>& out)
    {
        // can simplify this using binary search 
        auto from = _interval.first;
        auto h = _height;
        while(from >= 0)
        {
            if(compare(_container[from], val))
            {
                from--;
                h--;
            }
            else if(compare(val, _container[from]))
            {
                return false;
            }
            else{
                out.first = from;
                out.second = h;
                return true;
            }
        }

        return false;
    }

    bool _search2D(T val, std::pair<uint64_t, unsigned int>& out)
    {

        if(_size <= 0 || compare(_container[0], val))
        {
            return false;
        }

        size_t h = _height;
        size_t end = _interval.first;
        size_t start = _interval.second;
        std::pair<size_t, size_t> currentLevel = _interval;

        if(val == _container[end])
        {
            out.first = end;
            out.second = h;
            return true;
        }

        if(start >= _size)
        {
            start = start - h;
            currentLevel = getPreviousLevelInterval(h, currentLevel);
            h--;
        }

        size_t idx = start;

        while(true)
        {
            if (idx == end)
            {
                return false;
            }

            if (compare(_container[idx], val))
            {
                // auto firstParent = idx - h;
                // This condition is actually not needed. Idx must never go up 
                // when on the first element in any level
                if (idx == currentLevel.first)
                {
                    return false; 
                }

                idx -= h;
                currentLevel = getPreviousLevelInterval(h, currentLevel);
                h--;
            }
            else if (compare(val, _container[idx]))
            {
                auto firstChild = idx + h;
                if(firstChild >= _size)
                {
                    idx--;
                }
                else
                {
                    idx = firstChild;
                    currentLevel = getNextLevelInterval(h, currentLevel);
                    h++;
                    
                }
            }
            else {
                out.first = idx;
                out.second = h;
                return true;
            }

        }
    }

    bool _searchHelper(T val, std::pair<uint64_t, unsigned int>& out)
    {
        if constexpr (N == 1)
        {
            return _search1D(val, out);
        }
        else if constexpr (N == 2)
        {
            return _search2D(val, out);
        }
        else {
            return _searchND(val, _interval.first, _height, _interval, out);
        }

    }

    inline uint64_t getNumberOfElementInNextLevel(unsigned int currHeight, uint64_t numOfElemCurrLevel)
    {
        if constexpr (N == 1)
        {
            return numOfElemCurrLevel;
        }
        if constexpr (N == 2)
        {
            return ++numOfElemCurrLevel;
        }

        
        return  (numOfElemCurrLevel * (currHeight + N - 1 )) / currHeight;
    }

    inline uint64_t getNumOfElemInPrevLevel(unsigned int currHeight, uint64_t numOfElemCurrLevel)
    {
        if(currHeight <= 1)
            return 0;

        if constexpr (N == 1)
        {
            return numOfElemCurrLevel;
        }
        else if constexpr (N == 2)
        {
            return --numOfElemCurrLevel;
        }

        return (numOfElemCurrLevel * (currHeight - 1)) / (currHeight + N - 2);
    }

    inline std::pair<uint64_t, uint64_t> getPreviousLevelInterval(
        unsigned int currHeight, 
        std::pair<uint64_t, uint64_t> currLevInt
    )
    {
        auto l = getNumOfElemInPrevLevel(currHeight, INTERVALSIZE(currLevInt));

        return {currLevInt.first - l, currLevInt.first - 1};
    }

    inline std::pair<uint64_t, uint64_t> getNextLevelInterval(
        unsigned int currHeight, std::pair<uint64_t, uint64_t> currLevInt
    )
    { 
        auto l = getNumberOfElementInNextLevel(currHeight,  INTERVALSIZE(currLevInt));

        return {currLevInt.second + 1, currLevInt.second + l};
    }

    uint64_t getBestOtherChildren(
        uint64_t parentId, uint64_t currentPos, std::pair<uint64_t, uint64_t> levelInterval, 
        uint64_t maxNumOfChildren, uint64_t dOffset
    );

    inline std::array<uint64_t, 3> getNMinusOffsetLevelWithParentLevel(
        uint64_t prevLevelStart, uint64_t currentPos, uint64_t offset
    ); 

    inline uint64_t getBestRemainingParents(
        uint64_t currentPos, std::pair<uint64_t, uint64_t> parentsInterval, uint64_t dOffset
    );

    inline uint64_t INTERVALSIZE(const std::pair<uint64_t, uint64_t>& p)
    {
        return p.second - p.first + 1;
    }

    bool _searchND(
        T val, const uint64_t currPos, const unsigned int currHeight, 
        const std::pair<uint64_t, uint64_t> levelInterval,
        std::pair<uint64_t, unsigned int>& out
    );

    std::pair<uint64_t, size_t> getFirstUniqueChildPos(
        uint64_t parentIdx, uint64_t firstChildPos, size_t maxNumberOfChildren,
        std::pair<uint64_t, uint64_t> levelInterval
    );
    
};


template <Comparable T, size_t N, typename Compare>
std::pair<uint64_t, uint64_t> NBeap<T, N, Compare>::span(
    unsigned int preferredHeight,
    std::pair<uint64_t, uint64_t> knownHeightInterval,
    unsigned int knownHeight)
{
    if (preferredHeight == knownHeight)
    {
        return knownHeightInterval;
    }

    

    while(knownHeight > preferredHeight)
    {
        auto prevLevelElems = 
            getNumOfElemInPrevLevel(knownHeight, INTERVALSIZE(knownHeightInterval));
        knownHeightInterval.second = knownHeightInterval.first - 1;
        knownHeightInterval.first -= prevLevelElems; 
        knownHeight--;
    }

    return knownHeightInterval; 
}

template<Comparable T, size_t N, typename Compare>
void NBeap<T, N, Compare>::siftDown(
    uint64_t startPos, unsigned int h, std::pair<uint64_t, uint64_t> levelInterval
)
{
    T val = std::move(_container[startPos]);
    auto currPos = startPos;

    while(h < _height)
    {
        uint64_t bestChild = 
                getBestChild(currPos, h, levelInterval);
        
        if (bestChild == INVALID_INDEX || compare(_container[bestChild], val))
        {
            break;
        }
        
        _container[currPos] = std::move(_container[bestChild]);
        currPos = bestChild;
        levelInterval = getNextLevelInterval(h, levelInterval);
        h++;
    }

    _container[currPos] = std::move(val);

}

template<Comparable T, size_t N, typename Compare>
void NBeap<T, N, Compare>::bubbleUp(
    uint64_t toIndex, uint64_t fromIndex, 
    unsigned int h, std::pair<uint64_t, uint64_t> levelInterval
)
{
    auto currentPos = fromIndex;
    auto currentHeight = h;
    T val = std::move(_container[fromIndex]);
    
    while (currentPos > toIndex)
    {
        auto bestParent = getBestParentIndex(currentPos, currentHeight, levelInterval);

        if(compare(val, _container[bestParent]))
        {
            break;
        }
        _container[currentPos] = std::move(_container[bestParent]);
        
        levelInterval = getPreviousLevelInterval(currentHeight, levelInterval);
        currentPos = bestParent;
        currentHeight--;
        
    }

    _container[currentPos] = std::move(val);
}

template <Comparable T, size_t N, typename Compare>
uint64_t NBeap<T, N, Compare>::getBestParentIndex(
    uint64_t currentPos, 
    unsigned int childHeight,
    std::pair<uint64_t, uint64_t> levelInterval
)
{
    
    if constexpr (N == 1)
    {
        return currentPos - 1;
    }
    else if constexpr (N == 2) 
    { 
        //auto levelSize = INTERVALSIZE(levelInterval);
        auto firstParent = currentPos - childHeight;

        if(currentPos == levelInterval.first)
        {
            return firstParent + 1;
        }

        if(currentPos == levelInterval.second)
        {
            return firstParent;
        }

        return firstParent + !compare(_container[firstParent], _container[firstParent+1]);
    } 
    else 
    {
        auto parentsInterval = getPreviousLevelInterval(childHeight, levelInterval);

        if(currentPos == levelInterval.first)
        {
            return parentsInterval.first;
        }

        if(currentPos == levelInterval.second)
        {
            return parentsInterval.second;
        }

        auto parentsLevelSize = parentsInterval.second - parentsInterval.first + 1;
        if(parentsLevelSize == 1)
        {
            return parentsInterval.first;
        }

        auto bestParent = currentPos - parentsLevelSize;
        if(bestParent > parentsInterval.second)
        {
            return getBestRemainingParents(bestParent, parentsInterval, 1);
        }

    
        auto bestParentFromRest = 
            getBestRemainingParents(bestParent, parentsInterval, 1);

        if (bestParentFromRest >= INVALID_INDEX)
        {
            return bestParent;
        }

        return bestParentFromRest + (bestParent - bestParentFromRest) * 
                        !compare(_container[bestParentFromRest], _container[bestParent]);
        
    }
    
}

template <Comparable T, size_t N, typename Compare>
inline uint64_t NBeap<T, N, Compare>::getBestRemainingParents(
    uint64_t currentPos, std::pair<uint64_t, uint64_t> parentsInterval, uint64_t dOffset
)
{
    std::array<uint64_t, 3> res; 
    std::pair<uint64_t, uint64_t> innerLevel;

    std::pair<uint64_t, uint64_t> prevInnerLevel;

    uint64_t parent; 
    uint64_t bestParent = INVALID_INDEX;

    while (1)
    {
        // start by getting the inner level for both parents and child
        res = getNMinusOffsetLevelWithParentLevel(parentsInterval.first, currentPos, dOffset);
        innerLevel = std::pair{res[1]+1, res[2]};

        // get the previous inner level
        prevInnerLevel = std::pair{res[0], res[1]};
        auto prevInnerLevelSize = INTERVALSIZE(prevInnerLevel);

        if(currentPos == innerLevel.first)
        {
            if (bestParent == INVALID_INDEX)
            {
                return prevInnerLevel.first;
            }

            return prevInnerLevel.first + (bestParent - prevInnerLevel.first) * 
                !compare(_container[prevInnerLevel.first], _container[bestParent]); 
        }

        if(currentPos == innerLevel.second) 
        {
            if (bestParent == INVALID_INDEX)
            {
                return prevInnerLevel.second;
            }

            return prevInnerLevel.second + (bestParent - prevInnerLevel.second) * 
                !compare(_container[prevInnerLevel.second], _container[bestParent]); 
        }

        parent = currentPos - prevInnerLevelSize;

        if (parent <= prevInnerLevel.second)
        {
            if (bestParent == INVALID_INDEX)
            {
                bestParent = parent;
                
            }
            else {
                bestParent = parent + (bestParent - parent) * 
                    !compare(_container[parent], _container[bestParent]); 
            }
        }

        currentPos = parent;
        dOffset++;
        parentsInterval = prevInnerLevel;
    }
}

template <Comparable T, size_t N, typename Compare>
uint64_t NBeap<T, N, Compare>::getBestChild(
    uint64_t currentPos, 
    unsigned int parentHeight, 
    std::pair<uint64_t, uint64_t> levelInterval
)
{
    if constexpr (N == 1)
    {
        auto child = currentPos + 1;
        if (child >= _size)
            return INVALID_INDEX;

        return child;
    }
    else if constexpr (N == 2) 
    {
        auto child1 = currentPos + parentHeight;
        
        if(child1 >= _size)
            return INVALID_INDEX;

        auto child2 =  child1 + 1;
        
        if(child2 >= _size)
        {
            return child1;
        }

        return child1 + !compare(_container[child2], _container[child1]);
    } 
    else 
    {
        /*
        An N-Dimensional beap is just layers of N-1 Dimensional beap
        By looking at each layer of the N-Dimensional Beap, some patterns 
        emerge. Each node in the parent layer connects to a node in 
        the height of its sublayer in the children layer and connects to N-1
        nodes in the height + 1 of its sublayer.  
        */
        if(currentPos == levelInterval.first)
        {
            auto minChild = levelInterval.second + 1;
            if(minChild >= _size)
            {
                return INVALID_INDEX;
            }

            auto maxNumOfChildren = N - 1;
            //std::cout << "For index: " << currentPos << " First Child: " << minChild << ", ";
            auto nextChild = minChild + 1;
            while (maxNumOfChildren > 0 && nextChild < _size)
            {
                //std::cout << "Next Child: " << nextChild << ", ";
                minChild = minChild + (nextChild - minChild) * 
                            !compare(_container[nextChild], _container[minChild]);
                nextChild++;
                maxNumOfChildren--;
            }
            
            return minChild;
        }


        auto levelSize = levelInterval.second - levelInterval.first + 1;
        auto minChild =  currentPos + levelSize;
        if(minChild >= _size)
            return INVALID_INDEX;
        
        auto minChildRest = getBestOtherChildren(currentPos, minChild, levelInterval, N - 1, 1);
        if (minChildRest == INVALID_INDEX)
        {
            return minChild;
        }

        return minChild + (minChildRest - minChild) * !compare(_container[minChildRest], _container[minChild]);
    }
    
    
}

// Returns either NULL or the index and height of the element being found
template <Comparable T, size_t N, typename Compare>
bool NBeap<T, N, Compare>::_searchND(
    T val, const uint64_t currPos, const unsigned int currHeight, 
    const std::pair<uint64_t, uint64_t> levelInterval, 
    std::pair<uint64_t, unsigned int>& out
) 
{
    if(currPos >= _size)
    {
        return false;
    }

    if(compare(_container[currPos], val))
    {
        auto parentLevel = getPreviousLevelInterval(currHeight, levelInterval);
        auto newPos = currPos - INTERVALSIZE(parentLevel);
        if(newPos >= levelInterval.first)
        {
            return false;
        } 
        
        return _searchND(val, newPos, currHeight - 1, parentLevel, out);
    }
    else if(compare(val, _container[currPos]))
    {
    
        auto firstChild = currPos + INTERVALSIZE(levelInterval);
        //auto maxNumberOfChildren = N - 1;
        auto firstUniqueChildPosAndMaxNumOfChildren = 
            getFirstUniqueChildPos(currPos, firstChild, N-1, levelInterval);

        if(firstChild >= _size || firstUniqueChildPosAndMaxNumOfChildren.first >= _size)
        {
            auto parentSibling = 
                firstUniqueChildPosAndMaxNumOfChildren.first - INTERVALSIZE(levelInterval);
            auto maxSibling = firstUniqueChildPosAndMaxNumOfChildren.second;

            while(maxSibling > 0)
            {
                if(parentSibling >= _size)
                {
                    // Actually we want to go the grandparent
                    auto grandParentLevel = getPreviousLevelInterval(currHeight, levelInterval);
                    auto grandParent = parentSibling - INTERVALSIZE(grandParentLevel);
                    if(grandParent >= levelInterval.first) // only going to happen if the last level is not full
                    {
                        return false;
                    } 

                    if(_searchND(val, grandParent, currHeight - 1, grandParentLevel, out))
                    {
                        return true;
                    }
                }
                else {
                    if(_searchND(val, parentSibling, currHeight, levelInterval, out))
                    {
                        return true;
                    }
                }

                
                parentSibling++;
                maxSibling--;

            }
        }
        else 
        {
            auto childrenLevel = getNextLevelInterval(currHeight, levelInterval);
            auto childPos = firstUniqueChildPosAndMaxNumOfChildren.first;
            auto maxNumberOfChildren = firstUniqueChildPosAndMaxNumOfChildren.second;

            while(maxNumberOfChildren > 0)
            {
                if(childPos >= _size)
                {
                    // should i be breaking here?
                    // we should actually be going to that childs parent
                    // instead of break
                    //auto grandParentLevel = getPreviousLevelInterval(currHeight, levelInterval);
                    auto sibling = childPos - INTERVALSIZE(levelInterval);
                    if(sibling > levelInterval.second) // only going to happen if the last level is not full
                    {
                        return false;
                    } 

                    if(_searchND(val, sibling, currHeight, levelInterval, out))
                    {
                        return true;
                    }
                }
                else {
                    if(_searchND(val, childPos, currHeight + 1, childrenLevel, out))
                    {
                        return true;
                    }
                }

                
                childPos++;
                maxNumberOfChildren--;
            }
        }

        return false;
    }

    else
    {
        out.first = currPos; 
        out.second = currHeight;
        return true;
    }

}

template <Comparable T, size_t N, typename Compare>
std::pair<uint64_t, size_t> NBeap<T, N, Compare>::getFirstUniqueChildPos(
    uint64_t parentIdx, uint64_t firstChildPos, size_t maxNumberOfChildren,
    std::pair<uint64_t, uint64_t> levelInterval
)
{
    auto innerLevel = getNMinusOffsetLevel(parentIdx, levelInterval, 1);
    auto nextchildPos = firstChildPos + INTERVALSIZE(innerLevel);
    auto offset = 2;
    while(innerLevel.first != parentIdx)
    {
        innerLevel = getNMinusOffsetLevel(parentIdx, innerLevel, offset);
        nextchildPos += INTERVALSIZE(innerLevel);
        maxNumberOfChildren--;
        offset++;
    }


    return {nextchildPos, maxNumberOfChildren};
}

template <Comparable T, size_t N, typename Compare>
uint64_t NBeap<T, N, Compare>::getBestOtherChildren(
    uint64_t parentId, uint64_t currentPos, std::pair<uint64_t, uint64_t> levelInterval,
    uint64_t maxNumOfChildren, uint64_t dOffset
)
{
    std::pair<uint64_t, uint64_t> innerLayer = levelInterval;
    uint64_t bestChild = INVALID_INDEX;
    uint64_t currentChild = currentPos;

    while(dOffset <= N)
    {
        innerLayer = getNMinusOffsetLevel(parentId, innerLayer, dOffset);
        
        currentChild += INTERVALSIZE(innerLayer);

        if(currentChild >= _size)
        {
            return bestChild;
        }

        if(bestChild == INVALID_INDEX)
        {
            bestChild = currentChild;
        }
        else {
            bestChild = bestChild + (currentChild - bestChild) * 
                    !compare(_container[currentChild], _container[bestChild]);
        }

        dOffset++;
    }

    return bestChild;
}

template <Comparable T, size_t N, typename Compare>
inline std::pair<uint64_t, uint64_t> NBeap<T, N, Compare>::getNMinusOffsetLevel(
    uint64_t parentIdx, std::pair<uint64_t, uint64_t> levelInterval, uint64_t offset
) 
{

    if(parentIdx == levelInterval.first) // should not happen because this is already accounted for up
    {
        return {parentIdx, parentIdx};
    }

    // Possible optimisation here is to start searching
    // from the back since most element should be at the last level
    uint64_t h = 1, l = 1, end = levelInterval.first, start = levelInterval.first;
    //uint64_t maxInterval = levelInterval.second - levelInterval.first + 1; // can replace levelInterval with the level size acc
    while(1) 
    {
        l = getNumberOfElementInNextLevel(h, l, offset);
        start = end;
        end += l;
        h++;
        if(parentIdx <= end)
        {
            return {start + 1, end};
        }
    }
}

template <Comparable T, size_t N, typename Compare>
inline std::array<uint64_t, 3> NBeap<T, N, Compare>::getNMinusOffsetLevelWithParentLevel(
    uint64_t parentLevelStart, uint64_t currentPos, uint64_t offset
) 
{

    if(currentPos == parentLevelStart) // should not happen because this is already accounted for up
    {
        return {parentLevelStart-1, parentLevelStart-1, parentLevelStart};
    }

    // Possible optimisation here is to start searching
    // from the back since most element should be at the last level
    uint64_t curretHeight = 1, numOfElementInLevel = 1, curLevelEnd = parentLevelStart, 
    prevLevelEnd = parentLevelStart-1, prevLevelStart = parentLevelStart;
    while(true) 
    {
        numOfElementInLevel = getNumberOfElementInNextLevel(curretHeight, numOfElementInLevel, offset);
        prevLevelStart = prevLevelEnd + 1;
        prevLevelEnd = curLevelEnd;
        curLevelEnd += numOfElementInLevel;
        curretHeight++;
        if(currentPos <= curLevelEnd)
        {
            return {prevLevelStart, prevLevelEnd, curLevelEnd};
        }
    }

    //return {prevLevelStart, prevLevelEnd, curLevelEnd};
}

template <Comparable T, size_t N, typename Compare>
inline uint64_t NBeap<T, N, Compare>::getNumberOfElementInNextLevel(
    unsigned int currHeight, uint64_t numOfElemCurrLevel, int offsetDim
)
{
    switch (N-offsetDim)
    {
    case 1:
        return 1;
    case 2:
        return ++numOfElemCurrLevel;
    default:
        return (numOfElemCurrLevel * (currHeight + (N - offsetDim) - 1 )) / currHeight;
    }
}