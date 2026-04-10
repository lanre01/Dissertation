// A Bi-Parental heap implementation based on J. Ian Munro and Hendra Suwanda
// (see https://www.sciencedirect.com/science/article/pii/0022000080900379)

// Copyright Lawal Alongbija 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See http://www.boost.org/LICENSE_1_0.txt)

#include <utility>
#include <functional>
#include <vector>
#include <cassert>
#include <cmath>

template <typename T, typename Compare = std::greater<T>>
class Beap
{
public:
	Beap()
    : size_(0)
    , height_(0)
    , compare_(Compare())
    , span_(0,0)
    {};

	Beap(Compare c)
    : size_(0)
    , height_(0)
    , span_(0,0)
    , compare_(c)
    {};

    Beap(std::vector<T> vec)
    : Beap(std::move(vec), Compare()) 
    {}
    

    Beap(std::vector<T> vec, Compare c) 
    : size_(vec.size()) 
    , height_(0)
    , container_(std::move(vec))
    , compare_(c)
    , span_(0, 0) 
    {
        if (size_ <= 1)
        {
            if(size_ == 1)
            {
                height_ = 1;
            }
            return;
        }

        height_ = static_cast<std::size_t>(std::round(std::sqrt(size_ << 1)));
        span_ = span(height_);
        auto currSpan = span_;
        auto currentHeight = height_;
        for (size_t i = span_.first - 1; i >= 1; i--)
        {
            if (i < currSpan.first) {
                currentHeight--;
                currSpan = parentSpan(currSpan.first, currentHeight);
            }
              
            siftDownNoBubbleUp(i, currentHeight, currSpan);
        }
        
        siftDownNoBubbleUp(0, 1, {0, 0});
    }


	~Beap() {};

	Beap& operator=(const Beap&) = default;

    bool search(T value)
    {
        std::pair<size_t, size_t> out{0, 0};
        return searchImpl(value, out);
    }
    

	T extract();

	void insert(T value);

	void remove(T value);

    size_t height() {return height_-1;}

    size_t size() {return size_;}
    
    void clear() {
        container_.clear();
        size_ = 0;
        height_ = 0;
        span_ = {0,0};
    }

	bool empty(){ return container_.empty(); }

private:
    size_t size_;
	size_t height_;
    std::vector<T> container_;
    Compare compare_;
    std::pair<size_t, size_t> span_;
    static constexpr size_t INVALID_INDEX_ = static_cast<size_t>(-1);
    
    // Restores the beap invariant by pushing the pos element down to the
    // last level before bubbling it back up after extraction. 
    // Because the pos represents one of the largest elements in the
    // beap, moving it downward first avoids one less comparisons until the
    // bottom of beap and should reduces the overall number of comaparisons.
    void siftDown(
        const size_t pos, 
        const size_t h,
        const std::pair<size_t, size_t> levelSpan
    );

    // The normal sift down does not work for bulk insertion
    // This is due to the geometry of the beap. The path traverse
    // up may be different to the path traversed down. So beap 
    // invariant may not be respected after bubbling up.
    void siftDownNoBubbleUp(
        const size_t pos, 
        const size_t h,
        const std::pair<size_t, size_t> levelSpan
    );

    bool searchImpl(T value, std::pair<size_t, size_t>& out);

    // Moves the element at pos up toward startPos within a beap structure, 
    // swapping with the best parent based on Compare until heap order is 
    // restored or the top is reached.
    void bubbleUp(
        const size_t startPos, 
        const size_t pos, 
        const size_t childHeight,
        const std::pair<size_t, size_t> levelSpan
    );

	inline const std::pair<size_t, size_t> span(size_t height_);

    inline size_t getBestParent(
        const size_t childHeight, 
        const size_t childIndex,
        const std::pair<size_t, size_t> curr_child_Span
    );

    inline size_t getBestChild(
        const size_t parentHeight, const size_t index
    );

    inline std::pair<size_t, size_t> childSpan(const size_t parentspan_last_index_, const size_t childheight_)
    {
        return {parentspan_last_index_ + 1, parentspan_last_index_ + childheight_};
    }

    inline std::pair<size_t, size_t> parentSpan(const size_t childspan_first_index_, const size_t parentheight_)
    {
        return {childspan_first_index_ - parentheight_, childspan_first_index_ - 1};
    }
};


template <typename T, typename Compare>
void Beap<T, Compare>::siftDownNoBubbleUp(
    const size_t pos, 
    const size_t h, 
    const std::pair<size_t, size_t> levelSpan
)
{
    size_t currentPos = pos;
    size_t currentHeight = h;
    T newItem = std::move(container_[pos]);
    auto currentSpan = levelSpan;

    while (true)
    {
        auto bestChildPos = getBestChild(currentHeight, currentPos);

        if (bestChildPos == INVALID_INDEX_ || compare_(container_[bestChildPos], newItem)) {
            break;
        }

        container_[currentPos] = std::move(container_[bestChildPos]);
        currentPos = bestChildPos;
        currentHeight++;
        currentSpan = childSpan(currentSpan.second, currentHeight);
    }
    
    container_[currentPos] = std::move(newItem);
}

template <typename T, typename Compare>
void Beap<T, Compare>::siftDown(
    const size_t pos, 
    const size_t h, 
    const std::pair<size_t, size_t> levelSpan
)
{
    if (size_ == 0)
    {
        return;
    }

    size_t currentPos = pos;
    size_t currentHeight = h;
    T newItem = std::move(container_[pos]);
    auto currentSpan = levelSpan;

    while (true)
    {
        auto bestChildPos = getBestChild(currentHeight, currentPos);

        if (bestChildPos == INVALID_INDEX_) {
            break;
        }

        container_[currentPos] = std::move(container_[bestChildPos]);
        currentPos = bestChildPos;
        currentHeight++;
        currentSpan = childSpan(currentSpan.second, currentHeight);
    }
    
    container_[currentPos] = std::move(newItem);
    bubbleUp(pos, currentPos, currentHeight, currentSpan);
}

template <typename T, typename Compare>
void Beap<T, Compare>::bubbleUp(
    const size_t startPos, 
    const size_t pos, 
    const size_t childHeight,
    const std::pair<size_t, size_t> levelSpan
) 
{
    if (container_.empty() || size_ == 1)
    {
        return;
    }
    
    T newItem = std::move(container_[pos]);
    size_t currentPos = pos;
    size_t currentHeight = childHeight;
    auto currentSpan = levelSpan;
    
    while (currentPos > startPos)
    {
        size_t bestParentPos = getBestParent(currentHeight, currentPos, currentSpan);
        
        if (compare_(newItem, container_[bestParentPos])) {
            break;
        }

        container_[currentPos] = std::move(container_[bestParentPos]);
        currentPos = bestParentPos;
        currentHeight--;
        currentSpan = parentSpan(currentSpan.first, currentHeight);
    }

    container_[currentPos] = std::move(newItem);
}

template <typename T, typename Compare>
bool Beap<T, Compare>::searchImpl(T value, std::pair<size_t, size_t>& out)
{
    if(size_ <= 0 || compare_(container_[0], value))
    {
        return false;
    }

    size_t h = height_;
    size_t end = span_.first;
    size_t start = span_.second;
    std::pair<size_t, size_t> currentLevel = span_;

    if(value == container_[end])
    {
        out.first = end;
        out.second = h;
        return true;
    }

    // The search should begin from the penultimate level
    // if the last level is not full.
    if(start >= size_)
    {
        start = start - h;
        h--;
        currentLevel = parentSpan(currentLevel.first, h);
    }

    size_t idx = start;
    
    while(true)
    {   
        if(idx == end)
        {
            return false;
        }

        if(compare_(container_[idx], value))
        {
            // If search attempts to go up at the start of any level, 
            // this means the element does not exist in the structure 
            // and search must terminate
            if (idx == currentLevel.first)
            {
                return false; 
            }

            idx -= h;
            h--;
            currentLevel = parentSpan(currentLevel.first, h);
        }
        else if(compare_(value,container_[idx]))
        {
            auto firstChild = idx + h;
            if(firstChild >= size_)
            {
                idx--;
            }
            else
            {
                idx = firstChild;
                h++;
                currentLevel = childSpan(currentLevel.second, h);
            }
        }
        
        else{
            out.first = idx;
            out.second = h;
            return true;
        }
    }
}

template <typename T, typename Compare>
T Beap<T, Compare>::extract()
{
    if(size_ <= 0)
    {
        throw std::out_of_range("cannot extract empty Beap");
    }
    T minValue = std::exchange(container_[0], container_[size_-1]);
    size_--;
	container_.resize(size_);
	if(size_ <= 0)
	{
		height_ = 0;
        span_ = {0, 0};
		return minValue;
	}

    if (size_-1 < span_.first)
    {
        height_--;
       span_ = parentSpan(span_.first, height_);
    }
    
    
    siftDown(0, 1, std::pair{0,0});
    return minValue;
}

template <typename T, typename Compare>
void Beap<T, Compare>::insert(T value)
{
    size_++;

    if (size_ == 1 || size_ - 1 > span_.second)
    {
        height_++;
        if(size_ > 1) // no need to update the span for the first element inserted
            span_ = childSpan(span_.second, height_);
    }
    container_.push_back(value);
    if(size_ <= 1)
    {
        return;
    }
    bubbleUp(0, size_ - 1, height_, span_);
} 

template <typename T, typename Compare>
void Beap<T, Compare>::remove(T value)
{
    std::pair<size_t, size_t> out;
    
    if(!searchImpl(value, out))
    {
        return;
    }

    std::swap(container_[out.first], container_[size_ - 1]);

    size_--;
    container_.resize(size_);

    if (size_ == 0) {
        height_ = 0;
        span_ = {0, 0};
        return;
    }

    if(size_ - 1 < span_.first )
    {
        height_--;
        span_ = parentSpan(span_.first, height_);
    }

    if(out.first >= size_)
    {
        return;
    }

    auto currentSpan = span(out.second);
    if(compare_(value, container_[out.first]))
        bubbleUp(0, out.first, out.second, currentSpan);

    siftDown(out.first, out.second, currentSpan);
    
}

template <typename T, typename Compare>
inline const std::pair<size_t, size_t> Beap<T, Compare>::span(size_t h)
{
    assert(h > 0);

    size_t start = ((h * (h - 1)) / 2) + 1;
    size_t end = (h * (h + 1)) / 2;
    return {start-1, end-1};
}

template <typename T, typename Compare>
inline size_t Beap<T, Compare>::getBestParent(
        const size_t childHeight, 
        const size_t childIndex,
        const std::pair<size_t, size_t> currChildSpan
) 
{

    auto firstParent = childIndex - childHeight;
    if(childIndex == currChildSpan.second)
    {
        return firstParent;
    }

    if(childIndex == currChildSpan.first)
    {
        return firstParent + 1;
    }

    return firstParent + !compare_(container_[firstParent], container_[firstParent+1]);
}

template <typename T, typename Compare>
inline size_t Beap<T, Compare>::getBestChild(
    const size_t parentHeight, const size_t index
)
{

    auto firstChild = index + parentHeight;
    if (firstChild >= size_)
    {
        return INVALID_INDEX_;
    }

    auto secondChild = firstChild + 1;
    if (secondChild >= size_)
    {
        return firstChild;
    }

    return firstChild + compare_(container_[firstChild], container_[secondChild]);
}
