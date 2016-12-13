#ifndef DRIVE_THRU_H
#define DRIVE_THRU_H


typedef struct ct
{
	int x;
	int y;

}CT;

typedef struct driver_information
{
	/* drive information, node key */
}DRIVER_INFO;

/* Car Class */
class Car
{

	int direction; // 진행 방향을 나타내는 변수
	int avata; // 캐릭터 종류를 나타내는 변수
	CT car_point;
	DRIVER_INFO driver_info;
public:
	Car();
	~Car();
	/* car function */
	void enter_drive_thru();
	void order();
	void pay();
	void drive();
	void leave_drive_thru();
	int get_direction();
	int get_avata();
	int get_point_x();
	int get_point_y();
};

/* Drive_staff Class */
class Drive_staff
{
public:
	Drive_staff();
	~Drive_staff();
	
};

/* Drive_cashier Class */
class Drive_cashier
{
public:
	Drive_cashier();
	~Drive_cashier();
};

#endif DRIVE_THRU_H



