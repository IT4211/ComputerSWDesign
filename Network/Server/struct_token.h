//===============================================================
//2016 12 08 노광준 c++
//ThreadTCPServer 프로젝트 관련 헤더
//구조체 및 토큰화 함수 헤더
//===============================================================
#ifndef STR_H
#define STR_H

#include "common.h"

using Tokens = vector<string>;

//파일 io용 구조체
struct File_buf
{
	int menu;
	int num;
	int cal;
	char time[sizeof "2016-11-22T06:08:09Z"];
	bool point_card;
	int key;
};

//수신 데이터 토큰화 후 저장용 구조체
struct Tcp_dat
{
	char key[MAX_COM];
	char menu[MAX_MENU_LENGTH];			// 주문내역
	char num[MAX_NUM];						// 주문갯수
	char commend[MAX_COM];			//명령어 종류
	char cal[MAX_NUM];                  // 지불방식 현금,카드
	bool point_card;               // 포인트카드 유:1 , 무: 0
};

//토큰화
Tokens tokenize(const string& src, const string& delimiter);
//수신 데이터 토큰화 후 구조체 저장
Tcp_dat tokenize_tcpdat(Tcp_dat buf, string s);
//menu 레코드 토큰화 후 구조체에 저장
Tcp_dat tokenize_tcpdat_menu(Tcp_dat buf, string s);

#endif 