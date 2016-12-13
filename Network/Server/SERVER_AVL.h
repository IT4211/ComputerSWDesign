//=============================================================
//2016 12 08 �뱤�� �ѿ�Ź c++
//ThreadTCPSever ������Ʈ ���� ���
//AVL Ʈ�� ���� �Լ� ���
//AVL Ʈ�� �ѿ�Ź �ۼ�
//AVL Ʈ�� ���� �뱤�� �ۼ�
//�� �Լ��� ������ �̸� �߰�
//==============================================================
#ifndef AVL_H
#define AVL_H

#include "common.h"

//�ۼ��� �ѿ�Ź
class AvlNode
{
public:                 //   ������ getter�� setter�� ������ ������ �ϴ� �ӽù���.
	int key;
	char menu[MAX_MENU_LENGTH];
	AvlNode *left, *right;
	int height;
	bool point_card;               // ����Ʈī�� ��:1 , ��: 0
	char cal[20];                  // ���ҹ�� ����,ī��
	char nowtime[sizeof "2016-11-22T06:08:09Z"];
public:
	//�ۼ��� �ѿ�Ź, insert�� ������
	AvlNode(int key, char menu[MAX_MENU_LENGTH], int height, bool point_card, char cal[20])
	{
		time_t now;
		time(&now);
		strftime((char *)nowtime, sizeof nowtime, "%Y-%m-%dT%H:%M:%SZ", localtime(&now));
		this->key = key;
		strcpy(this->menu, menu);
		this->height = height;
		this->point_card = point_card;
		strcpy(this->cal, cal);
		left = NULL;
		right = NULL;
	}
	//�ۼ��� �뱤��, load�� ������
	AvlNode(int key, char menu[MAX_MENU_LENGTH], int height, bool point_card, char cal[20], char loadtime[sizeof "9999-12-31T99:99:99Z"])
	{
		strcpy(this->nowtime, loadtime);
		this->key = key;
		strcpy(this->menu, menu);
		this->height = height;
		this->point_card = point_card;
		strcpy(this->cal, cal);
		left = NULL;
		right = NULL;
	}

};


//�ۼ��� �뱤�� ��ȣȭ��
static char *return_menu[] = { "���ۼ�Ʈ","����","����������ũ���ż�Ʈ","����������ũ����","��Ʈ��ġ����ۼ�Ʈ","��Ʈ��ġ�����","�ؽ�ġ����ۼ�Ʈ","�ؽ�ġ�����","�����ִϾƮ","�����ִϾ�",
"�Ұ����ż�Ʈ","�Ұ�����","ġŲũ�����ǹ��ż�Ʈ","ġŲũ�����ǹ���","����ġ������","��Ͼ�","��ī�ݶ�","��ī�ݶ�����","��������Ʈ","�Ƹ޸�ī��","����","ī��" };

//========================�ۼ��� �ѿ�Ź=============================================
AvlNode *right_Rotate(AvlNode *y);               /* ���������� ȸ����Ű�� �Լ� */
AvlNode *left_Rotate(AvlNode *x);               /* �������� ȸ����Ű�� �Լ� */
int height(AvlNode *N);                        /* Ʈ���� ���̸� ��� �Լ� */
int getBalance(AvlNode *N);                     /* ���N�� �����μ� ��ȯ�ϴ� �Լ� */
AvlNode* insert(AvlNode* rootNode, AvlNode* newNode);        /* Avl Ʈ���� ���� �Լ� */
AvlNode* deleteNode(AvlNode* root, int key);             /* Avl Ʈ���� ���� �Լ� */
void updateNode(int x, AvlNode* node,Tcp_dat buf);                   /* Avl Ʈ���� ���� �Լ� */
AvlNode * searchNode(int x, AvlNode* &node);             /* Avl Ʈ���� Ž�� �Լ� */
void preOrder(AvlNode *root);                         /* Avl Ʈ���� ������ȸ(pre-order) �� ��� �Լ� */
AvlNode * minValueNode(AvlNode* node);                    /* ��� ������ �� ���� ���� key���� ã���ִ� �Լ� */
//==================================================================================

//===============================�ۼ��� �뱤��=======================================
//�Է��� ����� ���� Ű���� next_nodekey�� ����
void Next_Node(AvlNode* newnode, int key);
//���� ��� Ű�� return
char* Random_nodekey();
//===================================================================================
#endif // !AVL_H