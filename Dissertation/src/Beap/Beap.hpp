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

	size_t getMaxIndexIncontainer(size_t index1, size_t index2);

	std::pair<size_t, size_t> search(T value);

	T pop();

	void push(T value);

	void remove(T value);

    size_t height() {return _height-1;}

    size_t size() {return _size;}

	size_t capacity() {return container.capacity();}

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
    
	void siftUp(const size_t pos, const size_t h)
    {
        siftUp(pos, h, std::pair{0,0});
    }

    void siftUp(
        const size_t pos, 
        const size_t h,
        const std::pair<size_t, size_t> levelSpan
    );

	void siftDown(const size_t startPos, const size_t pos, const size_t childHeight)
    {
        siftDown(startPos, pos, childHeight, _span);
    }

    void siftDown(
        const size_t startPos, 
        const size_t pos, 
        const size_t childHeight,
        const std::pair<size_t, size_t> levelSpan
    );

	inline const std::pair<size_t, size_t> span(size_t _height);

	//inline const std::pair<size_t, size_t> getParents(const size_t childHeight, const size_t child);
    inline const std::pair<size_t, size_t> getParents(
        const size_t childHeight, 
        const size_t childIndex,
        const std::pair<size_t, size_t> curr_child_Span
    );

	/*inline const std::pair<size_t, size_t> getChildren(
        const size_t parentheight, const size_t index
    );*/

    inline const std::pair<size_t, size_t> getChildren(
        const size_t parentHeight, const size_t index, 
        const std::pair<size_t, size_t> curr_parent_span
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
void Beap<T, Compare>::siftUp(
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
    T newItem = container[pos];
    auto currentSpan = levelSpan;

    while (true)
    {
        std::pair<size_t, size_t> children = getChildren(currentHeight, currentPos, currentSpan);
        size_t leftChild = children.first;
        size_t rightChild = children.second;
        
        // Find the smallest child
        size_t minChildPos = INVALID_INDEX;
        if (leftChild < _size) {
            minChildPos = leftChild;
            if (rightChild < _size && compare(container[leftChild], container[rightChild])) {
                minChildPos = rightChild;
            }
        }

        // If no children or children are larger, we're done
        if (minChildPos == INVALID_INDEX) {
            break;
        }

        // Move the smaller child up
        container[currentPos] = container[minChildPos];
        currentPos = minChildPos;
        currentHeight++;
        currentSpan = childSpan(currentSpan.second, currentHeight);
    }
    
    container[currentPos] = newItem;
    siftDown(pos, currentPos, currentHeight, currentSpan);
}

template <typename T, typename Compare>
void Beap<T, Compare>::siftDown(
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
    
    T newItem = container[pos];
    size_t currentPos = pos;
    size_t currentHeight = childHeight;
    auto currentSpan = levelSpan;
    
    while (currentPos > startPos)
    {
        std::pair<size_t, size_t> parents = getParents(currentHeight, currentPos, currentSpan);
        size_t maxParentPos = getMaxIndexIncontainer(parents.first, parents.second);
        if (maxParentPos == INVALID_INDEX || compare(newItem, container[maxParentPos])) {
            break;
        }

        container[currentPos] = container[maxParentPos];
        currentPos = maxParentPos;
        currentHeight--;
        currentSpan = parentSpan(currentSpan.first, currentHeight);
    }
    container[currentPos] = newItem;
}

template <typename T, typename Compare>
size_t Beap<T, Compare>::getMaxIndexIncontainer(size_t index1, size_t index2)
{
    if (index2 == INVALID_INDEX) return index1;
    if (index1 == INVALID_INDEX) return index2;
    int diff = index2 - index1;
    
    auto result = index1 + (diff * !compare(container[index1], container[index2]));
    
    return result;
}

template <typename T, typename Compare>
std::pair<size_t, size_t> Beap<T, Compare>::search(T value)
{
    if(_size <= 0 || compare(container[0], value))
    {
        return {INVALID_INDEX, INVALID_INDEX};
    }

    size_t h = _height;
    std::pair<size_t, size_t> currentLevel = _span;
    size_t end = currentLevel.first;
    
    if(value == container[end])
    {
        return {end, h};
    }

    size_t start = currentLevel.second;

    if(start >= _size)
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
            return {INVALID_INDEX, INVALID_INDEX};
        }

        if(compare(container[idx], value))
        {
            std::pair<size_t, size_t> parents = getParents(h, idx, currentLevel);
            if(parents.first == INVALID_INDEX)
            {
                return {INVALID_INDEX, INVALID_INDEX};
            }
            idx = parents.first;
            h--;
            currentLevel = parentSpan(currentLevel.first, h);
        }
        else if(compare(value,container[idx]))
        {
            auto children = getChildren(h, idx, currentLevel);
            if(children.first >= _size)
            {
                idx--;
            }
            else{
                idx = children.first;
                h++;
                currentLevel = childSpan(currentLevel.second, h);
            }
        }
        
        else{
            return {idx, h};
        }
    }
}

template <typename T, typename Compare>
T Beap<T, Compare>::pop()
{
    if(_size <= 0)
    {
        throw std::out_of_range("cannot pop empty beap");
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
    
    
    siftUp(0, 1);
    return minValue;
}

template <typename T, typename Compare>
void Beap<T, Compare>::push(T value)
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
    siftDown(0, _size - 1, _height);
} 

template <typename T, typename Compare>
void Beap<T, Compare>::remove(T value)
{
    auto result = search(value);
    
    if(result.first == INVALID_INDEX)
    {
        return;
    }

    auto prevElem = std::exchange(container[result.first], container[_size - 1]);

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

    // if the last element in the vector was removed, no need to siftUp
    if(result.first >= _size)
    {
        return;
    }

    if(compare(value, container[result.first]))
        siftDown(0, result.first, result.second, span(result.second));

    siftUp(result.first, result.second, span(result.second));
    
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
inline const std::pair<size_t, size_t> Beap<T, Compare>::getParents(
    const size_t childHeight, 
    const size_t childIndex,
    const std::pair<size_t, size_t> curr_child_span
    ) 
    {
    size_t parentHeight = childHeight - 1;
    auto parentLevel = parentSpan(curr_child_span.first, parentHeight);
    size_t numberOfElementInTheLevel = parentLevel.second - parentLevel.first + 1;

    if (childIndex == curr_child_span.first)
    {
        return {INVALID_INDEX ,  parentLevel.first};
    }

    if (childIndex == curr_child_span.second)
    {
        return {parentLevel.second, INVALID_INDEX};
    }

    auto secondParent = childIndex - numberOfElementInTheLevel;

    return { secondParent - 1, secondParent};
}

template <typename T, typename Compare>
inline const std::pair<size_t, size_t> Beap<T, Compare>::getChildren(const size_t parentHeight, const size_t index, 
    const std::pair<size_t, size_t> curr_parent_span)
{
    auto currentLevel = childSpan(curr_parent_span.second, parentHeight);
    int numberOfElementsInTheLevel = currentLevel.second - currentLevel.first + 1;

    return { index + numberOfElementsInTheLevel, index + numberOfElementsInTheLevel + 1 };
}




