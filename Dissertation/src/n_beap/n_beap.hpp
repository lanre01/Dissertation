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
#include <cmath>
#include <cassert>
#include <array>
//#include <bits/stdc++.h>

template<typename T>
concept Comparable = requires(T a, T b) {
    { a < b } -> std::convertible_to<bool>;
    { a > b } -> std::convertible_to<bool>;
};

template<Comparable T, size_t N, typename Compare = std::greater<T>>
class NBeap
{
public:
    NBeap() 
    : NBeap(Compare())
    {}
    
    NBeap(Compare c) 
    : compare(c)
    , height_(0)
    , size_(0)
    , interval_(0, 0)
    , levelSize_(0)
    {
        initFactorials();
    }

    NBeap(std::vector<T> vec) 
    : NBeap(Compare(), std::move(vec))
    {
    }

    NBeap(Compare c, std::vector<T> vec) 
    : compare(c)
    , container_(std::move(vec))
    , height_(0)
    , size_(0)
    , interval_(0, 0)
    , levelSize_(0)
    {
        initFactorials();
    }

    ~NBeap() = default;

    NBeap& operator=(const NBeap&) = default;

    void reserve(size_t count)
    {
        container_.reserve(count);
    }

    T extract() {

        // check we can actually extract
        if(size_ <= 0)
        {
            throw std::out_of_range("cannot pop empty nbeap");
        }

        size_--;
        auto minElem = std::exchange(container_[0], container_[size_]);
        container_.resize(size_);

        // if we just extracted the last element
        if(size_ == 0)
        {
            levelSize_ = 0;
            height_ = 0;
            interval_ = {0, 0};
            return minElem;
        }

        // in case the last element on a level is removed 
        if(size_ - 1 < interval_.first)
        {
            if constexpr (N == 1)
            {
                height_--;
                interval_.first -= 1;
                interval_.second -= 1;
                
            }

            else if constexpr (N == 2)
            {
                levelSize_--;
                height_--;
                interval_.second = interval_.first - 1;
                interval_.first -= levelSize_;
            }
            else {
                interval_ = getPreviousLevelInterval(height_, interval_);
                levelSize_ = interval_.second - interval_.first + 1; 
                height_--;
            }
            
        }

        siftDown(0, 1);

        return minElem;
    }

    void insert(T val)
    {
        container_.push_back(std::move(val));

        if(size_ == 0)
        {
            size_++;
            height_++;
            interval_ = {0,0};
            levelSize_++;
            return;
        }

        if constexpr (N == 1)
        {
            height_++;
            interval_.first++;
            interval_.second++;
        }
        else {
            if (interval_.second < size_)
            {
                if constexpr(N == 2) 
                {
                    height_++;
                    levelSize_++;
                    interval_.first = interval_.second + 1; 
                    interval_.second += levelSize_;
                    
                }
                else {
                    interval_ = getNextLevelInterval(height_, interval_);
                    levelSize_ = interval_.second - interval_.first + 1;
                    height_++;
                }
                
            }
        }

       

        bubbleUp(0, size_, height_, interval_);

        size_++;
    }

    void remove(T val)
    {
        std::pair<size_t, size_t> out{-1,-1};

        if(!_searchHelper(val, out))
        {
            return;
        }
        
        std::swap(container_[out.first], container_[size_ - 1]);

        size_--;
        container_.resize(size_);

        if(size_ == 0)
        {
            levelSize_ = 0;
            height_ = 0;
            interval_ = {0, 0};
            return;
        }

        if(size_ - 1 < interval_.first)
        {
            levelSize_ = getNumOfElemInPrevLevel(height_, levelSize_);
            interval_.second = interval_.first  - 1;
            interval_.first -= levelSize_;
            height_--;
        }

        auto li = span(out.second, interval_, height_);


        if(compare(val, container_[out.first]))
        {
            bubbleUp(0, out.first, out.second, li);
        }
        

        siftDown(out.first, out.second, li);

    }

    bool search(T val) { 
        
        if(size_ <= 0 || compare(container_[0], val))
        {
            return false;
        }

        std::pair<size_t, size_t> out;

        if constexpr (N == 1)
        {
            return _search1D(val, out);
        }
        else if constexpr (N == 2)
        {
            return _search2D(val, out);
        }
        else {
            return _searchND(val, interval_.first, height_, interval_, out);
        }

    };

    size_t size() {return size_;}

    size_t height() {return height_ - 1;}

    bool empty() {return size_ == 0; }

    void clear()
    {
        height_ = 0;
        size_ = 0;
        container_.clear();
        interval_ = {0, 0};
        levelSize_ = 0;
    }

    void printState(const std::string& operation) {
    	std::cout << "After " << operation << ": " << std::endl;
        size_t i = 0;
        size_t h = 1;
        size_t n = 1;
        while(i < size_)
        {
            for (size_t k = 0; k < n; k++)
            {
                if(i+k >= size_)
                {
                    break;
                }
                std::cout << container_[i+k] << " ";
            }
            std::cout << std::endl;

            i+=n;
            n = getNumberOfElementInNextLevel(h, n);
            h++;
        }
		std::cout << " | size=" << size_ << " height=" << height_ << " levelSize=" << levelSize_ << std::endl;
        std::cout << " | intervals=" << interval_.first << "," << interval_.second << std::endl;
		std::cout.flush();
	}


private:
    Compare compare;
    std::vector<T> container_;
    size_t height_;
    size_t size_;
    std::pair<size_t, size_t> interval_;
    size_t levelSize_;
    std::array<size_t, N+1> factorials{};

    static constexpr size_t INVALID_INDEX_ = static_cast<size_t>(-1);

    void initFactorials()
    {
        factorials[0] = 1;
        for(size_t i = 1; i <= N; i++)
        {
            factorials[i] = i * factorials[i-1];
        }
    }

    std::pair<size_t, size_t> span(
        size_t currHeight, 
        size_t dim = N
    )
    {
        assert(currHeight > 0 && dim > 0);

        switch (dim)
        {
        case 1:
            return {currHeight, currHeight};
        default:
            size_t cumm = 1;
            for (auto i = currHeight - 1; i <= currHeight + dim - 1; i++)
            {
                cumm *= i;
            }

            size_t start = cumm / ((currHeight+1) * factorials[dim]);
            size_t end   = (cumm / ((currHeight-1) * factorials[dim])) + 1; 
            return {start, end};
        }
    }

    // idx is the index + 1 i.e size of the container up to that index
    // should idx represent the index or size??
    size_t getApproximateHeight(size_t idx, size_t dim = N)
    {
        switch (dim)
        {
        case 1:
            return idx;
        
        default:
            return std::round(std::pow(idx * factorials[dim], 1 / dim));
        }
        
    }

    std::pair<size_t, size_t> getSpan(size_t idx, size_t dim)
    {
        auto h = getApproximateHeight(idx, dim);
        auto interval = span(h, dim);

        if(idx < interval.first)
        {
            while(interval.first > idx)
            {
                interval = getPreviousLevelInterval(h, interval);
                h--;
            }
            return interval;
        }
        else if(idx > interval.second)
        {
            while(interval.second < idx)
            {
                interval = getNextLevelInterval(h, interval);
                h++;
            }
            return interval;
        }
        else {
            return interval;
        }
    }
    
    std::pair<size_t, size_t> span(
        size_t preferredHeight, 
        std::pair<size_t, size_t> knownHeighInterval, 
        size_t knowHeight
    );

    
    
    void siftDown(size_t startPos, size_t h, std::pair<size_t, size_t> levelInterval = {0,0});
    void bubbleUp(size_t index, size_t endIndex, size_t h, std::pair<size_t, size_t> levelInterval);
    
    size_t getBestParentIndex(
        size_t currentPos, size_t childHeight, std::pair<size_t, size_t> levelInterval
    );

    size_t getBestChild(
        size_t currentPos, size_t childHeight, std::pair<size_t, size_t> levelInterval
    );

    std::pair<size_t, size_t> getNMinusOffsetLevel(
        size_t levelIndex, std::pair<size_t, size_t> levelInterval, size_t offset = 1
    );

    size_t getNumberOfElementInNextLevel(
        size_t currHeight, size_t numOfElemCurrLevel, int offsetDim
    );

    // This can actually use binary search since it is sorted
    // Beap search is used for consistency reason
    bool _search1D(T val, std::pair<size_t, size_t>& out)
    { 
        auto from = interval_.first;
        auto h = height_;
        while(from >= 0)
        {
            if(compare(container_[from], val))
            {
                from--;
                h--;
            }
            else if(compare(val, container_[from]))
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

    bool _search2D(T val, std::pair<size_t, size_t>& out)
    {

        if(size_ <= 0 || compare(container_[0], val))
        {
            return false;
        }

        size_t h = height_;
        size_t end = interval_.first;
        size_t start = interval_.second;
        std::pair<size_t, size_t> currentLevel = interval_;

        if(val == container_[end])
        {
            out.first = end;
            out.second = h;
            return true;
        }

        if(start >= size_)
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

            if (compare(container_[idx], val))
            {
                // If search attempts to go up at the start of any level, 
                // this means the element does not exist in the structure 
                // and search must terminate
                if (idx == currentLevel.first)
                {
                    return false; 
                }

                idx -= h;
                currentLevel = getPreviousLevelInterval(h, currentLevel);
                h--;
            }
            else if (compare(val, container_[idx]))
            {
                auto firstChild = idx + h;
                if(firstChild >= size_)
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

    bool _searchHelper(T val, std::pair<size_t, size_t>& out)
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
            return _searchND(val, interval_.first, height_, interval_, out);
        }

    }

    size_t getNumberOfElementInNextLevel(size_t currHeight, size_t numOfElemCurrLevel)
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

    size_t getNumOfElemInPrevLevel(size_t currHeight, size_t numOfElemCurrLevel)
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

    // I need to update this functions to only accept the first or second 
    // value of the level interval 
    std::pair<size_t, size_t> getPreviousLevelInterval(
        size_t currHeight, 
        std::pair<size_t, size_t> currLevInt
    )
    {
        auto l = getNumOfElemInPrevLevel(currHeight, INTERVALSIZE(currLevInt));

        return {currLevInt.first - l, currLevInt.first - 1};
    }

    std::pair<size_t, size_t> getNextLevelInterval(
        size_t currHeight, std::pair<size_t, size_t> currLevInt
    )
    { 
        auto l = getNumberOfElementInNextLevel(currHeight,  INTERVALSIZE(currLevInt));

        return {currLevInt.second + 1, currLevInt.second + l};
    }

    size_t getBestOtherChildren(
        size_t parentId, size_t currentPos, std::pair<size_t, size_t> levelInterval, 
        size_t maxNumOfChildren, size_t dOffset
    );

    inline std::array<size_t, 3> getNMinusOffsetLevelWithParentLevel(
        size_t prevLevelStart, size_t currentPos, size_t offset
    ); 

    inline size_t getBestRemainingParents(
        size_t currentPos, std::pair<size_t, size_t> parentsInterval, size_t dOffset
    );

    inline size_t INTERVALSIZE(const std::pair<size_t, size_t>& p)
    {
        return p.second - p.first + 1;
    }

    bool _searchND(
        T val, const size_t currPos, const size_t currHeight, 
        const std::pair<size_t, size_t> levelInterval,
        std::pair<size_t, size_t>& out
    );

    std::pair<size_t, size_t> getFirstUniqueChildPos(
        size_t parentIdx, size_t firstChildPos, size_t maxNumberOfChildren,
        std::pair<size_t, size_t> levelInterval
    );
    
};


template <Comparable T, size_t N, typename Compare>
std::pair<size_t, size_t> NBeap<T, N, Compare>::span(
    size_t preferredHeight,
    std::pair<size_t, size_t> knownHeightInterval,
    size_t knownHeight)
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
    size_t startPos, size_t h, std::pair<size_t, size_t> levelInterval
)
{
    T val = std::move(container_[startPos]);
    auto currPos = startPos;

    while(h < height_)
    {
        size_t bestChild = 
                getBestChild(currPos, h, levelInterval);
        
        if (bestChild == INVALID_INDEX_ || compare(container_[bestChild], val))
        {
            break;
        }
        
        container_[currPos] = std::move(container_[bestChild]);
        currPos = bestChild;
        levelInterval = getNextLevelInterval(h, levelInterval);
        h++;
    }

    container_[currPos] = std::move(val);

}

template<Comparable T, size_t N, typename Compare>
void NBeap<T, N, Compare>::bubbleUp(
    size_t toIndex, size_t fromIndex, 
    size_t h, std::pair<size_t, size_t> levelInterval
)
{
    auto currentPos = fromIndex;
    auto currentHeight = h;
    T val = std::move(container_[fromIndex]);
    
    while (currentPos > toIndex)
    {
        auto bestParent = getBestParentIndex(currentPos, currentHeight, levelInterval);

        if(compare(val, container_[bestParent]))
        {
            break;
        }
        container_[currentPos] = std::move(container_[bestParent]);
        
        levelInterval = getPreviousLevelInterval(currentHeight, levelInterval);
        currentPos = bestParent;
        currentHeight--;
        
    }

    container_[currentPos] = std::move(val);
}

template <Comparable T, size_t N, typename Compare>
size_t NBeap<T, N, Compare>::getBestParentIndex(
    size_t currentPos, 
    size_t childHeight,
    std::pair<size_t, size_t> levelInterval
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

        return firstParent + !compare(container_[firstParent], container_[firstParent+1]);
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

        if (bestParentFromRest >= INVALID_INDEX_)
        {
            return bestParent;
        }

        return bestParentFromRest + (bestParent - bestParentFromRest) * 
                        !compare(container_[bestParentFromRest], container_[bestParent]);
        
    }
    
}

template <Comparable T, size_t N, typename Compare>
inline size_t NBeap<T, N, Compare>::getBestRemainingParents(
    size_t currentPos, std::pair<size_t, size_t> parentsInterval, size_t dOffset
)
{
    std::array<size_t, 3> res; 
    std::pair<size_t, size_t> innerLevel;

    std::pair<size_t, size_t> prevInnerLevel;

    size_t parent; 
    size_t bestParent = INVALID_INDEX_;

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
            if (bestParent == INVALID_INDEX_)
            {
                return prevInnerLevel.first;
            }

            return prevInnerLevel.first + (bestParent - prevInnerLevel.first) * 
                !compare(container_[prevInnerLevel.first], container_[bestParent]); 
        }

        if(currentPos == innerLevel.second) 
        {
            if (bestParent == INVALID_INDEX_)
            {
                return prevInnerLevel.second;
            }

            return prevInnerLevel.second + (bestParent - prevInnerLevel.second) * 
                !compare(container_[prevInnerLevel.second], container_[bestParent]); 
        }

        parent = currentPos - prevInnerLevelSize;

        if (parent <= prevInnerLevel.second)
        {
            if (bestParent == INVALID_INDEX_)
            {
                bestParent = parent;
                
            }
            else {
                bestParent = parent + (bestParent - parent) * 
                    !compare(container_[parent], container_[bestParent]); 
            }
        }

        currentPos = parent;
        dOffset++;
        parentsInterval = prevInnerLevel;
    }
}

template <Comparable T, size_t N, typename Compare>
size_t NBeap<T, N, Compare>::getBestChild(
    size_t currentPos, 
    size_t parentHeight, 
    std::pair<size_t, size_t> levelInterval
)
{
    if constexpr (N == 1)
    {
        auto child = currentPos + 1;
        if (child >= size_)
            return INVALID_INDEX_;

        return child;
    }
    else if constexpr (N == 2) 
    {
        auto child1 = currentPos + parentHeight;
        
        if(child1 >= size_)
            return INVALID_INDEX_;

        auto child2 =  child1 + 1;
        
        if(child2 >= size_)
        {
            return child1;
        }

        return child1 + !compare(container_[child2], container_[child1]);
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
            if(minChild >= size_)
            {
                return INVALID_INDEX_;
            }

            auto maxNumOfChildren = N - 1;
            //std::cout << "For index: " << currentPos << " First Child: " << minChild << ", ";
            auto nextChild = minChild + 1;
            while (maxNumOfChildren > 0 && nextChild < size_)
            {
                //std::cout << "Next Child: " << nextChild << ", ";
                minChild = minChild + (nextChild - minChild) * 
                            !compare(container_[nextChild], container_[minChild]);
                nextChild++;
                maxNumOfChildren--;
            }
            
            return minChild;
        }


        auto levelSize = levelInterval.second - levelInterval.first + 1;
        auto minChild =  currentPos + levelSize;
        if(minChild >= size_)
            return INVALID_INDEX_;
        
        auto minChildRest = getBestOtherChildren(currentPos, minChild, levelInterval, N - 1, 1);
        if (minChildRest == INVALID_INDEX_)
        {
            return minChild;
        }

        return minChild + (minChildRest - minChild) * !compare(container_[minChildRest], container_[minChild]);
    }
    
    
}

// Returns either NULL or the index and height of the element being found
template <Comparable T, size_t N, typename Compare>
bool NBeap<T, N, Compare>::_searchND(
    T val, const size_t currPos, const size_t currHeight, 
    const std::pair<size_t, size_t> levelInterval, 
    std::pair<size_t, size_t>& out
) 
{
    if(currPos >= size_)
    {
        return false;
    }

    if(compare(container_[currPos], val))
    {
        auto parentLevel = getPreviousLevelInterval(currHeight, levelInterval);
        auto newPos = currPos - INTERVALSIZE(parentLevel);
        if(newPos >= levelInterval.first)
        {
            return false;
        } 
        
        return _searchND(val, newPos, currHeight - 1, parentLevel, out);
    }
    else if(compare(val, container_[currPos]))
    {
    
        auto firstChild = currPos + INTERVALSIZE(levelInterval);
        //auto maxNumberOfChildren = N - 1;
        auto firstUniqueChildPosAndMaxNumOfChildren = 
            getFirstUniqueChildPos(currPos, firstChild, N-1, levelInterval);

        if(firstChild >= size_ || firstUniqueChildPosAndMaxNumOfChildren.first >= size_)
        {
            auto parentSibling = 
                firstUniqueChildPosAndMaxNumOfChildren.first - INTERVALSIZE(levelInterval);
            auto maxSibling = firstUniqueChildPosAndMaxNumOfChildren.second;

            while(maxSibling > 0)
            {
                if(parentSibling >= size_)
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
                if(childPos >= size_)
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
std::pair<size_t, size_t> NBeap<T, N, Compare>::getFirstUniqueChildPos(
    size_t parentIdx, size_t firstChildPos, size_t maxNumberOfChildren,
    std::pair<size_t, size_t> levelInterval
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
size_t NBeap<T, N, Compare>::getBestOtherChildren(
    size_t parentId, size_t currentPos, std::pair<size_t, size_t> levelInterval,
    size_t maxNumOfChildren, size_t dOffset
)
{
    std::pair<size_t, size_t> innerLayer = levelInterval;
    size_t bestChild = INVALID_INDEX_;
    size_t currentChild = currentPos;

    while(dOffset <= N)
    {
        innerLayer = getNMinusOffsetLevel(parentId, innerLayer, dOffset);
        
        currentChild += INTERVALSIZE(innerLayer);

        if(currentChild >= size_)
        {
            return bestChild;
        }

        if(bestChild == INVALID_INDEX_)
        {
            bestChild = currentChild;
        }
        else {
            bestChild = bestChild + (currentChild - bestChild) * 
                    !compare(container_[currentChild], container_[bestChild]);
        }

        dOffset++;
    }

    return bestChild;
}

template <Comparable T, size_t N, typename Compare>
inline std::pair<size_t, size_t> NBeap<T, N, Compare>::getNMinusOffsetLevel(
    size_t parentIdx, std::pair<size_t, size_t> levelInterval, size_t offset
) 
{

    if(parentIdx == levelInterval.first) // should not happen because this is already accounted for up
    {
        return {parentIdx, parentIdx};
    }

    // Possible optimisation here is to start searching
    // from the back since most element should be at the last level
    size_t h = 1, l = 1, end = levelInterval.first, start = levelInterval.first;
    //size_t maxInterval = levelInterval.second - levelInterval.first + 1; // can replace levelInterval with the level size acc
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
inline std::array<size_t, 3> NBeap<T, N, Compare>::getNMinusOffsetLevelWithParentLevel(
    size_t parentLevelStart, size_t currentPos, size_t offset
) 
{

    if(currentPos == parentLevelStart) // should not happen because this is already accounted for up
    {
        return {parentLevelStart-1, parentLevelStart-1, parentLevelStart};
    }

    // Possible optimisation here is to start searching
    // from the back since most element should be at the last level
    size_t curretHeight = 1, numOfElementInLevel = 1, curLevelEnd = parentLevelStart, 
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
inline size_t NBeap<T, N, Compare>::getNumberOfElementInNextLevel(
    size_t currHeight, size_t numOfElemCurrLevel, int offsetDim
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