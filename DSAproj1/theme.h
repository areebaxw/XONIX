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
		left = NULL;
		right = NULL;
		height = 1; 
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
	//----------------------------------------Constructor and initialization of themes----------------------------------------
	AVLTree()
	{
		root = NULL;
		initializeOceanTheme();
		initializeForestTheme();
		initializeSpaceTheme();
		cout << root->themeId << endl;
		this->printPreOrder(root);
	}

	//----------------------------------------AVLTree height and balancing methods----------------------------------------
	int getHeight(AVLNode* node)
	{
		if (node == NULL)
			return 0;

		return node->height;
	}

	int max(int a, int b)
	{
		return (a > b) ? a : b;
	}

	void updateHeight(AVLNode* node)
	{
		if (node != NULL)
			node->height = 1 + max(getHeight(node->left), getHeight(node->right));
	}

	int getBalance(AVLNode* node)
	{
		if (node == NULL)
			return 0;

		return getHeight(node->left) - getHeight(node->right);
	}

	//----------------------------------------Rotation methods (left and right)----------------------------------------
	AVLNode* rightRotate(AVLNode* y)
	{
		AVLNode* x = y->left;
		AVLNode* T2 = x->right;

		
		x->right = y;
		y->left = T2;

		
		updateHeight(y);
		updateHeight(x);


		return x;
	}

	AVLNode* leftRotate(AVLNode* x)
	{
		AVLNode* y = x->right;
		AVLNode* T2 = y->left;

		
		y->left = x;
		x->right = T2;

		updateHeight(x);
		updateHeight(y);

		
		return y;
	}

	//----------------------------------------Recursive insert method for AVL balancing----------------------------------------
	AVLNode* insertNode(AVLNode* node, AVLNode* newNode)
	{
	
		if (node == NULL)
			return newNode;

		if (newNode->themeId < node->themeId)
			node->left = insertNode(node->left, newNode);
		else if (newNode->themeId > node->themeId)
			node->right = insertNode(node->right, newNode);
		else 
			return node;

		
		updateHeight(node);


		int balance = getBalance(node);

		if (balance > 1 && newNode->themeId < node->left->themeId)
			return rightRotate(node);

	
		if (balance < -1 && newNode->themeId > node->right->themeId)
			return leftRotate(node);

		if (balance > 1 && newNode->themeId > node->left->themeId)
		{
			node->left = leftRotate(node->left);
			return rightRotate(node);
		}

		if (balance < -1 && newNode->themeId < node->right->themeId)
		{
			node->right = rightRotate(node->right);
			return leftRotate(node);
		}

	
		return node;
	}

	//----------------------------------------Public insert method----------------------------------------
	void insert(AVLNode* newNode)
	{
		// Use recursive insertion instead
		root = insertNode(root, newNode);
	}

	//----------------------------------------Method to find minimum value node----------------------------------------
	AVLNode* minValueNode(AVLNode* node)
	{
		AVLNode* current = node;

		// Loop down to find the leftmost leaf
		while (current->left != NULL)
			current = current->left;

		return current;
	}

	//----------------------------------------Recursive function to delete a node----------------------------------------
	AVLNode* deleteNode(AVLNode* root, int themeId)
	{
		
		if (root == NULL)
			return root;

		
		if (themeId < root->themeId)
			root->left = deleteNode(root->left, themeId);

	
		else if (themeId > root->themeId)
			root->right = deleteNode(root->right, themeId);

		
		else
		{
			
			if ((root->left == NULL) || (root->right == NULL))
			{
				AVLNode* temp = root->left ? root->left : root->right;

			
				if (temp == NULL)
				{
					temp = root;
					root = NULL;
				}
				else 
					*root = *temp; 

				delete temp;
			}
			else
			{
				
				AVLNode* temp = minValueNode(root->right);

			
				root->themeId = temp->themeId;
			
				for (int i = 0; i < 20; i++)
					root->themepaths[i] = temp->themepaths[i];

				
				root->right = deleteNode(root->right, temp->themeId);
			}
		}

		
		if (root == NULL)
			return root;

	
		updateHeight(root);

	
		int balance = getBalance(root);

	
		if (balance > 1 && getBalance(root->left) >= 0)
			return rightRotate(root);


		if (balance > 1 && getBalance(root->left) < 0)
		{
			root->left = leftRotate(root->left);
			return rightRotate(root);
		}


		if (balance < -1 && getBalance(root->right) <= 0)
			return leftRotate(root);

		
		if (balance < -1 && getBalance(root->right) > 0)
		{
			root->right = rightRotate(root->right);
			return leftRotate(root);
		}

		return root;
	}

	//----------------------------------------Public remove method----------------------------------------
	void remove(int removeItem)
	{
		root = deleteNode(root, removeItem);
	}

	//----------------------------------------Methods for tree traversal (InOrder, PreOrder, PostOrder)----------------------------------------
	void printInOrder(AVLNode* root)
	{
		if (root == NULL)
			return;

		printInOrder(root->left);
		cout << root->themeId << " ";
		printInOrder(root->right);
	}

	void printPreOrder(AVLNode* root)
	{
		if (root == NULL)
			return;

		cout << root->themeId << " ";
		printPreOrder(root->left);
		printPreOrder(root->right);
	}

	void printPostOrder(AVLNode* root)
	{
		if (root == NULL)
			return;

		printPostOrder(root->left);
		printPostOrder(root->right);
		cout << root->themeId << " ";
	}

	//----------------------------------------Methods to find Min and Max value nodes----------------------------------------
	int findMin()
	{
		AVLNode* temp = root;
		if (temp == NULL)
			return -1;

		while (temp->left != NULL)
			temp = temp->left;

		return temp->themeId;
	}

	int findMax()
	{
		AVLNode* temp = root;
		if (temp == NULL)
			return -1;

		while (temp->right != NULL)
			temp = temp->right;

		return temp->themeId;
	}


	AVLNode* getRoot()
	{
		return root;
	}

	//----------------------------------------Theme initialization methods----------------------------------------
	void initializeOceanTheme()
	{
		AVLNode* oceanTheme = new AVLNode(1);
		oceanTheme->themepaths[0] = "images/Oceantheme.jpg";
		oceanTheme->themepaths[1] = "images/mode.jpg";
		oceanTheme->themepaths[2] = "images/level.jpg";
		oceanTheme->themepaths[3] = "images/leaderboard.jpg";
		oceanTheme->themepaths[4] = "images/friends.jpg";
		oceanTheme->themepaths[5] = "images/player.jpg";
		insert(oceanTheme);
	}

	void initializeForestTheme()

	{

		AVLNode* forestTheme = new AVLNode(2);

		forestTheme->themepaths[0] = "images/jungle.jpg";

		forestTheme->themepaths[1] = "images/mode1.jpg";

		forestTheme->themepaths[2] = "images/level1.jpg";

		forestTheme->themepaths[3] = "images/leaderboard1.jpg";

		forestTheme->themepaths[4] = "images/friends2.jpg";
		forestTheme->themepaths[5] = "images/player2.jpg";


		insert(forestTheme);

	}



	void initializeSpaceTheme()

	{

		AVLNode* spaceTheme = new AVLNode(3);

		spaceTheme->themepaths[0] = "images/spooky.jpg";

		spaceTheme->themepaths[1] = "images/mode2.jpg";

		spaceTheme->themepaths[2] = "images/level2.jpg";

		spaceTheme->themepaths[3] = "images/leaderboard2.jpg";

		spaceTheme->themepaths[4] = "images/friends1.jpg";
		spaceTheme->themepaths[5] = "images/player1.jpg";


		insert(spaceTheme);

	}

	//----------------------------------------Search method for a theme----------------------------------------
	AVLNode* search(int themeId)
	{
		AVLNode* temp = root;

		while (temp != NULL)
		{
			if (themeId == temp->themeId)
				return temp;
			else if (themeId < temp->themeId)
				temp = temp->left;
			else
				temp = temp->right;
		}
		return NULL;
	}

};
