#include <cstdlib>
#include <algorithm>
#include <chrono>
#include <initializer_list>
#include <iostream>
#include <iterator>

template<typename T>
class Heap
{
	template<typename Ty> friend void selectSort(Heap<Ty> &heap);
public:
	Heap<T>() = default;
	Heap<T>(std::initializer_list<T> il)
		:arr(new T[il.size()]), h_size(il.size()), len(il.size())
	{
		int i = 0;
		for (auto it = il.begin(); it != il.end(); ++it)
			arr[i++] = *it;
	}
	~Heap<T>() { delete[] arr; arr = nullptr; }

	void printHeap() const 
	{ 
		std::copy(arr, arr + h_size, std::ostream_iterator<T>(std::cout, " "));
		std::cout << std::endl;
	}
	size_t heapSize() const
	{
		return h_size; 
	}
	size_t length() const 
	{ 
		return len; 
	}
	void maxHeapify(const size_t &i)
	{
		size_t l = (i << 1) + 1;
		size_t r = l + 1;
		size_t largest = i;
		if (l < h_size && arr[l] > arr[i])
			largest = l;
		if (r < h_size && arr[r] > arr[largest])
			largest = r;
		if (largest != i)
		{
			T temp = arr[largest];
			arr[largest] = arr[i];
			arr[i] = temp;
			maxHeapify(largest);
		}
	}
	void buildMaxHeap()
	{
		for (int i = static_cast<int>(h_size / 2 - 1); i >= 0; --i)
			maxHeapify(i);
	}
	void heapSort()
	{
		buildMaxHeap();
		for (size_t i = len - 1; i > 0; --i)
		{
			T temp = arr[--h_size];
			arr[h_size] = arr[0];
			arr[0] = temp;
			maxHeapify(0);
		}
		h_size = len;
	}

private:
	T *arr;
	size_t h_size;
	size_t len;
};

template<typename Ty>
void selectSort(Heap<Ty> &heap)
{
	for (int i = static_cast<int>(heap.h_size - 1); i > 0; --i)
	{
		int max = i;
		for (int j = i - 1; j >= 0; --j)
		{
			if (heap.arr[j] > heap.arr[max])
				max = j;
		}
		Ty temp = heap.arr[max];
		heap.arr[max] = heap.arr[i];
		heap.arr[i] = temp;
	}
}

int main()
{
	auto start_time = std::chrono::steady_clock::now();

	Heap<int> tree{ 1,2,3,4,5,6,7,8,9,10 };
	tree.printHeap();
	tree.buildMaxHeap();
	tree.printHeap();

	//Heap<int> tree{ 4,1,3,2,16,9,10,14,8,7 };
	//tree.printHeap();
	//tree.heapSort();
	////selectSort(tree);
	//tree.printHeap();

	//Heap<int> maxTree{ 27, 17, 3, 16, 13, 10, 1, 5, 7, 12, 4, 8, 9, 0 };
	//maxTree.printHeap();
	//maxTree.maxHeapify(2);
	//maxTree.printHeap();


	std::cout << "\nThe program run "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).count()
		<< " millisecond" << std::endl;
	system("pause");
	return 0;
}