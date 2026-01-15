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

    std::pair<uint64_t, uint64_t> getChildren(uint64_t parentIndex, unsigned int parentHeight);
    std::pair<uint64_t, uint64_t> getParents(uint64_t childIndex, unsigned int childHeight);

    std::optional<uint64_t> getMinOrMaxParentIndex(
        uint64_t currentPos, unsigned int childHeight, std::pair<uint64_t, uint64_t> levelInterval
    );
    std::optional<uint64_t> getMinOrMaxChildIndex(
        uint64_t currentPos, unsigned int childHeight, std::pair<uint64_t, uint64_t> levelInterval
    );

    std::pair<uint64_t, uint64_t> getDMinus1Level(uint64_t levelIndex, std::pair<uint64_t, uint64_t> levelInterval);

    uint64_t getNumberOfElementInNextLevel(unsigned int currHeight, uint64_t numOfElemCurrLevel, int offsetDim);

    inline uint64_t getNumberOfElementInNextLevel(unsigned int currHeight, uint64_t numOfElemCurrLevel)
    {
        if constexpr (N == 1)
        {
            return numOfElemCurrLevel;
        }
        if constexpr (N == 2)
        {
            return numOfElemCurrLevel++;
        }

        
        return  (numOfElemCurrLevel * (currHeight + N - 1 )) / currHeight;
    }

    inline uint64_t getNumOfElemInPrevLevel(unsigned int currHeight, uint64_t numOfElemCurrLevel)
    {
        if constexpr (N == 1)
        {
            return numOfElemCurrLevel;
        }

        if constexpr (N == 2)
        {
            return numOfElemCurrLevel--;
        }

        return  (numOfElemCurrLevel * (currHeight - 1)) / (currHeight + N - 2);
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



    inline uint64_t getNumOfElemInFirstInnerLayer(unsigned int currHeight, uint64_t numOfElemCurrLevle)
    {
        if(currHeight <= 3)
            return 0;
        
        uint64_t temp = (currHeight + N - 2) * (currHeight + N - 3) * (currHeight + N - 4);

        return temp / (currHeight - 3);
    }


    std::pair<uint64_t, uint64_t> getChildrenLayers(unsigned int parentLayer, std::pair<uint64_t, uint64_t> childLayer);
};

template<Comparable T, size_t N, typename Compare>
std::optional<std::pair<uint64_t, unsigned int>> _search(T val)
{
    return std::nullopt; 
}

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

    std::pair<uint64_t, uint64_t> resInterval;

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

template <Comparable T, size_t N, typename Compare>
void nBeap<T, N, Compare>::bubbleUp(
    uint64_t toIndex, uint64_t fromIndex, 
    unsigned int h, std::pair<uint64_t, uint64_t> levelInterval)
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
    auto previousInterval = getPreviousLevelInterval(childHeight, levelInterval);

    if constexpr (N <= 2) 
    { 
        // code only generated when N == 2 
        if(currentPos == levelInterval.first)
        {
            return previousInterval.first;
        }

        if(currentPos == levelInterval.second)
        {
            return previousInterval.second;
        }

        size_t numberOfElementInTheLevel = previousInterval.second - previousInterval.first + 1;
        auto parent2 = currentPos - numberOfElementInTheLevel;
        auto parent1 = parent2 - 1;
        
        // the index of parent1 is guaranteed to be greater than the index of parent2 by just 1
        return parent1 + !compare(container[parent1], container[parent2]);
    } 
    else 
    { 
        // dont need to check if an index is valid for this
        // i keep making this mistake
        if(currentPos == levelInterval.first)
        {
            return previousInterval.first;
        }

        if(currentPos == levelInterval.second)
        {
            return previousInterval.second;
        }

        // fallback
        return std::nullopt;
    }
    
}

template <Comparable T, size_t N, typename Compare>
inline std::optional<uint64_t> nBeap<T, N, Compare>::getMinOrMaxChildIndex(
    uint64_t currentPos, 
    unsigned int parentHeight, 
    std::pair<uint64_t, uint64_t> levelInterval
)
{
    
    auto numberOfElementInTheLevel = 
        getNumberOfElementInNextLevel(parentHeight, levelInterval.second - levelInterval.first + 1);

    if constexpr (N <= 2) 
    {
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
        // Steps
        // Get the level and boundary in the layer
        // first child is idx + layerSize
        // second child = idx in the next layer
        // third child = 

        // case first element in the level
        if(currentPos == levelInterval.first)
        {
            auto minChild = levelInterval.second + 1;
            if(minChild >= _size)
            {
                return std::nullopt;
            }

            for(auto i = 2; i <= N; i++)
            {
                auto nextChild = levelInterval.second + i;
                if(nextChild >= _size)
                {
                    return minChild;
                }

                minChild = minChild + (nextChild - minChild) * 
                            !compare(container[nextChild], container[minChild]);
            }
            return minChild;
        }


        auto levelSize = levelInterval.second - levelInterval.first + 1;

        auto child1 =  currentPos + levelSize;
        if(child1 >= _size)
            return std::nullopt;

        // Steps
        // calculate the parents index in the level
        auto parentIdInLevel = currentPos - levelInterval.first + 1;   
        // calculate the parent index in its layer level
        std::pair<uint64_t, uint64_t> parentLayerInterval = 
                    getDMinus1Level(parentIdInLevel, levelInterval); 
        auto parentIdInLayer = parentIdInLevel - parentLayerInterval.first + 1;
        // child2 = parent index in its layer + the end of the layer
        auto nextChild = parentLayerInterval.second +  parentIdInLayer;
        if(nextChild >= _size)
        {
            return child1;
        }

        auto minChild = child1 + (nextChild - child1) * !compare(container[nextChild], container[child1]);
        // while loop to find the other children
        // set max children in the next leyer to N - 1; i = 1
        // while i < parent index in its layer
        //  add max no of children to nextChild
        //  i++
        int i = 1;
        auto maxNumOfChildren = N - 2;
        while((i < parentIdInLayer) && (maxNumOfChildren > 0)) // need to update this
        {
            nextChild +=maxNumOfChildren;
            if(nextChild >= _size)
            {
                return minChild;
            }
            
            minChild = minChild + (nextChild - minChild) * !compare(container[nextChild], container[minChild]);
            maxNumOfChildren--;
            i++;
        }

        if(maxNumOfChildren <= 0)
            return minChild;
        
        while(maxNumOfChildren > 0)
        {
            nextChild++;
            if(nextChild >= _size)
            {
                return minChild;
            }

            minChild = minChild + (nextChild - minChild) * !compare(container[nextChild], container[minChild]);
            maxNumOfChildren--;
        }

        return minChild;
    }
    
    
}

// returns the layer interval of the parent
template <Comparable T, size_t N, typename Compare>
inline std::pair<uint64_t, uint64_t> nBeap<T, N, Compare>::getDMinus1Level(
    uint64_t levelIndex, std::pair<uint64_t, uint64_t> levelInterval
) 
{

    if(levelIndex == 1) // should not happen because this is already accounted for up
    {
        return {1, 1};
    }

    // Possible optimisation here is to start searching
    // from the back since most element should be at the last level
    uint64_t h = 1, l = 1, cumm = 1, prevCumm = 1;
    uint64_t maxInterval = levelInterval.second - levelInterval.first + 1;
    while(cumm <= maxInterval) 
    {
        l = getNumberOfElementInNextLevel(h, l, 1);
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
inline uint64_t nBeap<T, N, Compare>::getNumberOfElementInNextLevel(
    unsigned int currHeight, uint64_t numOfElemCurrLevel, int offsetDim
)
{
    return (numOfElemCurrLevel * (currHeight + (N - offsetDim) - 1 )) / currHeight;
}

template <Comparable T, size_t N, typename Compare>
inline std::pair<uint64_t, uint64_t> nBeap<T, N, Compare>::getChildrenLayers(
    unsigned int parentLayer, std::pair<uint64_t, uint64_t> childLayer
)
{
    if(parentLayer == 1)
    {
        return {childLayer.first, childLayer.first};
    }

    if(parentLayer == 2)
    {
        auto firstChild = childLayer.first + 1;
        return {firstChild, firstChild+1};
    }

    unsigned int h = 3;
    std::pair<uint64_t, uint64_t> currChildLevel = {4, 6};
    while(h <= parentLayer)
    {
        // Doing this right now since we know for 2d beap the number of
        // elements in each level just increases by one.
        h++;
        currChildLevel.first = currChildLevel.second + 1;
        currChildLevel.second =  currChildLevel.second + h;
    }

    return currChildLevel;
}