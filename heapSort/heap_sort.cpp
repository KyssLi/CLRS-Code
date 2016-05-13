#include <cstdlib>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <iterator>
#include <functional>
#include <vector>

template<typename T>
struct Ascend : public std::binary_function<T, T, bool>
{
	bool operator()(const T &left, const T &right) { return left <= right; }
};
template<typename T>
struct Descend : public std::binary_function<T, T, bool>
{
	bool operator()(const T &left, const T &right) { return left >= right; }
};
template<typename T, typename Pred>
void heapAdjust(std::vector<T> &arr, size_t heap_size, size_t index, Pred compare)
{
	while (index < heap_size)
	{
		size_t change_index = index;
		size_t left_child_index = (index << 1) + 1;
		size_t right_child_index = left_child_index + 1;
		if (left_child_index < heap_size && compare(arr[left_child_index], arr[index]))
			change_index = left_child_index;
		if (right_child_index < heap_size && compare(arr[right_child_index], arr[change_index]))
			change_index = right_child_index;
		if (change_index != index)
		{
			T temp_val = arr[index];
			arr[index] = arr[change_index];
			arr[change_index] = temp_val;
			index = change_index;
		}
		else
			break;
	}
}
template<typename T, typename Pred>
void heapBuild(std::vector<T> &arr, Pred compare)
{
	for (int i = static_cast<int>(arr.size() / 2 - 1); i >= 0; --i)
		heapAdjust(arr, arr.size(), i, compare);
}
template<typename T, typename Pred>
void heapSort(std::vector<T> &arr, Pred compare)
{
	heapBuild(arr, compare);
	size_t cnt = arr.size() - 1;
	for (; cnt >= 1; --cnt)
	{
		T temp_val = arr[cnt];
		arr[cnt] = arr[0];
		arr[0] = temp_val;
		heapAdjust(arr, cnt, 0, compare);
	}
}

template<typename T, typename Pred>
T medianWithWeight(std::vector<T> &arr, Pred compare)
{
	heapSort(arr, compare);
	T sum = 0;
	for (auto it = arr.cbegin(); it != arr.cend(); ++it)
	{
		sum += *it;
		if (sum >= static_cast<T>(0.5))
			return *it;
	}
	return 0;
}

int main()
{
	auto start_time = std::chrono::steady_clock::now();

	std::vector<float> arr{ 0.1f, 0.25f, 0.2f, 0.1f, 0.1f, 0.25f };
	std::copy(arr.cbegin(), arr.cend(), std::ostream_iterator<float>{std::cout, " "}); std::cout << std::endl;
	auto res = medianWithWeight(arr, Descend<float>());
	std::copy(arr.cbegin(), arr.cend(), std::ostream_iterator<float>{std::cout, " "}); std::cout << std::endl;
	std::cout << res << std::endl;

	//std::vector<int> arr{ 27, 17, 3, 16, 13, 10, 1, 5, 7, 12, 4, 8, 9, 0 };
	//std::copy(arr.cbegin(), arr.cend(), std::ostream_iterator<int>{std::cout, " "});std::cout << std::endl;
	//heapSort(arr, Ascend<int>());
	//std::copy(arr.cbegin(), arr.cend(), std::ostream_iterator<int>{std::cout, " "});std::cout << std::endl;

	std::cout << "\nThe program run "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).count()
		<< " millisecond" << std::endl;
	system("pause");
	return 0;
}