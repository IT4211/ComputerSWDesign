//===================================================================
//������Ʈ ��¥ 2016 12 06 �뱤�� c++
//TCPClient ������Ʈ ���� ����
//fget_Create_order() Network_DBTest�� �ʿ��� ��� �����Լ�
//===================================================================

#ifndef _TestOrder
#define _TestOrder

//buf : ��ɾ�		char_buf : ��Ʈ��ũ ����
//return 0 �����۵� return -x displat_inc ����
int fget_Create_order(int tcp_sock, char* buf,char* char_buf)
{
	int retval = 0;
	int key = 0;
	int err = 0;
	int len = 0;
	switch (Commend_str(buf)) {
	case 4:
		cout << "\t\t::::insert �����մϴ�." << endl;
		cout << "\t\t::::������ key���� �Է����ּ���.(0 = �ڵ� Ű������)" << endl;
		do {
			cout << "\t\t::::>>";
		} while ((len = Fgets_dat(char_buf, MAX_COM) == 0))
			;
		key = atoi(char_buf);
		if ((err = Com_Insert(tcp_sock, char_buf, key)) < 0)
			return err;
		break;
	case 5:
		cout << "\t\t::::delete �����մϴ�." << endl;
		cout << "\t\t::::������ ����� key���� �Է����ּ���" << endl;
		do {
			cout << "\t\t::::>>";
		} while ((len = Fgets_dat(char_buf, MAX_COM) == 0))
			;
		if ((key = atoi(char_buf)) == 0)
			return err_fget_atoi;
		if ((err = Com_Delete(tcp_sock, char_buf, key)) < 0)
			return err;
		break;
	case 6:
		cout << "\t\t::::search �����մϴ�." << endl;
		cout << "\t\t::::�˻��� ����� key���� �Է����ּ���" << endl;
		do {
			cout << "\t\t::::>>";
		} while ((len = Fgets_dat(char_buf, MAX_COM) == 0))
			;
		if ((key = atoi(char_buf)) == 0)
			return err_fget_atoi;
		if ((err = Com_Search(tcp_sock, char_buf, key)) < 0)
			return err;
		break;
	case 7:
		cout << "\t\t::::update �����մϴ�." << endl;
		cout << "\t\t::::������ ����� key���� �Է����ּ���" << endl;
		do {
			cout << "\t\t::::>>";
		} while ((len = Fgets_dat(char_buf, MAX_COM) == 0))
			;
		if ((key = atoi(char_buf)) == 0)
			return err_fget_atoi;
		if ((err = Com_Update(tcp_sock, char_buf, key)) < 0)
			return err;
		break;
	case 8:
		cout << "\t\t::::preorder �����մϴ�." << endl;
		if ((err=Com_Preorder(tcp_sock, char_buf)) < 0)
			return err;
		break;
	case 9:
		cout << "\t\t::::first �����մϴ�." << endl;
		if ((err = Com_First(tcp_sock, char_buf)) < 0)
			return err;
		cout << "\t\t::::���� ������ : " << char_buf << endl;
		break;
	case 10:
		cout << "\t\t::::next �����մϴ�." << endl;
		cout << "\t\t::::Ű ���� �Է����ּ���." << endl;
		do {
			cout << "\t\t::::>>";
		} while ((len = Fgets_dat(char_buf, MAX_COM) == 0))
			;
		if ((key = atoi(char_buf)) == 0)
			return err_fget_atoi;
		if ((err = Com_Next(tcp_sock, char_buf, key)) < 0)
			return err;
		cout << "\t\t::::���� ������ : " << char_buf << endl;
		break;
	case 11:
		cout <<"\t\t::::"<< "��� 1���� �����մϴ�." << endl;
		for (int i = 0; i < 10000; i++) {
			cout << "\t\t::::insert �����մϴ�." << endl;
			if ((err = Com_Insert(tcp_sock, char_buf, 0)) < 0)
				return err;
		}
		cout <<"\t\t::::"<< "1���� ���� �Ϸ�" << endl;
		break;
	case 12:
		cout <<"\t\t::::"<< "��� 1���� �����մϴ�." << endl;
		for (int i = 0; i < 10000; i++) {
			if ((err = Com_randomNode(tcp_sock, char_buf)) < 0)	//char_buf=������ Ű��
				return err;
			if ((key = atoi(char_buf)) == 0)
				return err_fget_atoi;
			if ((err = Com_Delete(tcp_sock, char_buf, key)) < 0)
				return err;
		}
		cout <<"\t\t::::"<< "1���� ���� �Ϸ�" << endl;
		break;
	case 13:
		cout <<"\t\t::::"<<"¦�� ��� 5õ�� �����մϴ�." << endl;
		for (int i = 1; i <= 5000; i++) {
			if ((err = Com_Insert(tcp_sock, char_buf, i*2)) < 0)
				return err;
		}
		cout <<"\t\t::::"<< "¦�� ��� 5õ�� ���ԿϷ�" << endl;
		break;
	case 14:
		cout <<"\t\t::::"<< "Ȧ�� ��� 5õ�� �����մϴ�." << endl;
		for (int i = 1; i < 10000; i = i + 2)
		{
			if ((err = Com_Insert(tcp_sock, char_buf, i)) < 0)
				return err;
		}
		cout <<"\t\t::::"<< "Ȧ�� ��� 5õ�� ���ԿϷ�" << endl;
		break;
	case 15:
		cout <<"\t\t::::"<< "¦�� ��� 5õ�� �����մϴ�." << endl;
		for (int i = 1; i <= 5000; i = i++)
		{
			if ((err = Com_Delete(tcp_sock, char_buf, i*2)) < 0)
				return err;
		}
		cout <<"\t\t::::"<<"¦�� ��� 5õ�� �����Ϸ�" << endl;
		break;
	case 16:
		cout <<"\t\t::::"<< "Ȧ�� ��� 5õ�� �����մϴ�." << endl;
		for (int i = 1; i < 10000; i = i + 2)
		{
			if ((err = Com_Delete(tcp_sock, char_buf, i)) < 0)
				return err;
		}
		cout <<"\t\t::::"<< "Ȧ�� ��� 5õ�� ���ԿϷ�" << endl;
		break;
	case 17:
		while (1) {
			fget_Create_order(tcp_sock, "insertNodeTest", char_buf);
			fget_Create_order(tcp_sock, "DeleteNodeTest", char_buf);
			fget_Create_order(tcp_sock, "oddinsertNode", char_buf);
			fget_Create_order(tcp_sock, "oddDeleteNode", char_buf);
			fget_Create_order(tcp_sock, "eveninsertNode", char_buf);
			fget_Create_order(tcp_sock, "evenDeleteNode", char_buf);
		}
		break;
	case 18:
		cout <<"\t\t::::"<< "Ʈ���� ���� �����մϴ�." << endl;
		if ((err=All_Delete(tcp_sock, char_buf))< 0)
			return err;
		break;
	}
	return 0;
}

#endif // !_TestOrder
