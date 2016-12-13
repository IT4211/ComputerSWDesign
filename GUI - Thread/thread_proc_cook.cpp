#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <queue>
#include "person.h"
#include "semaphore.h"

extern std::queue<int> Queue2;
extern std::queue<int> Queue3;

DWORD WINAPI thread_proc_cook(LPVOID param)
{
	int cook_num = *(int*)param;
	int c_cstmr;

	Cook obj_ck(cook_num);
	DWORD wait_stf;
	DWORD mutex2;
	BOOL loop_flag = TRUE;


	while (loop_flag){
		mutex2 = WaitForSingleObject(smp_qmutex2, INFINITE);
		if (!Queue2.empty()){
			c_cstmr = Queue2.front();
			_tprintf(_T("[debug_cook]test %d\n"), c_cstmr);
			Queue2.pop();
			if (!ReleaseSemaphore(smp_qmutex2, 1, NULL))
				_tprintf(_T("ReleaseSemaphore _mutex2_ error ; %d\n"), GetLastError());
		}
		else{
			if (!ReleaseSemaphore(smp_qmutex2, 1, NULL))
				_tprintf(_T("ReleaseSemaphore _mutex2_ error ; %d\n"), GetLastError());
			continue;
		}
		wait_stf = WaitForSingleObject(smp_ck_req[c_cstmr], INFINITE);
		if (wait_stf == WAIT_OBJECT_0) // 주문이 들어오고, 요리를 시작한다.
			obj_ck.cooking();

		if (!ReleaseSemaphore(smp_ck_resp[c_cstmr], 1, NULL))
			_tprintf(_T("ReleaseSemaphore _ck_resp_ error : %d\n"), GetLastError());
	}
	return TRUE;
}