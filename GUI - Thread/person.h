#ifndef PERSON_H
#define PERSON_H


#ifndef _WINDOWS_
#include "stdafx.h"
#include "ComputerSWDesign.h"
#include "ComputerSWDesignDlg.h"
#endif


typedef struct pt
{
	int x;
	int y;
}PT;

/* Customer Class */
class Customer
{
	int direction; // 진행 방향을 나타내는 변수
	int avata; // 캐릭터 종류를 나타내는 변수
	int motion; // 캐릭터 진행 동작을 나타내는 변수
	int num;
	int tcp_sock;
	PT cstmr_point;
	char char_buf[1000];
	//CString char_buf;
	
	
public:
	Customer();
	~Customer();

	/* Customer function */
	void set_num(int);
	int get_direction();
	int get_avata();
	int get_motion();
	int get_point_x();
	int get_point_y();
	char* get_char_buf();
	void Test();
	void enter_Mcd();
	void order(int);
	void pay();
	void find_table();
	void go_to_table(int);
	void eat_burger();
	void leave_table();
	void leave_Mcd();
	void Random();
	int order_num,sit,sit_R, sit_L;
	int n = 0;
	int order_p;
};

/* Staff Class */
class Staff
{
	int direction; // 진행 방향을 나타내는 변수
	int avata; // 캐릭터 종류를 나타내는 변수
	int motion; // 캐릭터 진행 동작을 나타내는 변수
	PT stf_point;
public:
	Staff();
	~Staff();
	/* Staff function */
	void order_processing();
	void set_point(int, int);
	int get_point_x();
	int get_point_y();
	int get_direction();
	int get_motion();
};

/* Cashier Class */
class Cashier
{

public:
	Cashier();
	~Cashier();
	/* cashier function */
	void pay_processing();
	void give_burger();
	
};

/* Cook Class */
class Cook
{
	int num;
public:
	Cook();
	Cook(int);
	~Cook();
	/* cook function */
	void cooking();
};


#endif PERSON_H