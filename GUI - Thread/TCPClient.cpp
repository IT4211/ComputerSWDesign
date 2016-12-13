//===========================================================
//업데이트 일자 2016-12-05 노광준 c++
//TcpClient 프로젝트 main함수
//main() 프로그램 스타트
//main_loop() Client_Service() 반복수행 -에러시 종료
//Client_Service() 메뉴 출력. 명령어 입력받은 후 수행
//===========================================================


//===========================================================
//Client_inc.cpp 정보
//#include ".\Common.h"
//#include ".\Display_inc.cpp"
//#include ".\Socket.cpp"
//#include <regex>
//+토큰화함수, 레코드 생성, 구분 함수
//===========================================================
#include ".\Client_inc.cpp"

//Client 서비스 시작
//명령을 입력받아 해당 명령 수행
int Client_Service(int tcp_sock)
{
	char commend[MAX_COM] = { 0 };
	char char_buf[MAX_MENU_LENGTH] = { 0 };
	int len = 0;
	int err = 0;
	cout << "\t\t::::::::::::::::::::::::::::::::::::::::::::::::" << endl;
	cout << "\t\t::::Enter insert : TCP 랜덤 삽입::::::::::::::::" << endl;
	cout << "\t\t::::Enter delete : TCP 랜덤 삭제:::::::::::" << endl;
	cout << "\t\t::::Enter search : TCP 랜덤 탐색:::::::::::" << endl;
	cout << "\t\t::::Enter update : TCP 랜덤 수정:::::::::::" << endl;
	cout << "\t\t::::Enter preorder : 출력(전위 순회):::::::::::" << endl;
	cout << "\t\t::::Enter first : 첫번째 노드 키:::::::::::" << endl;
	cout << "\t\t::::Enter next : 다음 노드 키 수신:::::::::::" << endl;
	cout << "\t\t::::Enter smul : 시뮬레이션 함수 테스트(랜덤 명령 수행):::::::::::" << endl;
	cout << "\t\t::::Enter autosmul : 자동 시뮬레이션 함수 테스트(랜덤 명령 수행):::::::::::" << endl;
	cout << "\t\t::::Enter random : 랜덤 노드 키 수신:::::::::::" << endl;
	
	cout << "\t\t::::Enter insertNodeTest : 노드 1만개 자동 삽입:::::::::::" << endl;
	cout << "\t\t::::Enter DeleteNodeTest : 노드 1만개 자동 삭제:::::::::::" << endl;
	cout << "\t\t::::Enter oddinsertNode : 홀수 노드 5천개 삽입(1~10000):::::::::::" << endl;
	cout << "\t\t::::Enter eveninsertNode : 짝수 노드 5천개 삽입(1~10000):::::::::::" << endl;
	cout << "\t\t::::Enter oddDeleteNode : 홀수 노드 5천개 삭제(1~10000):::::::::::" << endl;
	cout << "\t\t::::Enter evenDeleteNode : 짝수 노드 5천개 삭제(1~10000):::::::::::" << endl;
	cout << "\t\t::::Enter AVLTest : 노드 삽입, 삭제 무한 반복 테스트(주의! 중지 불가):::::::::::" << endl;
	cout << "\t\t::::Enter allDelete : 전체 노드 삭제:::::::::::" << endl;

	cout << "\t\t::::Enter save : 저장:::::::::::" << endl;
	cout << "\t\t::::Enter load : 파일 로드:::::::::::" << endl;
	cout << "\t\t::::Enter exit : 종료:::::::::::" << endl;
	cout << "\t\t::::::::::::::::::::::::::::::::::::::::::::::::\n" << endl;
	do {
		cout << "\t\t::::>>";
	}
	while ((len = Fgets_dat(commend, MAX_COM) == 0))
		;

	switch (Commend_str(commend))
	{
	case 1:
		if ((err=Create_order(tcp_sock, char_buf))<0) {
			err_return("Create_order",err);
		}
		break;
	case 2:
		if ((err=Com_randomNode(tcp_sock, char_buf))<0)
		{
			err_return("Create_order", err);
		}
		cout << "\t\t::::수신 데이터 : " << char_buf << endl;
		break;
	case 3:
		for (int i = 0; i <= 10000; i++) {
			if ((err=Create_order(tcp_sock, char_buf))<0) {
				err_return("Create_order", err);
			}
		}
		break;
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
		if ((err=fget_Create_order(tcp_sock, commend, char_buf))<0) {
			err_return("fget_Create_order", err);
		}
		break;
	case 19:
		if ((err=Com_Save(tcp_sock, char_buf))<0) {
			err_return("Com_Save", err);
		}
		break;
	case 20:
		if ((err=Com_Load(tcp_sock, char_buf))<0) {
			err_return("Com_Load", err);
		}
		break;
	case 21:
		cout << "\t\t::::종료합니다." << endl;
		return 0;
	}
	return 1;
}

//loop, Client_Service 반복 수행
void main_loop(int tcp_sock)
{
	while (Client_Service(tcp_sock));
}
//메인
int main(int argc, char* argv[])
{
	int tcp_sock;
	tcp_sock = Socket();	//필수
	main_loop(tcp_sock);	
	closesocket(tcp_sock);	//필수
	//윈속 종료
	WSACleanup();			//필수
	return 0;
}
