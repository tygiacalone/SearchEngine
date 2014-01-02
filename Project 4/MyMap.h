#ifndef MYMAP_INCLUDED
#define MYMAP_INCLUDED
#include <queue>
#include <iostream>
using namespace std;


template <class KeyType, class ValueType>
class MyMap
{
public:
	MyMap()
	{
		root = NULL;
	}

	~MyMap()
	{
		if (root == NULL)
			return;

		queue<Node *> holder;
		holder.push(root);
		
		//Push all nodes and their children and delete them.
		while (!holder.empty())
		{
			Node * temp = holder.front();
			holder.pop();

			if(temp->left != NULL)
				holder.push(temp->left);

			if(temp->right != NULL)
				holder.push(temp->right);

			delete temp;
		}
		root = NULL;
	}

	void clear()
	{
		if (root == NULL)
			return;

		queue<Node *> holder;
		holder.push(root);
		
		//Push all nodes and their children and delete them.
		while (!holder.empty())
		{
			Node * temp = holder.front();
			holder.pop();

			if(temp->left != NULL)
				holder.push(temp->left);

			if(temp->right != NULL)
				holder.push(temp->right);

			delete temp;

		}
		root = NULL;
	}

	int size() const
	{
		int counter = 0;

		if (root == NULL)
			return counter;

		queue<Node *> holder;
		holder.push(root);
		
		//Push all nodes and their children and count them.
		while (!holder.empty())
		{
			Node * temp = holder.front();
			holder.pop();

			counter++;

			if(temp->left != NULL)
				holder.push(temp->left);

			if(temp->right != NULL)
				holder.push(temp->right);
		}

		return counter;  // This compiles, but may not be incorrect
	}

	void associate(const KeyType& key, const ValueType& value)
	{
		if (root == NULL)
		{
			root = new Node;
			root->key = key;
			root->value = value;
			return;
		}

		queue<Node *> holder;
		holder.push(root);
		
		//Push all nodes and their children and search for key.
		while (!holder.empty())
		{
			Node * temp = holder.front();
			holder.pop();

			if (temp->key == key)
			{
				temp->value = value;
				temp->key = key;
				return;
			}

			if(temp->left != NULL)
				holder.push(temp->left);

			if(temp->right != NULL)
				holder.push(temp->right);
		}

		//Push nodes and add to proper place.
		holder.push(root);
		while (!holder.empty())
		{
			Node * temp = holder.front();
			holder.pop();
			

			if (temp->right == NULL && temp->left == NULL)
			{
				if (temp->key > key)
				{
					temp->left = new Node;
					temp->left->key = key;
					temp->left->value = value;
					return;
				}
				else
				{					
					temp->right = new Node;
					temp->right->key = key;
					temp->right->value = value;
					return;
				}
			}	

			if (temp->left == NULL && temp->key > key)
			{
				temp->left = new Node;
				temp->left->key = key;
				temp->left->value = value;
				return;
			}
			else if (temp->right == NULL)
			{
				temp->right = new Node;
				temp->right->key = key;
				temp->right->value = value;
				return;
			}

			if(temp->left != NULL)
				holder.push(temp->left);

			if(temp->right != NULL)
				holder.push(temp->right);
		}

	}

	const ValueType* find(const KeyType& key) const
	{
		Node * temp;

		queue<Node *> holder;
		holder.push(root);
		
		//Push nodes and search for key.
		while (!holder.empty())
		{
			temp = holder.front();
			holder.pop();

			if (temp == NULL)
				return NULL;

			if (temp->key == key)
			{
				return &temp->value;
			}

			if(temp->left != NULL)
				holder.push(temp->left);

			if(temp->right != NULL)
				holder.push(temp->right);
		}

		return NULL;  
	}

	ValueType* find(const KeyType& key)
	{
		// Do not change the implementation of this overload of find
		const MyMap<KeyType,ValueType>* constThis = this;
		return const_cast<ValueType*>(constThis->find(key));
	}

	ValueType* getFirst(KeyType& key)
	{
		if (root == NULL)
			return NULL;

		// Make sure to start with empty queue
		while ( !MapQueue.empty())
		{
			MapQueue.pop();
		}

		//Push children
		if (root->left != NULL)
			MapQueue.push(root->left);
		if (root->right != NULL)
			MapQueue.push(root->right);

		key = root->key;
		return &root->value;  
	}

	ValueType* getNext(KeyType& key)
	{
		Node * next;
		
		if(MapQueue.empty())
			return NULL;

		next = MapQueue.front();
		MapQueue.pop();
		
		//Push children
		if(next->left != NULL)
			MapQueue.push(next->left);
		if(next->right != NULL)
			MapQueue.push(next->right);

		key = next->key;
		
		return &next->value;  
	}

private:

struct Node
{
	Node()
	{
		left = NULL;
		right = NULL;
	}

	KeyType key;
	ValueType value;
	Node * left;
	Node * right;
};

	// Add needed members

MyMap(const MyMap &other);
MyMap &operator=(const MyMap &other);
Node * root;
queue<Node*> MapQueue;
};

#endif // MYMAP_INCLUDED