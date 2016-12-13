//===============================================================
//2016 12 08 노광준 c++
//ThreadTCPServer 프로젝트 관련 파일
//토큰화 함수
//===============================================================

#ifndef tokenc


#define tokenc

#include "struct_token.h"

//토큰화 함수 delimiter을 기준으로 토큰화
Tokens tokenize(const string& src, const string& delimiter)
{
	// 구분자 정규식으로 regex 객체 생성
	regex rgx(delimiter);

	// sregex_token_iterator 를 이용하여 토큰 벡터 생성.
	return Tokens(
		sregex_token_iterator(src.begin(), src.end(), rgx, -1),
		sregex_token_iterator());
}

//수신받은 레코드를 토큰화해서 구조체에 저장
Tcp_dat tokenize_tcpdat(Tcp_dat buf, string s)
{
	Tokens T = tokenize(s, "/");
	sprintf(buf.commend, "%s", T[0].c_str());
	sprintf(buf.key, "%s", T[1].c_str());
	sprintf(buf.menu, "%s", T[2].c_str());
	sprintf(buf.num, "%s", T[3].c_str());
	buf.point_card = atoi(T[4].c_str());
	sprintf(buf.cal, "%s", T[5].c_str());
	return buf;
}

//메뉴 레코드를 토큰화 하여 구조체에 저장. save시 사용.
Tcp_dat tokenize_tcpdat_menu(Tcp_dat buf, string s)
{
	memset(&buf, NULL, sizeof(buf));
	Tokens T = tokenize(s, "\t");
	sprintf(buf.menu, "%s", T[1].c_str());
	sprintf(buf.num, "%s", T[3].c_str());
	return buf;
}


#endif // !tokenc
