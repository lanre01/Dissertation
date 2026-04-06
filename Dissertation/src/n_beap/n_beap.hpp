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
    {
        initFactorials();
        size_ = container_.size();

        if(size_ <= 1)
        {
            if(size_ == 1)
            {
                height_ = 1;
            }
            
            return;
        }

        auto spanAndHeight = getSpanAndHeight(size_ - 1, N);
        interval_ = {spanAndHeight[0], spanAndHeight[1]};
        height_ = spanAndHeight[2];
        auto currInterval = interval_;
        for (size_t h = height_ - 1; h > 0; h-- )
        {
            currInterval = getPreviousLevelInterval(h + 1, currInterval);
            for(size_t j = currInterval.first; j <= currInterval.second; j++)
            {
                siftDownNoBubbleUp(j, h, currInterval);
            }

        }
        siftDownNoBubbleUp(0, 1, {0, 0}); 
    }

    ~NBeap() = default;

    NBeap& operator=(const NBeap&) = default;

    void reserve(size_t count)
    {
        container_.reserve(count);
    }

    T extract() 
    {
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
            else {
                interval_ = getPreviousLevelInterval(height_, interval_);
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
                interval_ = getNextLevelInterval(height_, interval_);
                height_++;
            }
        }

       

        bubbleUp(0, size_, height_, interval_);

        size_++;
    }

    void remove(T val)
    {
        std::pair<size_t, size_t> out{};

        if (!searchHelper(val, out))
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
            height_ = 0;
            interval_ = {0, 0};
            return;
        }

        if (size_ - 1 < interval_.first)
        {
            interval_ = getPreviousLevelInterval(height_, interval_);
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

        return searchHelper(val, out);
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
		std::cout << " | size=" << size_ << " height=" << height_ << " levelSize=" << INTERVALSIZE(interval_) << std::endl;
        std::cout << " | intervals=" << interval_.first << "," << interval_.second << std::endl;
		std::cout.flush();
	}


private:
    Compare compare;
    std::vector<T> container_;
    size_t height_;
    size_t size_;
    std::pair<size_t, size_t> interval_;
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
        if(idx < interval.first)
        {
            while(interval.first > idx)
            {
                interval = getPreviousLevelInterval(h, interval, dim);
                h--;
            }
            return {interval.first, interval.second, h};
        }
        else if(idx > interval.second)
        {
            while(interval.second < idx)
            {
                interval = getNextLevelInterval(h, interval, dim);
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
        const size_t childPos, 
        const size_t childHeight,
        const std::pair<size_t, size_t> levelInterval
    );

    size_t getBestChildIndex(
        size_t currentPos, size_t childHeight, std::pair<size_t, size_t> levelInterval
    );

    std::array<size_t, 3> getInnerLayerAndHeight(
        size_t levelIndex, const size_t levelHeight, std::pair<size_t, size_t> levelInterval, size_t dim
    );

    // This can actually use binary search since it is sorted
    // Beap search is used for consistency reason
    bool search1D(T val, std::pair<size_t, size_t>& out)
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

    bool search2D(T val, std::pair<size_t, size_t>& out)
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

    bool searchHelper(T val, std::pair<size_t, size_t>& out)
    {
        if constexpr (N == 1)
        {
            return search1D(val, out);
        }
        else if constexpr (N == 2)
        {
            return search2D(val, out);
        }

        else {
            return searchND(val, out);
            // return searchND(val, interval_.first, height_, interval_, out);
        }

    }

    size_t getNumberOfElementInNextLevel(size_t currHeight, size_t numOfElemCurrLevel, size_t dim = N)
    {
        switch (dim)
        {
        case 1:
            return 1;
        case 2:
            return ++numOfElemCurrLevel;
        default:
            return (numOfElemCurrLevel * (currHeight + dim - 1 )) / currHeight;
        }
    }

    size_t getNumOfElemInPrevLevel(size_t currHeight, size_t numOfElemCurrLevel, size_t dim = N)
    {
        assert(currHeight > 1);

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

    std::pair<size_t, size_t> getPreviousLevelInterval(
        size_t currHeight, 
        std::pair<size_t, size_t> currLevInt, 
        size_t dim = N
    )
    {
        auto l = getNumOfElemInPrevLevel(currHeight, INTERVALSIZE(currLevInt), dim);

        return {currLevInt.first - l, currLevInt.first - 1};
    }

    std::pair<size_t, size_t> getNextLevelInterval(
        size_t currHeight, std::pair<size_t, size_t> currLevInt,
        size_t dim = N 
    )
    { 
        auto l = getNumberOfElementInNextLevel(currHeight,  INTERVALSIZE(currLevInt), dim);

        return {currLevInt.second + 1, currLevInt.second + l};
    }

    size_t getBestRemainingParents(
        size_t currentPos, std::pair<size_t, size_t> parentsInterval, size_t dOffset
    );

    inline size_t INTERVALSIZE(size_t start, size_t end)
    {
        assert(end >= start);
        return end - start + 1;
    }

    inline size_t INTERVALSIZE(const std::pair<size_t, size_t>& p)
    {
        return INTERVALSIZE(p.first, p.second);
    }

    bool searchND(
        T val, const size_t currPos, const size_t currHeight, 
        const std::pair<size_t, size_t> levelInterval,
        std::pair<size_t, size_t>& out
    );

    // iterative
    bool searchND(
        T val,
        std::pair<size_t, size_t>& out
    );

    

    std::pair<size_t, size_t> getFirstUniqueChildPos(
        size_t parentIdx, size_t firstChildPos, size_t maxNumberOfChildren,
        std::pair<size_t, size_t> levelInterval
    );
    
};

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
    // bubbleUp(startPos, currPos, h, levelInterval);
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
    const size_t childPos, 
    const size_t childHeight,
    const std::pair<size_t, size_t> levelInterval
)
{
    
    if constexpr (N == 1)
    {
        return childPos - 1;
    }
    else if constexpr (N == 2) 
    { 
        //auto levelSize = INTERVALSIZE(levelInterval);
        auto firstParent = childPos - childHeight;

        if(childPos == levelInterval.first)
        {
            return firstParent + 1;
        }

        if(childPos == levelInterval.second)
        {
            return firstParent;
        }

        return firstParent + !compare(container_[firstParent], container_[firstParent+1]);
    } 
    else 
    {
        auto parentsInterval = getPreviousLevelInterval(childHeight, levelInterval);

        if(childPos == levelInterval.first)
        {
            return parentsInterval.first;
        }

        if(childPos == levelInterval.second)
        {
            return parentsInterval.second;
        }

        auto parentsLevelSize = INTERVALSIZE(parentsInterval);
        if(parentsLevelSize == 1)
        {
            return parentsInterval.first;
        }
        auto bestParent = INVALID_INDEX_;
        auto currentLoc = childPos - parentsLevelSize;
        if(currentLoc <= parentsInterval.second)
        {
            bestParent = currentLoc;
        } 

        std::array<size_t, 3> innerLevelAndHeight{levelInterval.first, levelInterval.second, childHeight};
        auto dim = N - 1;
        while(dim > 0)
        {
            innerLevelAndHeight = getInnerLayerAndHeight(childPos, 
                innerLevelAndHeight[2], {innerLevelAndHeight[0], innerLevelAndHeight[1]}, dim);
            const auto currentLevelSize = INTERVALSIZE(innerLevelAndHeight[0], innerLevelAndHeight[1]);
            
            

            if(childPos == innerLevelAndHeight[0])
            {
                size_t parent;
                if(innerLevelAndHeight[2] <= 1)
                {
                    parent = currentLoc - 1;
                }
                else {
                    parent = currentLoc - getNumOfElemInPrevLevel(innerLevelAndHeight[2], currentLevelSize, dim);
                }
                

                if (bestParent == INVALID_INDEX_)
                {
                    return parent;
                }

                return parent + (bestParent - parent) *
                    !compare(container_[parent], container_[bestParent]);

            }

            if(childPos == innerLevelAndHeight[1])
            {
                const size_t parent = currentLoc - currentLevelSize;

                if (bestParent == INVALID_INDEX_)
                {
                    return parent;
                }

                return parent + (bestParent - parent) *
                    !compare(container_[parent], container_[bestParent]);
            }

            const auto prevLevelSize = getNumOfElemInPrevLevel(innerLevelAndHeight[2], currentLevelSize, dim);
            currentLoc -= prevLevelSize;

            if(childPos - prevLevelSize < innerLevelAndHeight[0])
            {
                if (bestParent == INVALID_INDEX_)
                {
                    bestParent = currentLoc;
                }
                else
                {
                    bestParent = currentLoc + (bestParent - currentLoc) *
                        !compare(container_[currentLoc], container_[bestParent]);
                }
            }

            dim--;
        }

        return bestParent;


        // auto bestParent = childPos - parentsLevelSize;
        
        // if(bestParent > parentsInterval.second)
        // {
        //     return getBestRemainingParents(bestParent, parentsInterval, 1);
        // } 

        // auto bestParentFromRest = 
        //     getBestRemainingParents(bestParent, parentsInterval, 1);

        // if (bestParentFromRest >= INVALID_INDEX_)
        // {
        //     return bestParent;
        // }

        // return bestParentFromRest + (bestParent - bestParentFromRest) * 
        //                 !compare(container_[bestParentFromRest], container_[bestParent]);
        
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
            getNumOfElemInPrevLevel(
                innerLevelAndHeight[2],
                innerLevelSize,
                N - dOffset
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

        std::array<size_t, 3> innerLayerAndHeight{levelInterval.first, levelInterval.second, parentHeight};
        size_t currentChild = bestChild;
        size_t dim = N - 1;

        while(dim > 0)
        {
            innerLayerAndHeight = getInnerLayerAndHeight(currentPos, 
                innerLayerAndHeight[2], {innerLayerAndHeight[0], innerLayerAndHeight[1]}, dim);
            currentChild += INTERVALSIZE(innerLayerAndHeight[0], innerLayerAndHeight[1]);

            if(currentChild >= size_)
            {
                return bestChild;
            }

            bestChild = bestChild + (currentChild - bestChild) * 
                    !compare(container_[currentChild], container_[bestChild]);
            dim--;
        }

        return bestChild;
    }
}



template <Comparable T, size_t N, typename Compare>
bool NBeap<T, N, Compare>::searchND(
    T val, const size_t currPos, const size_t currHeight, 
    const std::pair<size_t, size_t> levelInterval, 
    std::pair<size_t, size_t>& out
) 
{
    if(currPos >= size_ || compare(container_[currPos], val))
    {
        auto parentLevel = getPreviousLevelInterval(currHeight, levelInterval);
        auto newPos = currPos - INTERVALSIZE(parentLevel);
        if(newPos >= levelInterval.first)
        {
            return false;
        } 
        
        return searchND(val, newPos, currHeight - 1, parentLevel, out);
    }
    else if(compare(val, container_[currPos]))
    {
        auto firstChild = currPos + INTERVALSIZE(levelInterval);
        const auto firstUniqueChildPosAndMaxNumOfChildren = 
                getFirstUniqueChildPos(currPos, firstChild, N-1, levelInterval);
        const auto childrenLevel = getNextLevelInterval(currHeight, levelInterval);
        auto childPos = firstUniqueChildPosAndMaxNumOfChildren.first;
        auto maxNumberOfChildren = firstUniqueChildPosAndMaxNumOfChildren.second;

        while (maxNumberOfChildren > 0)
        {
            if(searchND(val, childPos, currHeight + 1, childrenLevel, out))
            {
                return true;
            }
            childPos++;
            maxNumberOfChildren--;
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
inline bool NBeap<T, N, Compare>::searchND(T val, std::pair<size_t, size_t> &out)
{

    if (size_ <= 0 || compare(container_[0], val))
    {
        return false;
    }

    
    // I also want to create some cache for the 
    constexpr int CACHE_SIZE = 10;
    std::array<size_t, 2 * CACHE_SIZE> CACHE{};

    std::vector<size_t> stack;
    stack.reserve(2 * INTERVALSIZE(interval_));

    auto slot = 2 * (height_ % CACHE_SIZE);
    CACHE[slot] = interval_.first;
    CACHE[slot + 1] = interval_.second;

    stack.push_back(interval_.first);
    stack.push_back(height_);

    while (!stack.empty())
    {
        assert(stack.size() >= 2);

        const auto h = stack.back();
        stack.pop_back();

        const auto loc = stack.back();
        stack.pop_back();

        const auto slot = 2 * (h % CACHE_SIZE);

        std::pair<size_t, size_t> currentInterval;

        if (loc >= CACHE[slot] && loc <= CACHE[slot + 1])
        {
            currentInterval = {CACHE[slot], CACHE[slot + 1]};
        }
        else
        {
            auto spanAndHeight = getSpanAndHeight(loc, N);
            currentInterval = {spanAndHeight[0], spanAndHeight[1]};
            CACHE[slot] = currentInterval.first;
            CACHE[slot + 1] = currentInterval.second;
        }

        if(loc >= size_ || compare(container_[loc], val))
        {
            // going up 
            auto prevLevelInterval = getPreviousLevelInterval(h, currentInterval);
            auto parent = loc - INTERVALSIZE(prevLevelInterval);
            if(parent <= prevLevelInterval.second)
            {
                const auto pSlot = 2 * ((h - 1) % CACHE_SIZE);
                CACHE[pSlot] = prevLevelInterval.first;
                CACHE[pSlot + 1] = prevLevelInterval.second;
                stack.push_back(parent);
                stack.push_back(h - 1);
            }
            
        }
        else if(compare(val, container_[loc]))
        {
            // going down
            auto firstChild = loc + INTERVALSIZE(currentInterval);
            auto firstUniqueChildPosAndMaxNumOfChildren = 
                getFirstUniqueChildPos(loc, firstChild, h, currentInterval);
            auto maxNumberOfChildren = firstUniqueChildPosAndMaxNumOfChildren.second;
            auto firstUniqueChild = firstUniqueChildPosAndMaxNumOfChildren.first;
            auto nextLevelInterval = getNextLevelInterval(h, currentInterval);
            const auto cSlot = 2 * ((h + 1) % CACHE_SIZE);
            CACHE[cSlot] = nextLevelInterval.first;
            CACHE[cSlot + 1] = nextLevelInterval.second; 
            while(maxNumberOfChildren > 0)
            {
                stack.push_back(firstUniqueChild);
                stack.push_back(h + 1);
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
std::pair<size_t, size_t> NBeap<T, N, Compare>::getFirstUniqueChildPos(
    size_t parentIdx, size_t firstChildPos, size_t parentHeight,
    std::pair<size_t, size_t> levelInterval
)
{
    auto dim  = N - 1;
    auto innerLevelAndHeight = getInnerLayerAndHeight(parentIdx, parentHeight, levelInterval, dim);
    dim--;
    auto nextchildPos = firstChildPos + INTERVALSIZE(innerLevelAndHeight[0], innerLevelAndHeight[1]);
    while(innerLevelAndHeight[0] != parentIdx)
    {
        innerLevelAndHeight = getInnerLayerAndHeight(parentIdx, innerLevelAndHeight[2], 
            {innerLevelAndHeight[0], innerLevelAndHeight[1]}, dim);
        nextchildPos += INTERVALSIZE(innerLevelAndHeight[0], innerLevelAndHeight[1]);
        dim--;
    }


    return {nextchildPos, dim + 1};
}



template <Comparable T, size_t N, typename Compare>
inline std::array<size_t, 3> NBeap<T, N, Compare>::getInnerLayerAndHeight(
    size_t idx, const size_t levelHeight, std::pair<size_t, size_t> levelInterval, size_t dim
) 
{
    if(idx == levelInterval.first)
    {
        return {idx, idx, 1};
    }

    // get the previous level
    auto prevLevelSize = getNumOfElemInPrevLevel(levelHeight, INTERVALSIZE(levelInterval), dim + 1);

    size_t h = levelHeight, start = levelInterval.first + prevLevelSize, end = levelInterval.second;
    size_t currLevelSize = INTERVALSIZE(start, end);
    while(idx < start)
    {
        currLevelSize = getNumOfElemInPrevLevel(h, currLevelSize, dim);
        end = start - 1;
        start -= currLevelSize;
        h--;
    } 

    return {start, end, h};


    // Possible optimisation here is to start searching
    // from the back since most element should be at the last level
    // size_t h = 1, l = 1, end = levelInterval.first, start = levelInterval.first;
    // while(idx > end) 
    // {
    //     l = getNumberOfElementInNextLevel(h, l, dim);
    //     start = end;
    //     end += l;
    //     h++;
    // }

    // return {start + 1, end, h};
}
