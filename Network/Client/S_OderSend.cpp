//==============================================================
//������Ʈ 2016 12 06 �뱤�� c++
//TCPClient ������Ʈ ���� ����
//�ùķ��̼ǿ��� ���� ����� �Լ� ����
//���Ե� �Լ� ȣ��� �׿� �´� ���ڵ� ����, ����
//==============================================================

#ifndef _S_Order
#define _S_Order
//==============================================================
//��� ���������Լ�
//Com_randomNode()
//Com_First()
//Com_Next
//==============================================================

//������ Ű���� �����ɴϴ�. 
//char_buf ��Ʈ��ũ ����
//return 0 ������� return err_network ��Ʈ��ũ ���� return err_get_key Ű���� �������µ� ������
int Com_randomNode(int tcp_sock, char* char_buf)
{
	int err = 0;
	cout << "\t\t::::���� ��带 �����ɴϴ�." << endl;
	sprintf(char_buf, "random/1/1/1/1/����");
	if ((err=order_send(tcp_sock, char_buf)) < 0)
		return err;
	if ((err=atoi(char_buf))<0)
	{
		return err;
	}
	return 0;
}

//tree�� �ֻ�� ����� key�� �����ɴϴ�.
//char_buf ��Ʈ��ũ ����
//return 0 ������� return err_network ��Ʈ��ũ ���� return err_not_tree Ʈ���� ã�� �� ����
int Com_First(int tcp_sock, char* char_buf) 
{
	int err = 0;
	sprintf(char_buf, "first/0/0/0/0/0");
	if ((err = order_send(tcp_sock, char_buf)) < 0) {
		return err;
	}
	if ((err=atoi(char_buf)) < 0) {
		return err;
	}
	return 0;
}


//key���� ���� ��� key ���� �����ɴϴ�.
//char_buf ��Ʈ��ũ ����, key ���� Ű��,
//return 0 ���� ���� return err_network ��Ʈ��ũ ���� return Last_Node ������ ���
int Com_Next(int tcp_sock, char* char_buf, int key)
{
	int err = 0;
	sprintf(char_buf, "next/%d/0/0/0/0", key);
	if ((err = order_send(tcp_sock, char_buf)) < 0) {
		return err;
	}
	if ((err = atoi(char_buf)) < 0) {
		return err;
	}
	return 0;
}


//===========================================================================
//AVL Ʈ�� ����Լ�
//===========================================================================


//tree�� ��带 ����ϴ�.
//char_buf ��Ʈ��ũ ����, key delete�� key��.
//return 0 ������� return err_network ��Ʈ��ũ ���� return err_delete_node ���� ����
int Com_Delete(int tcp_sock, char* char_buf, int key) {
	int err = 0;
	cout << "\t\t::::delete �����մϴ�." << endl;
	sprintf(char_buf, "delete/%d/0/0/0/0", key);
	if ((err=order_send(tcp_sock, char_buf)) < 0)
		return err;
	if ((err=atoi(char_buf)) < 0) {
		return err;
	}
	cout << "\t\t::::���� ������ : " << char_buf << endl;
	return 0;
}



//tree�� ��带 �����մϴ�.
//char_buf ��Ʈ��ũ ����, key ������ key��. 0�ϰ�� �ڵ� Ű ����
//return 0 ������� return err_network ��Ʈ��ũ ���� return err_not_menu �߸��� �޴� return err_exist_key �ߺ��� Ű return err_negative_key ���� Ű�� �Է�
int Com_Insert(int tcp_sock, char* char_buf,int key) {
	int err=0;
	cout << "\t\t::::insert �����մϴ�." << endl;
	sprintf(char_buf, "insert/%d/%s/%d/%d/%s",key, menu[random(20)], random(10) + 1, random(2), cal[random(2)]);
	
	if ((err = order_send(tcp_sock, char_buf)) < 0) {
		return err;
	}
	if ((err = atoi(char_buf)) <0) {
		return err;
	}
	cout << "\t\t::::���� ������ : " << char_buf << endl;
	return 0;
}

//tree�� ��带 ã���ϴ�.
//char_buf ��Ʈ��ũ ����, key delete�� key��.
//return 0 ������� return err_network ��Ʈ��ũ ���� return err_search_node �˻� ����
int Com_Search(int tcp_sock, char* char_buf,int key) {
	int err = 0;
	cout << "\t\t::::search �����մϴ�." << endl;
	sprintf(char_buf, "search/%d/0/0/0/0", key);
	
	if ((err = order_send(tcp_sock, char_buf)) < 0) {
		return err;
	}
	if ((err = atoi(char_buf)) <0) {
		return err;
	}

	cout << "\t\t::::���� ������ : " << char_buf << endl;
	return 0;
}

//tree�� ��带 �����մϴ�.
//char_buf ��Ʈ��ũ ����, key delete�� key��.
//return 0 ������� return err_network ��Ʈ��ũ ���� return err_update_node ���� ����
int Com_Update(int tcp_sock, char* char_buf,int key) {
	int err = 0;
	cout << "\t\t::::update �����մϴ�." << endl;
	sprintf(char_buf, "update/%d/%s/%d/%d/%s", key, menu[random(20)], random(10) + 1, random(2), cal[random(2)]);
	if ((err = order_send(tcp_sock, char_buf)) < 0) {
		return err;
	}
	if ((err = atoi(char_buf)) <0) {
		return err;
	}

	cout << "\t\t::::���� ������ : " << char_buf << endl;
	return 0;
}


/*���� first ���� �� �ݺ���
do{
Com_Search()
}while(Com_Next==0)
*/
//Ʈ�� ���� ���(�������)
//char_buf ��Ʈ��ũ ����
//return Com_Search()�� Com_Next()�� ����
int Com_Preorder(int tcp_sock, char* char_buf)
{
	int err = 0;
	int key = 0;
	cout << "\t\t::::��� �����մϴ�." << endl;

	if ((err = Com_First(tcp_sock, char_buf))<0)
	{
		return err;
	}

	do {
		if ((key = atoi(char_buf)) == 0) {
			return err_fget_atoi;
		}	
		//seach ����
		if ((err = Com_Search(tcp_sock, char_buf, key)) < 0)
		{
			return err;
		}
		
	} while ((err = Com_Next(tcp_sock, char_buf, key))==0);//Next�� err�� ���� �Ҷ����� ����
	if (err == Last_Node)
	{
		cout << "\n\t\t::����� �Ϸ��߽��ϴ�.\n" << endl;
		return 0;
	}
	else
		return err;
}

//tree�� ��� ��带 �����մϴ�.
/*
Com_First()
do{
Com_Search()
}while(Com_First==0)
*/
//char_buf ��Ʈ��ũ ����
//return 0 ������� return -1 ��Ʈ��ũ ���� return 3 Ʈ���� ã�� �� ����
int All_Delete(int tcp_sock, char* char_buf)
{
	int err = 0;
	int key = 0;
	cout << "\t\t::::��� ��� �����մϴ�." << endl;
	if ((err = Com_First(tcp_sock, char_buf))<0)
	{
		return err;
	}
	do {
		//key�� �����κ��� �޾ƿ� key�� ����
		if ((key = atoi(char_buf)) == 0) {
			return err_fget_atoi;
		}
		//delete ����
		if ((err = Com_Delete(tcp_sock, char_buf, key)) < 0)
		{
			return err;
		}
	} while ((err = Com_First(tcp_sock, char_buf)) == 0);	//Com_First error�� �߻� �Ҷ����� ����
	if (err == err_not_tree)
	{
		cout << "\n\t\t::������ �Ϸ��߽��ϴ�.\n" << endl;
		return 0;
	}
	else
		return err;
}

//tree�� ��� ��带 �����մϴ�.
//char_buf ��Ʈ��ũ ����
//return 0 ������� return -1 ��Ʈ��ũ ���� return -3 Ʈ���� ã�� �� ���� return -4 ����I/O ����
int Com_Save(int tcp_sock, char* char_buf)
{
	int err = 0;
	cout << "\t\t::::�����մϴ�" << endl;
	sprintf(char_buf, "save/0/0/0/0/0");
	//������ �۽�
	if ((err = order_send(tcp_sock, char_buf)) < 0)
		return err;

	if ((err = atoi(char_buf)) < 0) {
		return err;
	}
	cout << "\t\t::::���� ������ : " << char_buf << endl;
	return 0;
}


//tree�� ��� ��带 �����ϰ� ������ �ҷ��ɴϴ�.
//char_buf ��Ʈ��ũ ����
//All_Delete() ���� �� load �޼��� ����
//return 0 ������� return -1 ��Ʈ��ũ ���� return -3 Ʈ���� ã�� �� ���� return -4 ����I/O ����
int Com_Load(int tcp_sock, char* char_buf)
{
	int err = 0;
	cout <<"\t\t::::�ҷ����� �� Ʈ�� ����" << endl;
	if ((err = All_Delete(tcp_sock, char_buf)) < 0)
		err_return("Com_Load",err);
	cout << "\t\t::::�ҷ��ɴϴ�" << endl;
	sprintf(char_buf, "load/0/0/0/0/0");
	//������ �۽�
	if ((err = order_send(tcp_sock, char_buf)) < 0)
		return err;

	if ((err = atoi(char_buf)) <0) {
		return err;
	}
	cout << "\t\t::::���� ������ : " << char_buf << endl;
	return 0;
}


//�ڵ� ��� ���� insert:delete:search:update:preorder 14:1:1:1 ����
int Create_order(int tcp_sock,char* char_buf)
{
	int err = 0;
	switch (Commend_str((char*)order[random(18)])) {
	case 4://insert
		if ((err=Com_Insert(tcp_sock, char_buf,0))<0)
			return err;
		break;
	case 5://delete
		if ((err=Com_randomNode(tcp_sock, char_buf))<0)		//char_buf�� ������ ��� �� ����
			return err;
		if ((err=Com_Delete(tcp_sock, char_buf,atoi(char_buf)))<0)
			return err;
		break;
	case 6://search
		if ((err=Com_randomNode(tcp_sock, char_buf))<0)	//char_buf�� ������ ��� �� ����
			return err;
		if ((err=Com_Search(tcp_sock, char_buf, atoi(char_buf)))<0)
			return err;
		break;
	case 7://update
		if ((err=Com_randomNode(tcp_sock, char_buf))<0)	//char_buf�� ������ ��� �� ����
			return err;
		if ((err=Com_Update(tcp_sock, char_buf, atoi(char_buf)))<0)
			return err;
		break;
	case 8://preorder
		if ((err=Com_Preorder(tcp_sock, char_buf)) < 0)
			return err;
		break;
	}
	return 0;
}

#endif // !_S_Order