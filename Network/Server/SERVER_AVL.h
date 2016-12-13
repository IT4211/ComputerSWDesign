//=============================================================
//2016 12 08 노광준 한우탁 c++
//ThreadTCPSever 프로젝트 관련 헤더
//AVL 트리 관련 함수 헤더
//AVL 트리 한우탁 작성
//AVL 트리 보충 노광준 작성
//각 함수에 제작자 이름 추가
//==============================================================
#ifndef AVL_H
#define AVL_H

#include "common.h"

//작성자 한우탁
class AvlNode
{
public:                 //   원래는 getter와 setter를 만들어야 하지만 일단 임시방편.
	int key;
	char menu[MAX_MENU_LENGTH];
	AvlNode *left, *right;
	int height;
	bool point_card;               // 포인트카드 유:1 , 무: 0
	char cal[20];                  // 지불방식 현금,카드
	char nowtime[sizeof "2016-11-22T06:08:09Z"];
public:
	//작성자 한우탁, insert용 생성자
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
	//작성자 노광준, load용 생성자
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


//작성자 노광준 복호화용
static char *return_menu[] = { "와퍼세트","와퍼","갈릭스테이크버거세트","갈릭스테이크버거","콰트로치즈와퍼세트","콰트로치즈와퍼","해쉬치즈와퍼세트","해쉬치즈와퍼","와퍼주니어세트","와퍼주니어",
"불고기버거세트","불고기버거","치킨크리스피버거세트","치킨크리스피버거","프렌치프라이","어니언링","코카콜라","코카콜라제로","스프라이트","아메리카노","현금","카드" };

//========================작성자 한우탁=============================================
AvlNode *right_Rotate(AvlNode *y);               /* 오른쪽으로 회전시키는 함수 */
AvlNode *left_Rotate(AvlNode *x);               /* 왼쪽으로 회전시키는 함수 */
int height(AvlNode *N);                        /* 트리의 높이를 얻는 함수 */
int getBalance(AvlNode *N);                     /* 노드N의 균형인수 반환하는 함수 */
AvlNode* insert(AvlNode* rootNode, AvlNode* newNode);        /* Avl 트리의 삽입 함수 */
AvlNode* deleteNode(AvlNode* root, int key);             /* Avl 트리의 삭제 함수 */
void updateNode(int x, AvlNode* node,Tcp_dat buf);                   /* Avl 트리의 수정 함수 */
AvlNode * searchNode(int x, AvlNode* &node);             /* Avl 트리의 탐색 함수 */
void preOrder(AvlNode *root);                         /* Avl 트리의 전위순회(pre-order) 및 출력 함수 */
AvlNode * minValueNode(AvlNode* node);                    /* 노드 삭제할 시 가장 작은 key값을 찾아주는 함수 */
//==================================================================================

//===============================작성자 노광준=======================================
//입력한 노드의 다음 키값을 next_nodekey에 저장
void Next_Node(AvlNode* newnode, int key);
//랜덤 노드 키값 return
char* Random_nodekey();
//===================================================================================
#endif // !AVL_H