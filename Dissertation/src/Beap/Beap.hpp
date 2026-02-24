#include <utility>
#include <functional>
#include <vector>
#include <array>
#include <iostream>



template <typename T, typename Compare = std::greater<T>>
class Beap
{
public:
	Beap(): _size(0), _height(0), compare(Compare()){};

	Beap(Compare c): _size(0), _height(0), compare(c){};

	~Beap() {};

	Beap& operator=(const Beap&) = default;

    bool search(T value)
    {
        std::pair<size_t, size_t> out{0, 0};
        return _search(value, out);
    }
    

	T extract();

	void insert(T value);

	void remove(T value);

    size_t height() {return _height-1;}

    size_t size() {return _size;}

	size_t capacity() {return container.capacity();}
    
    void clear() {
        container.clear();
        _size = 0;
        _height = 0;
        _span = {0,0};
    }

	bool empty(){return container.empty();}

	void reserve(size_t capacity) {container.reserve(capacity);};
	
	void printState(const std::string& operation) {
    	std::cout << "After " << operation << ": ";
		for (T val : container) {
			std::cout << val << " ";
		}
		std::cout << " | size=" << _size << " height=" << _height << std::endl;
		std::cout.flush();
	}

    const std::vector<T> getContainer() {return container;}

private:
    size_t _size;
	size_t _height;
    std::vector<T> container;
    Compare compare;
    std::pair<size_t, size_t> _span{0,0}; // ideally -1 should be used here, but cannot have -1 with size_t
    static constexpr size_t INVALID_INDEX = static_cast<size_t>(-1);
    

    void siftDown(
        const size_t pos, 
        const size_t h,
        const std::pair<size_t, size_t> levelSpan
    );

    bool _search(T value, std::pair<size_t, size_t>& out);

    void bubbleUp(
        const size_t startPos, 
        const size_t pos, 
        const size_t childHeight,
        const std::pair<size_t, size_t> levelSpan
    );

	inline const std::pair<size_t, size_t> span(size_t _height);

    inline size_t getBestParent(
        const size_t childHeight, 
        const size_t childIndex,
        const std::pair<size_t, size_t> curr_child_Span
    );

    inline size_t getBestChild(
        const size_t parentHeight, const size_t index
    );

    inline std::pair<size_t, size_t> childSpan(const size_t parent_span_last_index, const size_t child_height)
    {
        return {parent_span_last_index + 1, parent_span_last_index + child_height};
    }

    inline std::pair<size_t, size_t> parentSpan(const size_t child_span_first_index, const size_t parent_height)
    {
        return {child_span_first_index - parent_height, child_span_first_index - 1};
    }
};



// cpp code

template <typename T, typename Compare>
void Beap<T, Compare>::siftDown(
    const size_t pos, 
    const size_t h, 
    const std::pair<size_t, size_t> levelSpan
)
{
    if (empty())
    {
        return;
    }

    size_t currentPos = pos;
    size_t currentHeight = h;
    T newItem = std::move(container[pos]);
    auto currentSpan = levelSpan;

    while (true)
    {
        auto bestChildPos = getBestChild(currentHeight, currentPos);

        // If no children or children are larger, we're done
        if (bestChildPos == INVALID_INDEX) {
            break;
        }

        // Move the smaller child up
        container[currentPos] = std::move(container[bestChildPos]);
        currentPos = bestChildPos;
        currentHeight++;
        currentSpan = childSpan(currentSpan.second, currentHeight); // need to remove this
    }
    
    container[currentPos] = std::move(newItem);
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
    if (container.empty() || _size == 1)
    {
        return;
    }
    
    T newItem = std::move(container[pos]);
    size_t currentPos = pos;
    size_t currentHeight = childHeight;
    auto currentSpan = levelSpan;
    
    while (currentPos > startPos)
    {
        size_t bestParentPos = getBestParent(currentHeight, currentPos, currentSpan);
        
        if (compare(newItem, container[bestParentPos])) {
            break;
        }

        container[currentPos] = std::move(container[bestParentPos]);
        currentPos = bestParentPos;
        currentHeight--;
        currentSpan = parentSpan(currentSpan.first, currentHeight);
    }

    container[currentPos] = std::move(newItem);
}

template <typename T, typename Compare>
bool Beap<T, Compare>::_search(T value, std::pair<size_t, size_t>& out)
{
    if(_size <= 0 || compare(container[0], value))
    {
        return false;
    }

    size_t h = _height;
    size_t end = _span.first;
    size_t start = _span.second;
    std::pair<size_t, size_t> currentLevel = _span;

    if(value == container[end])
    {
        out.first = end;
        out.second = h;
        return true;
    }

    if(start >= _size)
    {
        start = start - h;
        h--;
        currentLevel = parentSpan(currentLevel.first, h);
    }

    size_t idx = start;
    //std::cout << "initial idx = " << idx << "initial h = " << h << std::endl;
    
    while(true)
    {
        //std::cout << "Segfaulting here and idx=" << idx << "h = " << h << std::endl;
        
        if(idx == end)
        {
            return false;
        }

        if(compare(container[idx], value))
        {
            // auto firstParent = idx - h;
            // This condition is actually not needed. Idx must never go up 
            // when on the first element in any level
            if (idx == currentLevel.first)
            {
                return false; 
            }

            idx -= h;
            h--;
            currentLevel = parentSpan(currentLevel.first, h);
        }
        else if(compare(value,container[idx]))
        {
            auto firstChild = idx + h;
            if(firstChild >= _size)
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
    if(_size <= 0)
    {
        throw std::out_of_range("cannot extract empty beap");
    }
    T minValue = std::exchange(container[0], container[_size-1]);
    _size--;
	container.resize(_size);
	if(_size <= 0)
	{
		_height = 0;
        _span = {0, 0};
		return minValue;
	}

    if (_size-1 < _span.first)
    {
        _height--;
       _span = parentSpan(_span.first, _height);
    }
    
    
    siftDown(0, 1, std::pair{0,0});
    return minValue;
}

template <typename T, typename Compare>
void Beap<T, Compare>::insert(T value)
{
    _size++;

    if (_size == 1 || _size - 1 > _span.second)
    {
        _height++;
        if(_size > 1) // no need to update the span for the first element inserted
            _span = childSpan(_span.second, _height);
    }
    container.push_back(value);
    if(_size <= 1)
    {
        return;
    }
    bubbleUp(0, _size - 1, _height, _span);
} 

template <typename T, typename Compare>
void Beap<T, Compare>::remove(T value)
{
    std::pair<size_t, size_t> out;
    
    if(!_search(value, out))
    {
        return;
    }

    auto prevElem = std::exchange(container[out.first], container[_size - 1]);

    // decrement the size and resize the container
    _size--;
    container.resize(_size);

    if (_size == 0) {
        _height = 0;
        _span = {0, 0};
        return;
    }

    // check if we just deleted the only element in a level
    if(_size - 1 < _span.first )
    {
        _height--;
        _span = parentSpan(_span.first, _height); // update global span
    }

    // if the last element in the vector was removed, no need to siftDown
    if(out.first >= _size)
    {
        return;
    }

    if(compare(value, container[out.first]))
        bubbleUp(0, out.first, out.second, span(out.second));

    siftDown(out.first, out.second, span(out.second));
    
}

template <typename T, typename Compare>
inline const std::pair<size_t, size_t> Beap<T, Compare>::span(size_t h)
{
	if(h <= 0 )
	{
		return {INVALID_INDEX, INVALID_INDEX};
	}
	
    int start = ((h * (h - 1)) / 2) + 1;
    int end = (h * (h + 1)) / 2;
    return {start-1, end-1};
}

template <typename T, typename Compare>
inline size_t Beap<T, Compare>::getBestParent(
        const size_t childHeight, 
        const size_t childIndex,
        const std::pair<size_t, size_t> curr_child_span
    ) 
{

    auto firstParent = childIndex - childHeight;
    if(childIndex == curr_child_span.second)
    {
        return firstParent;
    }

    if(childIndex == curr_child_span.first)
    {
        return firstParent + 1;
    }

    return firstParent + !compare(container[firstParent], container[firstParent+1]);
}

template <typename T, typename Compare>
inline size_t Beap<T, Compare>::getBestChild(const size_t parentHeight, const size_t index)
{

    auto firstChild = index + parentHeight;
    if (firstChild >= _size)
    {
        return INVALID_INDEX;
    }

    auto secondChild = firstChild + 1;
    if (secondChild >= _size)
    {
        return firstChild;
    }

    return firstChild + compare(container[firstChild], container[secondChild]);
}




