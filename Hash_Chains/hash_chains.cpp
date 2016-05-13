#include <cstdlib>
#include <algorithm>
#include <chrono>
#include <list>
#include <iostream>
#include <memory>
#include <vector>

template<typename T>
void hashInsert_Chains(std::unique_ptr<std::unique_ptr<std::list<T>>[]> &hashtable, const int &M, const T &val)
{
	int index = val % M;
	if (hashtable[index] == nullptr)
		hashtable[index].reset(new std::list<T>);
	hashtable[index]->push_back(val);
}

template<typename T>
void hashDelete_Chains(std::unique_ptr<std::unique_ptr<std::list<T>>[]> &hashtable, const int &M, const T &val)
{
	int index = val % M;
	if (hashtable[index] == nullptr)
		return;
	hashtable[index]->erase(std::find(hashtable[index]->cbegin(), hashtable[index]->cend(),val));
}

template<typename T>
bool hashFind_Chains(std::unique_ptr<std::unique_ptr<std::list<T>>[]> &hashtable, const int &M, const T &val)
{
	int index = val % M;
	if (hashtable[index] == nullptr)
		return false;
	for (auto it = hashtable[index]->cbegin(); it != hashtable[index]->cend(); ++it)
	{
		if (*it == val)
			return true;
	}
	return false;
}

template<typename T>
void hashInitialization_Chains(std::unique_ptr<std::unique_ptr<std::list<T>>[]> &hashtable, const int &M, const std::vector<T> &vec)
{
	for (auto it = vec.cbegin(); it != vec.cend(); ++it)
		hashInsert_Chains<T>(hashtable, M, *it);
}

template<typename T>
void printHashTable_Chains(std::unique_ptr<std::unique_ptr<std::list<T>>[]> &hashtable, const int &M)
{
	std::cout << "HashTable:" << std::endl;
	for (int i = 0; i != M; ++i)
	{
		std::cout << i << ":\t";
		if (hashtable[i] == nullptr)
		{
			std::cout << "NIL" << std::endl;
			continue;
		}
		for (auto it = hashtable[i]->cbegin(); it != hashtable[i]->cend(); ++it)
			std::cout << *it << "-->";
		std::cout << "NIL" << std::endl;
	}
}

int main()
{
	auto start_time = std::chrono::steady_clock::now();

	std::unique_ptr<std::unique_ptr<std::list<int>>[]> ptr(new std::unique_ptr<std::list<int>>[10]);
	for (int i = 0; i != 10; ++i)
		ptr[i] = nullptr;
	printHashTable_Chains<int>(ptr, 10);
	hashInitialization_Chains<int>(ptr, 10, { 2, 12, 3, 56, 3, 24, 1, 0, 2, 10, 8 });
	printHashTable_Chains<int>(ptr, 10);
	std::cout << std::boolalpha << hashFind_Chains<int>(ptr, 10, 33);
	std::cout << std::noboolalpha << std::endl;
	hashDelete_Chains<int>(ptr, 10, 2);
	printHashTable_Chains<int>(ptr, 10);
	
	std::cout << "\nThe program run "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).count()
		<< " millisecond" << std::endl;
	system("pause");
	return 0;
}