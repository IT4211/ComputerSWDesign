//=============================================================
//2016 12 08 노광준 c++
//ThreadTCPSever 프로젝트 관련 헤더
//윈속 에러 검출 함수 및 bind,listen,accept함수
//=============================================================

#ifndef NET_H
#define NET_H

#include "common.h"



void err_quit(char *msg);

void err_display(char *msg);

int Bind_listen(SOCKET listen_sock);

int Accept_sock(SOCKET listen_sock);

#endif // !NET_H


