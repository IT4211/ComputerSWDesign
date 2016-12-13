#include <stdio.h>
#include <Windows.h>
#include <queue>
#include "drive_thru.h"
#include "semaphore.h"

DWORD WINAPI thread_proc_car(LPVOID param)
{
	Car *obj_car = (Car*)param;

	DWORD wait_dt_enter;
	DWORD wait_dt_order;
	DWORD wait_dt_pay;

	do {
		wait_dt_enter = WaitForSingleObject(smp_wait_dt, INFINITE); // drive thru에는 1대만 있는다.
		obj_car->enter_drive_thru();

		wait_dt_order = WaitForSingleObject(smp_wait_order, INFINITE);
		if (wait_dt_order == WAIT_OBJECT_0) {
			obj_car->order(); // 주문은 자동 주문기에서 수행한다. 바로 요리사에게 알려준다!
		}

		obj_car->drive(); // 주문을 하고 난 후 버거를 받는 곳까지 이동

		ReleaseSemaphore(smp_cashier, 1, NULL);
		wait_dt_pay = WaitForSingleObject(smp_burger, INFINITE);
		if (wait_dt_pay == WAIT_OBJECT_0) {
			obj_car->pay(); // 버거를 받고 결제
		}

		obj_car->leave_drive_thru(); // drive thru를 나간다. 

		if (!ReleaseSemaphore(smp_wait_dt, 1, NULL))
			printf("ReleaseSemaphore _smp_wait_dt_ error : %d\n", GetLastError());

	} while (FALSE);

	return TRUE;
}