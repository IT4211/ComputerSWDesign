#include "stdafx.h"
#include "ComputerSWDesign.h"
#include "ComputerSWDesignDlg.h"
#include "afxdialogex.h"
#include <stdio.h>
#include <tchar.h>
#include <time.h>
#include <Windows.h>
#include <queue>
#include "person.h"

#define PERSON
#include "semaphore.h"


extern int customer_number;
extern int wait_start;
extern std::queue<int> Queue1;
extern std::queue<int> Queue3;

DWORD WINAPI thread_proc_customer(LPVOID param)
{
	Customer *obj_cstmr = (Customer*)param;
	int cstmr_num;
	int wait_count;

	DWORD wait_enter;
	DWORD wait_order;
	DWORD wait_pay;
	DWORD wait_burger;
	DWORD wait_table;
	DWORD mutex1;
	DWORD mutex2;
	DWORD wait_mutex;
	BOOL loop_flag = FALSE;
	/* ������ ���� �Լ������� ��ü ����, ��ü �޼ҵ� ȣ��� �̷������ �� */

	//Customer obj_cstmr(thread_num);
	do{

		wait_mutex = WaitForSingleObject(smp_wait_mutex, INFINITE);
		wait_count = wait_start;
		wait_start += 4;
		if (!ReleaseSemaphore(smp_wait_mutex, 1, NULL))
			printf("ReleaseSemaphore smp_wait_mutex error : %d\n", GetLastError());
		Sleep(wait_count * 1000);

		wait_enter = WaitForSingleObject(smp_gate, INFINITE);
		obj_cstmr->enter_Mcd(); // ���忡 ����

		// �������� customer_number�� �Ӱ迵���̹Ƿ� ��ȣ���� ����
		mutex1 = WaitForSingleObject(smp_mutex1, INFINITE);
		cstmr_num = customer_number;
		customer_number++;
		if (!ReleaseSemaphore(smp_mutex1, 1, NULL))
			printf("ReleaseSemaphore _mutex1_ error : %d\n", GetLastError());

		obj_cstmr->set_num(cstmr_num);

		// �������� Queue�� ������ �� ��ȣ���� ����
		mutex2 = WaitForSingleObject(smp_qmutex1, INFINITE);
		Queue1.push(cstmr_num);
			
		// Queue ��ȣ���� ����
		if (!ReleaseSemaphore(smp_qmutex1, 1, NULL))
			printf("ReleaseSemaphore _mutex2_ error : %d\n", GetLastError());

		// �ֹ� �������� ���� �ֹ� 
		if (!ReleaseSemaphore(smp_staff[cstmr_num], 1, NULL))
			printf("ReleaseSemaphore _staff_ error : %d\n", GetLastError());
		else
			obj_cstmr->order(cstmr_num);

		// ������ �ֹ� ó���� �Ϸ�Ǿ��ٰ� �����ֱ⸦ ��ٸ��� ��
		wait_order = WaitForSingleObject(smp_wait_order[cstmr_num], INFINITE);
		if (wait_order == WAIT_OBJECT_0){
			obj_cstmr->Test(); // �ֹ��� �Ϸ�Ǿ��� 
		}

		// ��� �������� ���� ���
		if (!ReleaseSemaphore(smp_cashier[cstmr_num], 1, NULL))
			printf("ReleaseSemaphore _cashier_ error : %d\n", GetLastError());
		else
			obj_cstmr->pay();

		// ����� ó���ǰ�, ���Ÿ� ��ٸ��� ��
		wait_burger = WaitForSingleObject(smp_burger, INFINITE);
		if (wait_burger == WAIT_OBJECT_0){
			obj_cstmr->find_table(); // ���Ÿ� �޾ұ� ������ ���̺��� ã��
		}

		// ���̺� �ڸ��� ����⸦ ��ٸ��� ��
		wait_table = WaitForSingleObject(smp_table, INFINITE);
		if (wait_table == WAIT_OBJECT_0){
			obj_cstmr->go_to_table(cstmr_num); // ���̺� �ڸ��� ���ܼ� ã�ư���. 
		}

		// ���Ÿ� �Դ� ��
		obj_cstmr->eat_burger();

		// ���̺��� �Ͼ��. 
		if (!ReleaseSemaphore(smp_table, 1, NULL))
			printf("ReleaseSemaphore _table_ error : %d\n", GetLastError());
		else
			obj_cstmr->leave_table();

		if (wait_enter == WAIT_OBJECT_0){
			obj_cstmr->leave_Mcd();

			// ������ ������ �Ѹ��� �� ���� �� �ְ� ����
			if (!ReleaseSemaphore(smp_gate, 1, NULL))
				printf("ReleaseSemaphore _gate_ error : %d\n", GetLastError());
			break;

		}
	} while (loop_flag);

	return TRUE;
}