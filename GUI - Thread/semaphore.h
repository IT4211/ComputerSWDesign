#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#ifndef PERSON
#include <Windows.h>
#endif 

extern HANDLE smp_gate; 
extern HANDLE smp_staff[50];
extern HANDLE smp_wait_order[50];
extern HANDLE smp_cashier[50];
extern HANDLE smp_burger[50]; 
extern HANDLE smp_table;
extern HANDLE smp_ck_req[50];
extern HANDLE smp_ck_resp[50];
extern HANDLE smp_mutex1;
extern HANDLE smp_wait_mutex;
extern HANDLE smp_qmutex1;
extern HANDLE smp_qmutex2;
extern HANDLE smp_qmutex3;
extern HANDLE smp_dc_eMcd;
extern HANDLE smp_dc_order;

/* drive thru */
extern HANDLE smp_wait_dt;


#endif SEMAPHORE_H