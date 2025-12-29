#include <utility>
#include <functional>
#include <vector>
#include <array>
#include <iostream>

constexpr size_t INVALID_INDEX = static_cast<size_t>(-1);

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
        //std::pair<size_t, size_t> children = getChildren(currentHeight, currentPos);
        std::pair<size_t, size_t> children = getChildren(currentHeight, currentPos, currentSpan);
        size_t leftChild = children.first;
        size_t rightChild = children.second;
        //std::cout << "leftChild is " << container[leftChild] << "and rightChild is " << container[rightChild] << std::endl;
        // Find the smallest child
        size_t minChildPos = INVALID_INDEX;
        if (leftChild < _size) {
            minChildPos = leftChild;
            if (rightChild < _size && compare(container[leftChild], container[rightChild])) {
                minChildPos = rightChild;
            }
        }

        //std::cout << "min child is " << container[minChildPos] << std::endl;

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
    //std::cout << "Current Span is: " << currentSpan.first << "," << currentSpan.second << std::endl;
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
    // siftDown(0, _size-1, height);
    //std::cout << "pushing " << container[pos] << " from " << pos << " to " << startPos << std::endl;
    T newItem = container[pos];
    size_t currentPos = pos;
    size_t currentHeight = childHeight;
    auto currentSpan = levelSpan;
    
    while (currentPos > startPos)
    {
        //std::pair<size_t, size_t> parents = getParents(currentHeight, currentPos);
        std::pair<size_t, size_t> parents = getParents(currentHeight, currentPos, currentSpan);
        //std::cout << parents.first << ',' << parents.second << " at height " << childHeight << std::endl;
        size_t maxParentPos = getMaxIndexIncontainer(parents.first, parents.second);
        if (maxParentPos == INVALID_INDEX || compare(newItem, container[maxParentPos])) {
            break;
        }

        container[currentPos] = container[maxParentPos];
        currentPos = maxParentPos;
        currentHeight--;
        currentSpan = parentSpan(currentSpan.first, currentHeight);
    }
    //std::cout << newItem << std::endl;
    container[currentPos] = newItem;
    //printState("SiftDown");
}

template <typename T, typename Compare>
size_t Beap<T, Compare>::getMaxIndexIncontainer(size_t index1, size_t index2)
{
    if (index2 == INVALID_INDEX) return index1;
    if (index1 == INVALID_INDEX) return index2;
    int diff = index2 - index1;
    
    auto result = index1 + (diff * !compare(container[index1], container[index2]));
    //std::cout << "Comparing " << index1 << "," << index2 << "with difference " <<
        //diff << "and compare result " << !!compare(index1, index2) << " final result " << result << std::endl;
    return result;
    //return compare(container[index1], container[index2]) ? index1 : index2;
}

template <typename T, typename Compare>
std::pair<size_t, size_t> Beap<T, Compare>::search(T value)
{
    if(_size <= 0 || compare(container[0], value))
    {
        return {INVALID_INDEX, INVALID_INDEX};
    }

    size_t h = _height;
    //std::pair<size_t, size_t> lastLevel = span(h);
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
        //start = span(h).second;
    }

    size_t idx = start;
    
    while(true)
    {
        
        if(idx == end)
        {
            return {INVALID_INDEX, INVALID_INDEX};
        }

        //std::cout << "Idx: " << idx << " Value: " << container[idx] << "Height: " << h << std::endl;
        //std::cout << "Child level 1: " << currentLevel.first << " Child level 2: " << currentLevel.second << std::endl;
        
        if(compare(container[idx], value))
        {
            //std::pair<size_t, size_t> parents = getParents(h, idx);
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
            //auto children = getChildren(h, idx);
            auto children = getChildren(h, idx, currentLevel);
            //std::pair<int, int> children()
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

    //std::pair<size_t, size_t> currentLevel = Beap::span(_height);
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
    //std::pair<size_t, size_t> currentLevel = span(_height);
    //std::cout << "in push " <<  currentLevel.first << "," << currentLevel.second << "," << _height << std::endl;
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
    // value does not exist in the beap
    if(result.first == INVALID_INDEX)
    {
        /*std::cout << "Search failed for " << value 
                  << " | size=" << _size 
                  << " height=" << _height
                  << " span=(" << _span.first << "," << _span.second << ")\n";*/
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
        /*std::cout << "Searching for " << value
                  << " found at " << result.first  
                  << " | size=" << _size 
                  << " height=" << _height
                  << " span=(" << _span.first << "," << _span.second << ")\n";*/

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
    /*
    0 - 0*(0-1) / 2 + 1 = 1. 0*(0+1) / 2 = 0 - (1,0) - (0, -1)
    1 - 1*(1-1)/2 + 1 = 1, 1*(1+1)/2 = 1 = (1,1) - (0, 0)
    2 - 2*(2-1)/2 + 1 = 2, 2*(2+1)/2 = 3 = (2,3) - (1, 2)
    */
}

/*template <typename T, typename Compare>
inline const std::pair<size_t, size_t> Beap<T, Compare>::getParents(
    const size_t childHeight, const size_t childIndex) {
    
    size_t parentHeight = childHeight - 1;
    std::pair<size_t, size_t> parentLevel = Beap::span(parentHeight);
    std::pair<size_t, size_t> childLevel(parentLevel.second + 1, parentLevel.second + childHeight);
    //std::pair<int, int> childLevel = Beap::span(childHeight);
    size_t numberOfElementInTheLevel = parentLevel.second - parentLevel.first + 1;

    if (childIndex == childLevel.first)
    {
        return {INVALID_INDEX ,  parentLevel.first};
    }

    if (childIndex == childLevel.second)
    {
        return {parentLevel.second, INVALID_INDEX};
    }

    int secondParent = childIndex - numberOfElementInTheLevel;

    return { secondParent - 1, secondParent};
}*/

template <typename T, typename Compare>
inline const std::pair<size_t, size_t> Beap<T, Compare>::getParents(
    const size_t childHeight, 
    const size_t childIndex,
    const std::pair<size_t, size_t> curr_child_span
    ) 
    {
    size_t parentHeight = childHeight - 1;
    auto parentLevel = parentSpan(curr_child_span.first, parentHeight);
    //std::pair<size_t, size_t> parentLevel = Beap::span(parentHeight);
    //std::pair<size_t, size_t> childLevel(parentLevel.second + 1, parentLevel.second + childHeight);
    //std::pair<int, int> childLevel = Beap::span(childHeight);
    size_t numberOfElementInTheLevel = parentLevel.second - parentLevel.first + 1;

    if (childIndex == curr_child_span.first)
    {
        return {INVALID_INDEX ,  parentLevel.first};
    }

    if (childIndex == curr_child_span.second)
    {
        return {parentLevel.second, INVALID_INDEX};
    }

    int secondParent = childIndex - numberOfElementInTheLevel;

    return { secondParent - 1, secondParent};
}

/*template <typename T, typename Compare>
inline const std::pair<size_t, size_t> Beap<T, Compare>::getChildren(const size_t parentHeight, const size_t index)
{
    std::pair<size_t, size_t> currentLevel = Beap::span(parentHeight);
    int numberOfElementsInTheLevel = currentLevel.second - currentLevel.first + 1;

    return { index + numberOfElementsInTheLevel, index + numberOfElementsInTheLevel + 1 };
}*/

template <typename T, typename Compare>
inline const std::pair<size_t, size_t> Beap<T, Compare>::getChildren(const size_t parentHeight, const size_t index, 
    const std::pair<size_t, size_t> curr_parent_span)
{
    //std::pair<size_t, size_t> currentLevel = Beap::span(parentHeight);
    auto currentLevel = childSpan(curr_parent_span.second, parentHeight);
    //std::cout << "GetChildren: " << currentLevel.first << " , " << currentLevel.second << std::endl;
    int numberOfElementsInTheLevel = currentLevel.second - currentLevel.first + 1;

    return { index + numberOfElementsInTheLevel, index + numberOfElementsInTheLevel + 1 };
}




