//=============================================================
//2016 12 08 노광준 한우탁 c++
//ThreadTCPSever 프로젝트 관련 파일
//AVL 트리 한우탁 작성
//AVL 트리 보충 노광준 작성
//각 함수에 제작자 이름 추가
//==============================================================

#ifndef AVLC
#define AVLC
#include "struct_token.h"
#include "SERVER_AVL.h"


bool judge=false;                     //탐색할 경우 해당 값이 존재하면 true 없으면 false
char ch_str[MAX_MENU_LENGTH];      //수정할 경우 바꿀 문자열 저장하는 배열
int next_nodekey;
vector<int> nodekey;			//next키값 저장용
AvlNode *root = NULL; //AVL트리 root


//==============================작성자 한우탁==================================
/* 오른쪽으로 회전시키는 함수 */
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

/* 왼쪽으로 회전시키는 함수 */
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

/* 트리의 높이를 얻는 함수 */
int height(AvlNode *N)
{
	if (N == NULL)
		return 0;
	return N->height;
}

/* 노드N의 균형인수 반환하는 함수 */
int getBalance(AvlNode *N)
{
	if (N == NULL)
		return 0;
	return height(N->left) - height(N->right);
}

//Avl 트리의 삽입 함수
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



	// LL상태(Left Left)
	if (balance > 1 && newNode->key < rootNode->left->key)
		return right_Rotate(rootNode);

	// RR상태(Right Right)
	if (balance < -1 && newNode->key > rootNode->right->key)
		return left_Rotate(rootNode);

	// LR상태(Left Right)
	if (balance > 1 && newNode->key > rootNode->left->key)
	{
		rootNode->left = left_Rotate(rootNode->left);
		return right_Rotate(rootNode);
	}

	// RL상태(Right Left)
	if (balance < -1 && newNode->key < rootNode->right->key)
	{
		rootNode->right = right_Rotate(rootNode->right);
		return left_Rotate(rootNode);
	}


	return rootNode;
}

/* Avl 트리의 노드 삭제 함수 */
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

	// LL상태(Left Left)
	if (balance > 1 && getBalance(root->left) >= 0)
		return right_Rotate(root);

	// LR상태(Left Right)
	if (balance > 1 && getBalance(root->left) < 0)
	{
		root->left = left_Rotate(root->left);
		return right_Rotate(root);
	}

	// RR상태(Right Right)
	if (balance < -1 && getBalance(root->right) <= 0)
		return left_Rotate(root);

	// RL상태(Right Left)
	if (balance < -1 && getBalance(root->right) > 0)
	{
		root->right = right_Rotate(root->right);
		return left_Rotate(root);
	}

	return root;
}

/* Avl 트리의 노드 수정 함수 */
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
			printf("%d %s 포인트 카드 여부 : %d || 결재 방식 %s || %s 로\n", node->key, node->menu, node->point_card, node->cal, node->nowtime);
			cout << "성공적으로 수정되었습니다.\n" << endl;
			judge = false;
		}
	}
}

//트리가 비어있지 않을 때, 그 트리에서 가장작은 key값을 찾아서 반환
//트리 전체를 탐색할 필요가 없음
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
		cout << " 찾을 수가 없습니다 " << endl;
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
				cout << " 찾았습니다 " << endl;
				printf("%d %s 포인트 카드 여부 : %d || 결재 방식 %s || %s ", node->key, node->menu,node->point_card, node->cal, node->nowtime);
				cout << endl;
				judge = true;
				return node;
			}
		}
	}
}

/* Avl 트리의 전위순회(pre - order) 및 출력 함수 */
// 전위,중위,후위 중 전위순회(pre-oder)이 앞에서부터 들리기때문에 가장 보기쉽다고 생각함
void preOrder(AvlNode *root)
{
	if (root != NULL)
	{
		printf(" %d %s %s 포인트 카드 : %d 결재 방식 : %s\n", root->key, root->menu, root->nowtime, root->point_card, root->cal);
		preOrder(root->left);
		preOrder(root->right);
	}

}
//=======================================================================================



//=====================================작성자 노광준=================================================

//입력한 노드의 다음 키값을 next_nodekey에 저장
void Next_Node(AvlNode* newnode, int key)
{
	
		if (judge) {	//왼쪽,오른쪽 노드 둘다 없을 경우
			if (newnode != NULL) {//judge가 true이고 newnode가 null값이 아닐경우 next_nodekey에 키값저장
				judge = false;
				next_nodekey =(newnode->key);
			}
			else
				next_nodekey = -10;	//newnode가 null값일 경우 에러
		}	

		if (newnode != NULL)	//트리가 없을 경우 에러
	{
		if (newnode->key == key)	//키값 대조
		{
			if ((newnode->left) != NULL) {	//왼쪽 노드가 존재할 경우
				next_nodekey =((newnode->left)->key);	//next_nodekey에 왼쪽노드 키 저장
			}
			else if ((newnode->right) != NULL) { //오른쪽 노드가 존재할 경우
				next_nodekey =((newnode->right)->key); //next_nodekey에 오른쪽노드 키 저장
			}
			else
				judge = true;	//왼쪽,오른쪽 노드 둘다 없을 경우
		}
		if (newnode != NULL)	//트리 탐색
		{
			Next_Node(newnode->left, key);
			Next_Node(newnode->right, key);
		}
	}

	if (next_nodekey == 0)	//next_nodekey에 null값이 저장됬을경우 에러
		{
			next_nodekey = -10;
		}
}

//랜덤 노드 키값 return
char* Random_nodekey()
{
	char key_buf[MAX_COM] = { 0 };
	if (root == NULL)	//트리가 없을 경우 에러
		return "-3";
	cout << "랜덤 노드를 전송합니다." << endl;
	int i = 0;
	srand((unsigned)time(NULL));	//랜덤 함수
	i = nodekey.size();
	if (i == 0)
		return "-2";
	sprintf(key_buf, "%d", nodekey[rand() % (i)]);	//랜덤 키 가져오기
	cout << "전송 값" << key_buf << endl;
	return key_buf;
}
//=========================================================================================
#endif