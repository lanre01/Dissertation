#include "nBeap.hpp"

template<Comparable Type, int N>
nBeap<Type, N>::nBeap()
{
}

template<Comparable Type, int N>
nBeap<Type, N>::~nBeap()
{
}

template<Comparable Type, int N>
Type nBeap<Type, N>::pop()
{
	return Type();
}

template<Comparable Type, int N>
void nBeap<Type, N>::push(Type val)
{
}

template<Comparable Type, int N>
void nBeap<Type, N>::remove(Type val)
{
}

template<Comparable Type, int N>
bool nBeap<Type, N>::search(Type val)
{
	return false;
}

template<Comparable Type, int N>
std::pair<int, int> nBeap<Type, N>::span(int height)
{
	return std::pair<int, int>();
}

template<Comparable Type, int N>
void nBeap<Type, N>::siftDown(int index, int height)
{
}

template<Comparable Type, int N>
int nBeap<Type, N>::siftUp(int index, int height)
{
	return 0;
}
