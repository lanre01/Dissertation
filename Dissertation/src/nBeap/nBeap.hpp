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
    , _interval(0, 0)
    , _levelSize(0)
    {}

    nBeap() 
    : _height(0)
    , _size(0)
    , compare(Compare())
    , _interval(0,0)
    , _levelSize(0)
    {}

    ~nBeap() = default;


    nBeap& operator=(const nBeap&) = default;

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
            _levelSize = 0;
            _height = 0;
            _interval = {0, 0};
            return minElem;
        }

        // in case the last element on a level is removed 
        if(_size - 1 < _interval.first)
        {
            _interval = getPreviousLevelInterval(_height, _interval);
            _levelSize = _interval.second - _interval.first + 1; 
            _height--;
        }

        siftDown(0, 1);

        return minElem;
    }

    void insert(T val)
    {
        container.push_back(std::move(val));

        if(_size == 0)
        {
            _size++;
            _height++;
            _interval = {0,0};
            _levelSize++;
            return;
        }

        if(_interval.second < _size)
        {
            _interval = getNextLevelInterval(_height, _interval);
            _levelSize = _interval.second - _interval.first + 1;
            _height++;
        }

        bubbleUp(0, _size, _height, _interval);

        _size++;
    }

    void remove(T val)
    {
        auto res = _search(val);

        if(!res.has_value())
        {
            return;
        }

        auto resVal = res.value();
        std::swap(container[resVal.first], container[_size - 1]);

        _size--;
        container.resize(_size);

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
        auto li = span(resVal.second);

        auto selected_parent_optional = getMinOrMaxParentIndex(resVal.first, resVal.second, li);
        if(!selected_parent_optional.has_value() || 
            compare(container[selected_parent_optional.value()], container[resVal.first])
        )
        {
            bubbleUp(resVal.first, 0, resVal.second, li);
            return;
        }


        siftDown(resVal.first, resVal.second, li);

    }

    bool search(T val) { 
        return _search(val).has_value(); 
    };

    uint64_t size() {return _size;}
    unsigned int height() {return _height - 1;}
    bool empty() {return _size == 0; }

    void printState(const std::string& operation) {
    	std::cout << "After " << operation << ": ";
		for (T val : container) {
			std::cout << val << " ";
		}
		std::cout << " | size=" << _size << " height=" << _height << " levelSize=" << _levelSize << std::endl;
        std::cout << " | intervals=" << _interval.first << "," << _interval.second << std::endl;
		std::cout.flush();
	}


private:
    unsigned int _height;
    uint64_t _size;
    std::vector<T> container;
    Compare compare;
    std::pair<uint64_t, uint64_t> _interval;
    uint64_t _levelSize;
    const size_t SECOND_LAYER_START = 3;

    struct Vec { std::array<unsigned int, N> data; };

    std::optional<std::pair<uint64_t, unsigned int>> _search(T val); // returns the index and height
    std::pair<uint64_t, uint64_t> span(
        unsigned int preferredHeight, 
        std::pair<uint64_t, uint64_t> knownHeighInterval, 
        unsigned int knowHeight
    );
    
    void siftDown(uint64_t startPos, unsigned int h, std::pair<uint64_t, uint64_t> levelInterval = {0,0});
    void bubbleUp(uint64_t index, uint64_t endIndex, unsigned int h, std::pair<uint64_t, uint64_t> levelInterval);
    
    std::optional<uint64_t> getMinOrMaxParentIndex(
        uint64_t currentPos, unsigned int childHeight, std::pair<uint64_t, uint64_t> levelInterval
    );
    std::optional<uint64_t> getMinOrMaxChildIndex(
        uint64_t currentPos, unsigned int childHeight, std::pair<uint64_t, uint64_t> levelInterval
    );

    std::optional<uint64_t> getOtherChildren(
        uint64_t currentPos, unsigned int parentHeight, 
        std::pair<uint64_t, uint64_t> levelInterval
    );

    std::pair<uint64_t, uint64_t> getNMinusOffsetLevel(
        uint64_t levelIndex, std::pair<uint64_t, uint64_t> levelInterval, uint64_t offset = 1
    );

    inline uint64_t getNumberOfElementInNextLevel(
        unsigned int currHeight, uint64_t numOfElemCurrLevel, int offsetDim
    );

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

        if constexpr (N == 2)
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
        auto l = getNumOfElemInPrevLevel(currHeight, currLevInt.second - currLevInt.first + 1);

        return {currLevInt.first - l, currLevInt.first - 1};
    }

    inline std::pair<uint64_t, uint64_t> getNextLevelInterval(
        unsigned int currHeight, std::pair<uint64_t, uint64_t> currLevInt
    )
    { 
        auto l = getNumberOfElementInNextLevel(currHeight, currLevInt.second - currLevInt.first + 1);

        return {currLevInt.second + 1, currLevInt.second + l};
    }

    std::optional<uint64_t> getOtherChildrenP(
        uint64_t currentPos, std::pair<uint64_t, uint64_t> levelInterval, 
        uint64_t maxNumOfChildren, uint64_t dOffset
    );

    std::optional<uint64_t> getOtherParents(
        uint64_t currentPos, std::pair<uint64_t, uint64_t> levelInterval,
        uint64_t parentsLevelSize, uint64_t dOffset
    );

    inline std::array<uint64_t, 3> getNMinusOffsetLevelWithParentLevel(
        uint64_t levelIndex, std::pair<uint64_t, uint64_t> levelInterval, uint64_t offset
    );
    
};

template <Comparable T, size_t N, typename Compare>
inline std::optional<std::pair<uint64_t, unsigned int>> nBeap<T, N, Compare>::_search(T val)
{
    return std::optional<std::pair<uint64_t, unsigned int>>();
}

template <Comparable T, size_t N, typename Compare>
std::pair<uint64_t, uint64_t> nBeap<T, N, Compare>::span(
    unsigned int preferredHeight,
    std::pair<uint64_t, uint64_t> knownHeightInterval,
    unsigned int knownHeight)
{
    if (preferredHeight == knownHeight)
    {
        return knownHeightInterval;
    }

    //std::pair<uint64_t, uint64_t> resInterval;

    while(knownHeight > preferredHeight)
    {
        auto prevLevelElems = 
            getNumOfElemInPrevLevel(knownHeight, knownHeightInterval.second - knownHeightInterval.first + 1);
        knownHeightInterval.second = knownHeightInterval.first - 1;
        knownHeightInterval.first -= prevLevelElems; 
        knownHeight--;
    }
    return knownHeightInterval; 
}

template<Comparable T, size_t N, typename Compare>
void nBeap<T, N, Compare>::siftDown(
    uint64_t startPos, unsigned int h, std::pair<uint64_t, uint64_t> levelInterval
)
{
    T val = std::move(container[startPos]);
    auto currPos = startPos;

    while(h < _height)
    {
        std::optional<uint64_t> selected_child_index = getMinOrMaxChildIndex(currPos, h, levelInterval);
        
        if(!selected_child_index.has_value() || compare(container[selected_child_index.value()], val))
        {
            break;
        }

        //std::cout << container[selected_child_index.value()] << std::endl;
        container[currPos] = std::move(container[selected_child_index.value()]);
        currPos = selected_child_index.value();
        levelInterval = getNextLevelInterval(h, levelInterval);
        h++;
    }

    container[currPos] = std::move(val);

}

template<Comparable T, size_t N, typename Compare>
void nBeap<T, N, Compare>::bubbleUp(
    uint64_t toIndex, uint64_t fromIndex, 
    unsigned int h, std::pair<uint64_t, uint64_t> levelInterval
)
{
    auto currentPos = fromIndex;
    auto currentHeight = h;
    T val = std::move(container[fromIndex]);
    
    while (currentPos > toIndex)
    {
        std::optional<uint64_t> selected_parent_index = 
            getMinOrMaxParentIndex(currentPos, currentHeight, levelInterval);

        if(!selected_parent_index.has_value() || compare(val, container[selected_parent_index.value()]))
        {
            break;
        }
        container[currentPos] = std::move(container[selected_parent_index.value()]);
        
        levelInterval = getPreviousLevelInterval(currentHeight, levelInterval);
        currentPos = selected_parent_index.value();
        currentHeight--;
        
    }

    container[currentPos] = std::move(val);
}

template <Comparable T, size_t N, typename Compare>
inline std::optional<uint64_t> nBeap<T, N, Compare>::getMinOrMaxParentIndex(
    uint64_t currentPos, 
    unsigned int childHeight,
    std::pair<uint64_t, uint64_t> levelInterval
)
{
    auto parentsInterval = getPreviousLevelInterval(childHeight, levelInterval);

    if constexpr (N <= 2) 
    { 
        if(currentPos == levelInterval.first)
        {
            return parentsInterval.first;
        }

        if(currentPos == levelInterval.second)
        {
            return parentsInterval.second;
        }

        size_t numberOfElementInTheLevel = parentsInterval.second - parentsInterval.first + 1;
        auto parent2 = currentPos - numberOfElementInTheLevel;
        auto parent1 = parent2 - 1;
        
        // the index of parent1 is guaranteed to be greater than the index of parent2 by just 1
        return parent1 + !compare(container[parent1], container[parent2]);
    } 
    else 
    {
        if(currentPos == levelInterval.first)
        {
            return parentsInterval.first;
        }

        if(currentPos == levelInterval.second)
        {
            return parentsInterval.second;
        }

        /*
        Steps:
           1    3   6
        1    3    6    10

        - check if there exist a child on the same level above
        */

        auto parentsLevelSize = parentsInterval.second - parentsInterval.first + 1;
        auto childIndex = currentPos - levelInterval.first + 1;
        if(childIndex > parentsLevelSize)
        {
            return getOtherParents(currentPos, levelInterval, parentsLevelSize, 1);
        }

        auto firstParent = currentPos - parentsLevelSize;

        auto minOtherParent = getOtherParents(currentPos, levelInterval, parentsLevelSize, 1).value_or(firstParent);

        return minOtherParent + (firstParent - minOtherParent) * 
                        !compare(container[minOtherParent], container[firstParent]);
        
    }
    
}

template <Comparable T, size_t N, typename Compare>
std::optional<uint64_t> nBeap<T, N, Compare>::getOtherParents(
    uint64_t currentPos, std::pair<uint64_t, uint64_t> levelInterval,
    uint64_t parentsLevelSize, uint64_t dOffset
)
{

    // get the inner level
    auto levelIndex = currentPos - levelInterval.first + 1;
    auto levelSize = levelInterval.second - levelInterval.first + 1;

    if(levelIndex >= levelSize)  // when we go past the parent's block
    {
        return std::nullopt;
    }

    auto res = getNMinusOffsetLevelWithParentLevel(levelIndex, levelInterval, dOffset);
    auto innerLevel = std::pair{res[1]+1, res[2]};
    // get the previous inner level
    
    auto prevInnerLevel = std::pair{res[0], res[1]};
    auto innerLevelSize = innerLevel.second - innerLevel.first + 1;

    // calculate new current Pos
    auto prevInnerLevelSize = prevInnerLevel.second - prevInnerLevel.first + 1;
    auto innerLevelIndex = levelIndex - innerLevel.first + 1;
    //int newCurrPos = innerLevelIndex - prevInnerLevelSize;

    auto parent = currentPos - parentsLevelSize - prevInnerLevelSize;

    if(N - dOffset - 1 <= 0)
    {
        return parent;
    }

    if(innerLevelIndex > prevInnerLevelSize)
    {
        return getOtherParents(currentPos - parentsLevelSize, innerLevel, prevInnerLevelSize, dOffset+1);
    }

    auto minOrMAxParent = getOtherParents(currentPos - parentsLevelSize, innerLevel, prevInnerLevelSize, dOffset+1).value_or(parent);

    if(compare(container[parent], container[minOrMAxParent]))
    {
        return parent;
    }
    
    return minOrMAxParent;
}



template <Comparable T, size_t N, typename Compare>
inline std::optional<uint64_t> nBeap<T, N, Compare>::getMinOrMaxChildIndex(
    uint64_t currentPos, 
    unsigned int parentHeight, 
    std::pair<uint64_t, uint64_t> levelInterval
)
{
    

    if constexpr (N <= 2) 
    {
        auto numberOfElementInTheLevel = 
            getNumberOfElementInNextLevel(parentHeight, levelInterval.second - levelInterval.first + 1);
        auto child2 = currentPos + numberOfElementInTheLevel;
        auto child1 = child2 - 1;
        if(child1 >= _size)
            return std::nullopt;

        if constexpr (N == 1)
        {   
            return child1;
        }

        if(child2 >= _size)
        {
            return child1;
        }

        // the index of child1 is guaranteed to be greater than the index of child2 by just 1
        return child1 + !compare(container[child2], container[child1]);
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
                return std::nullopt;
            }

            auto maxNumOfChildren = N - 1;
            auto nextChild = levelInterval.second + 2;
            while (maxNumOfChildren > 0 && nextChild < _size)
            {
                minChild = minChild + (nextChild - minChild) * 
                            !compare(container[nextChild], container[minChild]);
                nextChild++;
                maxNumOfChildren--;
            }
            
            return minChild;
        }


        auto levelSize = levelInterval.second - levelInterval.first + 1;

        auto child1 =  currentPos + levelSize;
        if(child1 >= _size)
            return std::nullopt;

        // Get the min/max child in the next sublayer in the children's layer
        auto minChildRest = getOtherChildrenP(currentPos, levelInterval, N - 1, 1)
                                .value_or(child1);
        
        return child1 + (minChildRest - child1) * !compare(container[minChildRest], container[child1]);
    }
    
    
}

// wrong implementation
template <Comparable T, size_t N, typename Compare>
std::optional<uint64_t> nBeap<T, N, Compare>::getOtherChildren(
    uint64_t currentPos, unsigned int parentHeight, 
    std::pair<uint64_t, uint64_t> levelInterval
)
{
    auto maxNumOfChildren = N-1; 
    auto levelSize = levelInterval.second - levelInterval.first + 1; 
    auto levelIndex = currentPos - levelInterval.first + 1;
    auto innerLayer = getNMinusOffsetLevel(levelIndex, levelInterval);

    auto innerLevelIndex = levelIndex - innerLayer.first + 1;
    auto innerLayerSize = innerLayer.second - innerLayer.first + 1;

    auto minOrMaxChild = currentPos + levelSize + innerLayerSize;
    maxNumOfChildren--;

    if(minOrMaxChild >= _size)
    {
        return std::nullopt;
    }

    if(innerLevelIndex == 1)
    {
        auto nextChild = minOrMaxChild + 1;
        
        while(maxNumOfChildren > 0 && nextChild < _size)
        {
            minOrMaxChild = minOrMaxChild + (nextChild - minOrMaxChild) * !compare(container[nextChild], container[minOrMaxChild]);
            nextChild++;
            maxNumOfChildren--;
        }

        return minOrMaxChild;
    }

    auto innerInnerLayer = getNMinusOffsetLevel(innerLevelIndex, innerLayer, 2);
    auto innerInnerLayerSize = innerInnerLayer.second - innerInnerLayer.first + 1;

    auto nextChild = minOrMaxChild + innerInnerLayerSize;

    while(maxNumOfChildren > 0 && nextChild < _size)
    {
        minOrMaxChild = minOrMaxChild + (nextChild - minOrMaxChild) * !compare(container[nextChild], container[minOrMaxChild]);
        nextChild++;
        maxNumOfChildren--;
    }

    return minOrMaxChild;
}


template <Comparable T, size_t N, typename Compare>
std::optional<uint64_t> nBeap<T, N, Compare>::getOtherChildrenP(
    uint64_t currentPos, std::pair<uint64_t, uint64_t> levelInterval, 
    uint64_t maxNumOfChildren, uint64_t dOffset
)
{
    auto levelSize = levelInterval.second - levelInterval.first + 1; 
    auto levelIndex = currentPos - levelInterval.first + 1;
    
    auto innerLayer = getNMinusOffsetLevel(levelIndex, levelInterval, dOffset);

    auto innerLevelIndex = levelIndex - innerLayer.first + 1;
    auto innerLayerSize = innerLayer.second - innerLayer.first + 1;

    auto minOrMaxChild = currentPos + levelSize + innerLayerSize;
    maxNumOfChildren--;

    if(minOrMaxChild >= _size)
    {
        return std::nullopt;
    }

    if(innerLevelIndex == 1)
    {
        auto nextChild = minOrMaxChild + 1;
        
        while(maxNumOfChildren > 0 && nextChild < _size)
        {
            minOrMaxChild = minOrMaxChild + (nextChild - minOrMaxChild) * !compare(container[nextChild], container[minOrMaxChild]);
            nextChild++;
            maxNumOfChildren--;
        }

        return minOrMaxChild;
    }

    auto minChildRest = 
        getOtherChildrenP(currentPos + levelSize, innerLayer, maxNumOfChildren, dOffset+1)
            .value_or(minOrMaxChild);

    return minOrMaxChild + (minChildRest - minOrMaxChild) * !compare(container[minChildRest], container[minOrMaxChild]);
    
}




template <Comparable T, size_t N, typename Compare>
inline std::pair<uint64_t, uint64_t> nBeap<T, N, Compare>::getNMinusOffsetLevel(
    uint64_t levelIndex, std::pair<uint64_t, uint64_t> levelInterval, uint64_t offset
) 
{

    if(levelIndex == 1) // should not happen because this is already accounted for up
    {
        return {1, 1};
    }

    // Possible optimisation here is to start searching
    // from the back since most element should be at the last level
    uint64_t h = 1, l = 1, cumm = 1, prevCumm = 1;
    uint64_t maxInterval = levelInterval.second - levelInterval.first + 1; // can replace levelInterval with the level size acc
    while(cumm <= maxInterval) 
    {
        l = getNumberOfElementInNextLevel(h, l, offset);
        prevCumm = cumm;
        cumm += l;
        h++;
        if(levelIndex <= cumm)
        {
            return {prevCumm + 1, cumm};
        }
    }

    return {prevCumm + 1, cumm};
}

template <Comparable T, size_t N, typename Compare>
inline std::array<uint64_t, 3> nBeap<T, N, Compare>::getNMinusOffsetLevelWithParentLevel(
    uint64_t levelIndex, std::pair<uint64_t, uint64_t> levelInterval, uint64_t offset
) 
{

    if(levelIndex == 1) // should not happen because this is already accounted for up
    {
        return {0, 0, 1};
    }

    // Possible optimisation here is to start searching
    // from the back since most element should be at the last level
    uint64_t curretHeight = 1, numOfElementInLevel = 1, curLevelEnd = 1, prevLevelEnd = 0, prevLevelStart = 0;
    uint64_t levelSize = levelInterval.second - levelInterval.first + 1; // can replace levelInterval with the level size acc
    while(curLevelEnd <= levelSize) 
    {
        numOfElementInLevel = getNumberOfElementInNextLevel(curretHeight, numOfElementInLevel, offset);
        prevLevelStart = prevLevelEnd + 1;
        prevLevelEnd = curLevelEnd;
        curLevelEnd += numOfElementInLevel;
        curretHeight++;
        if(levelIndex <= curLevelEnd)
        {
            return {prevLevelStart, prevLevelEnd, curLevelEnd};
        }
    }

    return {prevLevelStart, prevLevelEnd, curLevelEnd};
}



template <Comparable T, size_t N, typename Compare>
inline uint64_t nBeap<T, N, Compare>::getNumberOfElementInNextLevel(
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