#include <vector>


class Heap {
public:
	Heap(const std::vector<int>& vector) 
		: size_(0)
	{
		insertAll(vector);
	}
	Heap() : size_(0) { }

	void push(int x);
	int pop();
	int size() const { 
		return size_;
	}
	void remove(int value);
	





protected:
	std::vector<int> container;

private: 

	void insertAll(const std::vector<int>& vector) {
		for (int num : vector)
		{
			this->push(num);
		}
	}

	int size_;
	int findValue(int value, int index);
	void siftUp(int pos);
	void siftDown(int startPos, int pos);

	int find(int value);

};

