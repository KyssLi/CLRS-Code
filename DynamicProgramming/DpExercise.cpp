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

int main()
{
	auto start_time = std::chrono::steady_clock::now();

	Graph<int, int> graph{ 1,2,3,4,5,6 };
	graph.addEdge(1, { 2,3 }, { 1,1 });
	graph.addEdge(2, { 4,5 }, { 1,5 });
	graph.addEdge(3, {});
	graph.addEdge(4, { 3,5,6 }, { 1,1,1 });
	graph.addEdge(5, { 6 }, { 1 });
	graph.addEdge(6, {});
	graph.printGraph();
	int a = 1;
	int b = 6;
	std::cout << "The longest path from " << a << " to " << b << " is " << LSPofDAG(graph, a, b) << "." << std::endl;

	std::cout << "\nThe program run "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).count()
		<< " millisecond" << std::endl;
	system("pause");
	return 0;
}