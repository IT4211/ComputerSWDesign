//===============================================================
//2016 12 08 �뱤�� c++
//ThreadTCPServer ������Ʈ ���� ���
//����ü �� ��ūȭ �Լ� ���
//===============================================================
#ifndef STR_H
#define STR_H

#include "common.h"

using Tokens = vector<string>;

//���� io�� ����ü
struct File_buf
{
	int menu;
	int num;
	int cal;
	char time[sizeof "2016-11-22T06:08:09Z"];
	bool point_card;
	int key;
};

//���� ������ ��ūȭ �� ����� ����ü
struct Tcp_dat
{
	char key[MAX_COM];
	char menu[MAX_MENU_LENGTH];			// �ֹ�����
	char num[MAX_NUM];						// �ֹ�����
	char commend[MAX_COM];			//��ɾ� ����
	char cal[MAX_NUM];                  // ���ҹ�� ����,ī��
	bool point_card;               // ����Ʈī�� ��:1 , ��: 0
};

//��ūȭ
Tokens tokenize(const string& src, const string& delimiter);
//���� ������ ��ūȭ �� ����ü ����
Tcp_dat tokenize_tcpdat(Tcp_dat buf, string s);
//menu ���ڵ� ��ūȭ �� ����ü�� ����
Tcp_dat tokenize_tcpdat_menu(Tcp_dat buf, string s);

#endif 