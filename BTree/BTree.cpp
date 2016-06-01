#include <vld.h>
#include <cstdlib>
#include <chrono>
#include <iostream>
#include <memory>
#include <queue>
#include <string>
#include <tuple>
#include <utility>

template<typename T>
class BTree
{
	struct BTNode;
	static int t;
public:
	BTree()
	{
		treeInitialize();
	}
	~BTree() = default;
	void insert(const T &k)
	{
		auto r = root;
		if (r->n == 2 * t - 1)///if the root is full
		{
			auto p = createNode();
			root = p;
			p->is_leaf = false;
			p->n = 0;
			p->child[0] = r;
			insertAux(p, 0);///split
			insertNonfull(p, k);///insert k into p
		}
		else
			insertNonfull(r, k);
	}
	void erase(const T &k)
	{
		auto p = root;
		eraseAux(p, k);
	}
	std::pair<std::shared_ptr<BTNode>, int> find(const T &k) const
	{
		auto p = root;
		return findAux(p, k);
	}
	std::shared_ptr<BTNode> minimumPtr() const
	{
		auto p = root;
		return minimumPtrAux(p);
	}
	std::shared_ptr<BTNode> maximumPtr() const
	{
		auto p = root;
		return maximumPtrAux(p);
	}
	T predecessorKey(const T &k) const
	{
		auto p = root;
		auto res_find = findAuxExt(p, k);
		if (std::get<0>(res_find) == nullptr)///no key of "k" in B-Tree
			return{};
		if (!std::get<0>(res_find)->is_leaf)///if "k" is in the internal-node
		{
			auto max_ptr = maximumPtrAux(std::get<0>(res_find)->child[std::get<1>(res_find)]);
			return max_ptr->key[max_ptr->n - 1];
		}
		else if (std::get<0>(res_find)->is_leaf && std::get<1>(res_find) > 0)
			return std::get<0>(res_find)->key[std::get<1>(res_find) - 1];
		else if (std::get<3>(res_find) == 0)
			return{};
		else
			return std::get<2>(res_find)->key[std::get<3>(res_find) - 1];
	}
	T successorKey(const T &k) const
	{
		auto p = root;
		auto res_find = findAuxExt(p, k);
		if (std::get<0>(res_find) == nullptr)
			return{};
		if (!std::get<0>(res_find)->is_leaf)
		{
			auto min_ptr = minimumPtrAux(std::get<0>(res_find)->child[std::get<1>(res_find) + 1]);
			return min_ptr->key[0];
		}
		else if (std::get<0>(res_find)->is_leaf && std::get<1>(res_find) < std::get<0>(res_find)->n - 1)
			return std::get<0>(res_find)->key[std::get<1>(res_find) + 1];
		else if (std::get<3>(res_find) == 0)
			return{};
		else
			return std::get<2>(res_find)->key[std::get<3>(res_find)];
	}
	void printBTree() const
	{
		std::queue<std::shared_ptr<BTNode>> aux_q;
		if (root == nullptr)
		{
			std::cout << "B-Tree has nothing." << std::endl;
			return;
		}
		aux_q.push(root);
		int depth = 0;
		std::cout << "degree: " << t << std::endl;
		while (!aux_q.empty())
		{
			decltype(aux_q) next_q;
			std::cout << "depth " << depth << " :\t";
			while (!aux_q.empty())
			{
				auto p = aux_q.front();
				aux_q.pop();
				std::cout << "[ ";
				for (int i = 0; i != p->n; ++i)
				{
					std::cout << p->key[i] << " ";
					if (!p->is_leaf)
						next_q.push(p->child[i]);
				}
				std::cout << "]";
				if (!p->is_leaf)
					next_q.push(p->child[p->n]);
			}
			std::cout << std::endl;
			++depth;
			aux_q = next_q;
		}
	}

private:
	struct BTNode
	{
		BTNode(const bool &b_, const int &n_)
			:is_leaf(b_), n(n_)
		{
			key = std::make_unique<T[]>(2 * t - 1);
			child = std::make_unique<std::shared_ptr<BTNode>[]>(2 * t);
		}
		BTNode() :BTNode(true, 0) {}
		bool is_leaf;
		int n;
		std::unique_ptr<T[]> key;
		std::unique_ptr<std::shared_ptr<BTNode>[]> child;
	};
	void treeInitialize()
	{
		root = createNode();
	}
	std::shared_ptr<BTNode> createNode()
	{
		return std::make_shared<BTNode>();
	}
	void insertAux(std::shared_ptr<BTNode> &p, const int &pos)///a bit tricky
	{
		auto z = createNode();
		auto y = p->child[pos];
		z->is_leaf = y->is_leaf;
		z->n = t - 1;
		for (int j = 0; j != t - 1; ++j)
			z->key[j] = y->key[j + t];
		if (!y->is_leaf)
			for (int j = 0; j != t; ++j)
				z->child[j] = y->child[j + t];
		y->n = t - 1;
		for (int j = p->n; j > pos; --j)
			p->child[j + 1] = p->child[j];
		p->child[pos + 1] = z;
		for (int j = p->n - 1; j >= pos; --j)
			p->key[j + 1] = p->key[j];
		p->key[pos] = y->key[t - 1];
		++p->n;
	}
	void insertNonfull(std::shared_ptr<BTNode> &p, const T &val)
	{
		auto i = p->n - 1;
		if (p->is_leaf)///if node "p" is the leaf
		{
			while (i >= 0 && val < p->key[i])
			{
				p->key[i + 1] = p->key[i];
				--i;
			}
			p->key[i + 1] = val;
			++p->n;
		}
		else
		{
			while (i >= 0 && val < p->key[i])
				--i;
			++i;
			if (p->child[i]->n == 2 * t - 1)
			{
				insertAux(p, i);
				if (val > p->key[i])
					++i;
			}
			insertNonfull(p->child[i], val);
		}
	}
	void eraseAux(std::shared_ptr<BTNode> p, const T k)
	{
		auto res_find = findAux(p, k);
		if (res_find.first == nullptr)
			return;
		if (res_find.first == p)
		{
			if (res_find.first->is_leaf)///if k in node "p", and "p" is a leaf
			{
				for (int i = res_find.second; i < res_find.first->n - 1; ++i)
					res_find.first->key[i] = res_find.first->key[i + 1];
				--res_find.first->n;
			}
			else if (res_find.first->child[res_find.second]->n >= t)///if the pre-child node has n(n >= t) key words
			{
				auto pre_ptr = maximumPtrAux(res_find.first->child[res_find.second]);
				res_find.first->key[res_find.second] = pre_ptr->key[pre_ptr->n - 1];
				eraseAux(res_find.first->child[res_find.second], pre_ptr->key[pre_ptr->n - 1]);
				return;
			}
			else if (res_find.first->child[res_find.second + 1]->n >= t)
			{
				auto next_ptr = minimumPtrAux(res_find.first->child[res_find.second + 1]);
				res_find.first->key[res_find.second] = next_ptr->key[0];
				eraseAux(res_find.first->child[res_find.second + 1], next_ptr->key[0]);
				return;
			}
			else
			{
				mergeChild(res_find.first, res_find.second);
				eraseAux(res_find.first->child[res_find.second], k);
			}
		}
		else
		{
			int i = 0;
			while (i < p->n && p->key[i] < k)
				++i;
			auto p_child = p->child[i];
			if (p_child->n == t - 1)
			{
				auto p_left = (i == 0 ? nullptr : p->child[i - 1]);///left-brother node of p_child
				auto p_right = (i == p->n ? nullptr : p->child[i + 1]);
				int j;
				if (p_left && p_left->n >= t)
				{
					for (j = p_child->n; j > 0; --j)
						p_child->key[j] = p_child->key[j - 1];
					p_child->key[0] = p->key[i - 1];
					if (!p_left->is_leaf)
					{
						for (j = p_child->n + 1; j > 0; --j)
							p_child->child[j] = p_child->child[j - 1];
						p_child->child[0] = p_left->child[p_left->n];
					}
					++p_child->n;
					p->key[i - 1] = p_left->key[p_left->n - 1];
					--p_left->n;
				}
				else if (p_right && p_right->n >= t)
				{
					p_child->key[p_child->n] = p->key[i];
					++p_child->n;
					p->key[i] = p_right->key[0];
					for (j = 0; j < p_right->n - 1; ++j)
						p_right->key[j] = p_right->key[j + 1];
					if (!p_right->is_leaf)
					{
						p_child->child[p_child->n] = p_right->child[0];
						for (j = 0; j < p_right->n; ++j)
							p_right->child[j] = p_right->child[j + 1];
					}
					--p_right->n;
				}
				else if (p_left)
				{
					mergeChild(p, i - 1);
					p_child = p_left;
				}
				else
					mergeChild(p, i);
			}
			eraseAux(p_child, k);
		}
	}
	void mergeChild(std::shared_ptr<BTNode> p, const int &index)
	{
		auto pre_child = p->child[index];
		auto next_child = p->child[index + 1];
		pre_child->key[pre_child->n] = p->key[index];
		for (int i = 1; i <= next_child->n; ++i)
			pre_child->key[pre_child->n + i] = next_child->key[i - 1];
		if (!pre_child->is_leaf)
			for (int i = 0; i <= next_child->n; ++i)
				pre_child->child[pre_child->n + i + 1] = next_child->child[i];
		pre_child->n += next_child->n + 1;
		for (int i = index; i < p->n - 1; ++i)
		{
			p->key[i] = p->key[i + 1];
			p->child[i + 1] = p->child[i + 2];
		}
		--p->n;
	}
	std::pair<std::shared_ptr<BTNode>, int> findAux(std::shared_ptr<BTNode> p, const T &k) const
	{
		while (p)
		{
			int i = 0, j = p->n - 1;
			while (i <= j)
			{
				int mid = i + (j - i) / 2;
				if (p->key[mid] == k)
					return{ p, mid };
				else if (p->key[mid] < k)
					i = mid + 1;
				else
					j = mid - 1;
			}
			if (p->is_leaf)
				return{};
			else
				p = p->child[i];
		}
		return{};
	}
	std::tuple<std::shared_ptr<BTNode>, int, std::shared_ptr<BTNode>, int> findAuxExt(std::shared_ptr<BTNode> p, const T &k) const
	{
		auto pre = p;
		int pre_pos = 0;
		while (p)
		{
			int i = 0, j = p->n - 1;
			while (i <= j)
			{
				int mid = i + (j - i) / 2;
				if (p->key[mid] == k)
					return{ p, mid, pre, pre_pos };
				else if (p->key[mid] < k)
					i = mid + 1;
				else
					j = mid - 1;
			}
			if (p->is_leaf)
				return{};
			else
			{
				pre = p;
				pre_pos = i;
				p = p->child[i];
			}
		}
		return{};
	}
	std::shared_ptr<BTNode> minimumPtrAux(std::shared_ptr<BTNode> p) const
	{
		if (p->n == 0)
			return nullptr;
		while (!p->is_leaf)
			p = p->child[0];
		return p;
	}
	std::shared_ptr<BTNode> maximumPtrAux(std::shared_ptr<BTNode> p) const
	{
		if (p->n == 0)
			return nullptr;
		while (!p->is_leaf)
			p = p->child[p->n];
		return p;
	}
	std::shared_ptr<BTNode> root;
};
template<typename T>
int BTree<T>::t = 2;

int main()
{
	auto start_time = std::chrono::steady_clock::now();

	//BTree<int> tree;
	//for (int i = 1; i != 6; ++i)
	//	tree.insert(i);
	//tree.insert(6);
	//tree.printBTree();///241356

	std::string str = "FSQKCLHTVWMRNPABXYDZE";
	//std::string str = "PCGMTXABDEFJKLNOQRSUVYZ";
	BTree<char> tree;
	for (const auto &c : str)
		tree.insert(c);
	tree.erase('M');
	tree.printBTree();

	auto val = 'Y';
	auto res_find = tree.find(val);///
	if (res_find.first == nullptr)
		std::cout << val << " is not find." << std::endl;
	else
		std::cout << res_find.first->key[res_find.second] << " has been found." << std::endl;
	auto min_ptr = tree.minimumPtr();///
	if (min_ptr != nullptr)
		std::cout << min_ptr->key[0] << " is the minimum key in the B-Tree." << std::endl;
	else
		std::cout << "B-Tree has nothing." << std::endl;
	auto max_ptr = tree.maximumPtr();///
	if (max_ptr != nullptr)
		std::cout << max_ptr->key[max_ptr->n - 1] << " is the maximum key in the B-Tree." << std::endl;
	else
		std::cout << "B-Tree has nothing." << std::endl;

	auto val2 = 'Y';
	std::cout << "The pre-key of " << val2 << " is " << tree.predecessorKey(val2) << std::endl;
	std::cout << "The next-key of " << val2 << " is " << tree.successorKey(val2) << std::endl;

	std::cout << "\nThe program run "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).count()
		<< " millisecond" << std::endl;
	system("pause");
	return 0;
}