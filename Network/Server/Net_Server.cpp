//=============================================================
//2016 12 08 노광준 c++
//ThreadTCPSever 프로젝트 관련 파일
//윈속 에러 검출 함수 및 bind,listen,accept함수
//=============================================================
#ifndef NETC
#define NETC

#include "Network.h"


CRITICAL_SECTION cs;	//크리티컬 섹션

//accept 후 client_sock 리턴
int Accept_sock(SOCKET listen_sock) {
	//데이터 통신에 사용할 변수

	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	//accept()
	addrlen = sizeof(clientaddr);
	client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
	if (client_sock == INVALID_SOCKET) {
		err_display("accept()");
		return -1;
	}
	printf("[TCP 서버] 클라이언트 접속 : IP접속 : %s, 포트 번호 : %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	return client_sock;
}


//소켓함수 오류 출력후 종료 
void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(-1);
}

//소켓함수 오류출력
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (LPCTSTR)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

//Bind, listen 수행/오류 체크
int Bind_listen(SOCKET listen_sock)
{
	int retval;


	//bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(9000);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	retval = (bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr)));

	//listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR)err_quit("listen()");

	return retval;
}
#endif NETC