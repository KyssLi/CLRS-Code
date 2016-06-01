#include <cstdlib>
#include <algorithm>
#include <chrono>
#include <limits>
#include <iostream>
#include <iterator>
#include <vector>

///16.1:activity-select problem
struct ActTime
{
	ActTime() = default;
	ActTime(const int &s_, const int &f_)
		:s(s_), f(f_){}
	int s;
	int f;
};
std::vector<int> activitySelectGA(std::vector<ActTime> &input)
{
	std::vector<int> res;
	if (input.empty())
		return res;
	std::sort(input.begin(), input.end(), [](auto &l, auto &r)->bool {return l.f == r.f ? l.s > r.s : l.f > r.f; });
	input.insert(input.begin(), ActTime(std::numeric_limits<int>::max(), std::numeric_limits<int>::max()));
	size_t pos = 0;
	for (size_t i = 1; i != input.size(); ++i)
	{
		while (input[i].f >= input[pos].s)
			++i;
		res.push_back(i);
		pos = i;
	}
	return res;
}

int main()
{
	auto start_time = std::chrono::steady_clock::now();

	std::vector<ActTime> input{ { 3,5 },{ 1,4 },{ 5,7 },{ 0,6 },{ 12,16 },{ 8,12 },{ 8,11 },{ 2,14 },{ 5,9 },{ 3,9 },{ 6,10 } };
	auto res = activitySelectGA(input);
	std::copy(res.cbegin(), res.cend(), std::ostream_iterator<int>{std::cout, " "});

	std::cout << "\nThe program run "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).count()
		<< " millisecond" << std::endl;
	system("pause");
	return 0;
}