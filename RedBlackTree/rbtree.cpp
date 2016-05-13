#include <cstdlib>
#include <algorithm>
#include <chrono>
#include <functional>
#include <initializer_list>
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
	inline bool operator()(const T &left, const T &right) { return right < left; }
};

enum Color
{ 
	BLACK,
	RED,
};

template<typename T>
struct RBTNode
{
	RBTNode() :color(BLACK) {}
	RBTNode(const T &val, const Color &c = BLACK, RBTNode *const p = nullptr, RBTNode *const l = nullptr, RBTNode *const r = nullptr)
		: key(val), color(c), parent(p), left_child(l), right_child(r) {}
	T key;
	Color color;
	RBTNode *parent, *left_child, *right_child;
};

template<typename T, typename Pred = Ascend<T>>
class RedBlackTree
{
public:
	RedBlackTree() :nil(new RBTNode<T>(0)), root(nullptr), bh(0) {}
	RedBlackTree(std::initializer_list<T> il)
		:RedBlackTree()
	{
		for (auto it = il.begin(); it != il.end(); ++it)
			insertNode(*it);
	}

	~RedBlackTree()
	{
		if (root)
			deconstructAux(root);
		delete nil;
		nil = nullptr;
	}

	void insertNode(const T &key)
	{
		if (root == nullptr)
		{
			root = new RBTNode<T>(key, BLACK, nil, nil, nil);
			return;
		}
		RBTNode<T> *p = root, *pre = p;
		while (p != nil)
		{
			pre = p;
			if (key == p->key)
				return;
			else if (Pred()(key, p->key))
				p = p->left_child;
			else
				p = p->right_child;
		}
		p = new RBTNode<T>(key, RED, pre, nil, nil);
		if (Pred()(key, pre->key))
			pre->left_child = p;
		else
			pre->right_child = p;
		insertFixup(p);
	}

	void deleteNode(const T &key)
	{
		if (root == nullptr)
			return;
		RBTNode<T> *pos = searchNode(key);
		if (pos == nil)
			return;
		RBTNode<T> *x = nullptr;
		RBTNode<T> *y = pos;
		Color y_color = y->color;
		if (pos->left_child == nil)
		{
			x = y->right_child;
			deleteAux(pos, pos->right_child);
		}
		else if (pos->right_child == nil)
		{
			x = pos->left_child;
			deleteAux(pos, pos->left_child);
		}
		else
		{
			y = minimumPtr(pos->right_child);
			y_color = y->color;
			x = y->right_child;
			if (y->parent == pos)
				x->parent = y;
			else
			{
				deleteAux(y, y->right_child);
				y->right_child = pos->right_child;
				y->right_child->parent = y;
			}
			deleteAux(pos, y);
			y->left_child = pos->left_child;
			y->left_child->parent = y;
			y->color = pos->color;
		}
		delete pos;
		pos = nullptr;
		if (y_color == BLACK)
			deleteFixup(x);///this is the most trickiest function
	}

	RBTNode<T>* searchNode(const T &key) const
	{
		RBTNode<T> *p = root;
		if (p == nullptr)
			return nil;
		while (p != nil)
		{
			if (Pred()(key, p->key))
				p = p->left_child;
			else if (Pred()(p->key, key))
				p = p->right_child;
			else
				break;
		}
		return p;
	}

	inline std::pair<T, Color> minimum() const
	{
		auto res = minimumPtr(root);
		return { res->key, res->color };
	}
	inline std::pair<T, Color> maximum() const
	{
		auto res = maximumPtr(root);
		return { res->key, res->color };
	}
	inline RBTNode<T>* minimumPtr(RBTNode<T> *node) const
	{
		while (node != nil && node->left_child != nil)
			node = node->left_child;
		return node;
	}
	inline RBTNode<T>* maximumPtr(RBTNode<T> *node) const
	{
		while (node != nil && node->right_child != nil)
			node = node->right_child;
		return node;
	}

	std::vector<std::pair<T, Color>> printRBTree() const
	{
		std::vector<std::pair<T, Color>> res;
		if (root)
			printRBTreeAux(res, root);
		return res;
	}

	size_t blackHeight() const
	{
		return bh;
	}

private:
	void deleteAux(RBTNode<T> *old_node, RBTNode<T> *new_node)
	{
		if (old_node->parent == nil)
			root = new_node;
		else if (old_node == old_node->parent->left_child)
			old_node->parent->left_child = new_node;
		else
			old_node->parent->right_child = new_node;
		new_node->parent = old_node->parent;
	}

	void insertFixup(RBTNode<T> *node)
	{
		while (node->parent->color == RED)
		{
			if (node->parent == node->parent->parent->left_child)
			{
				RBTNode<T> *p = node->parent->parent->right_child;
				if (p->color == RED)
				{
					node->parent->color = BLACK;
					p->color = BLACK;
					p->parent->color = RED;
					node = p->parent;
				}
				else if (node == node->parent->right_child)
				{
					node = node->parent;
					leftRotate(node);
				}
				else
				{
					node->parent->color = BLACK;
					node->parent->parent->color = RED;
					rightRotate(node->parent->parent);
				}
			}
			else
			{
				RBTNode<T> *p = node->parent->parent->left_child;
				if (p->color == RED)
				{
					node->parent->color = BLACK;
					p->color = BLACK;
					p->parent->color = RED;
					node = p->parent;
				}
				else if (node == node->parent->left_child)
				{
					node = node->parent;
					rightRotate(node);
				}
				else
				{
					node->parent->color = BLACK;
					node->parent->parent->color = RED;
					leftRotate(node->parent->parent);
				}
			}
			root->color = BLACK;
		}
	}

	void deleteFixup(RBTNode<T> *node)///
	{
		while (node != root && node->color == BLACK)
		{
			if (node == node->parent->left_child)
			{
				RBTNode<T> *p = node->parent->right_child;
				if (p->color == RED)
				{
					p->color = BLACK;
					node->parent->color = RED;
					leftRotate(node->parent);
					p = node->parent->right_child;
				}
				if (p->left_child->color == BLACK && p->right_child->color == BLACK)
				{
					p->color = RED;
					node = node->parent;
				}
				else if (p->right_child->color == BLACK)
				{
					p->left_child->color = BLACK;
					p->color = RED;
					rightRotate(p);
					p = node->parent->right_child;
				}
				else
				{
					p->color = node->parent->color;
					node->parent->color = BLACK;
					p->right_child->color = BLACK;
					leftRotate(node->parent);
					node = root;
				}
			}
			else
			{
				RBTNode<T> *p = node->parent->left_child;
				if (p->color == RED)
				{
					p->color = BLACK;
					node->parent->color = RED;
					rightRotate(node->parent);
					p = node->parent->left_child;
				}
				if (p->left_child->color == BLACK && p->right_child->color == BLACK)
				{
					p->color = RED;
					node = node->parent;
				}
				else if (p->left_child->color == BLACK)
				{
					p->right_child->color = BLACK;
					p->color = RED;
					leftRotate(p);
					p = node->parent->left_child;
				}
				else
				{
					p->color = node->parent->color;
					node->parent->color = BLACK;
					p->left_child->color = BLACK;
					rightRotate(node->parent);
					node = root;
				}
			}
		}
		node->color = BLACK;
	}

	void leftRotate(RBTNode<T> *node)
	{
		RBTNode<T> *p = node->right_child;
		node->right_child = p->left_child;
		if (p->left_child != nil)
			p->left_child->parent = node;
		p->parent = node->parent;
		if (node->parent == nil)
			root = p;
		else if (node->parent->left_child == node)
			node->parent->left_child = p;
		else
			node->parent->right_child = p;
		p->left_child = node;
		node->parent = p;
	}
	void rightRotate(RBTNode<T> *node)
	{
		RBTNode<T> *p = node->left_child;
		node->left_child = p->right_child;
		if (p->right_child != nil)
			p->right_child->parent = node;
		p->parent = node->parent;
		if (node->parent == nil)
			root = p;
		else if (node->parent->left_child == node)
			node->parent->left_child = p;
		else
			node->parent->right_child = p;
		p->right_child = node;
		node->parent = p;
	}

	void printRBTreeAux(std::vector<std::pair<T, Color>> &arr, RBTNode<T> *p) const
	{
		std::stack<RBTNode<T>*> aux_s;
		while (p != nil || !aux_s.empty())
		{
			while (p != nil)
			{
				aux_s.push(p);
				p = p->left_child;
			}
			if (!aux_s.empty())
			{
				auto val = aux_s.top();
				aux_s.pop();
				arr.push_back(std::make_pair(val->key, val->color));
				p = val->right_child;
			}
		}
	}
	
	void deconstructAux(RBTNode<T> *node)
	{
		if (node->left_child != nil)
			deconstructAux(node->left_child);
		if (node->right_child != nil)
			deconstructAux(node->right_child);
		delete node;
		node = nullptr;
	}

	RBTNode<T> *nil, *root;
	size_t bh;	///black-height
};

int main()
{
	auto start_time = std::chrono::steady_clock::now();

	RedBlackTree<int> tree{ 11,2,1,7,5,4,8,14,15 };
	tree.deleteNode(7);
	std::cout << "max: " << tree.maximum().first << "(" << tree.maximum().second << ")" << std::endl;
	std::cout << "min: " << tree.minimum().first << "(" << tree.minimum().second << ")" << std::endl;
	auto res = tree.printRBTree();
	std::for_each(res.cbegin(), res.cend(), [](const std::pair<int, Color> &p) {std::cout << p.first << "(" << p.second << ")" << " "; });
	std::cout << std::endl;

	std::cout << "\nThe program run "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).count()
		<< " millisecond" << std::endl;
	system("pause");
	return 0;
}