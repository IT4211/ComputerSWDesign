#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <queue>
#include "person.h"
#include "semaphore.h"

extern std::queue<int> Queue1;
extern std::queue<int> Queue2;

DWORD WINAPI thread_proc_staff(LPVOID param)
{
	Staff *obj_stf = (Staff*)param;
	int s_cstmr;

	DWORD wait_cstmr;
	DWORD mutex2;
	//Staff obj_stf(staff_num);
	BOOL loop_flag = TRUE;

	while (loop_flag){
		//손님이 들어온 다음에 손님의 번호표를 처리 해야한다.
		mutex2 = WaitForSingleObject(smp_qmutex1, INFINITE);
		if (!Queue1.empty()){
			s_cstmr = Queue1.front();
			_tprintf(_T("[debug_staff]test %d\n"), s_cstmr);
			Queue1.pop();
			if (!ReleaseSemaphore(smp_qmutex1, 1, NULL))
				_tprintf(_T("ReleaseSemaphore _mutex2_ error ; %d\n"), GetLastError());
		}
		else{
			if (!ReleaseSemaphore(smp_qmutex1, 1, NULL))
				_tprintf(_T("ReleaseSemaphore _mutex2_ error ; %d\n"), GetLastError());
			continue;
		}

		wait_cstmr = WaitForSingleObject(smp_staff[s_cstmr], INFINITE);
		

		if (wait_cstmr == WAIT_OBJECT_0)
			obj_stf->order_processing();

		if (!ReleaseSemaphore(smp_ck_req[s_cstmr], 1, NULL))
			_tprintf(_T("ReleaseSemaphore _ck_req_ error : %d\n"), GetLastError());

		if (!ReleaseSemaphore(smp_wait_order[s_cstmr], 1, NULL))
			_tprintf(_T("ReleaseSemaphore _wait_order_ error : %d\n"), GetLastError());

	}
	return TRUE;
}