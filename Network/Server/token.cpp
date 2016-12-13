//===============================================================
//2016 12 08 �뱤�� c++
//ThreadTCPServer ������Ʈ ���� ����
//��ūȭ �Լ�
//===============================================================

#ifndef tokenc


#define tokenc

#include "struct_token.h"

//��ūȭ �Լ� delimiter�� �������� ��ūȭ
Tokens tokenize(const string& src, const string& delimiter)
{
	// ������ ���Խ����� regex ��ü ����
	regex rgx(delimiter);

	// sregex_token_iterator �� �̿��Ͽ� ��ū ���� ����.
	return Tokens(
		sregex_token_iterator(src.begin(), src.end(), rgx, -1),
		sregex_token_iterator());
}

//���Ź��� ���ڵ带 ��ūȭ�ؼ� ����ü�� ����
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

//�޴� ���ڵ带 ��ūȭ �Ͽ� ����ü�� ����. save�� ���.
Tcp_dat tokenize_tcpdat_menu(Tcp_dat buf, string s)
{
	memset(&buf, NULL, sizeof(buf));
	Tokens T = tokenize(s, "\t");
	sprintf(buf.menu, "%s", T[1].c_str());
	sprintf(buf.num, "%s", T[3].c_str());
	return buf;
}


#endif // !tokenc
