#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <queue>
#include "person.h"
#include "semaphore.h"

extern std::queue<int> Queue3;

DWORD WINAPI thread_proc_cashier(LPVOID param)
{
	int cs_cstmr;

	Cashier obj_cshr;
	DWORD wait_cstmr;
	DWORD wait_ck;
	DWORD mutex2;
	BOOL loop_flag = TRUE;

	while (loop_flag){
		mutex2 = WaitForSingleObject(smp_qmutex3, INFINITE);
		if (!Queue3.empty()){
			cs_cstmr = Queue3.front();
			_tprintf(_T("[debug_cook]test %d\n"), cs_cstmr);
			Queue3.pop();
			if (!ReleaseSemaphore(smp_qmutex3, 1, NULL))
				_tprintf(_T("ReleaseSemaphore smp_qmutex3 error ; %d\n"), GetLastError());
		}
		else{
			if (!ReleaseSemaphore(smp_qmutex3, 1, NULL))
				_tprintf(_T("ReleaseSemaphore smp_qmutex3 error ; %d\n"), GetLastError());
			continue;
		}

		wait_cstmr = WaitForSingleObject(smp_cashier[cs_cstmr], INFINITE);
		if (wait_cstmr == WAIT_OBJECT_0) // 손님이 계산 직원 앞에 온 상황
			obj_cshr.pay_processing();

		wait_ck = WaitForSingleObject(smp_ck_resp[cs_cstmr], INFINITE);
		if (wait_ck == WAIT_OBJECT_0)
			obj_cshr.give_burger();

		if (!ReleaseSemaphore(smp_burger[cs_cstmr], 1, NULL))
			printf("ReleaseSemaphore _smp_burger_ error : %d\n", GetLastError());
	}
	return TRUE;
}