//==================================================
//������Ʈ ��¥ 2016 12 06 �뱤�� c++
//TCPClient ������Ʈ
//#include ".\Common.h"		�������
//#include ".\Display_inc.cpp"		��������,���ڿ��Է�
//#include ".\Socket.cpp"		winsock���� network�Լ�
//#include ".\S_OderSend.cpp"	��ɹ� ���� �� ����
//#include ".\TestOrderSend.cpp"	Network_DBTest�� �ʿ��� ��� �����Լ�
//+��ūȭ�Լ�, ���ڵ� ����, ���� �Լ�
//==================================================

#ifndef _Cli_inc
#define _Cli_inc

#include ".\Common.h"
#include ".\display_inc.cpp"
#include ".\Socket.cpp"
#include <regex>

//=================================================
//�ڵ����� ��ɹ�,���ڵ� ���� ���� �Լ�
//=================================================
//���� �ֹ� �� ���� -���α׷� ���� �� �־��ּ���-

//�޴� �ڵ� ����
const char *menu[20] = {"����","���ۼ�Ʈ","����������ũ����","����������ũ���ż�Ʈ","��Ʈ��ġ�����","��Ʈ��ġ����ۼ�Ʈ","�ؽ�ġ�����","�ؽ�ġ����ۼ�Ʈ","�����ִϾ�","�����ִϾƮ","�Ұ�����","�Ұ����ż�Ʈ",
			"ġŲũ�����ǹ���","ġŲũ�����ǹ��ż�Ʈ","����ġ������","��Ͼ�","��ī�ݶ�","��ī�ݶ�����","��������Ʈ","�Ƹ޸�ī��"};
const char *order[18] = { "insert","insert" ,"insert" ,"insert" ,"insert" ,"insert" ,"insert" ,"insert" ,"insert" ,"insert" ,"insert" ,"insert" ,"insert" ,"insert" ,"delete","search","update","preorder" };
const char *cal[2] = { "����","ī��" };

//���� �����Լ� ��ȯ ���� 0~i-1
int random(int i)
{
	srand((unsigned)time(NULL));
	return rand() % i;//i�� rand�� �ִ밪 -1
}

//��ɺ� ����ġ ���
int Commend_str(char* commend)
{
	if (!strcmp(commend, "smul"))
		return 1;
	else if (!strcmp(commend, "random"))
		return 2;
	else if (!strcmp(commend, "autosmul"))
		return 3;
	else if (!strcmp(commend, "insert"))
		return 4;
	else if (!strcmp(commend, "delete"))
		return 5;
	else if (!strcmp(commend, "search"))
		return 6;
	else if (!strcmp(commend, "update"))
		return 7;
	else if (!strcmp(commend, "preorder"))
		return 8;
	else if (!strcmp(commend, "first"))
		return 9;
	else if (!strcmp(commend, "next"))
		return 10;
	else if (!strcmp(commend, "insertNodeTest"))
		return 11;
	else if (!strcmp(commend, "DeleteNodeTest"))
		return 12;
	else if (!strcmp(commend, "eveninsertNode"))
		return 13;
	else if (!strcmp(commend, "oddinsertNode"))
		return 14;
	else if (!strcmp(commend, "evenDeleteNode"))
		return 15;
	else if (!strcmp(commend, "oddDeleteNode"))
		return 16;
	else if (!strcmp(commend, "AVLTest"))
		return 17;
	else if (!strcmp(commend, "allDelete"))
		return 18;
	else if (!strcmp(commend, "save"))
		return 19;
	else if (!strcmp(commend, "load"))
		return 20;
	else if (!strcmp(commend, "exit"))
		return 21;
	else
		return 0;
}


#include ".\S_OderSend.cpp"
#include ".\TestOrderSend.cpp"		//�ùķ��̼ǿ��� �ּ�ó��

#endif // !_Cli_inc
