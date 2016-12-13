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
		wait_dt_enter = WaitForSingleObject(smp_wait_dt, INFINITE); // drive thru���� 1�븸 �ִ´�.
		obj_car->enter_drive_thru();

		wait_dt_order = WaitForSingleObject(smp_wait_order, INFINITE);
		if (wait_dt_order == WAIT_OBJECT_0) {
			obj_car->order(); // �ֹ��� �ڵ� �ֹ��⿡�� �����Ѵ�. �ٷ� �丮�翡�� �˷��ش�!
		}

		obj_car->drive(); // �ֹ��� �ϰ� �� �� ���Ÿ� �޴� ������ �̵�

		ReleaseSemaphore(smp_cashier, 1, NULL);
		wait_dt_pay = WaitForSingleObject(smp_burger, INFINITE);
		if (wait_dt_pay == WAIT_OBJECT_0) {
			obj_car->pay(); // ���Ÿ� �ް� ����
		}

		obj_car->leave_drive_thru(); // drive thru�� ������. 

		if (!ReleaseSemaphore(smp_wait_dt, 1, NULL))
			printf("ReleaseSemaphore _smp_wait_dt_ error : %d\n", GetLastError());

	} while (FALSE);

	return TRUE;
}