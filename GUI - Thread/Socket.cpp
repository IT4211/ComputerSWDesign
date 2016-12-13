//=======================================================================
//업데이트 날짜 2016 12 05 노광준 c++
//TCPClient 프로젝트 관련 파일
//winsock 함수
//Network 송수신 함수
//Socket() <<호출시 윈속 생성 초기화 소켓 생성 Connect까지 수행
//ip address 127.0.0.1 고정 
//port 9000 수정 필요시 Common.h의 TCP_PORT 수정
//=======================================================================


#ifndef _SOCKET
#define _SOCKET
#pragma comment(lib, "ws2_32")

//소켓구조체 초기화 후 connect
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

//윈소켓 생성 및 tcp_connect 함수 호출
int Socket()
{
	int retval;
	//원속 초기화
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


//데이터 송신
//buf 송신 버퍼
//return 받은 문자열 길이, return 0 send error
int tcp_send(int retval, SOCKET tcp_sock, char* buf)
{
	retval = send(tcp_sock, (const char*)buf, retval, 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
		return 0;
	}
	return retval;
}

//데이터 수신
//buf 수신 버퍼
//return 받은 문자열 길이, return 0 recv error
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

//명령 전송 후 수신 대기
//char_buf 네트워크 버퍼
//return 0 정상 작동 return -1 네트워크 에러
int order_send(int tcp_sock,char* char_buf)
{
	int retval = 0;
	retval = strlen(char_buf);
	//데이터 송신
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
