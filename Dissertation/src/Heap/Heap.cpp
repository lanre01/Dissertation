
#include <stdexcept>
#include "Heap.hpp"


int Heap::pop()
{
	if (this->container.size() <= 0)
		throw std::out_of_range("Heap is empty");
	
	int minElement = container[1];
	container[1] = container[size_];
	size_--;
	container.resize(size_);

	siftUp(0); // sifting up brings the smallest item to that position.

	return minElement;
}

void Heap::push(int x) {

	container.push_back(x);
	size_++;
	siftDown(0, size_);
}

int Heap::findValue(int value, int index)
{
	if (container.size() < index || container[index] > value)
		return -1;

	if (container[index] == value)
		return index;


	int childIndex = 2*index + 1, valueIndex = -1;

	if (childIndex != -1)
	{
		valueIndex = std::max(findValue(value, childIndex), findValue(value, childIndex + 1));
	}

	return valueIndex;
}

void Heap::siftUp(int pos)
{
	int endPos = size_;
	int startPos = pos;
	int newItem = container[pos];

	int childPos = 2 * pos + 1;
	while (childPos < endPos)
	{
		int rightPos = childPos + 1;
		if ((rightPos < endPos) && !(container[childPos] < container[rightPos]))
		{
			childPos = rightPos;
		}

		container[pos] = container[childPos];
		pos = childPos;
		childPos = 2 * pos + 1;
	}

	container[pos] = newItem;
	siftDown(startPos, pos);
}

void Heap::siftDown(int startPos, int pos)
{
	int newItem = container[pos];

	while (pos > startPos)
	{
		int parentPos = (pos - 1) >> 1;
		int parent = container[parentPos];
		if (newItem < parent)
		{
			container[pos] = parent;
			pos = parentPos;
			continue;
		}
		break;
	}

	container[pos] = newItem;
}

int Heap::find(int value)
{
	for (int i = 1; i <= size_; i++)
	{
		if (container[i] < value)
			return -1;

		if (container[i] == value)
			return i;
	}
	return -1;
}

void Heap::remove(int value)
{
	int index = find(value);
	if (index == -1)
		return;

	std::swap(container[index], container[size_]);
	size_--;
	container.resize(size_);
	siftUp(index);
}



