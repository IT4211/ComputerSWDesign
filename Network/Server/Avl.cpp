//=============================================================
//2016 12 08 �뱤�� �ѿ�Ź c++
//ThreadTCPSever ������Ʈ ���� ����
//AVL Ʈ�� �ѿ�Ź �ۼ�
//AVL Ʈ�� ���� �뱤�� �ۼ�
//�� �Լ��� ������ �̸� �߰�
//==============================================================

#ifndef AVLC
#define AVLC
#include "struct_token.h"
#include "SERVER_AVL.h"


bool judge=false;                     //Ž���� ��� �ش� ���� �����ϸ� true ������ false
char ch_str[MAX_MENU_LENGTH];      //������ ��� �ٲ� ���ڿ� �����ϴ� �迭
int next_nodekey;
vector<int> nodekey;			//nextŰ�� �����
AvlNode *root = NULL; //AVLƮ�� root


//==============================�ۼ��� �ѿ�Ź==================================
/* ���������� ȸ����Ű�� �Լ� */
AvlNode *right_Rotate(AvlNode *y)
{
	AvlNode *x = y->left;
	AvlNode *T2 = x->right;

	x->right = y;
	y->left = T2;

	y->height = max(height(y->left), height(y->right)) + 1;
	x->height = max(height(x->left), height(x->right)) + 1;

	return x;
}

/* �������� ȸ����Ű�� �Լ� */
AvlNode *left_Rotate(AvlNode *x)
{
	AvlNode *y = x->right;
	AvlNode *T2 = y->left;

	y->left = x;
	x->right = T2;

	x->height = max(height(x->left), height(x->right)) + 1;
	y->height = max(height(y->left), height(y->right)) + 1;

	return y;
}

/* Ʈ���� ���̸� ��� �Լ� */
int height(AvlNode *N)
{
	if (N == NULL)
		return 0;
	return N->height;
}

/* ���N�� �����μ� ��ȯ�ϴ� �Լ� */
int getBalance(AvlNode *N)
{
	if (N == NULL)
		return 0;
	return height(N->left) - height(N->right);
}

//Avl Ʈ���� ���� �Լ�
AvlNode* insert(AvlNode* rootNode, AvlNode* newNode)
{
	if (rootNode == NULL) {
		rootNode = newNode;
		return rootNode;
	}

	if (newNode->key < rootNode->key)
		rootNode->left = insert(rootNode->left, newNode);
	else
		rootNode->right = insert(rootNode->right, newNode);


	rootNode->height = max(height(rootNode->left), height(rootNode->right)) + 1;


	int balance = getBalance(rootNode);



	// LL����(Left Left)
	if (balance > 1 && newNode->key < rootNode->left->key)
		return right_Rotate(rootNode);

	// RR����(Right Right)
	if (balance < -1 && newNode->key > rootNode->right->key)
		return left_Rotate(rootNode);

	// LR����(Left Right)
	if (balance > 1 && newNode->key > rootNode->left->key)
	{
		rootNode->left = left_Rotate(rootNode->left);
		return right_Rotate(rootNode);
	}

	// RL����(Right Left)
	if (balance < -1 && newNode->key < rootNode->right->key)
	{
		rootNode->right = right_Rotate(rootNode->right);
		return left_Rotate(rootNode);
	}


	return rootNode;
}

/* Avl Ʈ���� ��� ���� �Լ� */
AvlNode* deleteNode(AvlNode* root, int key)
{
	if (root == NULL)
		return root;

	if (key < root->key)
		root->left = deleteNode(root->left, key);

	else if (key > root->key)
		root->right = deleteNode(root->right, key);

	else
	{
		if ((root->left == NULL) || (root->right == NULL))
		{
			AvlNode *temp = root->left ? root->left : root->right;

			if (temp == NULL)
			{
				temp = root;
				root = NULL;
			}
			else
				*root = *temp;

			free(temp);
		}
		else
		{
			AvlNode* temp = minValueNode(root->right);


			root->key = temp->key;


			root->right = deleteNode(root->right, temp->key);
		}
	}


	if (root == NULL)
		return root;

	root->height = max(height(root->left), height(root->right)) + 1;


	int balance = getBalance(root);

	// LL����(Left Left)
	if (balance > 1 && getBalance(root->left) >= 0)
		return right_Rotate(root);

	// LR����(Left Right)
	if (balance > 1 && getBalance(root->left) < 0)
	{
		root->left = left_Rotate(root->left);
		return right_Rotate(root);
	}

	// RR����(Right Right)
	if (balance < -1 && getBalance(root->right) <= 0)
		return left_Rotate(root);

	// RL����(Right Left)
	if (balance < -1 && getBalance(root->right) > 0)
	{
		root->right = right_Rotate(root->right);
		return left_Rotate(root);
	}

	return root;
}

/* Avl Ʈ���� ��� ���� �Լ� */
void updateNode(int x, AvlNode* node, Tcp_dat buf)
{
	if (x < node->key)
	{
		updateNode(x, node->left,buf);
	}
	else
	{
		if (x > node->key)
		{
			updateNode(x, node->right,buf);
		}
		else
		{
			strcpy(node->menu, buf.menu);
			strcpy(node->cal, buf.cal);
			node->point_card = buf.point_card;
			printf("%d %s ����Ʈ ī�� ���� : %d || ���� ��� %s || %s ��\n", node->key, node->menu, node->point_card, node->cal, node->nowtime);
			cout << "���������� �����Ǿ����ϴ�.\n" << endl;
			judge = false;
		}
	}
}

//Ʈ���� ������� ���� ��, �� Ʈ������ �������� key���� ã�Ƽ� ��ȯ
//Ʈ�� ��ü�� Ž���� �ʿ䰡 ����
AvlNode * minValueNode(AvlNode* node)
{
	AvlNode* current = node;


	while (current->left != NULL)
		current = current->left;

	return current;
}

AvlNode * searchNode(int x, AvlNode* &node)
{
	if (&node->key == NULL)
	{
		cout << " ã�� ���� �����ϴ� " << endl;
		judge = false;
	}

	else
	{
		if (x < node->key)
		{
			searchNode(x, node->left);
		}
		else
		{
			if (x > node->key)
			{
				searchNode(x, node->right);
			}
			else
			{
				cout << " ã�ҽ��ϴ� " << endl;
				printf("%d %s ����Ʈ ī�� ���� : %d || ���� ��� %s || %s ", node->key, node->menu,node->point_card, node->cal, node->nowtime);
				cout << endl;
				judge = true;
				return node;
			}
		}
	}
}

/* Avl Ʈ���� ������ȸ(pre - order) �� ��� �Լ� */
// ����,����,���� �� ������ȸ(pre-oder)�� �տ������� �鸮�⶧���� ���� ���⽱�ٰ� ������
void preOrder(AvlNode *root)
{
	if (root != NULL)
	{
		printf(" %d %s %s ����Ʈ ī�� : %d ���� ��� : %s\n", root->key, root->menu, root->nowtime, root->point_card, root->cal);
		preOrder(root->left);
		preOrder(root->right);
	}

}
//=======================================================================================



//=====================================�ۼ��� �뱤��=================================================

//�Է��� ����� ���� Ű���� next_nodekey�� ����
void Next_Node(AvlNode* newnode, int key)
{
	
		if (judge) {	//����,������ ��� �Ѵ� ���� ���
			if (newnode != NULL) {//judge�� true�̰� newnode�� null���� �ƴҰ�� next_nodekey�� Ű������
				judge = false;
				next_nodekey =(newnode->key);
			}
			else
				next_nodekey = -10;	//newnode�� null���� ��� ����
		}	

		if (newnode != NULL)	//Ʈ���� ���� ��� ����
	{
		if (newnode->key == key)	//Ű�� ����
		{
			if ((newnode->left) != NULL) {	//���� ��尡 ������ ���
				next_nodekey =((newnode->left)->key);	//next_nodekey�� ���ʳ�� Ű ����
			}
			else if ((newnode->right) != NULL) { //������ ��尡 ������ ���
				next_nodekey =((newnode->right)->key); //next_nodekey�� �����ʳ�� Ű ����
			}
			else
				judge = true;	//����,������ ��� �Ѵ� ���� ���
		}
		if (newnode != NULL)	//Ʈ�� Ž��
		{
			Next_Node(newnode->left, key);
			Next_Node(newnode->right, key);
		}
	}

	if (next_nodekey == 0)	//next_nodekey�� null���� ���������� ����
		{
			next_nodekey = -10;
		}
}

//���� ��� Ű�� return
char* Random_nodekey()
{
	char key_buf[MAX_COM] = { 0 };
	if (root == NULL)	//Ʈ���� ���� ��� ����
		return "-3";
	cout << "���� ��带 �����մϴ�." << endl;
	int i = 0;
	srand((unsigned)time(NULL));	//���� �Լ�
	i = nodekey.size();
	if (i == 0)
		return "-2";
	sprintf(key_buf, "%d", nodekey[rand() % (i)]);	//���� Ű ��������
	cout << "���� ��" << key_buf << endl;
	return key_buf;
}
//=========================================================================================
#endif