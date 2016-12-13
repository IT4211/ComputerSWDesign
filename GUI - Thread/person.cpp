#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>
//#include <Windows.h>
#include <time.h>
#include "person.h"

#define PERSON
#include "semaphore.h"

#include ".\Client_inc.cpp"

/* customer method */
Customer::Customer(){
	avata = rand()%5 + 1;
	direction = 1;		// 1-뒤 2-왼 3-오 4-앞
	motion = 1;
	cstmr_point.x = 580;
	cstmr_point.y = 770;
	strcpy(char_buf, "");
	tcp_sock = Socket();
	Com_Load(tcp_sock, char_buf);
}


Customer::~Customer(){}

void Customer::set_num(int cnum)
{
	num = cnum;
}

int Customer::get_direction()
{
	return direction;
}

int Customer::get_avata()
{
	return avata;
}

int Customer::get_motion()
{
	return motion;
}


int Customer::get_point_x()
{
	return cstmr_point.x;
}

int Customer::get_point_y()
{
	return cstmr_point.y;
}

char* Customer::get_char_buf() {

	return char_buf;
}

void Customer::Random() {

	
}

void Customer::Test(void){
	strcpy(char_buf, "");
}

void Customer::enter_Mcd(){
	
	for (int i = 0; i < 30; i++) {
		cstmr_point.y -= 10;
		motion += 1;
		if (motion == 3)
			motion = 1;
		Sleep(150);
	}
}

void Customer::order(int cstmr_num) {
	
	srand((unsigned)time(NULL));
	order_num = cstmr_num % 3;
	
	direction = 2;
	if (order_num == 0) {
		for (int i = 0; i < 8; i++) {
			if (motion != 3)
				motion += 1;
			else
				motion = 1;
			cstmr_point.x -= 10;
			Sleep(150);
			
			if (i == 7) {
				for (int b = 0; b < 10; b++) {
					direction = 1;
					if (motion != 3)
						motion += 1;
					else
						motion = 1;
					cstmr_point.y -= 7;
					order_p = 1;
					Sleep(150);
				}
			}
		}
	}
	if (order_num == 1) {
		for (int j = 0; j< 17; j++) {
			if (motion != 3)
				motion += 1;
			else
				motion = 1;
			cstmr_point.x -= 10;
			Sleep(150);
			order_p = 2;
			if (j == 16) {
				for (int b = 0; b < 10; b++) {
					direction = 1;
					if (motion != 3)
						motion += 1;
					else
						motion = 1;
					cstmr_point.y -= 7;
					order_p = 2;
					Sleep(150);
				}

			}
		}
	}
	if (order_num == 2) {
		for (int k = 0; k < 26; k++) {
			if (motion != 3)
				motion += 1;
			else
				motion = 1;
			cstmr_point.x -= 10;
			Sleep(150);
			order_p = 3;
			if (k == 25) {

				for (int b = 0; b < 10; b++) {
					direction = 1;
					if (motion != 3)
						motion += 1;
					else
						motion = 1;
					cstmr_point.y -= 7;
					order_p = 3;
					Sleep(150);
				}
			}
		}
		
	}
	
	
	//Com_Insert(tcp_sock, char_buf, num);
	Com_Update(tcp_sock, char_buf, num);
	Com_Save(tcp_sock, char_buf);
}

void Customer::pay() {
	TRACE(_T("pay\n"));
	for (int q = 0; q < 12; q++) {
		int a;
		direction = 4;
		if (motion != 3)
			motion += 1;
		else
			motion = 1;
		cstmr_point.y += 7;
		Sleep(150);
		if (q == 11) {
			for (int i = 0; i < 30; i++) {
				direction = 2;
				if (motion != 3)
					motion += 1;
				else
					motion = 1;
				cstmr_point.x -= 10;
				Sleep(150);
				if (cstmr_point.x <= 210) {
					
					//cstmr_point.x += 10;
					a = 1;
					i = 29;
					direction = 1;
					Sleep(150);
					
				}
			}
				if (a == 1) {
					for (int b = 0; b < 10; b++) {
						direction = 1;
						if (motion != 3)
							motion += 1;
						else
							motion = 1;
						cstmr_point.y -= 7;
						Sleep(150);
					}
				}
			
		}
		
	}

	
	_tprintf(_T("[+손님] %d번 손님 메뉴 계산하고 기다리는 중\n"), num);
}

void Customer::find_table() {

	

	

	/* 여기에 다른 손님들이 있지 않은 자리를 찾아 갈 수 있도록 해야 할 듯? */

}

void Customer::go_to_table(int cstmr_num){
	
	srand((unsigned)time(NULL));
	sit = rand() % 2+1;
	sit = cstmr_num % 2 + 1;
	srand((unsigned)time(NULL));
	sit_L = rand() % 8 + 1;
	srand((unsigned)time(NULL));
	sit_R = rand() % 10+1;
	//Random();
	
	
	for ( int q = 0; q < 10; q++) {
		direction = 6;
		if (motion != 3)
			motion += 1;
		else
			motion = 1;
		cstmr_point.y += 7;
		Sleep(150);
	
	}
	
	

	if (sit == 1) {					//왼쪽테이블에 앉는 경우
		if (sit_L == 1) {
			for (int i = 0; i < 8; i++) {
				direction = 6;
				if (motion != 3)
					motion += 1;
				else
					motion = 1;
				cstmr_point.y += 7;
				Sleep(150);
				direction = 3;
			}

		}
		if (sit_L == 2) {
			for (int i = 0; i < 23; i++) {
				direction = 6;
				if (motion != 3)
					motion += 1;
				else
					motion = 1;
				cstmr_point.y += 7;
				Sleep(150);
				direction = 3;
			}

		}
		if (sit_L == 3) {
			for (int i = 0; i < 11; i++) {
				direction = 7;
				if (motion != 3)
					motion += 1;
				else
					motion = 1;
				cstmr_point.x += 7;
				Sleep(150);

				if (i == 10) {
					for (int j = 0; j < 8; j++) {
						direction = 6;
						if (motion != 3)
							motion += 1;
						else
							motion = 1;
						cstmr_point.y += 7;
						Sleep(150);
						direction = 2;
					}
				}
			}

		}
		if (sit_L == 4) {
			for (int i = 0; i < 11; i++) {
				direction = 7;
				if (motion != 3)
					motion += 1;
				else
					motion = 1;
				cstmr_point.x += 7;
				Sleep(150);

				if (i == 10) {
					for (int j = 0; j < 23; j++) {
						direction = 6;
						if (motion != 3)
							motion += 1;
						else
							motion = 1;
						cstmr_point.y += 7;
						Sleep(150);
						direction = 2;
					}
				}
			}

		}
		if (sit_L == 5) {
			for (int i = 0; i < 22; i++) {
				direction = 7;
				if (motion != 3)
					motion += 1;
				else
					motion = 1;
				cstmr_point.x += 7;
				Sleep(150);

				if (i == 21) {
					for (int j = 0; j < 8; j++) {
						direction = 4;
						if (motion != 3)
							motion += 1;
						else
							motion = 1;
						cstmr_point.y += 7;
						Sleep(150);
						direction = 3;
					}
				}
			}

		}
		if (sit_L == 6) {
			for (int i = 0; i < 22; i++) {
				direction = 7;
				if (motion != 3)
					motion += 1;
				else
					motion = 1;
				cstmr_point.x += 7;
				Sleep(150);

				if (i == 21) {
					for (int j = 0; j < 23; j++) {
						direction = 6;
						if (motion != 3)
							motion += 1;
						else
							motion = 1;
						cstmr_point.y += 7;
						Sleep(150);
						direction = 3;
					}
				}
			}

		}
		if (sit_L == 7) {
			for (int i = 0; i < 33; i++) {
				direction = 7;
				if (motion != 3)
					motion += 1;
				else
					motion = 1;
				cstmr_point.x += 7;
				Sleep(150);

				if (i == 32) {
					for (int j = 0; j < 8; j++) {
						direction = 6;
						if (motion != 3)
							motion += 1;
						else
							motion = 1;
						cstmr_point.y += 7;
						Sleep(150);
						direction = 2;
					}
				}
			}

		}
		if (sit_L == 8) {
			for (int i = 0; i < 33; i++) {
				direction = 7;
				if (motion != 3)
					motion += 1;
				else
					motion = 1;
				cstmr_point.x += 7;
				Sleep(150);

				if (i == 32) {
					for (int j = 0; j < 23; j++) {
						direction = 6;
						if (motion != 3)
							motion += 1;
						else
							motion = 1;
						cstmr_point.y += 7;
						Sleep(150);
						direction = 2;
					}
				}
			}

		}
	}



	
	if (sit == 2) {					//오른쪽테이블에 앉는 경우
		for (int i = 0; i < 52; i++) {
			direction = 7;
			if (motion != 3)
				motion += 1;
			else
				motion = 1;
			cstmr_point.x += 10;
			Sleep(150);
			if (i == 51) {
				if (sit_R == 1) {
					for (int j = 0; j < 38; j++) {
						direction = 1;
						if (motion != 3)
							motion += 1;
						else
							motion = 1;
						cstmr_point.y -= 7;
						Sleep(150);
						if (j == 37) {
							for (int k = 0; k < 9; k++) {
								direction = 8;
								if (motion != 3)
									motion += 1;
								else
									motion = 1;
								cstmr_point.x -= 7;
								Sleep(150);
								direction = 4;
							}
						}
					}

				}
				if (sit_R == 2) {
					for (int j = 0; j < 30; j++) {
						direction = 1;
						if (motion != 3)
							motion += 1;
						else
							motion = 1;
						cstmr_point.y -= 7;
						Sleep(150);
						if (j == 29) {
							for (int k = 0; k < 9; k++) {
								direction = 8;
								if (motion != 3)
									motion += 1;
								else
									motion = 1;
								cstmr_point.x -= 7;
								Sleep(150);
								direction = 5;
							}
						}
					}

				}
				if (sit_R == 3) {
					for (int j = 0; j < 38; j++) {
						direction = 1;
						if (motion != 3)
							motion += 1;
						else
							motion = 1;
						cstmr_point.y -= 7;
						Sleep(150);
						if (j == 37) {
							for (int k = 0; k < 7; k++) {
								direction = 7;
								if (motion != 3)
									motion += 1;
								else
									motion = 1;
								cstmr_point.x += 7;
								Sleep(150);
								direction = 4;
							}
						}
					}

				}
				if (sit_R == 4) {
					for (int j = 0; j < 30; j++) {
						direction = 1;
						if (motion != 3)
							motion += 1;
						else
							motion = 1;
						cstmr_point.y -= 7;
						Sleep(150);
						if (j == 29) {
							for (int k = 0; k < 7; k++) {
								direction = 7;
								if (motion != 3)
									motion += 1;
								else
									motion = 1;
								cstmr_point.x += 7;
								Sleep(150);
								direction = 5;
							}
						}
					}

				}

				if (sit_R == 5) {
					for (int j = 0; j < 15; j++) {
						direction = 1;
						if (motion != 3)
							motion += 1;
						else
							motion = 1;
						cstmr_point.y -= 7;
						Sleep(150);
						if (j == 14) {
							for (int k = 0; k < 9; k++) {
								direction = 8;
								if (motion != 3)
									motion += 1;
								else
									motion = 1;
								cstmr_point.x -= 7;
								Sleep(150);
								direction = 4;
							}
						}
					}

				}
				if (sit_R == 6) {
					for (int j = 0; j < 7; j++) {
						direction = 1;
						if (motion != 3)
							motion += 1;
						else
							motion = 1;
						cstmr_point.y -= 7;
						Sleep(150);
						if (j == 6) {
							for (int k = 0; k < 9; k++) {
								direction = 8;
								if (motion != 3)
									motion += 1;
								else
									motion = 1;
								cstmr_point.x -= 7;
								Sleep(150);
								direction = 5;
							}
						}
					}

				}
				if (sit_R == 7) {
					for (int j = 0; j < 15; j++) {
						direction = 1;
						if (motion != 3)
							motion += 1;
						else
							motion = 1;
						cstmr_point.y -= 7;
						Sleep(150);
						if (j == 14) {
							for (int k = 0; k < 7; k++) {
								direction = 7;
								if (motion != 3)
									motion += 1;
								else
									motion = 1;
								cstmr_point.x += 7;
								Sleep(150);
								direction = 4;
							}
						}
					}

				}
				if (sit_R == 8) {
					for (int j = 0; j < 7; j++) {
						direction = 1;
						if (motion != 3)
							motion += 1;
						else
							motion = 1;
						cstmr_point.y -= 7;
						Sleep(150);
						if (j == 6) {
							for (int k = 0; k < 7; k++) {
								direction = 7;
								if (motion != 3)
									motion += 1;
								else
									motion = 1;
								cstmr_point.x += 7;
								Sleep(150);
								direction = 5;
							}
						}
					}

				}

				if (sit_R == 9) {
					for (int j = 0; j < 7; j++) {
						direction = 6;
						if (motion != 3)
							motion += 1;
						else
							motion = 1;
						cstmr_point.y += 7;
						Sleep(150);
						if (j == 6) {
							for (int k = 0; k < 7; k++) {
								direction = 7;
								if (motion != 3)
									motion += 1;
								else
									motion = 1;
								cstmr_point.x += 7;
								Sleep(150);
								direction = 4;
							}
						}
					}

				}
				if (sit_R == 10) {
					for (int j = 0; j < 15; j++) {
						direction = 6;
						if (motion != 3)
							motion += 1;
						else
							motion = 1;
						cstmr_point.y += 7;
						Sleep(150);
						if (j == 14) {
							for (int k = 0; k < 7; k++) {
								direction = 7;
								if (motion != 3)
									motion += 1;
								else
									motion = 1;
								cstmr_point.x += 7;
								Sleep(150);
								direction = 5;
							}
						}
					}

				}
			}
		}






	}


	_tprintf(_T("[+손님] %d번 손님 빈 테이블에 가서 앉음\n"), num);
}

void Customer::eat_burger(){
	int eat_time;
	_tprintf(_T("[+손님] %d번 손님 버거 먹는 중\n"), num);
	eat_time = rand() % 20 * 1000;
	Sleep(eat_time);
}

void Customer::leave_table() {
	if (sit == 1) {
		for (int i = 0; i < 8; i++) {
			direction = 4;
			if (motion != 3)
				motion += 1;
			else
				motion = 1;
			cstmr_point.y += 7;
			Sleep(150);
			if (i == 7) {
				for (int j = 0; j < 50; j++) {
					direction = 3;
					if (motion != 3)
						motion += 1;
					else
						motion = 1;
					cstmr_point.x += 7;
					Sleep(150);
					if (cstmr_point.x >= 550) {
						cstmr_point.x -= 7;
						direction = 3;
						for (int j = 0; j < 30; j++) {
							direction = 4;
							if (motion != 3)
								motion += 1;
							else
								motion = 1;
							cstmr_point.y += 7;
							Sleep(150);
						}
					}
				}
			}
		}
	}
	if (sit == 2) {
		for (int i = 0; i < 8; i++) {
			direction = 2;
			if (motion != 3)
				motion += 1;
			else
				motion = 1;
			cstmr_point.x -= 7;
			Sleep(150);
			if (i == 7) {
				for (int j = 0; j < 58; j++) {
					direction = 4;
					if (motion != 3)
						motion += 1;
					else
						motion = 1;
					cstmr_point.y += 10;
					Sleep(150);
					if (cstmr_point.y >= 680) {
						cstmr_point.y -= 10;
						for (int k = 0; k < 28; k++) {
							direction = 2;
							if (motion != 3)
								motion += 1;
							else
								motion = 1;
							cstmr_point.x -= 10;
							Sleep(150);
							if (cstmr_point.x <= 600) {
								for (int j = 0; j < 18; j++) {
									direction = 4;
									if (motion != 3)
										motion += 1;
									else
										motion = 1;
									cstmr_point.y += 10;
									Sleep(150);
								}
							}
						}
					}
				}
			}
		}


	}
	_tprintf(_T("[+손님] %d번 손님 테이블을 떠남\n"), num);
}

void Customer::leave_Mcd(){
	//_tprintf(_T("[+손님] %d번 손님 매장에 나감\n"), num);
}

/* staff method */
Staff::Staff(){
	direction = 5;		// 1-뒤 2-왼 3-오 4-앞
	motion = 1;
}

int Staff::get_direction()
{
	return direction;
}

int Staff::get_motion()
{
	return motion;
}


int Staff::get_point_x()
{
	return stf_point.x;
}

int Staff::get_point_y()
{
	return stf_point.y;
}


Staff::~Staff(){
	TRACE(_T("스태프 퇴근\n"));
}

void Staff::order_processing(){
	_tprintf(_T("[+직원] 직원이 주문 작업 처리중\n"));
	Sleep(8000);
}

void Staff::set_point(int x, int y)
{
	stf_point.x = x;
	stf_point.y = y;
}


/* cashier method */

Cashier::Cashier(){
	//_tprintf(_T("create cashier object\n"));
	TRACE(_T("캐셔 출근\n"));
}


Cashier::~Cashier(){
	TRACE(_T("캐셔 퇴근\n"));
}

void Cashier::pay_processing(){
	_tprintf(_T("[+캐셔] 캐셔가 결제를 진행함\n"));
	Sleep(8000);
}

void Cashier::give_burger(){
	_tprintf(_T("[+캐셔] 캐셔가 버거를 손님에게 줌\n"));
}

/* cook method */

Cook::Cook(){
	TRACE(_T("요리사 출근\n"));
}
Cook::Cook(int tnum){
	num = tnum;
	TRACE(_T("요리사 출근\n"));
}
Cook::~Cook(){
	TRACE(_T("요리사 퇴근\n"));
}

void Cook::cooking(){
	TRACE(_T("요리사가 요리중\n"));
	Sleep(2000);
}