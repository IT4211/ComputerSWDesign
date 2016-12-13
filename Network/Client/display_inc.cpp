//===========================================================
//업데이트 일자 2016-12-06 노광준 c++
//TCPClient 프로젝트 관련 파일
//err_return() 에러 정보 리턴
//Fgets_dat() 문자열 입력후 널문자 제가
//err_quit() 소켓 에러 검출 후 종료
//err_display() 소켓 에러 검출 후 출력
//===========================================================
//여러번 호출 됬을때 한번만 정의 되도록
#ifndef _Display
#define _Display

//===========================================================
//======================에러 상수 정의=======================
#define err_network -1	//네트워크 에러
#define err_get_key -2	//키 값 가져오기 실패
#define err_not_tree -3	//만들어진 AVL트리가 없음
#define err_open_file -4	//파일 오픈 실패
#define err_not_menu -5	//잘못된 메뉴(레코드)
#define err_exist_key -6	//중복된 키값
#define err_delete_node -7	//삭제에 실패함
#define err_search_node -8	//검색에 실패함
#define err_update_node -9	//수정에 실패함
#define Last_Node -10	//입력한 key가 없거나 마지막 key값임
#define err_fget_atoi -11	//입력받은 문자열이 숫자가아님
#define err_commend -12	//잘못된 명령(레코드)
#define err_negative_key -13	//음수 키값을 사용함
//===========================================================
//===========================================================


//error 리턴
char* err_return(char* fuc, int err)
{
	char err_msg[MAX_MENU_LENGTH];
	err = -err;			//음수를 양수로
	char* return_err[] = { "네트워크 에러 발생","키 값을 가져오는 데 실패했습니다.","트리를 찾을 수 없습니다.","파일 열기 실패","없는 메뉴입니다.","중복된 키값입니다.","삭제에 실패했습니다.",
		"검색에 실패했습니다.","수정에 실패했습니다.","key가 없거나 다음 노드가 없습니다.","잘못된 입력입니다.","잘못된 명령입니다.","음수 키값을 입력하셨습니다." };
	
	sprintf(err_msg, "%s : %s", fuc, return_err[err - 1]);
	cout <<"\t\t::::" <<err_msg << endl;
	return err_msg;
}
//키보드로 문자열 입력받은 후 널문자 제거
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

//에러 검출 후 종료
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

//에러 검출 후 출력
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

