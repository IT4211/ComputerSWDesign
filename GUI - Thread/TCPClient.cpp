//===========================================================
//������Ʈ ���� 2016-12-05 �뱤�� c++
//TcpClient ������Ʈ main�Լ�
//main() ���α׷� ��ŸƮ
//main_loop() Client_Service() �ݺ����� -������ ����
//Client_Service() �޴� ���. ��ɾ� �Է¹��� �� ����
//===========================================================


//===========================================================
//Client_inc.cpp ����
//#include ".\Common.h"
//#include ".\Display_inc.cpp"
//#include ".\Socket.cpp"
//#include <regex>
//+��ūȭ�Լ�, ���ڵ� ����, ���� �Լ�
//===========================================================
#include ".\Client_inc.cpp"

//Client ���� ����
//����� �Է¹޾� �ش� ��� ����
int Client_Service(int tcp_sock)
{
	char commend[MAX_COM] = { 0 };
	char char_buf[MAX_MENU_LENGTH] = { 0 };
	int len = 0;
	int err = 0;
	cout << "\t\t::::::::::::::::::::::::::::::::::::::::::::::::" << endl;
	cout << "\t\t::::Enter insert : TCP ���� ����::::::::::::::::" << endl;
	cout << "\t\t::::Enter delete : TCP ���� ����:::::::::::" << endl;
	cout << "\t\t::::Enter search : TCP ���� Ž��:::::::::::" << endl;
	cout << "\t\t::::Enter update : TCP ���� ����:::::::::::" << endl;
	cout << "\t\t::::Enter preorder : ���(���� ��ȸ):::::::::::" << endl;
	cout << "\t\t::::Enter first : ù��° ��� Ű:::::::::::" << endl;
	cout << "\t\t::::Enter next : ���� ��� Ű ����:::::::::::" << endl;
	cout << "\t\t::::Enter smul : �ùķ��̼� �Լ� �׽�Ʈ(���� ��� ����):::::::::::" << endl;
	cout << "\t\t::::Enter autosmul : �ڵ� �ùķ��̼� �Լ� �׽�Ʈ(���� ��� ����):::::::::::" << endl;
	cout << "\t\t::::Enter random : ���� ��� Ű ����:::::::::::" << endl;
	
	cout << "\t\t::::Enter insertNodeTest : ��� 1���� �ڵ� ����:::::::::::" << endl;
	cout << "\t\t::::Enter DeleteNodeTest : ��� 1���� �ڵ� ����:::::::::::" << endl;
	cout << "\t\t::::Enter oddinsertNode : Ȧ�� ��� 5õ�� ����(1~10000):::::::::::" << endl;
	cout << "\t\t::::Enter eveninsertNode : ¦�� ��� 5õ�� ����(1~10000):::::::::::" << endl;
	cout << "\t\t::::Enter oddDeleteNode : Ȧ�� ��� 5õ�� ����(1~10000):::::::::::" << endl;
	cout << "\t\t::::Enter evenDeleteNode : ¦�� ��� 5õ�� ����(1~10000):::::::::::" << endl;
	cout << "\t\t::::Enter AVLTest : ��� ����, ���� ���� �ݺ� �׽�Ʈ(����! ���� �Ұ�):::::::::::" << endl;
	cout << "\t\t::::Enter allDelete : ��ü ��� ����:::::::::::" << endl;

	cout << "\t\t::::Enter save : ����:::::::::::" << endl;
	cout << "\t\t::::Enter load : ���� �ε�:::::::::::" << endl;
	cout << "\t\t::::Enter exit : ����:::::::::::" << endl;
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
		cout << "\t\t::::���� ������ : " << char_buf << endl;
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
		cout << "\t\t::::�����մϴ�." << endl;
		return 0;
	}
	return 1;
}

//loop, Client_Service �ݺ� ����
void main_loop(int tcp_sock)
{
	while (Client_Service(tcp_sock));
}
//����
int main(int argc, char* argv[])
{
	int tcp_sock;
	tcp_sock = Socket();	//�ʼ�
	main_loop(tcp_sock);	
	closesocket(tcp_sock);	//�ʼ�
	//���� ����
	WSACleanup();			//�ʼ�
	return 0;
}
