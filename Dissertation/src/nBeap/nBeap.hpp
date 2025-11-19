#include <vector>
#include <concepts>

template<typename T>
concept Comparable = requires(T a, T b) {
	{ a < b } -> std::convertible_to<bool>;
	{ a > b } -> std::convertible_to<bool>;
	{ a == b } -> std::convertible_to<bool>;
};

template<Comparable Type, int N>
class nBeap
{
public:
	int height;
	int size;
	std::vector<Type> container;

	nBeap();
	~nBeap();
	Type remove_min();
	void push(Type val);
	void remove(Type val);
	bool search(Type val);


private:
	std::pair<int, int> _search(Type val);
	std::pair<int, int> span(int height);
	void siftDown(const int startPos, const int index, const int childHeight);
	void siftUp(const int index, const int h);
	std::pair<int, int> getChildren(int parenIndex, int parentHeight);
	std::pair<int, int> getParents(int childIndex, int childHeight);
	int getSmallestChildIndex(std::pair<int, int>& children);
	int getLargestParentIndex(std::pair<int, int>& parents);
};


