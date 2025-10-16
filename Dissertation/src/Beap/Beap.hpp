#include <vector>
#include <array>


class Beap
{
public:
	std::vector<int> container;
	unsigned int size;
	unsigned int height;

	Beap() {
		size = 0;
		height = 0;
	};

	~Beap() {};

	void siftUp(int startPos, int height);

	void siftDown(int startPos, int endPos, int startHeight);

	int getMinIndexIncontainer(int index1, int index2);

	std::pair<int, int> search(int value);

	int pop();

	void push(int value);

	void remove(int value);

	std::pair<int, int> span(int height);

	std::pair<int, int> getParents(int childHeight, int child);

	std::pair<int, int> getChildren(int parentheight, int index);

};


