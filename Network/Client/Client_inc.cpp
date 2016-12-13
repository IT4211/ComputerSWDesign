//==================================================
//업데이트 날짜 2016 12 06 노광준 c++
//TCPClient 프로젝트
//#include ".\Common.h"		헤더파일
//#include ".\Display_inc.cpp"		오류검출,문자열입력
//#include ".\Socket.cpp"		winsock관련 network함수
//#include ".\S_OderSend.cpp"	명령문 생성 후 전송
//#include ".\TestOrderSend.cpp"	Network_DBTest에 필요한 명령 제어함수
//+토큰화함수, 레코드 생성, 구분 함수
//==================================================

#ifndef _Cli_inc
#define _Cli_inc

#include ".\Common.h"
#include ".\display_inc.cpp"
#include ".\Socket.cpp"
#include <regex>

//=================================================
//자동으로 명령문,레코드 생성 관련 함수
//=================================================
//랜덤 주문 맵 생성 -프로그램 시작 시 넣어주세요-

//메뉴 자동 생성
const char *menu[20] = {"와퍼","와퍼세트","갈릭스테이크버거","갈릭스테이크버거세트","콰트로치즈와퍼","콰트로치즈와퍼세트","해쉬치즈와퍼","해쉬치즈와퍼세트","와퍼주니어","와퍼주니어세트","불고기버거","불고기버거세트",
			"치킨크리스피버거","치킨크리스피버거세트","프렌치프라이","어니언링","코카콜라","코카콜라제로","스프라이트","아메리카노"};
const char *order[18] = { "insert","insert" ,"insert" ,"insert" ,"insert" ,"insert" ,"insert" ,"insert" ,"insert" ,"insert" ,"insert" ,"insert" ,"insert" ,"insert" ,"delete","search","update","preorder" };
const char *cal[2] = { "현금","카드" };

//랜덤 생성함수 반환 범위 0~i-1
int random(int i)
{
	srand((unsigned)time(NULL));
	return rand() % i;//i는 rand의 최대값 -1
}

//명령별 스위치 제어문
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
#include ".\TestOrderSend.cpp"		//시뮬레이션에선 주석처리

#endif // !_Cli_inc
