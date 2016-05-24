#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <chrono>
#include <limits>
#include <list>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <queue>
#include <stack>
#include <string>
#include <utility>
#include <vector>

template<typename T, typename W> struct Edge;
template<typename T, typename W>
struct Vertex
{
	Vertex() = default;
	Vertex(const T &k)
		:key(k)
	{}
	~Vertex() = default;
	T key;
	std::shared_ptr<std::list<Edge<T, W>>> to;
};
template<typename T, typename W>
struct Edge
{
	Edge() = default;
	~Edge() = default;
	Edge(std::shared_ptr<Vertex<T, W>> &v, const W &w)
		:end_point(v), weight(w)
	{}
	std::shared_ptr<Vertex<T, W>> end_point;
	W weight;
};
template<typename T, typename W>
class Graph
{
public:
	Graph() = default;
	Graph(std::initializer_list<T> il)
	{
		size_t cnt = 0;
		for (auto it = il.begin(); it != il.end(); ++it)
		{
			addVertex(*it);
			vertex_order_map[*it] = cnt++;
		}
	}
	~Graph() = default;
	void addEdge(const T &key, const std::initializer_list<T> &il, const std::initializer_list<W> &il2 = {})
	{
		if (vertex_pos_map[key]->to == nullptr)
			vertex_pos_map[key]->to = std::make_shared<std::list<Edge<T, W>>>();
		if (il2.size() == 0)
			for (auto it = il.begin(); it != il.end(); ++it)
				vertex_pos_map[key]->to->push_back(Edge<T, W>(vertex_pos_map[*it], 0));
		else
			for (auto it = il.begin(), it2 = il2.begin(); it != il.end(); ++it, ++it2)
				vertex_pos_map[key]->to->push_back(Edge<T, W>(vertex_pos_map[*it], *it2));
	}
	void printGraph() const
	{
		for (auto it = G.cbegin(); it != G.cend(); ++it)///"it" is point to "std::shared_ptr<Vertex<T, W>>"
		{
			std::cout << (*it)->key << "->";
			if ((*it)->to != nullptr)/// "(*it)->to" is std::shared_ptr<std::list<Edge<T, W>>>
				for (auto p = (*it)->to->cbegin(); p != (*it)->to->cend(); ++p)
					std::cout << "(" << p->weight << ")" << p->end_point->key << "->";
			std::cout << "/" << std::endl;
		}
	}
	bool empty() const
	{
		return G.empty();
	}
	bool find(const T &key) const
	{
		return (vertex_pos_map.find(key) != vertex_pos_map.cend());
	}
	std::queue<T> topologicalSort() const
	{
		std::queue<T> topoSeq;
		std::map<T, int> indegree = getIndegree();
		std::queue<T> tmp_que;
		for (auto it = indegree.begin(); it != indegree.end();)
		{
			if (it->second == 0)
			{
				tmp_que.push(it->first);
				it = indegree.erase(it);
			}
			else
				++it;
		}
		while (!tmp_que.empty())
		{
			T val = tmp_que.front();
			tmp_que.pop();
			topoSeq.push(val);
			if (vertex_pos_map.at(val)->to != nullptr)
				for (auto p = vertex_pos_map.at(val)->to->cbegin(); p != vertex_pos_map.at(val)->to->cend(); ++p)
					if (--indegree.at(p->end_point->key) == 0)
					{
						tmp_que.push(p->end_point->key);
						indegree.erase(p->end_point->key);
					}
		}
		return topoSeq;
	}
	size_t getOrder(const T &key) const { return vertex_order_map.at(key); }
	std::vector<T> getAdjPoint(const T &key) const
	{
		std::vector<T> res;
		if (vertex_pos_map.at(key)->to != nullptr)
			for (auto p = vertex_pos_map.at(key)->to->cbegin(); p != vertex_pos_map.at(key)->to->cend(); ++p)
					res.push_back(p->end_point->key);
		return res;
	}
	W getWeight(const T &left, const T &right) const
	{
		for (auto p = vertex_pos_map.at(left)->to->cbegin(); p != vertex_pos_map.at(left)->to->cend(); ++p)
			if (p->end_point->key == right)
				return p->weight;
		return{};
	}
private:
	std::map<T, int> getIndegree() const///get the indegree of every vertex
	{
		std::map<T, int> indegree;
		for (auto it = G.cbegin(); it != G.cend(); ++it)
		{
			indegree[(*it)->key] += 0;
			if ((*it)->to != nullptr)
				for (auto p = (*it)->to->cbegin(); p != (*it)->to->cend(); ++p)
					++indegree[p->end_point->key];
		}
		return indegree;
	}
	void addVertex(const T &key)
	{
		vertex_pos_map[key] = std::make_shared<Vertex<T, W>>(key);
		G.push_back(vertex_pos_map[key]);
	}
	std::vector<std::shared_ptr<Vertex<T, W>>> G;///the graphy
	std::map<T, std::shared_ptr<Vertex<T, W>>> vertex_pos_map;///the position of vertex-allocated
	std::map<T, size_t> vertex_order_map;///the order of vertex-inserted
};

///15-1:The longest simple path of DAG
template<typename T, typename W>
W LSPofDAG(const Graph<T, W> &g, const T &s, const T &t)
{
	if (g.find(s) == false || g.find(t) == false)
		return std::numeric_limits<W>::min();
	size_t s_pos = g.getOrder(s), t_pos = g.getOrder(t);
	std::queue<T> topoSeq = g.topologicalSort();
	std::unique_ptr<W[]> dist = std::make_unique<W[]>(topoSeq.size());
	for (size_t i = 0; i != topoSeq.size(); ++i)
		dist[i] = std::numeric_limits<T>::min();
	dist[s_pos] = 0;
	while (!topoSeq.empty())
	{
		T val = topoSeq.front();
		topoSeq.pop();
		if (dist[g.getOrder(val)] != std::numeric_limits<W>::min())
		{
			auto adj_point = g.getAdjPoint(val);
			for (auto it = adj_point.cbegin(); it != adj_point.cend(); ++it)
				if (dist[g.getOrder(*it)] < (dist[g.getOrder(val)] + g.getWeight(val, *it)))
					dist[g.getOrder(*it)] = dist[g.getOrder(val)] + g.getWeight(val, *it);
		}
	}
	return dist[t_pos];
}
//////////////////////////////////////////////////////////////////////////////////////////////

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
	Matrix(const size_t &m)
		:Matrix(m, m)
	{
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
	std::unique_ptr<T[]>& operator[](const size_t &m) const
	{
		return matrix[m];
	}
	size_t Row() const
	{
		return row;
	}
	size_t Col() const
	{
		return col;
	}
	void printMatrix() const
	{
		for (size_t i = 0; i != row; ++i)
		{
			for (size_t j = 0; j != col; ++j)
				std::cout << matrix[i][j] << " ";
			std::cout << std::endl;
		}
	}
	template<typename Pred>
	void printMatrix(Pred &func) const
	{
		for (size_t i = 0; i != row; ++i)
		{
			for (size_t j = 0; j != col; ++j)
				func(matrix[i][j]);
			std::cout << std::endl;
		}
	}
private:
	size_t row;
	size_t col;
	std::unique_ptr<std::unique_ptr<T[]>[]> matrix;
};

///15-2:The longest palindrome subsequence
std::string getLPS(const std::string &input)
{
	std::string tmp_str;
	std::copy(input.crbegin(), input.crend(), std::back_inserter(tmp_str));
	size_t m = input.size() + 1;
	Matrix<char> c(m, m, 0);
	for (size_t i = 1; i != m; ++i)
		for (size_t j = 1; j != m; ++j)
		{
			if (input[i - 1] == tmp_str[j - 1])
				c[i][j] = c[i - 1][j - 1] + 1;
			else if (c[i - 1][j] >= c[i][j - 1])
				c[i][j] = c[i - 1][j];
			else
				c[i][j] = c[i][j - 1];
		}
	size_t i = input.size(), j = tmp_str.size();
	std::string res;
	while (i != 0 && j != 0)///reconstruct the LPS use table "c", need O(m+n) time
	{
		if (input[i - 1] == tmp_str[j - 1])
		{
			res.push_back(input[--i]);
			--j;
		}
		else if (c[i - 1][j] >= c[i][j - 1])
			--i;
		else
			--j;
	}
	return res;
}

///15-3:bitonic euclidean traveling-salesman problem
template<typename T>
struct point2D
{
	point2D() = default;
	point2D(const T &x_, const T &y_) :x(x_), y(y_) {}
	T x;
	T y;
};
template<typename T>
double distPoint2D(const point2D<T> &one, const point2D<T> &two)
{
	T a = (two.y - one.y) * (two.y - one.y);
	T b = (two.x - one.x) * (two.x - one.x);
	return sqrt(static_cast<double>(a + b));
}
template<typename T>
std::pair<double, std::vector<T>> bitonicEuclideanTSP(std::vector<point2D<T>> &input)
{
	size_t size = input.size();
	std::sort(input.begin(), input.end(), [](const auto &l, const auto &r)->bool {return l.x < r.x; });
	///d[i][j] is the distance of input[i] and input[j]
	Matrix<double> d(size, size);
	for (size_t i = 0; i != size; ++i)
		for (size_t j = 0; j != size; ++j)
			if (i == j)
				d[i][j] = 0;
			else if (j > i)
				d[i][j] = distPoint2D(input[i], input[j]);
			else
				d[i][j] = d[j][i];
	///path[i][j] is the predecessor "point" of point "j"
	Matrix<T> path(size, size, -1);///assume that all the point is (x >= 0, y >= 0)
	path[0][0] = 0;
	path[0][1] = 0;
	///b[i][j] is the shortest path include ("i" to "0") and ("0" to "j"), assume that ("j" > "i"), at the last ("j" == "i")
	Matrix<double> b(size, size);
	b[0][0] = 0.0;
	b[0][1] = d[0][1];
	for (size_t j = 2; j != size; ++j)
		for (size_t i = 0; i != j; ++i)
		{
			if (j > i + 1)
			{
				b[i][j] = b[i][j - 1] + d[j - 1][j];
				path[i][j] = j - 1;
			}
			else
			{
				b[i][j] = std::numeric_limits<double>::max();
				for (size_t k = 0; k != i; ++k)
				{
					double tmp = b[k][i] + d[k][j];
					if (tmp < b[i][j])
					{
						b[i][j] = tmp;
						path[i][j] = k;
					}
				}
			}
		}
	b[size - 1][size - 1] = b[size - 2][size - 1] + d[size - 2][size - 1];
	//b.printMatrix();
	//path.printMatrix([](const auto &v)->void {std::cout << v<< " "; });
	std::vector<T> tmp_vec;
	std::stack<T> S[2];///S[0] store one path and S[1] store another path
	size_t i = size - 2, j = size - 1, k = 0;
	while (j > 0)
	{
		S[k].push(j);
		if ((j = path[i][j]) < i)
		{
			std::swap(i, j);
			k = 1 - k;
		}
	}
	S[0].push(0);
	while (!S[1].empty())
	{
		S[0].push(S[1].top());
		S[1].pop();
	}
	for (size_t m = 0; m != size; ++m)
	{
		tmp_vec.push_back(S[0].top());
		S[0].pop();
	}
	return{ b[size - 1][size - 1],  tmp_vec };
}

///15-4pretty printing problem
template<typename T>
T cubeCal(const T &val)
{
	return val * val * val;
}
int NeatlyPrinting(const std::vector<std::string> &input, const int &M)///M is the maximum char in each line
{
	if (input.empty())
		return 0;
	const int MAX_SIZE = 1000;///each word's size is less than 1000
	int cap = static_cast<int>(input.size());
	Matrix<int> s(cap, cap, 0);///s[i][j] is the number of trailing spaces when the words("i" to "j") in the same line
	Matrix<int> c(cap, cap, 0);///c[i][j] is the cube of the s[i][j] in some situation
	std::unique_ptr<int[]> C = std::make_unique<int[]>(cap + 1);///C[j] is the optimal cost to print words("1" to "j")
	std::unique_ptr<int[]> p = std::make_unique<int[]>(cap);///p[j] is the first word's position in line that contains word "j";
	for (int i = 0; i != cap; ++i)
	{
		s[i][i] = M - input[i].size();
		for (int j = i + 1; j != cap; ++j)
			s[i][j] = s[i][j - 1] - input[j].size() - 1;
	}
	for (int i = 0; i != cap; ++i)
		for (int j = i; j != cap; ++j)
			if (s[i][j] < 0)
				c[i][j] = MAX_SIZE;
			else if (j == cap - 1 && s[i][j] >= 0)
				c[i][j] = 0;
			else
				c[i][j] = cubeCal(s[i][j]);
	C[0] = 0;
	for (int j = 1; j <= cap; ++j)
	{
		C[j] = std::numeric_limits<int>::max();
		for (int i = 1; i <= j; ++i)
			if (C[i - 1] + c[i - 1][j - 1] < C[j])
			{
				C[j] = C[i - 1] + c[i - 1][j - 1];
				p[j - 1] = i - 1;
			}
	}
	int start_pos = 0, end_pos = cap - 1;
	std::stack<std::pair<int, int>> aux_stack_pair;
	while (end_pos >= 0)
	{
		start_pos = p[end_pos];
		aux_stack_pair.push(std::make_pair(start_pos, end_pos));
		end_pos = --start_pos;
	}
	std::cout << "Neatly Printing: " << std::endl;
	while (!aux_stack_pair.empty())
	{
		auto v = aux_stack_pair.top();
		aux_stack_pair.pop();
		for (auto i = v.first; i <= v.second; ++i)
			std::cout << input[i] << " ";
		std::cout << std::endl;
	}
	return C[cap];
}

int main()
{
	auto start_time = std::chrono::steady_clock::now();

	///15-4
	std::vector<std::string> input{ "a", "abc", "a", "abcd", "a" };
	auto res = NeatlyPrinting(input, 7);
	std::cout << "The sum of trailing space is: " << res << std::endl;

	///15-3
	//std::vector<point2D<int>> input{ {0,6}, {2,3},{5,4},{7,5},{8,2},{1,0},{6,1} };
	//auto res = bitonicEuclideanTSP(input);
	//std::cout << "The shortest path: " << res.first << std::endl;
	//std::cout << "The path may be: ";
	//for (const auto &v : res.second)
	//	std::cout << v << " ";
	//std::cout << std::endl;

	///15-2
	//const std::string str = "10234321";
	//std::cout << "LPS of " << str << " is: ";
	//std::cout << getLPS(str) << std::endl;

	///15-1
	//Graph<int, int> graph{ 1,2,3,4,5,6 };
	//graph.addEdge(1, { 2,3 }, { 1,1 });
	//graph.addEdge(2, { 4,5 }, { 1,5 });
	//graph.addEdge(3, {});
	//graph.addEdge(4, { 3,5,6 }, { 1,1,1 });
	//graph.addEdge(5, { 6 }, { 1 });
	//graph.addEdge(6, {});
	//graph.printGraph();
	//int a = 1;
	//int b = 6;
	//std::cout << "The longest path from " << a << " to " << b << " is " << LSPofDAG(graph, a, b) << "." << std::endl;

	std::cout << "\nThe program run "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).count()
		<< " millisecond" << std::endl;
	system("pause");
	return 0;
}
