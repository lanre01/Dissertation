#include <vector>
#include <array>
#include <iostream>

constexpr size_t INVALID_INDEX = static_cast<size_t>(-1);

template <typename T, typename Compare = std::greater<T>>
class Beap
{
public:
	std::vector<int> container;
	size_t size;
	size_t height;
	Compare compare;
	
	
	Beap(): size(0), height(0), compare(Compare()){};

	Beap(Compare c): size(0), height(0), compare(c){};

	~Beap() {};
	Beap& operator=(const Beap&) = default;

	

	size_t getMaxIndexIncontainer(size_t index1, size_t index2);

	std::pair<size_t, size_t> search(T value);

	T pop();

	void push(T value);

	void remove(T value);

	size_t capacity() {return container.capacity();}

	bool empty(){return container.empty();}

	void reserve(size_t capacity) {container.reserve(capacity);};
	
	void printState(const std::string& operation) {
    	std::cout << "After " << operation << ": ";
		for (T val : container) {
			std::cout << val << " ";
		}
		std::cout << " | size=" << size << " height=" << height << std::endl;
		std::cout.flush();
	}

private:

	void siftUp(const size_t pos, const size_t h);

	void siftDown(const size_t startPos, const size_t pos, const size_t childHeight) ;

	inline const std::pair<size_t, size_t> span(size_t height);

	inline const std::pair<size_t, size_t> getParents(const size_t childHeight, const size_t child);

	inline const std::pair<size_t, size_t> getChildren(const size_t parentheight, const size_t index);

};



// cpp code

template <typename T, typename Compare>
void Beap<T, Compare>::siftUp(const size_t pos, const size_t h)
{
    if (empty())
    {
        return;
    }

    size_t currentPos = pos;
    size_t currentHeight = h;
    T newItem = container[pos];

    while (true)
    {
        std::pair<int, int> children = getChildren(currentHeight, currentPos);
        int leftChild = children.first;
        int rightChild = children.second;
        //std::cout << "leftChild is " << container[leftChild] << "and rightChild is " << container[rightChild] << std::endl;
        // Find the smallest child
        size_t minChildPos = INVALID_INDEX;
        if (leftChild < size) {
            minChildPos = leftChild;
            if (rightChild < size && compare(container[leftChild], container[rightChild])) {
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
    }

    container[currentPos] = newItem;
    siftDown(pos, currentPos, currentHeight);
}

template <typename T, typename Compare>
void Beap<T, Compare>::siftDown(const size_t startPos, const size_t pos, const size_t childHeight) 
{
    if (container.empty() || size == 1)
    {
        return;
    }
    // siftDown(0, size-1, height);
    //std::cout << "pushing " << container[pos] << " from " << pos << " to " << startPos << std::endl;
    T newItem = container[pos];
    size_t currentPos = pos;
    size_t currentHeight = childHeight;

    while (currentPos > startPos)
    {
        std::pair<size_t, size_t> parents = getParents(currentHeight, currentPos);
        //std::cout << parents.first << ',' << parents.second << " at height " << childHeight << std::endl;
        size_t maxParentPos = getMaxIndexIncontainer(parents.first, parents.second);
        if (maxParentPos == INVALID_INDEX || compare(newItem, container[maxParentPos])) {
            break;
        }

        container[currentPos] = container[maxParentPos];
        currentPos = maxParentPos;
        currentHeight--;
    }
    //std::cout << newItem << std::endl;
    container[currentPos] = newItem;
}

template <typename T, typename Compare>
size_t Beap<T, Compare>::getMaxIndexIncontainer(size_t index1, size_t index2)
{
    if (index2 == INVALID_INDEX) return index1;
    if (index1 == INVALID_INDEX) return index2;

    return compare(container[index1], container[index2]) ? index1 : index2;
}

template <typename T, typename Compare>
std::pair<size_t, size_t> Beap<T, Compare>::search(T value)
{
    if(size <= 0 || compare(container[0], value))
    {
        return {INVALID_INDEX, INVALID_INDEX};
    }

    size_t h = height;
    std::pair<size_t, size_t> lastLevel = span(h);
    size_t end = lastLevel.first;
    
    if(value == container[end])
    {
        return {end, h};
    }
    size_t start = lastLevel.second;

    if(start >= size)
    {
        start = start - h;
        h--;
        //start = span(h).second;
    }

    size_t idx = start;
    
    while(true)
    {
        /*std::cout 
            << "\n[SEARCH STEP]"
            << " idx=" << idx
            << " h=" << h
            << " value=" << value
            << " current=" << (idx >= 0 && idx < size ? std::to_string(container[idx]) : "OUT_OF_RANGE")
            << " | level_span=" << span(h).first << "-" << span(h).second
            << " | height=" << height
            << " | size=" << size
            << std::endl;*/
        
        #ifdef DEBUG
        auto row = span(h);
        if (idx < row.first || idx > row.second || idx < 0 || idx >= size) {
            std::cerr << "[INVARIANT BROKEN] idx=" << idx
                    << " h=" << h
                    << " span=" << row.first << "-" << row.second
                    << " size=" << size << std::endl;
            throw std::logic_error("search left beap geometry");
        }
        #endif

        if(idx == end)
        {
            return {INVALID_INDEX, INVALID_INDEX};
        }

        if(value < container[idx])
        {
            std::pair<size_t, size_t> parents = getParents(h, idx);
            if(parents.first == INVALID_INDEX)
            {
                return {INVALID_INDEX, INVALID_INDEX};
            }
            idx = parents.first;
            h--;
        }
        else if(value > container[idx])
        {
            auto children = getChildren(h, idx);
            //std::pair<int, int> children()
            if(children.first >= size)
            {
                idx--;
            }
            else{
                idx = children.first;
                h++;
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
    if(size <= 0)
    {
        throw std::out_of_range("cannot pop empty beap");
    }
    T minValue = container[0];
    std::swap(container[0], container[size-1]);
    size--;
	container.resize(size);
	if(size <= 0)
	{
		height = 0;
		return minValue;
	}
    std::pair<size_t, size_t> currentLevel = Beap::span(height);
    if (size-1 < currentLevel.first)
    {
        height--;
    }
    
    
    siftUp(0, 1);
    return minValue;
}

template <typename T, typename Compare>
void Beap<T, Compare>::push(T value)
{
    size++;
    std::pair<size_t, size_t> currentLevel = span(height);
    //std::cout << "in push " <<  currentLevel.first << "," << currentLevel.second << "," << height << std::endl;
    if (currentLevel.second == INVALID_INDEX || size - 1 > currentLevel.second)
    {
        height++;
    }
    container.push_back(value);
    siftDown(0, size - 1, height);
} 

template <typename T, typename Compare>
void Beap<T, Compare>::remove(T value)
{
    std::pair<size_t, size_t> indexAndHeight = search(value);
    size_t index = indexAndHeight.first;

    if (index == INVALID_INDEX) {
        return;
    }

    std::swap(container[index], container[size-1]);
    size--;
    std::pair<int,int> lastLevel = span(height);
    if (lastLevel.first >= size)
    {
        height--;
    }
    container.resize(size);
    if (index+1 == size)
    {
        // removing the last element in the vector.
        return;
    }
    siftUp(index, indexAndHeight.second);
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

template <typename T, typename Compare>
inline const std::pair<size_t, size_t> Beap<T, Compare>::getParents(const size_t childHeight, const size_t childIndex) {
    
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
}

template <typename T, typename Compare>
inline const std::pair<size_t, size_t> Beap<T, Compare>::getChildren(size_t parentHeight, size_t index)
{
    std::pair<size_t, size_t> currentLevel = Beap::span(parentHeight);
    int numberOfElementsInTheLevel = currentLevel.second - currentLevel.first + 1;

    return { index + numberOfElementsInTheLevel, index + numberOfElementsInTheLevel + 1 };
}


