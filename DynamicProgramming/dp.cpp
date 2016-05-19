#include <cstdlib>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <iterator>
#include <limits>
#include <memory>
#include <tuple>
#include <utility>
#include <vector>

template<typename T> inline
T Max(const T &left,
	const T &right)
{
	return left > right ? left : right;
}

///cutting of iron rod
std::vector<int> length{ 1,2,3,4,5,6,7,8,9,10 };
std::vector<int> price{ 1,5,8,9,10,17,17,20,24,30 };

int cutRod(const int &len)///only can deal with len of 1~length.size()
{
	if (len <= 0)
		return 0;
	int res = 0;
	for (int i = 0; i != len; ++i)
		res = Max(res, price[i] + cutRod(len - length[i]));
	return res;
}
int cutRodExt(const int &len)///can deal with len more than length.size()
{
	int res = 0;
	if (len >= static_cast<int>(length.size()))
		res += len / length.size() * cutRod(length.size());
	return res += cutRod(len % length.size());
}

int memoizedCutRodAux(std::unique_ptr<int[]> &r,
	const int &len)
{
	if (len <= 0)
		return 0;
	if (r[len - 1] > 0)
		return r[len - 1];
	int res = 0;
	for (int i = 0; i != len; ++i)
		res = Max(res, price[i] + memoizedCutRodAux(r, len - length[i]));
	r[len - 1] = res;
	return res;
}
int memoizedCutRod(const int &len)///use memoization to calculate
{
	if (len <= 0)
		return 0;
	std::unique_ptr<int[]> r = std::make_unique<int[]>(len);
	for (int i = 0; i != len; ++i)
		r[i] = 0;
	return memoizedCutRodAux(r, len);
}
int memoizedCutRodExt(const int &len)
{
	if (len <= 0)
		return 0;
	int res = 0;
	std::unique_ptr<int[]> r = std::make_unique<int[]>(length.size());///limit the size of dynamic array r
	for (int i = 0; i != length.size(); ++i)
		r[i] = 0;
	if (len >= static_cast<int>(length.size()))
		res += len / length.size() * memoizedCutRodAux(r, length.size());
	return res += memoizedCutRodAux(r, len % length.size());
}

int bottomUpCutRod(const int &len)
{
	if (len <= 0)
		return 0;
	std::unique_ptr<int[]> r = std::make_unique<int[]>(len + 1);
	r[0] = 0;
	for (int i = 1; i != len + 1; ++i)
	{
		int res = 0;
		for (int j = 1; j != i + 1; ++j)
			res = Max(res, price[j - 1] + r[i - j]);
		r[i] = res;
	}
	return r[len];
}
int bottomUpCutRodExt(const int &len)
{
	int res = 0;
	if (len >= static_cast<int>(length.size()))
		res += len / length.size() * bottomUpCutRod(length.size());
	return res += bottomUpCutRod(len % length.size());
}

///matrix-chain multiplication
void printOptimalParens(std::unique_ptr<std::unique_ptr<int[]>[]> &s, 
	const int &i, const int &j)
{
	if (i == j)
		std::cout << "A" << i + 1;
	else
	{
		std::cout << "(";
		printOptimalParens(s, i, s[i][j]);
		printOptimalParens(s, s[i][j] + 1, j);
		std::cout << ")";
	}
}
int matrixChainOrder(const std::vector<int> &p)
{
	int n = static_cast<int>(p.size() - 1);
	if (n == 0)
		return 0;
	std::unique_ptr<std::unique_ptr<int[]>[]> m = std::make_unique<std::unique_ptr<int[]>[]>(n);
	std::unique_ptr<std::unique_ptr<int[]>[]> s = std::make_unique<std::unique_ptr<int[]>[]>(n);
	for (int i = 0; i != n; ++i)
	{
		m[i] = std::make_unique<int[]>(n);
		s[i] = std::make_unique<int[]>(n);
	}
	for (int i = 0; i != n; ++i)///the length of 1
		m[i][i] = 0;
	for (int len = 2; len != n + 1; ++len)///the length from 2 to n
	{
		for (int i = 0; i != n - len + 1; ++i)
		{
			int j = i + len - 1;
			m[i][j] = 0x7fffffff;
			for (int k = i; k != j; ++k)
			{
				int res = m[i][k] + m[k + 1][j] + p[i] * p[k + 1] * p[j + 1];
				if (res < m[i][j])
				{
					m[i][j] = res;
					s[i][j] = k;
				}
			}
		}
	}
	printOptimalParens(s, 0, n - 1);
	std::cout << std::endl;
	return m[0][n - 1];
}
std::vector<int> getData()
{
	std::vector<int> p{ 30,35,15,5,10,20,25 };
	return std::move(p);
}

///deal whit LCS problem
template<typename T>
size_t lcsRecursive(const std::vector<T> &seq1, 
	const std::vector<T> &seq2, 
	size_t len1,
	size_t len2)///recursive version
{
	if (len1 == 0 || len2 == 0)
		return 0;
	if (seq1[len1 - 1] == seq2[len2 - 1])
		return lcsRecursive(seq1, seq2, len1 - 1, len2 - 1) + 1;
	else
		return Max(lcsRecursive(seq1, seq2, len1 - 1, len2), lcsRecursive(seq1, seq2, len1, len2 - 1));
}
template<typename T>
std::vector<std::tuple<T, size_t, size_t>> lcsRecursiveExt(const std::vector<T> &seq1, 
	const std::vector<T> &seq2, 
	size_t len1, 
	size_t len2)///get more details
{
	if (len1 == 0 || len2 == 0)
		return{};
	if (seq1[len1 - 1] == seq2[len2 - 1])
	{
		auto tmp = lcsRecursiveExt(seq1, seq2, len1 - 1, len2 - 1);
		tmp.push_back(std::make_tuple(seq1[len1 - 1], len1, len2));
		return tmp;
	}
	else
	{
		auto tmp1 = lcsRecursiveExt(seq1, seq2, len1 - 1, len2);
		auto tmp2 = lcsRecursiveExt(seq1, seq2, len1, len2 - 1);
		return tmp1.size() < tmp2.size() ? tmp2 : tmp1;
	}
}

template<typename T>
std::vector<std::tuple<T, size_t, size_t>> bottomUpLCS(const std::vector<T> &seq1,
	const std::vector<T> &seq2)///use O(m*n) space
{
	std::vector<std::tuple<T, size_t, size_t>> res;
	if (seq1.empty() || seq2.empty())
		return res;
	std::unique_ptr<std::unique_ptr<size_t[]>[]> c = std::make_unique<std::unique_ptr<size_t[]>[]>(seq1.size() + 1);
	for (size_t i = 0; i <= seq1.size(); ++i)
		c[i] = std::make_unique<size_t[]>(seq2.size() + 1);
	for (size_t i = 1; i <= seq1.size(); ++i)
		c[i][0] = 0;
	for (size_t i = 0; i <= seq2.size(); ++i)
		c[0][i] = 0;
	for (size_t i = 1; i <= seq1.size(); ++i)///calculate the length
		for (size_t j = 1; j <= seq2.size(); ++j)
		{
			if (seq1[i - 1] == seq2[j - 1])
				c[i][j] = c[i - 1][j - 1] + 1;
			else if (c[i - 1][j] >= c[i][j - 1])
				c[i][j] = c[i - 1][j];
			else
				c[i][j] = c[i][j - 1];
		}
	size_t i = seq1.size(), j = seq2.size();
	while (i != 0 && j != 0)///reconstruct the lcs use table "c", need O(m+n) time
	{
		if (seq1[i - 1] == seq2[j - 1])
			res.push_back(std::make_tuple(seq1[i], i--, j--));
		else if (c[i - 1][j] >= c[i][j - 1])
			--i;
		else
			--j;
	}
	std::reverse(res.begin(), res.end());
	return res;
}
template<typename T>
size_t bottomUpLCSExt(std::vector<T> &seq1,
	std::vector<T> &seq2)///use 2*min(m,n) and another O(1) space
{
	if (seq1.size() < seq2.size())
		std::swap(seq1, seq2);
	std::unique_ptr<std::unique_ptr<size_t[]>[]> c = std::make_unique<std::unique_ptr<size_t[]>[]>(2);
	for (int i = 0; i != 2; ++i)
		c[i] = std::make_unique<size_t[]>(seq2.size());
	for (size_t i = 0; i != seq2.size(); ++i)
	{
		c[0][i] = 0;
		c[1][i] = 0;
	}
	for (size_t i = 0; i != seq1.size(); ++i)
	{
		for (size_t j = 0; j != seq2.size(); ++j)
		{
			if (j == 0 && seq1[i] == seq2[j])
				c[1][j] = 1;
			else if (j != 0 && seq1[i] == seq2[j])
				c[1][j] = c[0][j - 1] + 1;
			else if (j != 0 && c[0][j] >= c[1][j - 1])
				c[1][j] = c[0][j];
			else if (j != 0 && c[0][j] < c[1][j - 1])
				c[1][j] = c[1][j - 1];
		}
		for (size_t k = 0; k != seq2.size(); ++k)
			c[0][k] = c[1][k];
	}
	return c[1][seq2.size() - 1];
}
template<typename T>
size_t bottomUpLCSExt2(std::vector<T> &seq1,
	std::vector<T> &seq2)///use min(m,n) and another O(1) space
{
	if (seq1.size() < seq2.size())
		std::swap(seq1, seq2);
	std::unique_ptr<size_t[]> c = std::make_unique<size_t[]>(seq2.size());
	for (size_t i = 0; i != seq2.size(); ++i)
		c[i] = 0;
	for (size_t i = 0; i != seq1.size(); ++i)
	{
		size_t temp_val = 0;
		for (size_t j = 0; j != seq2.size(); ++j)
		{
			size_t tmp = c[j];
			if (j == 0 && seq1[i] == seq2[j])
				c[j] = 1;
			else if (j != 0 && seq1[i] == seq2[j])
				c[j] = temp_val + 1;
			else if (j != 0 && c[j] < c[j - 1])
				c[j] = c[j - 1];
			temp_val = tmp;
		}
	}
	return c[seq2.size() - 1];
}

///find the longest monotonically increasing subsequence of the "input" sequence, O(n^2) time
template<typename T = int>
std::vector<std::tuple<T, size_t, size_t>> getLMI(const std::vector<T> &input)
{
	std::vector<T> input2 = input;
	std::sort(input2.begin(), input2.end());
	return lcsRecursiveExt(input, input2, input.size(), input2.size());
}

///O(n*lgn) time to find the LMI of sequence of n numbers
template<typename T>
size_t binarySearch(std::unique_ptr<T[]> &a,
	const size_t &len,
	const T &n)
{
	size_t left(0), right(len), mid = left + (right - left) / 2;
	while (left <= right)
	{
		if (n > a[mid])
			left = mid + 1;
		else if (n < a[mid])
			right = mid - 1;
		else
			return mid;
		mid = left + (right - left) / 2;
	}
	return left;
}
template<typename T>
std::vector<T> getLMIExt(const std::vector<T> &input)
{
	std::vector<T> res;
	if (input.empty())
		return res;
	std::unique_ptr<T[]> c = std::make_unique<T[]>(input.size() + 1);
	c[0] = -1;
	c[1] = input[0];
	size_t len = 1;
	for (size_t i = 1; i != input.size(); ++i)
	{
		size_t j = binarySearch(c, len, input[i]);
		c[j] = input[i];
		if (j > len)
			len = j;
	}
	for (size_t i = 0; i != len; ++i)
		res.push_back(c[i + 1]);
	return res;
}

///Optimal binary search tree
template<typename T>
class Matrix
{
public:
	Matrix() = default;
	Matrix(const size_t &m, const size_t &n)
		:row(m), col(n), matrix(std::make_unique<std::unique_ptr<T[]>[]>(m))
	{
		for (size_t i = 0; i != m; ++i)
			matrix[i] = std::make_unique<T[]>(n);
	}
	Matrix(const size_t &m, const size_t &n, const T &val)
		:Matrix(m, n)
	{
		for (size_t i = 0; i != m; ++i)
			for (size_t j = 0; j != n; ++j)
				matrix[i][j] = val;
	}
	Matrix(Matrix &rhs)
		:row(rhs.row), col(rhs.col)
	{
		rhs.row = rhs.col = 0;
		matrix.reset(rhs.matrix.release());
	}
	Matrix& operator=(Matrix &rhs)
	{
		if (rhs != *this)
		{
			row = rhs.row;
			col = rhs.col;
			rhs.row = rhs.col = 0;
			matrix.reset(rhs.matrix.release());
		}
		return *this;
	}
	~Matrix() = default;
	std::unique_ptr<T[]>& operator[](const size_t &m) const { return matrix[m]; }
	size_t Row() const { return row; }
	size_t Col() const { return col; }
private:
	size_t row;
	size_t col;
	std::unique_ptr<std::unique_ptr<T[]>[]> matrix;
};

template<typename T = double>
std::pair<Matrix<T>, Matrix<size_t>> optimalBST(const std::vector<T> &p,
	const std::vector<T> &q,
	const size_t &n)
{
	Matrix<T> e(n + 2, n + 2), w(n + 2, n + 2);
	Matrix<size_t> root(n, n);
	for (size_t i = 1; i != n + 2; ++i)
	{
		e[i][i - 1] = q[i - 1];
		w[i][i - 1] = q[i - 1];
	}
	for (size_t l = 1; l <= n; ++l)
		for (size_t i = 1; i <= n - l + 1; ++i)
		{
			size_t j = i + l - 1;
			e[i][j] = std::numeric_limits<T>::max();
			w[i][j] = w[i][j - 1] + p[j] + q[j];
			for (size_t r = i; r <= j; ++r)
			{
				T t = e[i][r - 1] + e[r + 1][j] + w[i][j];
				if (t < e[i][j])
				{
					e[i][j] = t;
					root[i - 1][j - 1] = r;
				}
			}
		}
	return{ e, root };
}
void constructOptimalBSTAux(const Matrix<size_t> &root, const size_t &i, const size_t &j, const size_t &root_key)
{
	if (j < i - 1)
		return;
	else if (j == i - 1)
	{
		std::cout << "d" << j << "为k" << root_key << "的";
		if (j < root_key)
			std::cout << "左孩子" << std::endl;
		else
			std::cout << "右孩子" << std::endl;
		return;
	}
	else
	{
		std::cout << "k" << root[i - 1][j - 1] << "为k" << root_key << "的";
		if (root[i - 1][j - 1] < root_key)
			std::cout << "左孩子" << std::endl;
		else
			std::cout << "右孩子" << std::endl;
	}
	constructOptimalBSTAux(root, i, root[i - 1][j - 1] - 1, root[i - 1][j - 1]);
	constructOptimalBSTAux(root, root[i - 1][j - 1] + 1, j, root[i - 1][j - 1]);
}
void constructOptimalBST(const Matrix<size_t> &root)
{
	if (root.Row() == 0)
		return;
	std::cout << "Construct the Optimal binary search tree: " << std::endl;
	std::cout << "k" << root[0][root.Row() - 1] << "为根" << std::endl;
	constructOptimalBSTAux(root, 1, root[0][root.Row() - 1] - 1, root[0][root.Row() - 1]);
	constructOptimalBSTAux(root, root[0][root.Row() - 1] + 1, root.Row(), root[0][root.Row() - 1]);
}

int main()
{
	auto start_time = std::chrono::steady_clock::now();

	std::vector<double> p{ 0, 0.04, 0.06, 0.08, 0.02, 0.10, 0.12, 0.14 };
	std::vector<double> q{ 0.06, 0.06, 0.06, 0.06, 0.05, 0.05, 0.05, 0.05 };
	auto res6 = optimalBST(p, q, 7);
	std::cout << "Optimal E[search]: " << res6.first[1][7] << std::endl;
	constructOptimalBST(res6.second);


	//std::vector<int> seq{ 0,1,2,3,4,1,2,3,4,5 };
	//auto res5 = getLMIExt(seq);
	//std::cout << "Seq: ";
	//std::copy(seq.cbegin(), seq.cend(), std::ostream_iterator<int>{std::cout, " "});
	//std::cout << std::endl;
	//std::cout << "LMI: ";
	//std::copy(res5.cbegin(), res5.cend(), std::ostream_iterator<int>{std::cout, " "});
	//std::cout << std::endl;


	//std::vector<char> seq1{ 'A', 'B', 'C', 'B', 'D', 'A', 'B' };
	//std::vector<char> seq2{ 'B', 'D', 'C', 'A', 'B', 'A' };
	//std::cout << "Seq1: ";
	//std::copy(seq1.cbegin(), seq1.cend(), std::ostream_iterator<char>{std::cout, " "});
	//std::cout << std::endl;
	//std::cout << "Seq2: ";
	//std::copy(seq2.cbegin(), seq2.cend(), std::ostream_iterator<char>{std::cout, " "});
	//std::cout << std::endl;
	//std::cout << "LCS-length: " << lcsRecursive(seq1, seq2, seq1.size(), seq2.size()) << std::endl;

	//auto res = lcsRecursiveExt(seq1, seq2, seq1.size(), seq2.size());
	//std::cout << "LCS-length: " << res.size() << std::endl;
	//for (size_t i = 0; i != res.size(); ++i)
	//	std::cout << std::get<0>(res[i]) << "(" << std::get<1>(res[i]) << ", " << std::get<2>(res[i]) << ") ";
	//std::cout << std::endl;

	//auto res2 = bottomUpLCS(seq1, seq2);
	//std::cout << "LCS-length: " << res2.size() << std::endl;
	//for (size_t i = 0; i != res2.size(); ++i)
	//	std::cout << std::get<0>(res2[i]) << "(" << std::get<1>(res2[i]) << ", " << std::get<2>(res2[i]) << ") ";
	//std::cout << std::endl;

	//std::cout << "LCS-length: " << bottomUpLCSExt(seq1, seq2) << std::endl;
	//std::cout << "LCS-length: " << bottomUpLCSExt2(seq1, seq2) << std::endl;

	//auto res3 = getLMI(seq1);
	//std::cout << "LMI-length: " << res3.size() << std::endl;
	//for (size_t i = 0; i != res3.size(); ++i)
	//	std::cout << std::get<0>(res3[i]) << "(" << std::get<1>(res3[i]) << ")";
	//std::cout << std::endl;


	//auto p = getData();
	//int res = matrixChainOrder(p);
	//std::cout << res << std::endl;


	//for (int i = 1; i != 100001; ++i)
	//{
	//	int pri = bottomUpCutRodExt(i);
	//	//std::cout << "length:" << i << "\t" << "price:" << pri << std::endl;
	//}

	//for (int i = 1; i != 100001; ++i)
	//{
	//	//int pri = cutRodExt(i);
	//	int pri = memoizedCutRodExt(i);
	//	//std::cout << "length:" << i << "\t" << "price:" << pri << std::endl;
	//}

	std::cout << "\nThe program run "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).count()
		<< " millisecond" << std::endl;
	system("pause");
	return 0;
}