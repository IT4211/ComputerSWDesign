//====================================================================
//2016 12 08 �뱤��  c++
//ThreadTcpServer ������Ʈ ��������
//������� ���� �� ��� ���� ����
//===================================================================

#ifndef COM_H
#define COM_H

#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <winsock2.h>
#include <map>
#include <tuple>
#include <regex>


using namespace std;

#define max(a,b) ((a)>(b) ? (a) : (b))
#define MAX_MENU_LENGTH 1000
#define MAX_NUM 20 //�ֹ� ���� ���ڿ� ����
#define MAX_COM 10 //AVLƮ�� ��ɾ�

//���� ���� �� ����
#define err_network -1
#define err_get_key -2
#define err_not_tree -3
#define err_open_file -4
#define err_not_menu -5
#define err_exist_key -6
#define err_delete_node -7
#define err_search_node -8
#define err_update_node -9
#define Last_Node -10
#define err_fget_atoi -11
#define err_commend -12
#define err_negative_key -13

#endif