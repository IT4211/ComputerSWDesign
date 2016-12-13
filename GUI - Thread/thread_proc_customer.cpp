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
	/* 쓰레드 실행 함수에서는 객체 생성, 객체 메소드 호출로 이루어져야 됨 */

	//Customer obj_cstmr(thread_num);
	do{

		wait_mutex = WaitForSingleObject(smp_wait_mutex, INFINITE);
		wait_count = wait_start;
		wait_start += 4;
		if (!ReleaseSemaphore(smp_wait_mutex, 1, NULL))
			printf("ReleaseSemaphore smp_wait_mutex error : %d\n", GetLastError());
		Sleep(wait_count * 1000);

		wait_enter = WaitForSingleObject(smp_gate, INFINITE);
		obj_cstmr->enter_Mcd(); // 매장에 입장

		// 전역변수 customer_number가 임계영역이므로 상호배제 수행
		mutex1 = WaitForSingleObject(smp_mutex1, INFINITE);
		cstmr_num = customer_number;
		customer_number++;
		if (!ReleaseSemaphore(smp_mutex1, 1, NULL))
			printf("ReleaseSemaphore _mutex1_ error : %d\n", GetLastError());

		obj_cstmr->set_num(cstmr_num);

		// 전역변수 Queue에 접근할 때 상호배제 수행
		mutex2 = WaitForSingleObject(smp_qmutex1, INFINITE);
		Queue1.push(cstmr_num);
			
		// Queue 상호배제 해제
		if (!ReleaseSemaphore(smp_qmutex1, 1, NULL))
			printf("ReleaseSemaphore _mutex2_ error : %d\n", GetLastError());

		// 주문 직원에게 가서 주문 
		if (!ReleaseSemaphore(smp_staff[cstmr_num], 1, NULL))
			printf("ReleaseSemaphore _staff_ error : %d\n", GetLastError());
		else
			obj_cstmr->order(cstmr_num);

		// 직원이 주문 처리가 완료되었다고 말해주기를 기다리는 중
		wait_order = WaitForSingleObject(smp_wait_order[cstmr_num], INFINITE);
		if (wait_order == WAIT_OBJECT_0){
			obj_cstmr->Test(); // 주문이 완료되었음 
		}

		// 계산 직원에게 가서 계산
		if (!ReleaseSemaphore(smp_cashier[cstmr_num], 1, NULL))
			printf("ReleaseSemaphore _cashier_ error : %d\n", GetLastError());
		else
			obj_cstmr->pay();

		// 계산이 처리되고, 버거를 기다리는 중
		wait_burger = WaitForSingleObject(smp_burger, INFINITE);
		if (wait_burger == WAIT_OBJECT_0){
			obj_cstmr->find_table(); // 버거를 받았기 때문에 테이블을 찾음
		}

		// 테이블 자리가 생기기를 기다리는 중
		wait_table = WaitForSingleObject(smp_table, INFINITE);
		if (wait_table == WAIT_OBJECT_0){
			obj_cstmr->go_to_table(cstmr_num); // 테이블 자리가 생겨서 찾아간다. 
		}

		// 버거를 먹는 중
		obj_cstmr->eat_burger();

		// 테이블에서 일어난다. 
		if (!ReleaseSemaphore(smp_table, 1, NULL))
			printf("ReleaseSemaphore _table_ error : %d\n", GetLastError());
		else
			obj_cstmr->leave_table();

		if (wait_enter == WAIT_OBJECT_0){
			obj_cstmr->leave_Mcd();

			// 매장을 나가고 한명이 더 들어올 수 있게 해줌
			if (!ReleaseSemaphore(smp_gate, 1, NULL))
				printf("ReleaseSemaphore _gate_ error : %d\n", GetLastError());
			break;

		}
	} while (loop_flag);

	return TRUE;
}