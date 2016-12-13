#include "stdafx.h"
#include "drive_thru.h"

Car::Car()
{
	avata = rand()%6 + 1;
	car_point.x = 930;
	car_point.y = 800;
	direction = 1;
	
}

Car::~Car()
{
}

int Car::get_direction()
{
	return direction;
}

int Car::get_avata()
{
	return avata;
}

int Car::get_point_x()
{
	return car_point.x;
}

int Car::get_point_y()
{
	return car_point.y;

}

void Car::enter_drive_thru()
{
	for (int i = 0; i < 42; i++) {
		car_point.y -= 15;
		Sleep(150);
	}


}

void Car::order()
{
	Sleep(500);
}

void Car::pay()
{
	Sleep(1000);
}

void Car::drive()
{
	for (int i = 0; i < 10; i++) {
		car_point.y -= 15;
		Sleep(150);
		if (i == 9) {
			for (int j = 0; j < 58; j++) {
				direction = 2;
				car_point.x -= 15;
				Sleep(150);
				if (j == 57) {
					for (int j = 0; j < 20; j++) {
						direction = 3;
						car_point.y += 15;
						Sleep(150);
					}
				}
			}
		}
	}
}

void Car::leave_drive_thru()
{
	for (int j = 0; j < 30; j++) {
		
		car_point.y += 15;
		Sleep(150);
	}
}
