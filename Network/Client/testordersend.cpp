//===================================================================
//업데이트 날짜 2016 12 06 노광준 c++
//TCPClient 프로젝트 관련 파일
//fget_Create_order() Network_DBTest에 필요한 명령 제어함수
//===================================================================

#ifndef _TestOrder
#define _TestOrder

//buf : 명령어		char_buf : 네트워크 버퍼
//return 0 정상작동 return -x displat_inc 참고
int fget_Create_order(int tcp_sock, char* buf,char* char_buf)
{
	int retval = 0;
	int key = 0;
	int err = 0;
	int len = 0;
	switch (Commend_str(buf)) {
	case 4:
		cout << "\t\t::::insert 실행합니다." << endl;
		cout << "\t\t::::삽입할 key값을 입력해주세요.(0 = 자동 키값생성)" << endl;
		do {
			cout << "\t\t::::>>";
		} while ((len = Fgets_dat(char_buf, MAX_COM) == 0))
			;
		key = atoi(char_buf);
		if ((err = Com_Insert(tcp_sock, char_buf, key)) < 0)
			return err;
		break;
	case 5:
		cout << "\t\t::::delete 실행합니다." << endl;
		cout << "\t\t::::삭제할 노드의 key값을 입력해주세요" << endl;
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
		cout << "\t\t::::search 실행합니다." << endl;
		cout << "\t\t::::검색할 노드의 key값을 입력해주세요" << endl;
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
		cout << "\t\t::::update 실행합니다." << endl;
		cout << "\t\t::::수정할 노드의 key값을 입력해주세요" << endl;
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
		cout << "\t\t::::preorder 실행합니다." << endl;
		if ((err=Com_Preorder(tcp_sock, char_buf)) < 0)
			return err;
		break;
	case 9:
		cout << "\t\t::::first 실행합니다." << endl;
		if ((err = Com_First(tcp_sock, char_buf)) < 0)
			return err;
		cout << "\t\t::::수신 데이터 : " << char_buf << endl;
		break;
	case 10:
		cout << "\t\t::::next 실행합니다." << endl;
		cout << "\t\t::::키 값을 입력해주세요." << endl;
		do {
			cout << "\t\t::::>>";
		} while ((len = Fgets_dat(char_buf, MAX_COM) == 0))
			;
		if ((key = atoi(char_buf)) == 0)
			return err_fget_atoi;
		if ((err = Com_Next(tcp_sock, char_buf, key)) < 0)
			return err;
		cout << "\t\t::::수신 데이터 : " << char_buf << endl;
		break;
	case 11:
		cout <<"\t\t::::"<< "노드 1만개 삽입합니다." << endl;
		for (int i = 0; i < 10000; i++) {
			cout << "\t\t::::insert 실행합니다." << endl;
			if ((err = Com_Insert(tcp_sock, char_buf, 0)) < 0)
				return err;
		}
		cout <<"\t\t::::"<< "1만개 삽입 완료" << endl;
		break;
	case 12:
		cout <<"\t\t::::"<< "노드 1만개 삭제합니다." << endl;
		for (int i = 0; i < 10000; i++) {
			if ((err = Com_randomNode(tcp_sock, char_buf)) < 0)	//char_buf=랜덤한 키값
				return err;
			if ((key = atoi(char_buf)) == 0)
				return err_fget_atoi;
			if ((err = Com_Delete(tcp_sock, char_buf, key)) < 0)
				return err;
		}
		cout <<"\t\t::::"<< "1만개 삭제 완료" << endl;
		break;
	case 13:
		cout <<"\t\t::::"<<"짝수 노드 5천개 삽입합니다." << endl;
		for (int i = 1; i <= 5000; i++) {
			if ((err = Com_Insert(tcp_sock, char_buf, i*2)) < 0)
				return err;
		}
		cout <<"\t\t::::"<< "짝수 노드 5천개 삽입완료" << endl;
		break;
	case 14:
		cout <<"\t\t::::"<< "홀수 노드 5천개 삽입합니다." << endl;
		for (int i = 1; i < 10000; i = i + 2)
		{
			if ((err = Com_Insert(tcp_sock, char_buf, i)) < 0)
				return err;
		}
		cout <<"\t\t::::"<< "홀수 노드 5천개 삽입완료" << endl;
		break;
	case 15:
		cout <<"\t\t::::"<< "짝수 노드 5천개 삭제합니다." << endl;
		for (int i = 1; i <= 5000; i = i++)
		{
			if ((err = Com_Delete(tcp_sock, char_buf, i*2)) < 0)
				return err;
		}
		cout <<"\t\t::::"<<"짝수 노드 5천개 삭제완료" << endl;
		break;
	case 16:
		cout <<"\t\t::::"<< "홀수 노드 5천개 삭제합니다." << endl;
		for (int i = 1; i < 10000; i = i + 2)
		{
			if ((err = Com_Delete(tcp_sock, char_buf, i)) < 0)
				return err;
		}
		cout <<"\t\t::::"<< "홀수 노드 5천개 삽입완료" << endl;
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
		cout <<"\t\t::::"<< "트리를 완전 삭제합니다." << endl;
		if ((err=All_Delete(tcp_sock, char_buf))< 0)
			return err;
		break;
	}
	return 0;
}

#endif // !_TestOrder
