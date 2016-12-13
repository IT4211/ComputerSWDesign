//===========================================================
//������Ʈ ���� 2016-12-06 �뱤�� c++
//TCPClient ������Ʈ ���� ����
//err_return() ���� ���� ����
//Fgets_dat() ���ڿ� �Է��� �ι��� ����
//err_quit() ���� ���� ���� �� ����
//err_display() ���� ���� ���� �� ���
//===========================================================
//������ ȣ�� ������ �ѹ��� ���� �ǵ���
#ifndef _Display
#define _Display

//===========================================================
//======================���� ��� ����=======================
#define err_network -1	//��Ʈ��ũ ����
#define err_get_key -2	//Ű �� �������� ����
#define err_not_tree -3	//������� AVLƮ���� ����
#define err_open_file -4	//���� ���� ����
#define err_not_menu -5	//�߸��� �޴�(���ڵ�)
#define err_exist_key -6	//�ߺ��� Ű��
#define err_delete_node -7	//������ ������
#define err_search_node -8	//�˻��� ������
#define err_update_node -9	//������ ������
#define Last_Node -10	//�Է��� key�� ���ų� ������ key����
#define err_fget_atoi -11	//�Է¹��� ���ڿ��� ���ڰ��ƴ�
#define err_commend -12	//�߸��� ���(���ڵ�)
#define err_negative_key -13	//���� Ű���� �����
//===========================================================
//===========================================================


//error ����
char* err_return(char* fuc, int err)
{
	char err_msg[MAX_MENU_LENGTH];
	err = -err;			//������ �����
	char* return_err[] = { "��Ʈ��ũ ���� �߻�","Ű ���� �������� �� �����߽��ϴ�.","Ʈ���� ã�� �� �����ϴ�.","���� ���� ����","���� �޴��Դϴ�.","�ߺ��� Ű���Դϴ�.","������ �����߽��ϴ�.",
		"�˻��� �����߽��ϴ�.","������ �����߽��ϴ�.","key�� ���ų� ���� ��尡 �����ϴ�.","�߸��� �Է��Դϴ�.","�߸��� ����Դϴ�.","���� Ű���� �Է��ϼ̽��ϴ�." };
	
	sprintf(err_msg, "%s : %s", fuc, return_err[err - 1]);
	cout <<"\t\t::::" <<err_msg << endl;
	return err_msg;
}
//Ű����� ���ڿ� �Է¹��� �� �ι��� ����
int Fgets_dat(char* buf, int length)
{

	int len;

	if (fgets(buf, length, stdin) == NULL)
		return 0;
	len = strlen(buf);
	if (buf[len - 1] == '\n') {
		buf[len - 1] = '\0';
	}
	if (strlen(buf) == 0)
		return 0;
	return len;
}

//���� ���� �� ����
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

//���� ���� �� ���
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s\n", msg, (LPCTSTR)lpMsgBuf);
	LocalFree(lpMsgBuf);
}



#endif // !_Display

