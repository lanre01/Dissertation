#include <vector>
#include <array>
#include <iostream>


class Beap
{
public:
	std::vector<int> container;
	int size;
	int height;

	Beap() {
		size = 0;
		height = 0;
	};

	~Beap() {};

	

	int getMaxIndexIncontainer(int index1, int index2);

	std::pair<int, int> search(int value);

	int pop();

	void push(int value);

	void remove(int value);
	
	void printState(const std::string& operation) {
    	std::cout << "After " << operation << ": ";
		for (int val : container) {
			std::cout << val << " ";
		}
		std::cout << " | size=" << size << " height=" << height << std::endl;
	}

private:

	void siftUp(int startPos, int height);

	void siftDown(int startPos, int endPos, int startHeight);

	std::pair<int, int> span(int height);

	std::pair<int, int> getParents(const int childHeight, const int child);

	std::pair<int, int> getChildren(const int parentheight, const int index);

};


