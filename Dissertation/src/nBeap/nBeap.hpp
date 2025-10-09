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
	nBeap();
	~nBeap();
	Type pop();
	void push(Type val);
	void remove(Type val);
	bool search(Type val);


private:
	int height;
	std::vector<Type> container;


	std::pair<int, int> span(int height);
	void siftDown(int index, int height);
	int siftUp(int index, int height);
};


