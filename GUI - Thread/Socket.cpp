//=======================================================================
//������Ʈ ��¥ 2016 12 05 �뱤�� c++
//TCPClient ������Ʈ ���� ����
//winsock �Լ�
//Network �ۼ��� �Լ�
//Socket() <<ȣ��� ���� ���� �ʱ�ȭ ���� ���� Connect���� ����
//ip address 127.0.0.1 ���� 
//port 9000 ���� �ʿ�� Common.h�� TCP_PORT ����
//=======================================================================


#ifndef _SOCKET
#define _SOCKET
#pragma comment(lib, "ws2_32")

//���ϱ���ü �ʱ�ȭ �� connect
int tcp_connect(SOCKET tcp_sock)
{
	int retval;
	//connect()
	SOCKADDR_IN serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(TCP_PORT);
	serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	retval = connect(tcp_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");
	return retval;
}

//������ ���� �� tcp_connect �Լ� ȣ��
int Socket()
{
	int retval;
	//���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return -1;
	//socket()
	SOCKET tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
	int sock = tcp_sock;
	if (tcp_sock == INVALID_SOCKET) err_quit("socket ()");

	//connect()
	retval = tcp_connect(tcp_sock);
	return tcp_sock;
}


//������ �۽�
//buf �۽� ����
//return ���� ���ڿ� ����, return 0 send error
int tcp_send(int retval, SOCKET tcp_sock, char* buf)
{
	retval = send(tcp_sock, (const char*)buf, retval, 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
		return 0;
	}
	return retval;
}

//������ ����
//buf ���� ����
//return ���� ���ڿ� ����, return 0 recv error
int tcp_recv(SOCKET tcp_sock, char* buf, int len) {
	int retval = 0;
	retval = recv(tcp_sock, buf, len, 0);
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
		return 0;
	}
	buf[retval] = 0;
	return retval;
}

//��� ���� �� ���� ���
//char_buf ��Ʈ��ũ ����
//return 0 ���� �۵� return -1 ��Ʈ��ũ ����
int order_send(int tcp_sock,char* char_buf)
{
	int retval = 0;
	retval = strlen(char_buf);
	//������ �۽�
	if ((retval = tcp_send(retval, tcp_sock, char_buf)) == 0) {
		err_display("send");
		return -1;
	}
	if ((retval = tcp_recv(tcp_sock, char_buf, MAX_MENU_LENGTH)) == 0) {
		err_display("recv");
		return -1;
	}
	return 0;
}

#endif // !_SOCKET
