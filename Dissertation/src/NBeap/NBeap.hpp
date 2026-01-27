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

    T extract_min() {

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
            _interval = getPreviousLevelInterval(_height, _interval);
            _levelSize = _interval.second - _interval.first + 1; 
            _height--;
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
        std::swap(_container[resVal.first], _container[_size - 1]);

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
        auto li = span(resVal.second);

        auto selected_parent_optional = getMinOrMaxParentIndex(resVal.first, resVal.second, li);
        if(!selected_parent_optional.has_value() || 
            compare(_container[selected_parent_optional.value()], _container[resVal.first])
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
		for (T val : _container) {
			std::cout << val << " ";
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

    std::optional<uint64_t> getMinOrMaxInOtherChildren(
        uint64_t parentId, uint64_t currentPos, std::pair<uint64_t, uint64_t> levelInterval, 
        uint64_t maxNumOfChildren, uint64_t dOffset
    );

    inline std::array<uint64_t, 3> getNMinusOffsetLevelWithParentLevel(
        uint64_t prevLevelStart, uint64_t currentPos, uint64_t offset
    ); 

    std::optional<uint64_t> getMinOrMaxInRemainingParents(
        uint64_t currentPos, std::pair<uint64_t, uint64_t> parentsInterval, uint64_t dOffset
    );
    
};

template <Comparable T, size_t N, typename Compare>
inline std::optional<std::pair<uint64_t, unsigned int>> NBeap<T, N, Compare>::_search(T val)
{
    return std::optional<std::pair<uint64_t, unsigned int>>();
}

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
void NBeap<T, N, Compare>::siftDown(
    uint64_t startPos, unsigned int h, std::pair<uint64_t, uint64_t> levelInterval
)
{
    T val = std::move(_container[startPos]);
    auto currPos = startPos;

    while(h < _height)
    {
        std::optional<uint64_t> selected_child_index = getMinOrMaxChildIndex(currPos, h, levelInterval);
        
        if(!selected_child_index.has_value() || compare(_container[selected_child_index.value()], val))
        {
            break;
        }

        //std::cout << _container[selected_child_index.value()] << std::endl;
        _container[currPos] = std::move(_container[selected_child_index.value()]);
        currPos = selected_child_index.value();
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
        std::optional<uint64_t> bestParentOpt = 
            getMinOrMaxParentIndex(currentPos, currentHeight, levelInterval);

        if(!bestParentOpt.has_value() || compare(val, _container[bestParentOpt.value()]))
        {
            break;
        }
        _container[currentPos] = std::move(_container[bestParentOpt.value()]);
        
        levelInterval = getPreviousLevelInterval(currentHeight, levelInterval);
        currentPos = bestParentOpt.value();
        currentHeight--;
        
    }

    _container[currentPos] = std::move(val);
}

template <Comparable T, size_t N, typename Compare>
inline std::optional<uint64_t> NBeap<T, N, Compare>::getMinOrMaxParentIndex(
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
        auto parentsInterval = getPreviousLevelInterval(childHeight, levelInterval);

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
        return parent1 + !compare(_container[parent1], _container[parent2]);
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

        //auto childIndex = currentPos - levelInterval.first + 1;
        auto bestParent = currentPos - parentsLevelSize;
        if(bestParent > parentsInterval.second)
        {
            //std::cout << "For index: " << currentPos << ", ";
            return getMinOrMaxInRemainingParents(bestParent, parentsInterval, 1);
        }

        

        //std::cout << "For index: " << currentPos << ", " <<"First parent: " << bestParent << ", ";

        //auto bestParentFromRest = getOtherParents(currentPos, levelInterval, parentsLevelSize, 1).value_or(bestParent);
        
        auto bestParentFromRest = 
            getMinOrMaxInRemainingParents(bestParent, parentsInterval, 1).value_or(bestParent);

        //std::cout << std::endl;
        return bestParentFromRest + (bestParent - bestParentFromRest) * 
                        !compare(_container[bestParentFromRest], _container[bestParent]);
        
    }
    
}

template <Comparable T, size_t N, typename Compare>
std::optional<uint64_t> NBeap<T, N, Compare>::getMinOrMaxInRemainingParents(
    uint64_t currentPos, std::pair<uint64_t, uint64_t> parentsInterval, uint64_t dOffset
)
{
    if(N-dOffset <= 0)
    {
        return std::nullopt;
    }

    // Conditions
    // Do no go past 1 dimension
    // if the parent1 is not upto any value in the parent layer, continue recursion
    // get the inner level
    auto res = getNMinusOffsetLevelWithParentLevel(parentsInterval.first, currentPos, dOffset);
    auto innerLevel = std::pair{res[1]+1, res[2]};
    auto levelIndex = currentPos - innerLevel.first + 1;
    // get the previous inner level
    auto prevInnerLevel = std::pair{res[0], res[1]};
    auto prevInnerLevelSize = prevInnerLevel.second - prevInnerLevel.first + 1;

    if(currentPos == innerLevel.first)
    {
        //std::cout << "Last Parent: " << prevInnerLevel.first << ", "; 
        return prevInnerLevel.first;
    }

    if(currentPos == innerLevel.second)
    {
        //std::cout << "Last Parent: " << prevInnerLevel.second << ", "; 
        return prevInnerLevel.second;
    }
    
    if(prevInnerLevelSize == 1)
    {
        // Avoids unnecessary recursion
        auto parent = currentPos - levelIndex;
        //std::cout << "Last Parent: " << parent << ", "; 
        return  parent;
    }

    auto parent1 = currentPos - prevInnerLevelSize;

    if(parent1 > prevInnerLevel.second)
    {
        return getMinOrMaxInRemainingParents(parent1, prevInnerLevel, dOffset+1);
    }

    //std::cout << "Another Parent: " << parent1 << ", "; 
    auto minOrMaxParentRest = getMinOrMaxInRemainingParents(parent1, prevInnerLevel, dOffset+1)
                                    .value_or(parent1);


    return minOrMaxParentRest + (parent1 - minOrMaxParentRest) * 
                !compare(_container[minOrMaxParentRest], _container[parent1]); 
}





template <Comparable T, size_t N, typename Compare>
inline std::optional<uint64_t> NBeap<T, N, Compare>::getMinOrMaxChildIndex(
    uint64_t currentPos, 
    unsigned int parentHeight, 
    std::pair<uint64_t, uint64_t> levelInterval
)
{
    
    if constexpr (N == 1)
    {
        auto child = currentPos + 1;
        if (child >= _size)
            return std::nullopt;

        return child;
    }
    else if constexpr (N == 2) 
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
                return std::nullopt;
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
            return std::nullopt;
        
        auto minChildRest = getMinOrMaxInOtherChildren(currentPos, minChild, levelInterval, N - 1, 1)
                                .value_or(minChild);

        return minChild + (minChildRest - minChild) * !compare(_container[minChildRest], _container[minChild]);
    }
    
    
}


template <Comparable T, size_t N, typename Compare>
std::optional<uint64_t> NBeap<T, N, Compare>::getMinOrMaxInOtherChildren(
    uint64_t parentId, uint64_t currentPos,  std::pair<uint64_t, uint64_t> levelInterval, 
    uint64_t maxNumOfChildren, uint64_t dOffset
)
{
    auto innerLayer = getNMinusOffsetLevel(parentId, levelInterval, dOffset);
    auto innerLayerSize = innerLayer.second - innerLayer.first + 1;

    auto bestChild = currentPos + innerLayerSize;
    if(bestChild >= _size)
    {
        return std::nullopt;
    }

    maxNumOfChildren--;

    uint64_t currentOffset = dOffset + 1;
    auto currentChild = bestChild;
    
    while(maxNumOfChildren > 0)
    {
        innerLayer = getNMinusOffsetLevel(parentId, innerLayer, currentOffset);
        innerLayerSize = innerLayer.second - innerLayer.first + 1;
        
        currentChild += innerLayerSize;

        if(currentChild >= _size)
        {
            break;
        }

        bestChild = bestChild + (currentChild - bestChild) * 
                    !compare(_container[currentChild], _container[bestChild]);

        maxNumOfChildren--;
        currentOffset++;
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