#pragma once
#include <vector>
#include <iostream>
#include "Node.H"

using namespace std;

//Indexed Minimum Priority Queue adapted from Sedgewick and Simon

template<class T>
class IndexMinPQ
{
public:
	int size;
	int min_size;
	IndexMinPQ();

	void add(T& t);
	void update(T t);
	T remove();
	void clear_space(float percent);

	void swap_indeces(int i, int j);
	bool empty();
	vector<T> pq;

protected:
	void swim(int k);
	void sink(int k);
};

//For some reason, you cannot use cpp files and generic types at the same time or you get unresolved external errors
//The only way I found to fix that was to define everything in the header file :P

template<class T>
IndexMinPQ<T>::IndexMinPQ()
{
	T t = *new T();
	t.set_index(0);
	pq.push_back(t);
	min_size = 1500000;
}

template<class T>
inline void IndexMinPQ<T>::add(T& t)
{
	size++;
	pq.emplace_back(t);
	swim(size);
}

template<class T>
inline void IndexMinPQ<T>::update(T t)
{
	swim(t.get_index());
}

template<class T>
inline T IndexMinPQ<T>::remove()
{
	if (empty())
		throw - 1;

	T min = pq[1];
	swap_indeces(1, size);
	size--;

	sink(1);
	pq.pop_back();
	min.set_index(0);
	return min;
}

template<class T>
inline void IndexMinPQ<T>::clear_space(float percent)
{
	int amount = (int)(percent * size);
	if (size - amount < min_size)
		amount = size - min_size;
	if (amount <= 0)
		return;

	for (int i = 0; i < amount; i++)
		pq.pop_back();
	size -= amount;
}

template<class T>
inline void IndexMinPQ<T>::swap_indeces(int i, int j)
{
	swap(pq[i], pq[j]);
	pq[i].set_index(pq[j].get_index());
	pq[j].set_index(pq[i].get_index());
}

template<class T>
inline bool IndexMinPQ<T>::empty()
{
	return size == 0;
}

template<class T>
inline void IndexMinPQ<T>::swim(int k)
{
	T t = pq[k];
	while (k > 1 && pq[k / 2] > t) {
		pq[k] = pq[k / 2];
		//pq[k / 2].set_index(k);
		pq[k].set_index(k);
		k = k / 2;
	}
	pq[k] = t;
	pq[k].set_index(k);
}

template<class T>
inline void IndexMinPQ<T>::sink(int k)
{
	T t = pq[k];
	while (2 * k <= size) {
		int j = 2 * k;
		if (j < size && pq[j] > pq[j + 1]) j++; //get greater of two child nodes
		if (t < pq[j] || t == pq[j]) break;   //break if less than child
		pq[k] = pq[j];
		//pq[j].set_index(k);
		pq[k].set_index(k);
		k = j;
	}
	pq[k] = t;
	pq[k].set_index(k);
}