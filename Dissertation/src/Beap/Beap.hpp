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




