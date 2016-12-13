//=======================================================================
//2016-12-08 �뱤�� c++
//ThreadTcpSever������Ʈ ���� ����
//=======================================================================

#include "common.h"
#include "struct_token.h"
#include "SERVER_AVL.h"
#include "Menu_con.h"
#include "Network.h"

extern bool judge;                     //Ž���� ��� �ش� ���� �����ϸ� true ������ false
extern char ch_str[MAX_MENU_LENGTH];      //������ ��� �ٲ� ���ڿ� �����ϴ� �迭

extern int next_nodekey;
extern AvlNode *root;
extern CRITICAL_SECTION cs;	//ũ��Ƽ�� ����
extern vector<int> nodekey;;


int key = 1; //AVLƮ�� key

#define DB_ERR_FILE_OPEN		-4


//�Է� ���� ���ڿ��� ���� int �� ����
int Commend_str(char* commend)
{
	if (!strcmp(commend, "insert"))
		return 1;
	else if (!strcmp(commend, "delete"))
		return 2;
	else if (!strcmp(commend, "search"))
		return 3;
	else if (!strcmp(commend, "update"))
		return 4;
	else if (!strcmp(commend, "first"))
		return 5;
	else if (!strcmp(commend, "next"))
		return 6;
	else if (!strcmp(commend, "random"))
		return 7;
	else if (!strcmp(commend, "save"))
		return 8;
	else if (!strcmp(commend, "load"))
		return 9;
	else
		return 0;
}

//Thread ���� Ŭ���̾�Ʈ ��û ����
DWORD WINAPI ThClient(LPVOID arg)
{
	SOCKET client_sock = (SOCKET)arg;
	SOCKADDR_IN clientaddr;
	
	Tcp_dat buf;	//����ü
	File_buf file_buf;

	int addrlen;	//Ŭ���̾�Ʈ �ּ�ü ����
	int retval;		//��� ���۱���
	int Tcp_num;
	int price;
	int insert_key = 0;
	char insert_buf[MAX_MENU_LENGTH];;
	AvlNode *newNode;
	memset(&buf, 0, sizeof(buf));
	//Ŭ���̾�Ʈ ���� ���
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);


	ofstream fout;
	ifstream fin;

	cout << "������ ����" << endl;

	while (1) {
		cout << "Ŭ���̾�Ʈ �޼��� ���� ��� ��..." << endl;
		//������ �ޱ�
		retval = recv(client_sock, (char*)&insert_buf, MAX_MENU_LENGTH, 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0)
			break;
		insert_buf[retval] = '\0';	//\n����
		//CriticalSection ����
		EnterCriticalSection(&cs);

		//��ūȭ ����
		buf = tokenize_tcpdat(buf, insert_buf);	
		
		//���Ź��� key���� 0���� ������� ���� -13
		if (atoi(buf.key) < 0) {
			sprintf(insert_buf, "-13");
			printf("[TCP/%s:%d] ��� : %s Ű�� : %s ���� key�� ����  \n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), buf.commend, buf.key);
			retval = send(client_sock, (const char*)insert_buf, strlen(insert_buf), 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");
			}
		}
		else
		{
			//������ ��� �� AVLƮ�� ����
			printf("[TCP/%s:%d] ��� : %s \n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), buf.commend);

			switch (Commend_str(buf.commend))
			{
			case(0):
				//����� �߸����� ���
				cout << buf.commend << "�� �߸��� �Է��Դϴ�." << endl;
				sprintf(insert_buf, "-12");
				retval = send(client_sock, (const char*)insert_buf, strlen(insert_buf), 0);
				if (retval == SOCKET_ERROR) {
					err_display("send()");
					break;
				}
				break;

			case(1):
				//insert ����
				cout << "\n��带 �����մϴ�." << endl;
				if (!root == NULL) {						//Ʈ���� ������ �˻� Ʈ���� �������� ���� ��� �׳� ����.
					searchNode(atoi(buf.key), root);
				}

				if (!judge) {							//�˻������ ������
					if ((Tcp_num = map_at(buf.menu, 1)) == 0)				//���� ã��
					{
						//map ������ �˻� ���и޼��� ������
						sprintf(insert_buf, "-5");
					}
					else
					{
						//�������� ��������
						price = atoi(buf.num)*Tcp_num;

						//�޴� ���ڵ� ����
						sprintf(insert_buf, " ||�ֹ� ���� :\t%s\t���� :\t%s\t���� : %d||", buf.menu, buf.num, price);

					
						//key�� ����
						cout << "AVLƮ���� �����մϴ�." << endl;
						if (!atoi(buf.key))							//buf.key=0 �̸� key�� ������ node ����
						{
							insert_key = key++;
						}
						else                                       //buf.key=0�� �ƴϸ� buf.key�� ������ node����. ���� key<=buf.key�̸� �ƴҶ����� key++
						{
							insert_key = atoi(buf.key);
							while (key <= atoi(buf.key)) {
								key++;
							}
						}
					
						//AVLƮ���� ����
						newNode = new AvlNode(insert_key++, insert_buf, 0, buf.point_card, buf.cal);
						root = insert(root, newNode);
						printf("\n\n");
						cout << "���������� ���ԵǾ����ϴ�.\n" << endl;
						printf("%d %s ����Ʈ ī�� ���� : %d || ���� ��� %s || %s \n", newNode->key, newNode->menu, newNode->point_card, newNode->cal, newNode->nowtime);
						sprintf(insert_buf, "%d %s ����Ʈ ī�� ���� : %d || ���� ��� %s || %s ", newNode->key, newNode->menu, newNode->point_card, newNode->cal, newNode->nowtime);
						nodekey.push_back(newNode->key);	//random key������ ���� nodekey�� key�� ����
					}
				}
				else                          //�˻��� ��尡 ���� ���
				{
					cout << "�ߺ��� Ű �� �Դϴ�." << endl;
					sprintf(insert_buf, "-6");
					judge = false;
				}
				//������ ����
				retval = send(client_sock, (const char*)insert_buf, strlen(insert_buf), 0);
				if (retval == SOCKET_ERROR) {
					err_display("send()");
					break;
				}
				break;

			case(2):
				//delete
				cout << "\n��带 �����մϴ�." << endl;
				//Ʈ�� ��� �˻�
				searchNode(atoi(buf.key), root);
				if (judge) {
					//Ʈ�� ��� ����
					root = deleteNode(root, atoi(buf.key));
					//���� ���� ����
					cout << "���������� �����Ǿ����ϴ�.\n" << endl;
					sprintf(insert_buf, "Ű�� %s ||���������� �����Ǿ����ϴ�.", buf.key);

					nodekey.erase(remove(nodekey.begin(), nodekey.end(), atoi(buf.key)));//random key������ ���� nodekey�� key�� ����

					judge = false;
				}
				else {
					//���� ���� ����
					cout << "������ �����߽��ϴ�.\n" << endl;
					sprintf(insert_buf, "-7");
				}
				//������ ����
				retval = send(client_sock, (const char*)insert_buf, strlen(insert_buf), 0);
				if (retval == SOCKET_ERROR) {
					err_display("send()");
					break;
				}
				break;
			case(3):
				//search
				cout << "\n\t��带 Ž���մϴ�.." << endl;
				newNode = searchNode(atoi(buf.key), root);
				if (judge) {
					//Ž�� ����
					sprintf(insert_buf, "%d %s %d %s %s", newNode->key, newNode->menu, newNode->point_card, newNode->cal, newNode->nowtime);
					judge = false;
				}
				else {
					//Ž�� ����
					cout << "�˻��� �����߽��ϴ�.\n" << endl;
					buf.key[strlen(buf.key)] = 0;
					sprintf(insert_buf, "-8");
				}
				//������ ����
				retval = send(client_sock, (const char*)insert_buf, strlen(insert_buf), 0);
				if (retval == SOCKET_ERROR) {
					err_display("send()");
					break;
				}
				break;

			case 4:
				//update
				cout << "������ ��带 �˻��մϴ�.\n" << endl;

				//Ʈ�� ��� Ž��
				newNode = searchNode(atoi(buf.key), root);
				if (judge)
				{
					//������ ����
					cout << "������ ��带 ã�ҽ��ϴ�.\n" << endl;
					Tcp_num = map_at(buf.menu, 1);				//���� ã��
					price = atoi(buf.num)*Tcp_num;
					sprintf(insert_buf, " || �ֹ� ���� :\t%s\t���� :\t%s\t���� : %d ||", buf.menu, buf.num, price);
					strcpy(buf.menu, insert_buf);
					updateNode(atoi(buf.key), newNode, buf);
					//���� ���� 
					sprintf(insert_buf, "%d %s ����Ʈ ī�� ���� : %d || ���� ��� %s || %s ", newNode->key, newNode->menu, newNode->point_card, newNode->cal, newNode->nowtime);
				}
				else {
					//������ ��� �˻� ����
					cout << "������ ��� �˻��� �����߽��ϴ�.\n" << endl;
					sprintf(insert_buf, "-9");
				}
				//������ ����
				retval = send(client_sock, (const char*)insert_buf, strlen(insert_buf), 0);
				if (retval == SOCKET_ERROR) {
					err_display("send()");
					break;
				}
				break;
			case 5:
				//first
				cout << "ù��° ��带 �����մϴ�." << endl;
				if (root == NULL)
					sprintf(insert_buf, "-3");
				else
					sprintf(insert_buf, "%d", root->key);
				retval = send(client_sock, (const char*)insert_buf, strlen(insert_buf), 0);
				if (retval == SOCKET_ERROR) {
					err_display("send()");
					break;
				}
				break;

			case 6:
				//next
				cout << "���� ��带 �����մϴ�." << endl;
				cout << "���۹��� ��� : " << buf.key << endl;
				buf.key[strlen(buf.key)] = '\n';
				Next_Node(root, atoi(buf.key));	//next��� Ž��
				sprintf(buf.key, "%d", next_nodekey);
				if (next_nodekey < 0) {			//���� �߻���
					judge = false;
					sprintf(buf.key, "-10");
				}
				cout << "���� �� : " << buf.key << endl;
				//������ ����
				retval = send(client_sock, (const char*)buf.key, strlen(buf.key), 0);
				if (retval == SOCKET_ERROR) {
					err_display("send()");
					break;
				}
				next_nodekey = 0;
				break;

			case 7:
				//random
				strcpy(insert_buf, Random_nodekey());
				//������ ������
				retval = send(client_sock, (const char*)insert_buf, strlen(insert_buf), 0);
				if (retval == SOCKET_ERROR) {
					err_display("send()");
					break;
				}
				next_nodekey = 0;
				break;


			case 8:
				//save
				if (root == NULL)	//Ʈ���� ���� ���
				{
					cout << "Ʈ���� ������ϴ�." << endl;
					sprintf(insert_buf, "-3");
				}
				else
				{
					fout.open("data.bin", ios::binary | ios::out);
					if (fout.is_open()) {
						cout << "�����մϴ�." << endl;
						next_nodekey = root->key;	//first��� ��������
						do {
							newNode = searchNode(next_nodekey, root);	//��� �˻�
							if (judge) {								//�˻� ������ ����
	
								//��ūȭ
								buf = tokenize_tcpdat_menu(buf, newNode->menu);
								//��ȣȭ �� ����ü�� �� ����
								
								file_buf.menu = map_at(buf.menu, 0);
								file_buf.num = atoi(buf.num);
								file_buf.cal = map_at(newNode->cal, 0);
								strcpy(file_buf.time, newNode->nowtime);
								file_buf.key = newNode->key;
								file_buf.point_card = newNode->point_card;
								
								//���� ����
								fout.write((char*)&file_buf, sizeof(file_buf));
								Next_Node(root, newNode->key);	//next_nodekey�� ���� ��� Ű�� ����
							}
						} while (!(next_nodekey < 0));
						sprintf(insert_buf, "���� �Ϸ�");
						cout << "���� �Ϸ�" << endl;
					}
					else//���� ���� ����
					{
						cout << "���� ���� ����" << endl;
						sprintf(insert_buf, "-4");
					}
					fout.close();
				}
				//������ ����
				retval = send(client_sock, (const char*)insert_buf, strlen(insert_buf), 0);
				if (retval == SOCKET_ERROR) {
					err_display("send()");
					break;
				}
				break;

			case 9:
				//load
				fin.open("data.bin", ios::in | ios::binary);
				if (fin.is_open()) {
					cout << "������ �ҷ����� �� �Դϴ�." << endl;
					while ((fin.read((char*)&file_buf, sizeof(file_buf)))) {
						price = map_at(return_menu[file_buf.menu], 1)*file_buf.num;
						//�޴� ���ڵ� ����
						sprintf(insert_buf, " ||�ֹ� ���� :\t%s\t���� :\t%d\t���� : %d||", return_menu[file_buf.menu], file_buf.num, price);
						
						//��ȣȭ �� ��� ����
						newNode = new AvlNode(file_buf.key, insert_buf, 0, file_buf.point_card, return_menu[file_buf.cal],file_buf.time);
						//Ʈ���� ����
						
						root = insert(root, newNode);
						printf("%d %s ����Ʈ ī�� ���� : %d || ���� ��� %s || %s \n", newNode->key, newNode->menu, newNode->point_card, newNode->cal, newNode->nowtime);
						//random key�� ���� nodekey ����
						nodekey.push_back(newNode->key);

						if (key <= file_buf.key)
						{
							key = file_buf.key + 1;
						}

					}
					cout << "�ҷ����� �Ϸ�" << endl;
					sprintf(insert_buf, "�ҷ����� �Ϸ�");
				}

				else {
					cout << "���� ���� ����" << endl;
					sprintf(insert_buf, "-4");
				}
				fin.close();
				retval = send(client_sock, (const char*)insert_buf, strlen(insert_buf), 0);

				if (retval == SOCKET_ERROR) {
					err_display("send()");
					break;
				}

				break;
			}
		}
		//CriticalSection ����
		LeaveCriticalSection(&cs);
	}
		closesocket(client_sock);
		printf("[TCP server] Ŭ���̾�Ʈ ���� : TP�ּ�=%s, ��Ʈ��ȣ=%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
		return 0;
}


int main(int argc, char* argv[])
{
	//CriticalSection �ʱ�ȭ
	InitializeCriticalSection(&cs);

	int retval;
	//���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return -1;

	//socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket ()");

	//bind, listen
	retval = Bind_listen(listen_sock);

	SOCKET client_sock;
	HANDLE hThread;
	DWORD ThreadId;
	
	insert_map();
	
	while (1) 
	{
		
		cout << "Ŭ���̾�Ʈ Connect ��� ��..." << endl;
		//accept()
		client_sock = Accept_sock(listen_sock);
		//������ ����
		hThread = CreateThread(NULL, 0, ThClient, (LPVOID)client_sock, 0, &ThreadId);
		if (hThread == NULL)
			printf("[����] ������ ���� ����\n");
		else
			CloseHandle(hThread);
	}
	//closesocket()
	closesocket(listen_sock);
	//CriticalSectin ����
	DeleteCriticalSection(&cs);
	//���� ����
	WSACleanup();
	return 0;
}