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
        size_ = container_.size();

        if(size_ <= 1)
        {
            if(size_ == 1)
            {
                height_ = 1;
                levelSize_ = 1;
            }
            
            return;
        }

        auto spanAndHeight = getSpanAndHeight(size_ - 1, N);
        interval_ = {spanAndHeight[0], spanAndHeight[1]};
        levelSize_ = INTERVALSIZE(interval_);
        height_ = spanAndHeight[2];
        auto currInterval = interval_;
        auto currentHeight = height_;
        for(size_t i = interval_.first - 1; i > 0; i--)
        {
            if (i < currInterval.first) {
                currInterval = getPreviousLevelInterval(currentHeight, currInterval);
                currentHeight--;
            }

            siftDownNoBubbleUp(i, currentHeight, currInterval);
            // siftDown(i, currentHeight, currInterval);
        }
        // siftdown from index 0 to avoid overflow in the for loop
        siftDownNoBubbleUp(0, 1, {0, 0}); 
        // siftDown(0, 1);

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
        // std::cout << "Inserting: " << val << std::endl; 
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
        std::pair<size_t, size_t> out{};

        if (!_searchHelper(val, out))
        {
            return;
        }

        const size_t pos = out.first;
        const size_t h = out.second;

        std::swap(container_[pos], container_[size_ - 1]);

        size_--;
        container_.resize(size_);

        if (size_ == 0)
        {
            levelSize_ = 0;
            height_ = 0;
            interval_ = {0, 0};
            return;
        }

        if (size_ - 1 < interval_.first)
        {
            interval_ = getPreviousLevelInterval(height_, interval_);
            levelSize_ = INTERVALSIZE(interval_);
            height_--;
        }

        if (pos >= size_)
        {
            return;
        }

        auto li = span(h, N);

        if (compare(val, container_[pos]))
        {
            bubbleUp(0, pos, h, li);
        }
        
        siftDown(pos, h, li);
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
            return _searchND(val, out);
            // return _searchND(val, interval_.first, height_, interval_, out);
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

    void insertParent(
        std::vector<size_t>& stack, 
        size_t loc, size_t h, 
        std::pair<size_t, size_t> currInterval
    );


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
        factorials[0] = 1.0;
        for(size_t i = 1; i <= N; i++)
        {
            factorials[i] = i * factorials[i-1];
        }
        

    }

    std::pair<size_t, size_t> span(
        size_t currHeight, 
        size_t dim
    )
    {
        assert(currHeight > 0);
        assert(dim > 0);

        switch (dim)
        {
        case 1:
            return {currHeight - 1, currHeight - 1};
        default:
            if (currHeight == 1)
            {
                return {0 , 0};
            }

            size_t start = 1;
            size_t end = 1;
            for (size_t i = 0; i < dim; i++)
            {
                start *= (currHeight - 1 + i);      
                end   *= (currHeight + i); 
            }
            start = start / factorials[dim];
            end   = end   / factorials[dim];

            return {start, end - 1};
        }
    }

    std::array<size_t, 3> getSpanAndHeight(size_t idx, size_t dim)
    {
        auto h = getApproximateHeight(idx, dim);
        auto interval = span(h, dim);
        auto offsetDim = N - dim;
        if(idx < interval.first)
        {
            while(interval.first > idx)
            {
                auto num = getNumberOfElementInPreviousLevel(h, INTERVALSIZE(interval), offsetDim);
                interval.second = interval.first - 1;
                interval.first = interval.first - num;
                h--;
            }
            return {interval.first, interval.second, h};
        }
        else if(idx > interval.second)
        {
            while(interval.second < idx)
            {
                auto num = getNumberOfElementInNextLevel(h, INTERVALSIZE(interval), offsetDim);
                interval.first = interval.second + 1;
                interval.second = interval.second + num;
                h++;
            }
            return {interval.first, interval.second, h};
        }
        else {
            return {interval.first, interval.second, h};
        }
    }

    // idx is the index of the element in container_
    size_t getApproximateHeight(size_t idx, size_t dim)
    {
        assert(dim > 0);

        switch (dim)
        {
        case 1:
            return idx+1;
        
        default:
            auto result = std::max(
                static_cast<size_t>(std::round(
                    std::pow(static_cast<double>(idx) * static_cast<double>(factorials[dim]), 1.0 / dim)
                )),
                static_cast<size_t>(1)
            );
        return result;

        }
        
    }

    
    
    // std::pair<size_t, size_t> span(
    //     size_t preferredHeight, 
    //     std::pair<size_t, size_t> knownHeighInterval, 
    //     size_t knowHeight
    // );

    
    
    void siftDown(size_t startPos, size_t h, std::pair<size_t, size_t> levelInterval = {0,0});
    void bubbleUp(size_t index, size_t endIndex, size_t h, std::pair<size_t, size_t> levelInterval);
    void siftDownNoBubbleUp(
        size_t startPos, size_t h, std::pair<size_t, size_t> levelInterval
    );
    
    size_t getBestParentIndex(
        size_t currentPos, size_t childHeight, std::pair<size_t, size_t> levelInterval
    );

    size_t getBestChildIndex(
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

    // iterative
    bool _searchND(
        T val,
        std::pair<size_t, size_t>& out
    );

    

    std::pair<size_t, size_t> getFirstUniqueChildPos(
        size_t parentIdx, size_t firstChildPos, size_t maxNumberOfChildren,
        std::pair<size_t, size_t> levelInterval
    );

    size_t getNumberOfElementInPreviousLevel(
        size_t currHeight, size_t numOfElemCurrLevel, int offsetDim
    );
    
};


// template <Comparable T, size_t N, typename Compare>
// std::pair<size_t, size_t> NBeap<T, N, Compare>::span(
//     size_t preferredHeight,
//     std::pair<size_t, size_t> knownHeightInterval,
//     size_t knownHeight)
// {
//     if (preferredHeight == knownHeight)
//     {
//         return knownHeightInterval;
//     }

//     while(knownHeight > preferredHeight)
//     {
//         auto prevLevelElems = 
//             getNumOfElemInPrevLevel(knownHeight, INTERVALSIZE(knownHeightInterval));
//         knownHeightInterval.second = knownHeightInterval.first - 1;
//         knownHeightInterval.first -= prevLevelElems; 
//         knownHeight--;
//     }

//     return knownHeightInterval; 
// }

template<Comparable T, size_t N, typename Compare>
void NBeap<T, N, Compare>::siftDown(
    size_t startPos, size_t h, std::pair<size_t, size_t> levelInterval
)
{
    T val = std::move(container_[startPos]);
    auto currPos = startPos;

    while(true)
    {
        size_t bestChild = 
                getBestChildIndex(currPos, h, levelInterval);
        
        if (bestChild == INVALID_INDEX_ /*|| compare(container_[bestChild], val)*/)
        {
            break;
        }
        
        container_[currPos] = std::move(container_[bestChild]);
        currPos = bestChild;
        levelInterval = getNextLevelInterval(h, levelInterval);
        h++;
    }

    container_[currPos] = std::move(val);
    bubbleUp(startPos, currPos, h, levelInterval);
}

template<Comparable T, size_t N, typename Compare>
void NBeap<T, N, Compare>::siftDownNoBubbleUp(
    size_t startPos, size_t h, std::pair<size_t, size_t> levelInterval
)
{
    T val = std::move(container_[startPos]);
    auto currPos = startPos;

    while(true)
    {
        size_t bestChild = 
                getBestChildIndex(currPos, h, levelInterval);
        
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

        // std::cout << " Parents for " << currentPos << " are : ";

        if(currentPos == levelInterval.first)
        {
            // std::cout << parentsInterval.first << std::endl;
            return parentsInterval.first;
        }

        if(currentPos == levelInterval.second)
        {
            // std::cout << parentsInterval.second << std::endl;
            return parentsInterval.second;
        }

        auto parentsLevelSize = INTERVALSIZE(parentsInterval);
        if(parentsLevelSize == 1)
        {
            // std::cout << parentsInterval.first << std::endl;
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
    size_t bestParent = INVALID_INDEX_;

    while (true)
    {
        const size_t idx = currentPos - parentsInterval.first;

        auto innerLevelAndHeight = getSpanAndHeight(idx, N - dOffset);
        std::pair<size_t, size_t> innerLevelLocal{
            innerLevelAndHeight[0], innerLevelAndHeight[1]
        };

        const size_t innerLevelSize = INTERVALSIZE(innerLevelLocal);

        const size_t prevInnerLevelSize =
            getNumberOfElementInPreviousLevel(
                innerLevelAndHeight[2],
                innerLevelSize,
                dOffset
            );

        std::pair<size_t, size_t> prevInnerLocal{
            innerLevelLocal.first - prevInnerLevelSize,
            innerLevelLocal.first - 1
        };

        std::pair<size_t, size_t> prevInnerGlobal{
            parentsInterval.first + prevInnerLocal.first,
            parentsInterval.first + prevInnerLocal.second
        };

        if (idx == innerLevelLocal.first)
        {
            const size_t parent = prevInnerGlobal.first;

            if (bestParent == INVALID_INDEX_)
            {
                return parent;
            }

            return parent + (bestParent - parent) *
                !compare(container_[parent], container_[bestParent]);
        }

        if (idx == innerLevelLocal.second)
        {
            const size_t parent = prevInnerGlobal.second;

            if (bestParent == INVALID_INDEX_)
            {
                return parent;
            }

            return parent + (bestParent - parent) *
                !compare(container_[parent], container_[bestParent]);
        }

        const size_t parent = currentPos - prevInnerLevelSize;

        if (parent <= prevInnerGlobal.second)
        {
            if (bestParent == INVALID_INDEX_)
            {
                bestParent = parent;
            }
            else
            {
                bestParent = parent + (bestParent - parent) *
                    !compare(container_[parent], container_[bestParent]);
            }
        }

        currentPos = parent;
        parentsInterval = prevInnerGlobal;
        dOffset++;
    }
}

template <Comparable T, size_t N, typename Compare>
size_t NBeap<T, N, Compare>::getBestChildIndex(
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
            auto bestChild = levelInterval.second + 1;
            if(bestChild >= size_)
            {
                return INVALID_INDEX_;
            }

            auto remainingChildren = N - 1;
            auto nextChild = bestChild + 1;
            while (remainingChildren > 0 && nextChild < size_)
            {
                bestChild = bestChild + (nextChild - bestChild) * 
                            !compare(container_[nextChild], container_[bestChild]);
                nextChild++;
                remainingChildren--;
            }
            
            return bestChild;
        }

        auto bestChild =  currentPos + INTERVALSIZE(levelInterval);
        if(bestChild >= size_)
            return INVALID_INDEX_;
        
        std::pair<size_t, size_t> innerLayer {}; 
        size_t currentChild = bestChild;
        auto idx = currentPos - levelInterval.first;
        size_t dOffset = 1;

        while(dOffset < N)
        {
            auto innerLayerAndHeight = getSpanAndHeight(idx, N - dOffset);
            innerLayer = {innerLayerAndHeight[0], innerLayerAndHeight[1]};
            currentChild += INTERVALSIZE(innerLayer);
            
            if(currentChild >= size_)
            {
                return bestChild;
            }

            bestChild = bestChild + (currentChild - bestChild) * 
                    !compare(container_[currentChild], container_[bestChild]);


            idx = idx - innerLayer.first;
            dOffset++;
        }

        return bestChild;
    }
}



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
inline bool NBeap<T, N, Compare>::_searchND(T val, std::pair<size_t, size_t> &out)
{

    // // where do we start searching from
    // if (size_ <= 0 || compare(container_[0], val))
    // {
    //     return false;
    // }
    // auto currHeight = height_;
    // auto currentInterval = interval_;

    // // start = currentInterval.second;
    // if (currentInterval.second >= size_)
    // {
    //     currentInterval = getPreviousLevelInterval(currHeight, currentInterval);
    //     // start = currentInterval.second;
    // }

    // std::vector<size_t> stack;
    // stack.reserve(INTERVALSIZE(currentInterval));
    // stack.push_back(currentInterval.second);
    // while(!stack.empty())
    // {
    //     const std::size_t loc = stack.back();
    //     stack.pop_back();

        
        
    //     const auto spanAndHeight = getSpanAndHeight(loc, N);
    //     currentInterval = {spanAndHeight[0], spanAndHeight[1]};
    //     const size_t h = spanAndHeight[2];

    //     if(loc >= size_)
    //     {
    //         // need to go the parent here if i can;
    //         insertParent(stack, loc, h, currentInterval);
    //         continue;
    //     }
        
        
    //     if(compare(container_[loc], val))
    //     {
    //         // We want to go up
    //         // case where we are at the start of a level
    //         if(loc == currentInterval.first)
    //         {
    //             continue;
    //         }

    //         insertParent(stack, loc, h, currentInterval);
    //         // What are the cases here
    //         // when in index 
    //     }
    //     else if(compare(val, container_[loc]))
    //     {
    //         // we want to go down
    //         // Need to insert the children here
    //         // How many children though ??
    //         if(loc == currentInterval.first)
    //         {
    //             auto child = currentInterval.second + 1;
    //             for(auto i = 1; i < N; i++)
    //             {
    //                 stack.push_back(child);
    //                 child++;
    //             }
    //             continue;
    //         }

    //         auto idx = loc - currentInterval.first;
    //         auto currentIntervalSize = INTERVALSIZE(currentInterval);
    //         auto firstChild = loc + currentIntervalSize;
    //         stack.push_back(firstChild);
    //         auto offset = 1;
    //         auto currentPos = firstChild;
    //         while (offset < N)
    //         {
    //             auto innerLayerAndHeight = getSpanAndHeight(idx, N - offset);
    //             currentInterval = {innerLayerAndHeight[0], innerLayerAndHeight[1]};
    //             currentIntervalSize = INTERVALSIZE(currentInterval);
    //             if(currentIntervalSize == 1)
    //             {
    //                 break;
    //             }

    //             currentPos += currentIntervalSize;
    //             stack.push_back(currentPos);

    //             idx -= currentInterval.first;
    //             offset++;
    //         }
    //     }
    //     else 
    //     {
    //         out.first = loc;
    //         out.second = h;
    //         return true;
    //     }
    // }

    // I will try searching from the left

    if (size_ <= 0 || compare(container_[0], val))
    {
        return false;
    }

    std::vector<size_t> stack;
    stack.push_back(interval_.first);
    while(!stack.empty())
    {
        auto loc = stack.back();
        stack.pop_back();
        auto spanAndHeight = getSpanAndHeight(loc, N);
        const auto h = spanAndHeight[2];
        std::pair<size_t, size_t> currentInterval = {spanAndHeight[0], spanAndHeight[1]};

        if(loc >= size_)
        {
            // push the parent and then -
            auto prevLevelInterval = getPreviousLevelInterval(h, currentInterval);
            auto parent = loc - INTERVALSIZE(prevLevelInterval);
            if(parent <= prevLevelInterval.second)
            {
                stack.push_back(parent);
            }
            continue;
        }

        if(compare(container_[loc], val))
        {
            // going up 
            auto prevLevelInterval = getPreviousLevelInterval(h, currentInterval);
            auto parent = loc - INTERVALSIZE(prevLevelInterval);
            if(parent <= prevLevelInterval.second)
            {
                stack.push_back(parent);
            }
            
        }
        else if(compare(val, container_[loc]))
        {
            // going down
            auto firstChild = loc + INTERVALSIZE(currentInterval);
            auto firstUniqueChildPosAndMaxNumOfChildren = 
                getFirstUniqueChildPos(loc, firstChild, N-1, currentInterval);
            auto maxNumberOfChildren = firstUniqueChildPosAndMaxNumOfChildren.second;
            auto firstUniqueChild = firstUniqueChildPosAndMaxNumOfChildren.first;
            while(maxNumberOfChildren > 0)
            {
                stack.push_back(firstUniqueChild);
                firstUniqueChild++;
                maxNumberOfChildren--;
            }
        }
        else 
        {
            out.first = loc;
            out.second = h;
            return true;
        }
    }


    return false;
}

template <Comparable T, size_t N, typename Compare>
void NBeap<T, N, Compare>::insertParent(
    std::vector<size_t>& stack, 
    size_t loc, size_t h, 
    std::pair<size_t, size_t> currInterval
)
{
    if(loc == currInterval.second)
    {
        stack.push_back(currInterval.first - 1);
        return;
    }

    auto parentsInterval = getPreviousLevelInterval(h , currInterval);
    auto currentPos = loc - INTERVALSIZE(parentsInterval);
    auto dOffset = 1;
    size_t lastParent = INVALID_INDEX_;
    auto numberOfParents = 0;

    while(true)
    {
        const size_t idx = currentPos - parentsInterval.first;
        auto innerLevelAndHeight = getSpanAndHeight(idx, N - dOffset);
        std::pair<size_t, size_t> innerLevelLocal{
            innerLevelAndHeight[0], innerLevelAndHeight[1]
        };
        const size_t innerLevelSize = INTERVALSIZE(innerLevelLocal);
        const size_t prevInnerLevelSize =
            getNumberOfElementInPreviousLevel(
                innerLevelAndHeight[2],
                innerLevelSize,
                dOffset
            );   
        
        std::pair<size_t, size_t> prevInnerLocal{
            innerLevelLocal.first - prevInnerLevelSize,
            innerLevelLocal.first - 1
        };

        std::pair<size_t, size_t> prevInnerGlobal{
            parentsInterval.first + prevInnerLocal.first,
            parentsInterval.first + prevInnerLocal.second
        };

        if(idx == innerLevelLocal.first)
        {
            if(idx == innerLevelLocal.second /*&& numberOfParents > 0*/)
            {
                stack.push_back(prevInnerGlobal.second);
            }
            
            break;
        }

        if(idx == innerLevelLocal.second)
        {
            stack.push_back(prevInnerGlobal.second);
            break;
        }

        const size_t parent = currentPos - prevInnerLevelSize;

        if (parent <= prevInnerGlobal.second)
        {
            lastParent = parent;    
            numberOfParents++;
        }

        currentPos = parent;
        parentsInterval = prevInnerGlobal;
        dOffset++;

    }

    return;
}

template <Comparable T, size_t N, typename Compare>
std::pair<size_t, size_t> NBeap<T, N, Compare>::getFirstUniqueChildPos(
    size_t parentIdx, size_t firstChildPos, size_t maxNumberOfChildren,
    std::pair<size_t, size_t> levelInterval)
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

template <Comparable T, size_t N, typename Compare>
inline size_t NBeap<T, N, Compare>::getNumberOfElementInPreviousLevel(
    size_t currHeight, size_t numOfElemCurrLevel, int offsetDim
)
{
    auto dim = N - offsetDim;

    switch (dim)
    {
    case 1:
        return 1;
    case 2:
        return --numOfElemCurrLevel;
    default:
        return (numOfElemCurrLevel * (currHeight - 1)) / (currHeight + dim - 2);
    }
}