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
	Beap& operator=(Beap const&);

	

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
		std::cout.flush();
	}

private:

	void siftUp(const int pos, const int h);

	void siftDown(const int startPos, const int pos, const int childHeight) ;

	inline std::pair<int, int> span(int height);

	std::pair<int, int> getParents(const int childHeight, const int child);

	std::pair<int, int> getChildren(const int parentheight, const int index);

};


