#include <cstdlib>
#include <algorithm>
#include <chrono>
#include <functional>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <stack>
#include <utility>
#include <vector>

template<typename T>
struct Ascend : public std::binary_function<T, T, bool>
{
	inline bool operator()(const T &left, const T &right) { return left < right; }
};
template<typename T>
struct Descend : public std::binary_function<T, T, bool>
{
	inline bool operator()(const T &left, const T &right) { return left > right; }
};

template<typename T>
struct BSTNode
{
	BSTNode() {}
	BSTNode(const T &val, BSTNode *const p = nullptr, BSTNode *const l = nullptr, BSTNode *const r = nullptr) 
		: key(val), parent(p), left_child(l), right_child(r){}
	T key;
	BSTNode *parent, *left_child, *right_child;
};

template<typename T, typename Pred = Ascend<T>>
class BinarySearchTree
{
public:
	BinarySearchTree() :root(nullptr) {}
	BinarySearchTree(std::initializer_list<T> il)
	{
		for (auto it = il.begin(); it != il.end(); ++it)
			insertNode(*it);
	}

	~BinarySearchTree()
	{
		if (root)
			deleteAuxiliary(root);
	}

	void insertNode(const T &key)
	{
		if (root == nullptr)
		{
			root = new BSTNode<T>(key);
			return;
		}
		BSTNode<T> *p = root, *pre = p;
		bool flag_left = true;
		while (p)
		{
			pre = p;
			if (key == p->key)
				return;
			else if (Pred()(key, p->key))
			{
				p = p->left_child;
				flag_left = true;
			}
			else
			{
				p = p->right_child;
				flag_left = false;
			}
		}
		if (flag_left)
			pre->left_child = new BSTNode<T>(key, pre);
		else
			pre->right_child = new BSTNode<T>(key, pre);
	}
	void insertNodeRecursive(const T &key)
	{
		if (root == nullptr)
			root = new BSTNode<T>(key);
		else
			insertAuxiliary(root, key);
	}

	void deleteNode(const T &key)
	{
		std::pair<BSTNode<T>*, bool> pos_msg = searchNode(key);
		if (!pos_msg.second)
			return;
		if (pos_msg.first->left_child == nullptr)//delete node with only right_child
			deleteNodeAuxiliary(pos_msg.first, pos_msg.first->right_child);
		else if (pos_msg.first->right_child == nullptr)
			deleteNodeAuxiliary(pos_msg.first, pos_msg.first->left_child);
		else
		{
			auto p = minimumPtr(pos_msg.first->right_child);//use "successor": actually use "minimumPtr"
			if (p->parent != pos_msg.first)
			{
				deleteNodeAuxiliary(p, p->right_child);
				p->right_child = pos_msg.first->right_child;
				p->right_child->parent = p;
			}
			deleteNodeAuxiliary(pos_msg.first, p);
			p->left_child = pos_msg.first->left_child;
			p->left_child->parent = p;
		}
		delete pos_msg.first;
		pos_msg.first = nullptr;
	}

	std::pair<BSTNode<T>*, bool> searchNode(const T &key) const
	{
		BSTNode<T> *p = root;
		while (p)
		{
			if (key == p->key)
				return{ p, true };
			else if (Pred()(key, p->key))
				p = p->left_child;
			else
				p = p->right_child;
		}
		return{ nullptr, false };
	}

	inline T minimum() const
	{
		auto p = minimumPtrRecursive(root);
		return p ? p->key : 0;
	}
	inline BSTNode<T>* minimumPtr(BSTNode<T> *p) const
	{
		if (p == nullptr)
			return p;
		while (p && p->left_child)
			p = p->left_child;
		return p;
	}
	inline BSTNode<T>* minimumPtrRecursive(BSTNode<T> *p) const//recursive version
	{
		if (p == nullptr)
			return p;
		if (p->left_child == nullptr)
			return p;
		return minimumPtrRecursive(p->left_child);
	}
	inline T maximum() const
	{
		auto p = maximumPtrRecursive(root);
		return p ? p->key : 0;
	}
	inline BSTNode<T>* maximumPtr(BSTNode<T> *p) const
	{
		if (p == nullptr)
			return p;
		while (p && p->right_child)
			p = p->right_child;
		return p;
	}
	inline BSTNode<T>* maximumPtrRecursive(BSTNode<T> *p) const 
	{
		if (p == nullptr)
			return p;
		if (p->right_child == nullptr)
			return p;
		return maximumPtrRecursive(p->right_child);
	}

	inline BSTNode<T>* predecessor(BSTNode<T> *node) const
	{
		if (node->left_child != nullptr)
			return maximumPtr(node->left_child);
		BSTNode<T> *p = node->parent;
		while (p != nullptr && node == p->left_child)
		{
			node = p;
			p = p->parent;
		}
		return p;
	}
	inline BSTNode<T>* successor(BSTNode<T> *node) const
	{
		if (node->right_child != nullptr)
			return minimumPtr(node->right_child);
		BSTNode<T> *p = node->parent;
		while (p != nullptr && node == p->right_child)
		{
			node = p;
			p = p->parent;
		}
		return p;
	}

	inline std::vector<T> printBSTree() const
	{
		std::vector<T> res;
		if (root)
			printBSTreeAuxiliary(res, root);
		return res;
	}
	inline std::vector<T> printBSTree2() const//use successor function
	{
		std::vector<T> res;
		auto p = minimumPtr(root);
		while (p)
		{
			res.push_back(p->key);
			p = successor(p);
		}
		return res;
	}
	inline std::vector<T> printBSTree3() const///use predecessor function
	{
		std::vector<T> res;
		auto p = maximumPtr(root);
		while (p)
		{
			res.push_back(p->key);
			p = predecessor(p);
		}
		std::reverse(res.begin(), res.end());
		return res;
	}
	inline std::vector<T> printBSTreePreorder() const
	{
		std::vector<T> res;
		if (root)
			printBSTreeAuxiliary2(res, root);
		return res;
	}
	inline std::vector<T> printBSTreePostorder() const
	{
		std::vector<T> res;
		if (root)
			printBSTreeAuxiliary3(res, root);
		return res;
	}

private:
	BSTNode<T>* insertAuxiliary(BSTNode<T> *const p, const T &key)
	{
		if (p == nullptr)
			return p;
		bool flag_left = true;
		if (Pred()(key, p->key))
		{
			if (insertAuxiliary(p->left_child, key) == nullptr)
				p->left_child = new BSTNode<T>(key, p);
			flag_left = true;
		}
		else
		{
			if (insertAuxiliary(p->right_child, key) == nullptr)
				p->right_child = new BSTNode<T>(key, p);
			flag_left = false;
		}
		return flag_left ? p->left_child : p->right_child;
	}

	inline void deleteNodeAuxiliary(BSTNode<T> *cur, BSTNode<T> *child)//actually move child to cur
	{
		if (cur->parent == nullptr)
			root = child;
		else if (cur->parent->left_child == cur)
			cur->parent->left_child = child;
		else
			cur->parent->right_child = child;
		if (child)
			child->parent = cur->parent;
	}

	void printBSTreeAuxiliary(std::vector<T> &arr, BSTNode<T> *p) const
	{
		std::stack<BSTNode<T>*> aux_s;
		while (p || !aux_s.empty())
		{
			while (p)
			{
				aux_s.push(p);
				p = p->left_child;
			}
			if (!aux_s.empty())
			{
				auto val = aux_s.top();
				aux_s.pop();
				arr.push_back(val->key);
				p = val->right_child;
			}
		}
	}
	void printBSTreeAuxiliary2(std::vector<T> &arr, BSTNode<T> *p) const
	{
		///recursive version
		//arr.push_back(p->key);
		//if (p->left_child)
		//	printBSTreeAuxiliary2(arr, p->left_child);
		//if (p->right_child)
		//	printBSTreeAuxiliary2(arr, p->right_child);
		///iteration version
		std::stack<BSTNode<T>*> aux_s;
		aux_s.push(p);
		while (!aux_s.empty())
		{
			auto val = aux_s.top();
			aux_s.pop();
			arr.push_back(val->key);
			if (val->right_child)
				aux_s.push(val->right_child);
			if (val->left_child)
				aux_s.push(val->left_child);
		}
	}
	void printBSTreeAuxiliary3(std::vector<T> &arr, BSTNode<T> *p) const
	{
		std::stack<BSTNode<T>*> aux_s;
		while (true)
		{
			while (p)
			{
				aux_s.push(p);
				p = p->left_child;
			}
			while (!aux_s.empty() && aux_s.top()->right_child == p)
			{
				p = aux_s.top();
				aux_s.pop();
				arr.push_back(p->key);
			}
			if (aux_s.empty())
				break;
			p = aux_s.top()->right_child;
		}
	}
	inline void deleteAuxiliary(BSTNode<T> *node)
	{
		if (node->left_child)
			deleteAuxiliary(node->left_child);
		if (node->right_child)
			deleteAuxiliary(node->right_child);
		delete node;
		node = nullptr;
	}

	BSTNode<T> *root;
};

int main()
{
	auto start_time = std::chrono::steady_clock::now();

	BinarySearchTree<int> tree{ 27, 17, 30, 16, 4, 18, 35, 19, 40, 41, 38, 39, 37 };
	tree.deleteNode(37);
	std::cout << "Largest: " << tree.maximum() << std::endl;
	std::cout << "Smallest: " << tree.minimum() << std::endl;
	auto res = tree.printBSTree();
	std::copy(res.cbegin(), res.cend(), std::ostream_iterator<int>{std::cout, " "}); 
	std::cout << std::endl;
	res = tree.printBSTreePreorder();
	std::copy(res.cbegin(), res.cend(), std::ostream_iterator<int>{std::cout, " "});
	std::cout << std::endl;
	res = tree.printBSTreePostorder();
	std::copy(res.cbegin(), res.cend(), std::ostream_iterator<int>{std::cout, " "});
	std::cout << std::endl;

	std::cout << "\nThe program run "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).count()
		<< " millisecond" << std::endl;
	system("pause");
	return 0;
}