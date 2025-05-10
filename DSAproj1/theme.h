#pragma once

#include<iostream>
#include<string>
using namespace std;


struct AVLNode
{
	string themepaths[20];
	AVLNode* left;
	AVLNode* right;
	int themeId;
	int height;

	AVLNode(int id)
	{
		themeId = id;
		left = nullptr;
		right = nullptr;
		height = 1; // Initialize height to 1 for leaf nodes
	}

	string getThemePath(int index)
	{
		if (index >= 0 && index < 20)
			return themepaths[index];

		return "";
	}
};

class AVLTree
{
	AVLNode* root;
public:
	AVLTree()
	{
		root = nullptr;
		initializeOceanTheme();
		initializeForestTheme();
		initializeSpaceTheme();
		cout << root->themeId << endl;
		this->printPreOrder(root);
	}

	int getHeight(AVLNode* node)
	{
		if (node == nullptr)
			return 0;

		return node->height;
	}

	int max(int a, int b)
	{
		return (a > b) ? a : b;
	}

	void updateHeight(AVLNode* node)
	{
		if (node != nullptr)
			node->height = 1 + max(getHeight(node->left), getHeight(node->right));
	}

	int getBalance(AVLNode* node)
	{
		if (node == nullptr)
			return 0;

		return getHeight(node->left) - getHeight(node->right);
	}

	AVLNode* rightRotate(AVLNode* y)
	{
		AVLNode* x = y->left;
		AVLNode* T2 = x->right;

		// Perform rotation
		x->right = y;
		y->left = T2;

		// Update heights
		updateHeight(y);
		updateHeight(x);

		// Return new root
		return x;
	}

	AVLNode* leftRotate(AVLNode* x)
	{
		AVLNode* y = x->right;
		AVLNode* T2 = y->left;

		// Perform rotation
		y->left = x;
		x->right = T2;

		// Update heights
		updateHeight(x);
		updateHeight(y);

		// Return new root
		return y;
	}

	// Recursive insert function that maintains AVL properties
	AVLNode* insertNode(AVLNode* node, AVLNode* newNode)
	{
		// Perform normal BST insertion
		if (node == nullptr)
			return newNode;

		if (newNode->themeId < node->themeId)
			node->left = insertNode(node->left, newNode);
		else if (newNode->themeId > node->themeId)
			node->right = insertNode(node->right, newNode);
		else // Equal keys not allowed
			return node;

		// Update height of this ancestor node
		updateHeight(node);

		// Get the balance factor to check if this node became unbalanced
		int balance = getBalance(node);

		// Left Left Case
		if (balance > 1 && newNode->themeId < node->left->themeId)
			return rightRotate(node);

		// Right Right Case
		if (balance < -1 && newNode->themeId > node->right->themeId)
			return leftRotate(node);

		// Left Right Case
		if (balance > 1 && newNode->themeId > node->left->themeId)
		{
			node->left = leftRotate(node->left);
			return rightRotate(node);
		}

		// Right Left Case
		if (balance < -1 && newNode->themeId < node->right->themeId)
		{
			node->right = rightRotate(node->right);
			return leftRotate(node);
		}

		// Return the (unchanged) node pointer
		return node;
	}

	void insert(AVLNode* newNode)
	{
		// Use recursive insertion instead
		root = insertNode(root, newNode);
	}

	// Find the node with minimum value
	AVLNode* minValueNode(AVLNode* node)
	{
		AVLNode* current = node;

		// Loop down to find the leftmost leaf
		while (current->left != nullptr)
			current = current->left;

		return current;
	}

	// Recursive function to delete a node
	AVLNode* deleteNode(AVLNode* root, int themeId)
	{
		// Standard BST delete
		if (root == nullptr)
			return root;

		// If the key to be deleted is smaller than the root's key,
		// then it lies in left subtree
		if (themeId < root->themeId)
			root->left = deleteNode(root->left, themeId);

		// If the key to be deleted is greater than the root's key,
		// then it lies in right subtree
		else if (themeId > root->themeId)
			root->right = deleteNode(root->right, themeId);

		// If key is same as root's key, then this is the node to be deleted
		else
		{
			// Node with only one child or no child
			if ((root->left == nullptr) || (root->right == nullptr))
			{
				AVLNode* temp = root->left ? root->left : root->right;

				// No child case
				if (temp == nullptr)
				{
					temp = root;
					root = nullptr;
				}
				else // One child case
					*root = *temp; // Copy the contents of the non-empty child

				delete temp;
			}
			else
			{
				// Node with two children: Get the inorder successor
				AVLNode* temp = minValueNode(root->right);

				// Copy the inorder successor's data to this node
				root->themeId = temp->themeId;
				// Copy theme paths as well
				for (int i = 0; i < 20; i++)
					root->themepaths[i] = temp->themepaths[i];

				// Delete the inorder successor
				root->right = deleteNode(root->right, temp->themeId);
			}
		}

		// If the tree had only one node then return
		if (root == nullptr)
			return root;

		// Update height of the current node
		updateHeight(root);

		// Get the balance factor
		int balance = getBalance(root);

		// Left Left Case
		if (balance > 1 && getBalance(root->left) >= 0)
			return rightRotate(root);

		// Left Right Case
		if (balance > 1 && getBalance(root->left) < 0)
		{
			root->left = leftRotate(root->left);
			return rightRotate(root);
		}

		// Right Right Case
		if (balance < -1 && getBalance(root->right) <= 0)
			return leftRotate(root);

		// Right Left Case
		if (balance < -1 && getBalance(root->right) > 0)
		{
			root->right = rightRotate(root->right);
			return leftRotate(root);
		}

		return root;
	}

	void remove(int removeItem)
	{
		root = deleteNode(root, removeItem);
	}

	void printInOrder(AVLNode* root)
	{
		if (root == nullptr)
			return;

		printInOrder(root->left);
		cout << root->themeId << " ";
		printInOrder(root->right);
	}

	void printPreOrder(AVLNode* root)
	{
		if (root == nullptr)
			return;

		cout << root->themeId << " ";
		printPreOrder(root->left);
		printPreOrder(root->right);
	}

	void printPostOrder(AVLNode* root)
	{
		if (root == nullptr)
			return;

		printPostOrder(root->left);
		printPostOrder(root->right);
		cout << root->themeId << " ";
	}

	int findMin()
	{
		AVLNode* temp = root;
		if (temp == nullptr)
			return -1;

		while (temp->left != nullptr)
			temp = temp->left;

		return temp->themeId;
	}

	int findMax()
	{
		AVLNode* temp = root;
		if (temp == nullptr)
			return -1;

		while (temp->right != nullptr)
			temp = temp->right;

		return temp->themeId;
	}

	AVLNode* getRoot()
	{
		return root;
	}

	void initializeOceanTheme()
	{
		AVLNode* oceanTheme = new AVLNode(1);
		oceanTheme->themepaths[0] = "images/Oceantheme.jpg";
		oceanTheme->themepaths [1] = "images/mode.jpg";
		oceanTheme->themepaths[2] = "images/level.jpg";
		oceanTheme->themepaths[3] = "images/leaderboard.jpg";

		insert(oceanTheme);
	}

	void initializeForestTheme()
	{
		AVLNode* forestTheme = new AVLNode(2);
		/*forestTheme->themepaths[0] = "forest_theme_path_1";
		forestTheme->themepaths[1] = "forest_theme_path_2";
		forestTheme->themepaths[2] = "forest_theme_path_3";*/
		insert(forestTheme);
	}

	void initializeSpaceTheme()
	{
		AVLNode* spaceTheme = new AVLNode(3);
		/*spaceTheme->themepaths[0] = "space_theme_path_1";
		spaceTheme->themepaths[1] = "space_theme_path_2";
		spaceTheme->themepaths[2] = "space_theme_path_3";*/
		insert(spaceTheme);
	}

	AVLNode* search(int themeId)
	{
		AVLNode* temp = root;

		while (temp != nullptr)
		{
			if (themeId == temp->themeId)
				return temp;
			else if (themeId < temp->themeId)
				temp = temp->left;
			else
				temp = temp->right;
		}
		return nullptr;
	}

};