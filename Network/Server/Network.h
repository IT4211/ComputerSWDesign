//=============================================================
//2016 12 08 �뱤�� c++
//ThreadTCPSever ������Ʈ ���� ���
//���� ���� ���� �Լ� �� bind,listen,accept�Լ�
//=============================================================

#ifndef NET_H
#define NET_H

#include "common.h"



void err_quit(char *msg);

void err_display(char *msg);

int Bind_listen(SOCKET listen_sock);

int Accept_sock(SOCKET listen_sock);

#endif // !NET_H


