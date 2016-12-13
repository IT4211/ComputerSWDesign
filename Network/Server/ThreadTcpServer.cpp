//=======================================================================
//2016-12-08 노광준 c++
//ThreadTcpSever프로젝트 메인 파일
//=======================================================================

#include "common.h"
#include "struct_token.h"
#include "SERVER_AVL.h"
#include "Menu_con.h"
#include "Network.h"

extern bool judge;                     //탐색할 경우 해당 값이 존재하면 true 없으면 false
extern char ch_str[MAX_MENU_LENGTH];      //수정할 경우 바꿀 문자열 저장하는 배열

extern int next_nodekey;
extern AvlNode *root;
extern CRITICAL_SECTION cs;	//크리티컬 섹션
extern vector<int> nodekey;;


int key = 1; //AVL트리 key

#define DB_ERR_FILE_OPEN		-4


//입력 받은 문자열에 따라 int 값 리턴
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

//Thread 수행 클라이언트 요청 수행
DWORD WINAPI ThClient(LPVOID arg)
{
	SOCKET client_sock = (SOCKET)arg;
	SOCKADDR_IN clientaddr;
	
	Tcp_dat buf;	//구조체
	File_buf file_buf;

	int addrlen;	//클라이언트 주소체 길이
	int retval;		//통신 버퍼길이
	int Tcp_num;
	int price;
	int insert_key = 0;
	char insert_buf[MAX_MENU_LENGTH];;
	AvlNode *newNode;
	memset(&buf, 0, sizeof(buf));
	//클라이언트 정보 얻기
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);


	ofstream fout;
	ifstream fin;

	cout << "스레드 생성" << endl;

	while (1) {
		cout << "클라이언트 메세지 수신 대기 중..." << endl;
		//데이터 받기
		retval = recv(client_sock, (char*)&insert_buf, MAX_MENU_LENGTH, 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0)
			break;
		insert_buf[retval] = '\0';	//\n제거
		//CriticalSection 설정
		EnterCriticalSection(&cs);

		//토큰화 수행
		buf = tokenize_tcpdat(buf, insert_buf);	
		
		//수신받은 key값이 0보다 작을경우 에러 -13
		if (atoi(buf.key) < 0) {
			sprintf(insert_buf, "-13");
			printf("[TCP/%s:%d] 명령 : %s 키값 : %s 음수 key값 수신  \n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), buf.commend, buf.key);
			retval = send(client_sock, (const char*)insert_buf, strlen(insert_buf), 0);
			if (retval == SOCKET_ERROR) {
				err_display("send()");
			}
		}
		else
		{
			//데이터 출력 및 AVL트리 조작
			printf("[TCP/%s:%d] 명령 : %s \n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), buf.commend);

			switch (Commend_str(buf.commend))
			{
			case(0):
				//명령이 잘못됬을 경우
				cout << buf.commend << "는 잘못된 입력입니다." << endl;
				sprintf(insert_buf, "-12");
				retval = send(client_sock, (const char*)insert_buf, strlen(insert_buf), 0);
				if (retval == SOCKET_ERROR) {
					err_display("send()");
					break;
				}
				break;

			case(1):
				//insert 수행
				cout << "\n노드를 삽입합니다." << endl;
				if (!root == NULL) {						//트리가 있으면 검색 트리가 존재하지 않을 경우 그냥 삽입.
					searchNode(atoi(buf.key), root);
				}

				if (!judge) {							//검색결과가 없으면
					if ((Tcp_num = map_at(buf.menu, 1)) == 0)				//가격 찾기
					{
						//map 데이터 검색 실패메세지 보내기
						sprintf(insert_buf, "-5");
					}
					else
					{
						//가격정보 가져오기
						price = atoi(buf.num)*Tcp_num;

						//메뉴 레코드 생성
						sprintf(insert_buf, " ||주문 내역 :\t%s\t갯수 :\t%s\t가격 : %d||", buf.menu, buf.num, price);

					
						//key값 설정
						cout << "AVL트리에 저장합니다." << endl;
						if (!atoi(buf.key))							//buf.key=0 이면 key을 값으로 node 생성
						{
							insert_key = key++;
						}
						else                                       //buf.key=0이 아니면 buf.key를 값으로 node생성. 만약 key<=buf.key이면 아닐때까지 key++
						{
							insert_key = atoi(buf.key);
							while (key <= atoi(buf.key)) {
								key++;
							}
						}
					
						//AVL트리에 저장
						newNode = new AvlNode(insert_key++, insert_buf, 0, buf.point_card, buf.cal);
						root = insert(root, newNode);
						printf("\n\n");
						cout << "성공적으로 삽입되었습니다.\n" << endl;
						printf("%d %s 포인트 카드 여부 : %d || 결재 방식 %s || %s \n", newNode->key, newNode->menu, newNode->point_card, newNode->cal, newNode->nowtime);
						sprintf(insert_buf, "%d %s 포인트 카드 여부 : %d || 결재 방식 %s || %s ", newNode->key, newNode->menu, newNode->point_card, newNode->cal, newNode->nowtime);
						nodekey.push_back(newNode->key);	//random key전송을 위한 nodekey에 key값 저장
					}
				}
				else                          //검색된 노드가 있을 경우
				{
					cout << "중복된 키 값 입니다." << endl;
					sprintf(insert_buf, "-6");
					judge = false;
				}
				//데이터 전송
				retval = send(client_sock, (const char*)insert_buf, strlen(insert_buf), 0);
				if (retval == SOCKET_ERROR) {
					err_display("send()");
					break;
				}
				break;

			case(2):
				//delete
				cout << "\n노드를 삭제합니다." << endl;
				//트리 노드 검색
				searchNode(atoi(buf.key), root);
				if (judge) {
					//트리 노드 삭제
					root = deleteNode(root, atoi(buf.key));
					//삭제 성공 전송
					cout << "성공적으로 삭제되었습니다.\n" << endl;
					sprintf(insert_buf, "키값 %s ||성공적으로 삭제되었습니다.", buf.key);

					nodekey.erase(remove(nodekey.begin(), nodekey.end(), atoi(buf.key)));//random key전송을 위한 nodekey에 key값 삭제

					judge = false;
				}
				else {
					//삭제 실패 전송
					cout << "삭제에 실패했습니다.\n" << endl;
					sprintf(insert_buf, "-7");
				}
				//데이터 전송
				retval = send(client_sock, (const char*)insert_buf, strlen(insert_buf), 0);
				if (retval == SOCKET_ERROR) {
					err_display("send()");
					break;
				}
				break;
			case(3):
				//search
				cout << "\n\t노드를 탐색합니다.." << endl;
				newNode = searchNode(atoi(buf.key), root);
				if (judge) {
					//탐색 성공
					sprintf(insert_buf, "%d %s %d %s %s", newNode->key, newNode->menu, newNode->point_card, newNode->cal, newNode->nowtime);
					judge = false;
				}
				else {
					//탐색 실패
					cout << "검색에 실패했습니다.\n" << endl;
					buf.key[strlen(buf.key)] = 0;
					sprintf(insert_buf, "-8");
				}
				//데이터 전송
				retval = send(client_sock, (const char*)insert_buf, strlen(insert_buf), 0);
				if (retval == SOCKET_ERROR) {
					err_display("send()");
					break;
				}
				break;

			case 4:
				//update
				cout << "수정할 노드를 검색합니다.\n" << endl;

				//트리 노드 탐색
				newNode = searchNode(atoi(buf.key), root);
				if (judge)
				{
					//데이터 수정
					cout << "수정할 노드를 찾았습니다.\n" << endl;
					Tcp_num = map_at(buf.menu, 1);				//가격 찾기
					price = atoi(buf.num)*Tcp_num;
					sprintf(insert_buf, " || 주문 내역 :\t%s\t갯수 :\t%s\t가격 : %d ||", buf.menu, buf.num, price);
					strcpy(buf.menu, insert_buf);
					updateNode(atoi(buf.key), newNode, buf);
					//수정 성공 
					sprintf(insert_buf, "%d %s 포인트 카드 여부 : %d || 결재 방식 %s || %s ", newNode->key, newNode->menu, newNode->point_card, newNode->cal, newNode->nowtime);
				}
				else {
					//수정할 노드 검색 실패
					cout << "수정할 노드 검색에 실패했습니다.\n" << endl;
					sprintf(insert_buf, "-9");
				}
				//데이터 전송
				retval = send(client_sock, (const char*)insert_buf, strlen(insert_buf), 0);
				if (retval == SOCKET_ERROR) {
					err_display("send()");
					break;
				}
				break;
			case 5:
				//first
				cout << "첫번째 노드를 전송합니다." << endl;
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
				cout << "다음 노드를 전송합니다." << endl;
				cout << "전송받은 노드 : " << buf.key << endl;
				buf.key[strlen(buf.key)] = '\n';
				Next_Node(root, atoi(buf.key));	//next노드 탐색
				sprintf(buf.key, "%d", next_nodekey);
				if (next_nodekey < 0) {			//에러 발생시
					judge = false;
					sprintf(buf.key, "-10");
				}
				cout << "보낼 값 : " << buf.key << endl;
				//데이터 전송
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
				//데이터 보내기
				retval = send(client_sock, (const char*)insert_buf, strlen(insert_buf), 0);
				if (retval == SOCKET_ERROR) {
					err_display("send()");
					break;
				}
				next_nodekey = 0;
				break;


			case 8:
				//save
				if (root == NULL)	//트리가 없을 경우
				{
					cout << "트리가 비었습니다." << endl;
					sprintf(insert_buf, "-3");
				}
				else
				{
					fout.open("data.bin", ios::binary | ios::out);
					if (fout.is_open()) {
						cout << "저장합니다." << endl;
						next_nodekey = root->key;	//first노드 가져오기
						do {
							newNode = searchNode(next_nodekey, root);	//노드 검색
							if (judge) {								//검색 성공시 수행
	
								//토큰화
								buf = tokenize_tcpdat_menu(buf, newNode->menu);
								//암호화 및 구조체에 값 삽입
								
								file_buf.menu = map_at(buf.menu, 0);
								file_buf.num = atoi(buf.num);
								file_buf.cal = map_at(newNode->cal, 0);
								strcpy(file_buf.time, newNode->nowtime);
								file_buf.key = newNode->key;
								file_buf.point_card = newNode->point_card;
								
								//파일 쓰기
								fout.write((char*)&file_buf, sizeof(file_buf));
								Next_Node(root, newNode->key);	//next_nodekey에 다음 노드 키값 저장
							}
						} while (!(next_nodekey < 0));
						sprintf(insert_buf, "저장 완료");
						cout << "저장 완료" << endl;
					}
					else//파일 오픈 실패
					{
						cout << "파일 오픈 실패" << endl;
						sprintf(insert_buf, "-4");
					}
					fout.close();
				}
				//데이터 전송
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
					cout << "파일을 불러오는 중 입니다." << endl;
					while ((fin.read((char*)&file_buf, sizeof(file_buf)))) {
						price = map_at(return_menu[file_buf.menu], 1)*file_buf.num;
						//메뉴 레코드 생성
						sprintf(insert_buf, " ||주문 내역 :\t%s\t갯수 :\t%d\t가격 : %d||", return_menu[file_buf.menu], file_buf.num, price);
						
						//복호화 및 노드 생성
						newNode = new AvlNode(file_buf.key, insert_buf, 0, file_buf.point_card, return_menu[file_buf.cal],file_buf.time);
						//트리에 삽입
						
						root = insert(root, newNode);
						printf("%d %s 포인트 카드 여부 : %d || 결재 방식 %s || %s \n", newNode->key, newNode->menu, newNode->point_card, newNode->cal, newNode->nowtime);
						//random key를 위한 nodekey 삽입
						nodekey.push_back(newNode->key);

						if (key <= file_buf.key)
						{
							key = file_buf.key + 1;
						}

					}
					cout << "불러오기 완료" << endl;
					sprintf(insert_buf, "불러오기 완료");
				}

				else {
					cout << "파일 오픈 실패" << endl;
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
		//CriticalSection 해제
		LeaveCriticalSection(&cs);
	}
		closesocket(client_sock);
		printf("[TCP server] 클라이언트 종료 : TP주소=%s, 포트번호=%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
		return 0;
}


int main(int argc, char* argv[])
{
	//CriticalSection 초기화
	InitializeCriticalSection(&cs);

	int retval;
	//원속 초기화
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
		
		cout << "클라이언트 Connect 대기 중..." << endl;
		//accept()
		client_sock = Accept_sock(listen_sock);
		//스레드 생성
		hThread = CreateThread(NULL, 0, ThClient, (LPVOID)client_sock, 0, &ThreadId);
		if (hThread == NULL)
			printf("[오류] 스레드 생성 실패\n");
		else
			CloseHandle(hThread);
	}
	//closesocket()
	closesocket(listen_sock);
	//CriticalSectin 제거
	DeleteCriticalSection(&cs);
	//윈속 종료
	WSACleanup();
	return 0;
}