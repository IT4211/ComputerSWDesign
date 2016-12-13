//==============================================================
//업데이트 2016 12 06 노광준 c++
//TCPClient 프로젝트 관련 파일
//시뮬레이션에서 실제 사용할 함수 모음
//포함된 함수 호출시 그에 맞는 레코드 생성, 전송
//==============================================================

#ifndef _S_Order
#define _S_Order
//==============================================================
//노드 가져오는함수
//Com_randomNode()
//Com_First()
//Com_Next
//==============================================================

//랜덤한 키값을 가져옵니다. 
//char_buf 네트워크 버퍼
//return 0 정상수행 return err_network 네트워크 에러 return err_get_key 키값을 가져오는데 실패함
int Com_randomNode(int tcp_sock, char* char_buf)
{
	int err = 0;
	cout << "\t\t::::랜덤 노드를 가져옵니다." << endl;
	sprintf(char_buf, "random/1/1/1/1/현금");
	if ((err=order_send(tcp_sock, char_buf)) < 0)
		return err;
	if ((err=atoi(char_buf))<0)
	{
		return err;
	}
	return 0;
}

//tree의 최상단 노드의 key를 가져옵니다.
//char_buf 네트워크 버퍼
//return 0 정상수행 return err_network 네트워크 에러 return err_not_tree 트리를 찾을 수 없음
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


//key값의 다음 노드 key 값을 가져옵니다.
//char_buf 네트워크 버퍼, key 현재 키값,
//return 0 정상 수행 return err_network 네트워크 에러 return Last_Node 마지막 노드
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
//AVL 트리 명령함수
//===========================================================================


//tree의 노드를 지웁니다.
//char_buf 네트워크 버퍼, key delete할 key값.
//return 0 정상수행 return err_network 네트워크 에러 return err_delete_node 삭제 실패
int Com_Delete(int tcp_sock, char* char_buf, int key) {
	int err = 0;
	cout << "\t\t::::delete 실행합니다." << endl;
	sprintf(char_buf, "delete/%d/0/0/0/0", key);
	if ((err=order_send(tcp_sock, char_buf)) < 0)
		return err;
	if ((err=atoi(char_buf)) < 0) {
		return err;
	}
	cout << "\t\t::::수신 데이터 : " << char_buf << endl;
	return 0;
}



//tree에 노드를 삽입합니다.
//char_buf 네트워크 버퍼, key 삽입할 key값. 0일경우 자동 키 생성
//return 0 정상수행 return err_network 네트워크 에러 return err_not_menu 잘못된 메뉴 return err_exist_key 중복된 키 return err_negative_key 음수 키값 입력
int Com_Insert(int tcp_sock, char* char_buf,int key) {
	int err=0;
	cout << "\t\t::::insert 실행합니다." << endl;
	sprintf(char_buf, "insert/%d/%s/%d/%d/%s",key, menu[random(20)], random(10) + 1, random(2), cal[random(2)]);
	
	if ((err = order_send(tcp_sock, char_buf)) < 0) {
		return err;
	}
	if ((err = atoi(char_buf)) <0) {
		return err;
	}
	cout << "\t\t::::수신 데이터 : " << char_buf << endl;
	return 0;
}

//tree의 노드를 찾습니다.
//char_buf 네트워크 버퍼, key delete할 key값.
//return 0 정상수행 return err_network 네트워크 에러 return err_search_node 검색 실패
int Com_Search(int tcp_sock, char* char_buf,int key) {
	int err = 0;
	cout << "\t\t::::search 실행합니다." << endl;
	sprintf(char_buf, "search/%d/0/0/0/0", key);
	
	if ((err = order_send(tcp_sock, char_buf)) < 0) {
		return err;
	}
	if ((err = atoi(char_buf)) <0) {
		return err;
	}

	cout << "\t\t::::수신 데이터 : " << char_buf << endl;
	return 0;
}

//tree의 노드를 수정합니다.
//char_buf 네트워크 버퍼, key delete할 key값.
//return 0 정상수행 return err_network 네트워크 에러 return err_update_node 수정 실패
int Com_Update(int tcp_sock, char* char_buf,int key) {
	int err = 0;
	cout << "\t\t::::update 실행합니다." << endl;
	sprintf(char_buf, "update/%d/%s/%d/%d/%s", key, menu[random(20)], random(10) + 1, random(2), cal[random(2)]);
	if ((err = order_send(tcp_sock, char_buf)) < 0) {
		return err;
	}
	if ((err = atoi(char_buf)) <0) {
		return err;
	}

	cout << "\t\t::::수신 데이터 : " << char_buf << endl;
	return 0;
}


/*최초 first 실행 후 반복문
do{
Com_Search()
}while(Com_Next==0)
*/
//트리 전부 출력(전위출력)
//char_buf 네트워크 버퍼
//return Com_Search()와 Com_Next()와 같음
int Com_Preorder(int tcp_sock, char* char_buf)
{
	int err = 0;
	int key = 0;
	cout << "\t\t::::출력 실행합니다." << endl;

	if ((err = Com_First(tcp_sock, char_buf))<0)
	{
		return err;
	}

	do {
		if ((key = atoi(char_buf)) == 0) {
			return err_fget_atoi;
		}	
		//seach 수행
		if ((err = Com_Search(tcp_sock, char_buf, key)) < 0)
		{
			return err;
		}
		
	} while ((err = Com_Next(tcp_sock, char_buf, key))==0);//Next가 err를 리턴 할때까지 루프
	if (err == Last_Node)
	{
		cout << "\n\t\t::출력을 완료했습니다.\n" << endl;
		return 0;
	}
	else
		return err;
}

//tree의 모든 노드를 삭제합니다.
/*
Com_First()
do{
Com_Search()
}while(Com_First==0)
*/
//char_buf 네트워크 버퍼
//return 0 정상수행 return -1 네트워크 에러 return 3 트리를 찾을 수 없음
int All_Delete(int tcp_sock, char* char_buf)
{
	int err = 0;
	int key = 0;
	cout << "\t\t::::모든 노드 삭제합니다." << endl;
	if ((err = Com_First(tcp_sock, char_buf))<0)
	{
		return err;
	}
	do {
		//key에 서버로부터 받아온 key값 저장
		if ((key = atoi(char_buf)) == 0) {
			return err_fget_atoi;
		}
		//delete 수행
		if ((err = Com_Delete(tcp_sock, char_buf, key)) < 0)
		{
			return err;
		}
	} while ((err = Com_First(tcp_sock, char_buf)) == 0);	//Com_First error가 발생 할때까지 루프
	if (err == err_not_tree)
	{
		cout << "\n\t\t::삭제를 완료했습니다.\n" << endl;
		return 0;
	}
	else
		return err;
}

//tree의 모든 노드를 저장합니다.
//char_buf 네트워크 버퍼
//return 0 정상수행 return -1 네트워크 에러 return -3 트리를 찾을 수 없음 return -4 파일I/O 실패
int Com_Save(int tcp_sock, char* char_buf)
{
	int err = 0;
	cout << "\t\t::::저장합니다" << endl;
	sprintf(char_buf, "save/0/0/0/0/0");
	//데이터 송신
	if ((err = order_send(tcp_sock, char_buf)) < 0)
		return err;

	if ((err = atoi(char_buf)) < 0) {
		return err;
	}
	cout << "\t\t::::수신 데이터 : " << char_buf << endl;
	return 0;
}


//tree의 모든 노드를 삭제하고 파일을 불러옵니다.
//char_buf 네트워크 버퍼
//All_Delete() 수행 후 load 메세지 전송
//return 0 정상수행 return -1 네트워크 에러 return -3 트리를 찾을 수 없음 return -4 파일I/O 실패
int Com_Load(int tcp_sock, char* char_buf)
{
	int err = 0;
	cout <<"\t\t::::불러오기 전 트리 삭제" << endl;
	if ((err = All_Delete(tcp_sock, char_buf)) < 0)
		err_return("Com_Load",err);
	cout << "\t\t::::불러옵니다" << endl;
	sprintf(char_buf, "load/0/0/0/0/0");
	//데이터 송신
	if ((err = order_send(tcp_sock, char_buf)) < 0)
		return err;

	if ((err = atoi(char_buf)) <0) {
		return err;
	}
	cout << "\t\t::::수신 데이터 : " << char_buf << endl;
	return 0;
}


//자동 명령 생성 insert:delete:search:update:preorder 14:1:1:1 비율
int Create_order(int tcp_sock,char* char_buf)
{
	int err = 0;
	switch (Commend_str((char*)order[random(18)])) {
	case 4://insert
		if ((err=Com_Insert(tcp_sock, char_buf,0))<0)
			return err;
		break;
	case 5://delete
		if ((err=Com_randomNode(tcp_sock, char_buf))<0)		//char_buf에 랜덤한 노드 값 저장
			return err;
		if ((err=Com_Delete(tcp_sock, char_buf,atoi(char_buf)))<0)
			return err;
		break;
	case 6://search
		if ((err=Com_randomNode(tcp_sock, char_buf))<0)	//char_buf에 랜덤한 노드 값 저장
			return err;
		if ((err=Com_Search(tcp_sock, char_buf, atoi(char_buf)))<0)
			return err;
		break;
	case 7://update
		if ((err=Com_randomNode(tcp_sock, char_buf))<0)	//char_buf에 랜덤한 노드 값 저장
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