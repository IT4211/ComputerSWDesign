//=============================================================
//2016 12 08 �뱤�� c++
//ThreadTCPSever ������Ʈ ���� ����
//���� ���� ���� �Լ� �� bind,listen,accept�Լ�
//=============================================================
#ifndef NETC
#define NETC

#include "Network.h"


CRITICAL_SECTION cs;	//ũ��Ƽ�� ����

//accept �� client_sock ����
int Accept_sock(SOCKET listen_sock) {
	//������ ��ſ� ����� ����

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
	printf("[TCP ����] Ŭ���̾�Ʈ ���� : IP���� : %s, ��Ʈ ��ȣ : %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	return client_sock;
}


//�����Լ� ���� ����� ���� 
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

//�����Լ� �������
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

//Bind, listen ����/���� üũ
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