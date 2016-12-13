
// ComputerSWDesignDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "ComputerSWDesign.h"
#include "ComputerSWDesignDlg.h"
#include "afxdialogex.h"
#include <queue>
#include "person.h"
#include "drive_thru.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/* define constant */
#define MAX_CUSTOMER_CNT 20 
#define TABLE_CNT 10 
#define THREAD_CNT 50
#define DRIVER_CNT 10

/* declare semaphore */
HANDLE smp_gate;
HANDLE smp_staff[THREAD_CNT];
HANDLE smp_wait_order[THREAD_CNT];
HANDLE smp_cashier[THREAD_CNT];
HANDLE smp_burger[THREAD_CNT];
HANDLE smp_table;
HANDLE smp_ck_req[THREAD_CNT];
HANDLE smp_ck_resp[THREAD_CNT];
HANDLE smp_mutex1;
HANDLE smp_wait_mutex;
HANDLE smp_qmutex1;
HANDLE smp_qmutex2;
HANDLE smp_qmutex3;
HANDLE smp_dc_eMcd;
HANDLE smp_dc_order;

/* drive thru */
HANDLE smp_wait_dt;

/* declare thread proc */
extern DWORD WINAPI thread_proc_customer(LPVOID);
extern DWORD WINAPI thread_proc_staff(LPVOID);
extern DWORD WINAPI thread_proc_cashier(LPVOID);
extern DWORD WINAPI thread_proc_cook(LPVOID);
extern DWORD WINAPI thread_proc_car(LPVOID);

/* declare queue */
std::queue<int> Queue1;
std::queue<int> Queue2;
std::queue<int> Queue3;

/* global variable */
int customer_number = 1;
int wait_start = 1;


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CComputerSWDesignDlg 대화 상자



CComputerSWDesignDlg::CComputerSWDesignDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_COMPUTERSWDESIGN_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CComputerSWDesignDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CComputerSWDesignDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_ERASEBKGND()
	ON_WM_GETMINMAXINFO()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CComputerSWDesignDlg 메시지 처리기

BOOL CComputerSWDesignDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	// 메인 다이얼로그 외부 참조 포인터 초기화
	pDlg = this;

	obj_cstmr = new Customer[50];
	obj_stf = new Staff[3];
	obj_stf[0].set_point(500, 365);
	obj_stf[1].set_point(410, 365);
	obj_stf[2].set_point(320, 365);
	//obj_stf[0] = new Staff(500, 365);
	//obj_stf[1] = new Staff(410, 365);
	//obj_stf[2] = new Staff(320, 365);
	
	obj_car = new Car[10];
	
	HANDLE thread_customer[50];
	HANDLE thread_staff[3];
	HANDLE thread_cashier;
	HANDLE thread_cook[2];
	HANDLE thread_car[10];

	DWORD customer_id;
	DWORD staff_id;
	DWORD cashier_id;
	DWORD cook_id;
	DWORD car_id;

	int i;

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	// 필요한 그림을 로드한다.
	m_background.LoadBitmapW(IDB_BACKGROUND);
	m_background.GetObject(sizeof(BITMAP), (LPVOID)&m_Bitmap);

	/* Create a semaphore */

	// [카운팅] 매장안으로 손님이 20명만 입장하도록 제한한다.
	smp_gate = CreateSemaphore(NULL, MAX_CUSTOMER_CNT, MAX_CUSTOMER_CNT, NULL);
	if (smp_gate == NULL) {
		_tprintf(_T("CreateSemaphore _gate_ error : %d\n"), GetLastError());
		return 1;
	}

	for (i = 0; i < THREAD_CNT; i++) {
		// [동기화] 손님이 직원에게 갔을 때 직원이 주문을 받는다.
		smp_staff[i] = CreateSemaphore(NULL, 0, 1, NULL);
		if (smp_staff == NULL) {
			_tprintf(_T("CreateSemaphore _staff_ error : %d\n"), GetLastError());
			return 1;
		}
	}

	for (i = 0; i < THREAD_CNT; i++) {
		// [동기화] 손님이 캐셔에게 갔을 때 캐셔가 계산을 해준다.
		smp_cashier[i] = CreateSemaphore(NULL, 0, 1, NULL);
		if (smp_cashier[i] == NULL) {
			_tprintf(_T("CreateSemaphore _cashier_ error : %d\n"), GetLastError());
			return 1;
		}
	}

	for (i = 0; i < THREAD_CNT; i++) {
		// [동기화] 직원이 손님에게 주문이 완료되었음을 알려준다(손님은 캐셔에게 간다).
		smp_wait_order[i] = CreateSemaphore(NULL, 0, 1, NULL);
		if (smp_wait_order[i] == NULL) {
			_tprintf(_T("CreateSemaphore _wait_order_ error : %d\n"), GetLastError());
			return 1;
		}
	}

	for (i = 0; i < THREAD_CNT; i++) {
		// [동기화] 캐셔가 손님에게 햄버거가 다 만들어졌음을 알려준다(햄버거를 손님에게 준다).
		smp_burger[i] = CreateSemaphore(NULL, 0, 1, NULL);
		if (smp_burger[i] == NULL) {
			_tprintf(_T("CreateSemaphore _burger_ error : %d\n"), GetLastError());
			return 1;
		}
	}
	// [카운팅] 손님이 앉을 수 있는 테이블의 수가 제한되어 있다(식사할 수 있는 수가 제한된다).
	smp_table = CreateSemaphore(NULL, TABLE_CNT, TABLE_CNT, NULL);
	if (smp_table == NULL) {
		_tprintf(_T("CreateSemaphore _table_ error : %d\n"), GetLastError());
		return 1;
	}

	for (i = 0; i < THREAD_CNT; i++) {
		// [동기화] 직원이 요리사에게 햄버거를 만들라고 요청한다.
		smp_ck_req[i] = CreateSemaphore(NULL, 0, 1, NULL);
		if (smp_ck_req[i] == NULL) {
			_tprintf(_T("CreateSemaphore _ck_req_ error : %d\n"), GetLastError());
			return 1;
		}
	}

	for (i = 0; i < THREAD_CNT; i++) {
		// [동기화] 요리사가 캐셔에서 햄버거가 다 만들어졌다고 알려준다(햄버거를 캐셔에게 준다).
		smp_ck_resp[i] = CreateSemaphore(NULL, 0, 1, NULL);
		if (smp_ck_resp[i] == NULL) {
			_tprintf(_T("CreateSemaphore _ck_resp_ error : %d\n"), GetLastError());
			return 1;
		}
	}

	// 전역변수 customer_number에 대한 상호배제를 수행하는 세마포어 
	smp_mutex1 = CreateSemaphore(NULL, 1, 1, NULL);
	if (smp_mutex1 == NULL) {
		_tprintf(_T("CreateSemaphore _mutex1_ error : %d\n"), GetLastError());
		return 1;
	}

	// 전역변수 customer_number에 대한 상호배제를 수행하는 세마포어 
	smp_wait_mutex = CreateSemaphore(NULL, 1, 1, NULL);
	if (smp_wait_mutex == NULL) {
		_tprintf(_T("CreateSemaphore smp_wait_mutex_ error : %d\n"), GetLastError());
		return 1;
	}

	// 전역 큐에 대한 상호배제를 수행하는 세마포어
	smp_qmutex1 = CreateSemaphore(NULL, 1, 1, NULL);
	if (smp_qmutex1 == NULL) {
		_tprintf(_T("CreateSemaphore _qmutex1_ error : %d\n"), GetLastError());
		return 1;
	}

	// 전역 큐에 대한 상호배제를 수행하는 세마포어
	smp_qmutex2 = CreateSemaphore(NULL, 1, 1, NULL);
	if (smp_qmutex2 == NULL) {
		_tprintf(_T("CreateSemaphore _qmutex2_ error : %d\n"), GetLastError());
		return 1;
	}

	// 전역 큐에 대한 상호배제를 수행하는 세마포어
	smp_qmutex3 = CreateSemaphore(NULL, 1, 1, NULL);
	if (smp_qmutex3 == NULL) {
		_tprintf(_T("CreateSemaphore _qmutex3_ error : %d\n"), GetLastError());
		return 1;
	}

	// enter_Mcd에서 dc에 대한 상호배제를 수행하는 세마포어
	smp_dc_eMcd = CreateSemaphore(NULL, 1, 1, NULL);
	if (smp_dc_eMcd == NULL) {
		_tprintf(_T("CreateSemaphore _smp_dc_eMcd_ error : %d\n"), GetLastError());
		return 1;
	}

	// order에서 dc에 대한 상호배제를 수행하는 세마포어
	smp_dc_order = CreateSemaphore(NULL, 1, 1, NULL);
	if (smp_dc_order == NULL) {
		_tprintf(_T("CreateSemaphore _smp_dc_order_ error : %d\n"), GetLastError());
		return 1;
	}

	// drive thru에 자동차 한 대만 입장할 수 있도록 카운팅을 수행하는 세마포어
	smp_wait_dt = CreateSemaphore(NULL, 1, 1, NULL);
	if (smp_wait_dt == NULL) {
		_tprintf(_T("CreateSemaphore _smp_wait_dt_ error : %d\n"), GetLastError());
		return 1;
	}

	/* Create Thread */



	/* Create Staff threads */
	for (i = 0; i < 3; i++) {
		thread_staff[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_proc_staff, &obj_stf[i], 0, &staff_id);
		if (thread_staff[i] == NULL) {
			_tprintf(_T("CreateThread _staff_ error : %d\n"), GetLastError());
			return 1;
		}
	}

	/* Create cashier thread */
	thread_cashier = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_proc_cashier, NULL, 0, &cashier_id);
	if (thread_cashier == NULL) {
		_tprintf(_T("CreateThread _cashier_ error : %d\n"), GetLastError());
		return 1;
	}

	for (i = 0; i < 2; i++) {
		thread_cook[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_proc_cook, &i, 0, &cook_id);
		if (thread_cook[i] == NULL) {
			_tprintf(_T("CreateThread _cook_ error : %d\n"), GetLastError());
			return 1;
		}
	}

	/* Create Customer threads */
	for (i = 0; i < THREAD_CNT; i++) {
		thread_customer[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_proc_customer, &obj_cstmr[i], 0, &customer_id);
		if (thread_customer[i] == NULL) {
			_tprintf(_T("CreateThread _customer_ error : %d\n"), GetLastError());
			return 1;
		}
	}

	/* Create Car threads */
	for (i = 0; i < DRIVER_CNT; i++) {
		thread_car[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_proc_car, &obj_car[i], 0, &car_id);
		if (thread_car[i] == NULL) {
			_tprintf(_T("CreateThread _car_ error : %d\n"), GetLastError());
			return 1;
		}
	}

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CComputerSWDesignDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CComputerSWDesignDlg::OnPaint()
{

	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		
		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this);

		CDC memDC;
		CDC textDC;
		CDC mdcOffScreen;
		CBitmap bmpOffScreen;

		CBitmap *oldbitmap;
	
		CBitmap A1_F1, A1_F2, A1_F3, A1_H1, A1_H2, A1_H3, A1_L1, A1_L2, A1_L3,A1_R1, A1_R2, A1_R3, A1_SIT;
		CBitmap A1_CF1, A1_CF2, A1_CF3, A1_CR1, A1_CR2, A1_CR3, A1_CL1, A1_CL2, A1_CL3;
		CBitmap A3_F1, A3_F2, A3_F3, A3_H1, A3_H2, A3_H3, A3_L1, A3_L2, A3_L3, A3_R1, A3_R2, A3_R3, A3_SIT;
		CBitmap A3_CF1, A3_CF2, A3_CF3, A3_CR1, A3_CR2, A3_CR3, A3_CL1, A3_CL2, A3_CL3;
		CBitmap A4_F1, A4_F2, A4_F3, A4_H1, A4_H2, A4_H3, A4_L1, A4_L2, A4_L3, A4_R1, A4_R2, A4_R3, A4_SIT;
		CBitmap A4_CF1, A4_CF2, A4_CF3, A4_CR1, A4_CR2, A4_CR3, A4_CL1, A4_CL2, A4_CL3;
		CBitmap A5_F1, A5_F2, A5_F3, A5_H1, A5_H2, A5_H3, A5_L1, A5_L2, A5_L3, A5_R1, A5_R2, A5_R3, A5_SIT;
		CBitmap A5_CF1, A5_CF2, A5_CF3, A5_CR1, A5_CR2, A5_CR3, A5_CL1, A5_CL2, A5_CL3;
		CBitmap A6_F1, A6_F2, A6_F3, A6_H1, A6_H2, A6_H3, A6_L1, A6_L2, A6_L3, A6_R1, A6_R2, A6_R3, A6_SIT;
		CBitmap A6_CF1, A6_CF2, A6_CF3, A6_CR1, A6_CR2, A6_CR3, A6_CL1, A6_CL2, A6_CL3;
		CBitmap CASH_F1, CASH_F2, CASH_F3, CASH_H1, CASH_H2, CASH_H3, CASH_L1, CASH_L2, CASH_L3, CASH_R1, CASH_R2, CASH_R3, CASH_P;
		CBitmap COOK_F1, COOK_F2, COOK_F3, COOK_H1, COOK_H2, COOK_H3, COOK_L1, COOK_L2, COOK_L3, COOK_R1, COOK_R2, COOK_R3, COOK_P;
		CBitmap CAR1_F, CAR1_L, CAR1_H;
		CBitmap CAR2_F, CAR2_L, CAR2_H;
		CBitmap CAR3_F, CAR3_L, CAR3_H;
		CBitmap CAR4_F, CAR4_L, CAR4_H;
		CBitmap CAR5_F, CAR5_L, CAR5_H;
		CBitmap CAR6_F, CAR6_L, CAR6_H;
		BITMAP A1_bmpinfo, A3_bmpinfo, A4_bmpinfo, A5_bmpinfo, A6_bmpinfo, CASH_bmpinfo, COOK_bmpinfo;
		BITMAP CAR1_bmpinfo, CAR2_bmpinfo, CAR3_bmpinfo, CAR4_bmpinfo, CAR5_bmpinfo, CAR6_bmpinfo;
		
		//1번 캐릭터
		A1_F1.LoadBitmapW(IDB_A1_F1);
		A1_F1.GetObject(sizeof(BITMAP), (LPVOID)&A1_bmpinfo);
		A1_F2.LoadBitmapW(IDB_A1_F2);
		A1_F2.GetObject(sizeof(BITMAP), (LPVOID)&A1_bmpinfo);
		A1_F3.LoadBitmapW(IDB_A1_F3);
		A1_F3.GetObject(sizeof(BITMAP), (LPVOID)&A1_bmpinfo);
		A1_H1.LoadBitmapW(IDB_A1_H1);
		A1_H1.GetObject(sizeof(BITMAP), (LPVOID)&A1_bmpinfo);
		A1_H2.LoadBitmapW(IDB_A1_H2);
		A1_H2.GetObject(sizeof(BITMAP), (LPVOID)&A1_bmpinfo);
		A1_H3.LoadBitmapW(IDB_A1_H3);
		A1_H3.GetObject(sizeof(BITMAP), (LPVOID)&A1_bmpinfo);
		A1_L1.LoadBitmapW(IDB_A1_L1);
		A1_L1.GetObject(sizeof(BITMAP), (LPVOID)&A1_bmpinfo);
		A1_L2.LoadBitmapW(IDB_A1_L2);
		A1_L2.GetObject(sizeof(BITMAP), (LPVOID)&A1_bmpinfo);
		A1_L3.LoadBitmapW(IDB_A1_L3);
		A1_L3.GetObject(sizeof(BITMAP), (LPVOID)&A1_bmpinfo);
		A1_R1.LoadBitmapW(IDB_A1_R1);
		A1_R1.GetObject(sizeof(BITMAP), (LPVOID)&A1_bmpinfo);
		A1_R2.LoadBitmapW(IDB_A1_R2);
		A1_R2.GetObject(sizeof(BITMAP), (LPVOID)&A1_bmpinfo);
		A1_R3.LoadBitmapW(IDB_A1_R3);
		A1_R3.GetObject(sizeof(BITMAP), (LPVOID)&A1_bmpinfo);
		A1_SIT.LoadBitmapW(IDB_A1_SIT);
		A1_SIT.GetObject(sizeof(BITMAP), (LPVOID)&A1_bmpinfo);
		A1_CF1.LoadBitmapW(IDB_A1_CF1);
		A1_CF1.GetObject(sizeof(BITMAP), (LPVOID)&A1_bmpinfo);
		A1_CF2.LoadBitmapW(IDB_A1_CF2);
		A1_CF2.GetObject(sizeof(BITMAP), (LPVOID)&A1_bmpinfo);
		A1_CF3.LoadBitmapW(IDB_A1_CF3);
		A1_CF3.GetObject(sizeof(BITMAP), (LPVOID)&A1_bmpinfo);
		A1_CL1.LoadBitmapW(IDB_A1_CL1);
		A1_CL1.GetObject(sizeof(BITMAP), (LPVOID)&A1_bmpinfo);
		A1_CL2.LoadBitmapW(IDB_A1_CL2);
		A1_CL2.GetObject(sizeof(BITMAP), (LPVOID)&A1_bmpinfo);
		A1_CL3.LoadBitmapW(IDB_A1_CL3);
		A1_CL3.GetObject(sizeof(BITMAP), (LPVOID)&A1_bmpinfo);
		A1_CR1.LoadBitmapW(IDB_A1_CR1);
		A1_CR1.GetObject(sizeof(BITMAP), (LPVOID)&A1_bmpinfo);
		A1_CR2.LoadBitmapW(IDB_A1_CR2);
		A1_CR2.GetObject(sizeof(BITMAP), (LPVOID)&A1_bmpinfo);
		A1_CR3.LoadBitmapW(IDB_A1_CR3);
		A1_CR3.GetObject(sizeof(BITMAP), (LPVOID)&A1_bmpinfo);
		//3번캐릭터
		A3_F1.LoadBitmapW(IDB_A3_F1);
		A3_F1.GetObject(sizeof(BITMAP), (LPVOID)&A3_bmpinfo);
		A3_F2.LoadBitmapW(IDB_A3_F2);
		A3_F2.GetObject(sizeof(BITMAP), (LPVOID)&A3_bmpinfo);
		A3_F3.LoadBitmapW(IDB_A3_F3);
		A3_F3.GetObject(sizeof(BITMAP), (LPVOID)&A3_bmpinfo);
		A3_H1.LoadBitmapW(IDB_A3_H1);
		A3_H1.GetObject(sizeof(BITMAP), (LPVOID)&A3_bmpinfo);
		A3_H2.LoadBitmapW(IDB_A3_H2);
		A3_H2.GetObject(sizeof(BITMAP), (LPVOID)&A3_bmpinfo);
		A3_H3.LoadBitmapW(IDB_A3_H3);
		A3_H3.GetObject(sizeof(BITMAP), (LPVOID)&A3_bmpinfo);
		A3_L1.LoadBitmapW(IDB_A3_L1);
		A3_L1.GetObject(sizeof(BITMAP), (LPVOID)&A3_bmpinfo);
		A3_L2.LoadBitmapW(IDB_A3_L2);
		A3_L2.GetObject(sizeof(BITMAP), (LPVOID)&A3_bmpinfo);
		A3_L3.LoadBitmapW(IDB_A3_L3);
		A3_L3.GetObject(sizeof(BITMAP), (LPVOID)&A3_bmpinfo);
		A3_R1.LoadBitmapW(IDB_A3_R1);
		A3_R1.GetObject(sizeof(BITMAP), (LPVOID)&A3_bmpinfo);
		A3_R2.LoadBitmapW(IDB_A3_R2);
		A3_R2.GetObject(sizeof(BITMAP), (LPVOID)&A3_bmpinfo);
		A3_R3.LoadBitmapW(IDB_A3_R3);
		A3_R3.GetObject(sizeof(BITMAP), (LPVOID)&A3_bmpinfo);
		A3_SIT.LoadBitmapW(IDB_A3_SIT);
		A3_SIT.GetObject(sizeof(BITMAP), (LPVOID)&A3_bmpinfo);
		A3_CF1.LoadBitmapW(IDB_A3_CF1);
		A3_CF1.GetObject(sizeof(BITMAP), (LPVOID)&A3_bmpinfo);
		A3_CF2.LoadBitmapW(IDB_A3_CF2);
		A3_CF2.GetObject(sizeof(BITMAP), (LPVOID)&A3_bmpinfo);
		A3_CF3.LoadBitmapW(IDB_A3_CF3);
		A3_CF3.GetObject(sizeof(BITMAP), (LPVOID)&A3_bmpinfo);
		A3_CL1.LoadBitmapW(IDB_A3_CL1);
		A3_CL1.GetObject(sizeof(BITMAP), (LPVOID)&A3_bmpinfo);
		A3_CL2.LoadBitmapW(IDB_A3_CL2);
		A3_CL2.GetObject(sizeof(BITMAP), (LPVOID)&A3_bmpinfo);
		A3_CL3.LoadBitmapW(IDB_A3_CL3);
		A3_CL3.GetObject(sizeof(BITMAP), (LPVOID)&A3_bmpinfo);
		A3_CR1.LoadBitmapW(IDB_A3_CR1);
		A3_CR1.GetObject(sizeof(BITMAP), (LPVOID)&A3_bmpinfo);
		A3_CR2.LoadBitmapW(IDB_A3_CR2);
		A3_CR2.GetObject(sizeof(BITMAP), (LPVOID)&A3_bmpinfo);
		A3_CR3.LoadBitmapW(IDB_A3_CR3);
		A3_CR3.GetObject(sizeof(BITMAP), (LPVOID)&A3_bmpinfo);
		//4번캐릭터
		A4_F1.LoadBitmapW(IDB_A4_F1);
		A4_F1.GetObject(sizeof(BITMAP), (LPVOID)&A4_bmpinfo);
		A4_F2.LoadBitmapW(IDB_A4_F2);
		A4_F2.GetObject(sizeof(BITMAP), (LPVOID)&A4_bmpinfo);
		A4_F3.LoadBitmapW(IDB_A4_F3);
		A4_F3.GetObject(sizeof(BITMAP), (LPVOID)&A4_bmpinfo);
		A4_H1.LoadBitmapW(IDB_A4_H1);
		A4_H1.GetObject(sizeof(BITMAP), (LPVOID)&A4_bmpinfo);
		A4_H2.LoadBitmapW(IDB_A4_H2);
		A4_H2.GetObject(sizeof(BITMAP), (LPVOID)&A4_bmpinfo);
		A4_H3.LoadBitmapW(IDB_A4_H3);
		A4_H3.GetObject(sizeof(BITMAP), (LPVOID)&A4_bmpinfo);
		A4_L1.LoadBitmapW(IDB_A4_L1);
		A4_L1.GetObject(sizeof(BITMAP), (LPVOID)&A4_bmpinfo);
		A4_L2.LoadBitmapW(IDB_A4_L2);
		A4_L2.GetObject(sizeof(BITMAP), (LPVOID)&A4_bmpinfo);
		A4_L3.LoadBitmapW(IDB_A4_L3);
		A4_L3.GetObject(sizeof(BITMAP), (LPVOID)&A4_bmpinfo);
		A4_R1.LoadBitmapW(IDB_A4_R1);
		A4_R1.GetObject(sizeof(BITMAP), (LPVOID)&A4_bmpinfo);
		A4_R2.LoadBitmapW(IDB_A4_R2);
		A4_R2.GetObject(sizeof(BITMAP), (LPVOID)&A4_bmpinfo);
		A4_R3.LoadBitmapW(IDB_A4_R3);
		A4_R3.GetObject(sizeof(BITMAP), (LPVOID)&A4_bmpinfo);
		A4_SIT.LoadBitmapW(IDB_A4_SIT);
		A4_SIT.GetObject(sizeof(BITMAP), (LPVOID)&A4_bmpinfo);
		A4_CF1.LoadBitmapW(IDB_A4_CF1);
		A4_CF1.GetObject(sizeof(BITMAP), (LPVOID)&A4_bmpinfo);
		A4_CF2.LoadBitmapW(IDB_A4_CF2);
		A4_CF2.GetObject(sizeof(BITMAP), (LPVOID)&A4_bmpinfo);
		A4_CF3.LoadBitmapW(IDB_A4_CF3);
		A4_CF3.GetObject(sizeof(BITMAP), (LPVOID)&A4_bmpinfo);
		A4_CL1.LoadBitmapW(IDB_A4_CL1);
		A4_CL1.GetObject(sizeof(BITMAP), (LPVOID)&A4_bmpinfo);
		A4_CL2.LoadBitmapW(IDB_A4_CL2);
		A4_CL2.GetObject(sizeof(BITMAP), (LPVOID)&A4_bmpinfo);
		A4_CL3.LoadBitmapW(IDB_A4_CL3);
		A4_CL3.GetObject(sizeof(BITMAP), (LPVOID)&A4_bmpinfo);
		A4_CR1.LoadBitmapW(IDB_A4_CR1);
		A4_CR1.GetObject(sizeof(BITMAP), (LPVOID)&A4_bmpinfo);
		A4_CR2.LoadBitmapW(IDB_A4_CR2);
		A4_CR2.GetObject(sizeof(BITMAP), (LPVOID)&A4_bmpinfo);
		A4_CR3.LoadBitmapW(IDB_A4_CR3);
		A4_CR3.GetObject(sizeof(BITMAP), (LPVOID)&A4_bmpinfo);
		//5번캐릭터
		A5_F1.LoadBitmapW(IDB_A5_F1);
		A5_F1.GetObject(sizeof(BITMAP), (LPVOID)&A5_bmpinfo);
		A5_F2.LoadBitmapW(IDB_A5_F2);
		A5_F2.GetObject(sizeof(BITMAP), (LPVOID)&A5_bmpinfo);
		A5_F3.LoadBitmapW(IDB_A5_F3);
		A5_F3.GetObject(sizeof(BITMAP), (LPVOID)&A5_bmpinfo);
		A5_H1.LoadBitmapW(IDB_A5_H1);
		A5_H1.GetObject(sizeof(BITMAP), (LPVOID)&A5_bmpinfo);
		A5_H2.LoadBitmapW(IDB_A5_H2);
		A5_H2.GetObject(sizeof(BITMAP), (LPVOID)&A5_bmpinfo);
		A5_H3.LoadBitmapW(IDB_A5_H3);
		A5_H3.GetObject(sizeof(BITMAP), (LPVOID)&A5_bmpinfo);
		A5_L1.LoadBitmapW(IDB_A5_L1);
		A5_L1.GetObject(sizeof(BITMAP), (LPVOID)&A5_bmpinfo);
		A5_L2.LoadBitmapW(IDB_A5_L2);
		A5_L2.GetObject(sizeof(BITMAP), (LPVOID)&A5_bmpinfo);
		A5_L3.LoadBitmapW(IDB_A5_L3);
		A5_L3.GetObject(sizeof(BITMAP), (LPVOID)&A5_bmpinfo);
		A5_R1.LoadBitmapW(IDB_A5_R1);
		A5_R1.GetObject(sizeof(BITMAP), (LPVOID)&A5_bmpinfo);
		A5_R2.LoadBitmapW(IDB_A5_R2);
		A5_R2.GetObject(sizeof(BITMAP), (LPVOID)&A5_bmpinfo);
		A5_R3.LoadBitmapW(IDB_A5_R3);
		A5_R3.GetObject(sizeof(BITMAP), (LPVOID)&A5_bmpinfo);
		A5_SIT.LoadBitmapW(IDB_A5_SIT);
		A5_SIT.GetObject(sizeof(BITMAP), (LPVOID)&A5_bmpinfo);
		A5_CF1.LoadBitmapW(IDB_A5_CF1);
		A5_CF1.GetObject(sizeof(BITMAP), (LPVOID)&A5_bmpinfo);
		A5_CF2.LoadBitmapW(IDB_A5_CF2);
		A5_CF2.GetObject(sizeof(BITMAP), (LPVOID)&A5_bmpinfo);
		A5_CF3.LoadBitmapW(IDB_A5_CF3);
		A5_CF3.GetObject(sizeof(BITMAP), (LPVOID)&A5_bmpinfo);
		A5_CL1.LoadBitmapW(IDB_A5_CL1);
		A5_CL1.GetObject(sizeof(BITMAP), (LPVOID)&A5_bmpinfo);
		A5_CL2.LoadBitmapW(IDB_A5_CL2);
		A5_CL2.GetObject(sizeof(BITMAP), (LPVOID)&A5_bmpinfo);
		A5_CL3.LoadBitmapW(IDB_A5_CL3);
		A5_CL3.GetObject(sizeof(BITMAP), (LPVOID)&A5_bmpinfo);
		A5_CR1.LoadBitmapW(IDB_A5_CR1);
		A5_CR1.GetObject(sizeof(BITMAP), (LPVOID)&A5_bmpinfo);
		A5_CR2.LoadBitmapW(IDB_A5_CR2);
		A5_CR2.GetObject(sizeof(BITMAP), (LPVOID)&A5_bmpinfo);
		A5_CR3.LoadBitmapW(IDB_A5_CR3);
		A5_CR3.GetObject(sizeof(BITMAP), (LPVOID)&A5_bmpinfo);
		//6번캐릭터
		A6_F1.LoadBitmapW(IDB_A6_F1);
		A6_F1.GetObject(sizeof(BITMAP), (LPVOID)&A6_bmpinfo);
		A6_F2.LoadBitmapW(IDB_A6_F2);
		A6_F2.GetObject(sizeof(BITMAP), (LPVOID)&A6_bmpinfo);
		A6_F3.LoadBitmapW(IDB_A6_F3);
		A6_F3.GetObject(sizeof(BITMAP), (LPVOID)&A6_bmpinfo);
		A6_H1.LoadBitmapW(IDB_A6_H1);
		A6_H1.GetObject(sizeof(BITMAP), (LPVOID)&A6_bmpinfo);
		A6_H2.LoadBitmapW(IDB_A6_H2);
		A6_H2.GetObject(sizeof(BITMAP), (LPVOID)&A6_bmpinfo);
		A6_H3.LoadBitmapW(IDB_A6_H3);
		A6_H3.GetObject(sizeof(BITMAP), (LPVOID)&A6_bmpinfo);
		A6_L1.LoadBitmapW(IDB_A6_L1);
		A6_L1.GetObject(sizeof(BITMAP), (LPVOID)&A6_bmpinfo);
		A6_L2.LoadBitmapW(IDB_A6_L2);
		A6_L2.GetObject(sizeof(BITMAP), (LPVOID)&A6_bmpinfo);
		A6_L3.LoadBitmapW(IDB_A6_L3);
		A6_L3.GetObject(sizeof(BITMAP), (LPVOID)&A6_bmpinfo);
		A6_R1.LoadBitmapW(IDB_A6_R1);
		A6_R1.GetObject(sizeof(BITMAP), (LPVOID)&A6_bmpinfo);
		A6_R2.LoadBitmapW(IDB_A6_R2);
		A6_R2.GetObject(sizeof(BITMAP), (LPVOID)&A6_bmpinfo);
		A6_R3.LoadBitmapW(IDB_A6_R3);
		A6_R3.GetObject(sizeof(BITMAP), (LPVOID)&A6_bmpinfo);
		A6_SIT.LoadBitmapW(IDB_A6_SIT);
		A6_SIT.GetObject(sizeof(BITMAP), (LPVOID)&A6_bmpinfo);
		A6_CF1.LoadBitmapW(IDB_A6_CF1);
		A6_CF1.GetObject(sizeof(BITMAP), (LPVOID)&A6_bmpinfo);
		A6_CF2.LoadBitmapW(IDB_A6_CF2);
		A6_CF2.GetObject(sizeof(BITMAP), (LPVOID)&A6_bmpinfo);
		A6_CF3.LoadBitmapW(IDB_A6_CF3);
		A6_CF3.GetObject(sizeof(BITMAP), (LPVOID)&A6_bmpinfo);
		A6_CL1.LoadBitmapW(IDB_A6_CL1);
		A6_CL1.GetObject(sizeof(BITMAP), (LPVOID)&A6_bmpinfo);
		A6_CL2.LoadBitmapW(IDB_A6_CL2);
		A6_CL2.GetObject(sizeof(BITMAP), (LPVOID)&A6_bmpinfo);
		A6_CL3.LoadBitmapW(IDB_A6_CL3);
		A6_CL3.GetObject(sizeof(BITMAP), (LPVOID)&A6_bmpinfo);
		A6_CR1.LoadBitmapW(IDB_A6_CR1);
		A6_CR1.GetObject(sizeof(BITMAP), (LPVOID)&A6_bmpinfo);
		A6_CR2.LoadBitmapW(IDB_A6_CR2);
		A6_CR2.GetObject(sizeof(BITMAP), (LPVOID)&A6_bmpinfo);
		A6_CR3.LoadBitmapW(IDB_A6_CR3);
		A6_CR3.GetObject(sizeof(BITMAP), (LPVOID)&A6_bmpinfo);
		//캐셔
		CASH_F1.LoadBitmapW(IDB_CASH_F1);
		CASH_F1.GetObject(sizeof(BITMAP), (LPVOID)&CASH_bmpinfo);
		CASH_F2.LoadBitmapW(IDB_CASH_F2);
		CASH_F2.GetObject(sizeof(BITMAP), (LPVOID)&CASH_bmpinfo);
		CASH_F3.LoadBitmapW(IDB_CASH_F3);
		CASH_F3.GetObject(sizeof(BITMAP), (LPVOID)&CASH_bmpinfo);
		CASH_H1.LoadBitmapW(IDB_CASH_H1);
		CASH_H1.GetObject(sizeof(BITMAP), (LPVOID)&CASH_bmpinfo);
		CASH_H2.LoadBitmapW(IDB_CASH_H2);
		CASH_H2.GetObject(sizeof(BITMAP), (LPVOID)&CASH_bmpinfo);
		CASH_H3.LoadBitmapW(IDB_CASH_H3);
		CASH_H3.GetObject(sizeof(BITMAP), (LPVOID)&CASH_bmpinfo);
		CASH_L1.LoadBitmapW(IDB_CASH_L1);
		CASH_L1.GetObject(sizeof(BITMAP), (LPVOID)&CASH_bmpinfo);
		CASH_L2.LoadBitmapW(IDB_CASH_L2);
		CASH_L2.GetObject(sizeof(BITMAP), (LPVOID)&CASH_bmpinfo);
		CASH_L3.LoadBitmapW(IDB_CASH_L3);
		CASH_L3.GetObject(sizeof(BITMAP), (LPVOID)&CASH_bmpinfo);
		CASH_R1.LoadBitmapW(IDB_CASH_R1);
		CASH_R1.GetObject(sizeof(BITMAP), (LPVOID)&CASH_bmpinfo);
		CASH_R2.LoadBitmapW(IDB_CASH_R2);
		CASH_R2.GetObject(sizeof(BITMAP), (LPVOID)&CASH_bmpinfo);
		CASH_R3.LoadBitmapW(IDB_CASH_R3);
		CASH_R3.GetObject(sizeof(BITMAP), (LPVOID)&CASH_bmpinfo);
		CASH_P.LoadBitmapW(IDB_CASH_P);
		CASH_P.GetObject(sizeof(BITMAP), (LPVOID)&CASH_bmpinfo);
		//요리사
		COOK_F1.LoadBitmapW(IDB_COOK_F1);
		COOK_F1.GetObject(sizeof(BITMAP), (LPVOID)&COOK_bmpinfo);
		COOK_F2.LoadBitmapW(IDB_COOK_F2);
		COOK_F2.GetObject(sizeof(BITMAP), (LPVOID)&COOK_bmpinfo);
		COOK_F3.LoadBitmapW(IDB_COOK_F3);
		COOK_F3.GetObject(sizeof(BITMAP), (LPVOID)&COOK_bmpinfo);
		COOK_H1.LoadBitmapW(IDB_COOK_H1);
		COOK_H1.GetObject(sizeof(BITMAP), (LPVOID)&COOK_bmpinfo);
		COOK_H2.LoadBitmapW(IDB_COOK_H2);
		COOK_H2.GetObject(sizeof(BITMAP), (LPVOID)&COOK_bmpinfo);
		COOK_H3.LoadBitmapW(IDB_COOK_H3);
		COOK_H3.GetObject(sizeof(BITMAP), (LPVOID)&COOK_bmpinfo);
		COOK_L1.LoadBitmapW(IDB_COOK_L1);
		COOK_L1.GetObject(sizeof(BITMAP), (LPVOID)&COOK_bmpinfo);
		COOK_L2.LoadBitmapW(IDB_COOK_L2);
		COOK_L2.GetObject(sizeof(BITMAP), (LPVOID)&COOK_bmpinfo);
		COOK_L3.LoadBitmapW(IDB_COOK_L3);
		COOK_L3.GetObject(sizeof(BITMAP), (LPVOID)&COOK_bmpinfo);
		COOK_R1.LoadBitmapW(IDB_COOK_R1);
		COOK_R1.GetObject(sizeof(BITMAP), (LPVOID)&COOK_bmpinfo);
		COOK_R2.LoadBitmapW(IDB_COOK_R2);
		COOK_R2.GetObject(sizeof(BITMAP), (LPVOID)&COOK_bmpinfo);
		COOK_R3.LoadBitmapW(IDB_COOK_R3);
		COOK_R3.GetObject(sizeof(BITMAP), (LPVOID)&COOK_bmpinfo);
		COOK_P.LoadBitmapW(IDB_COOK_P);
		COOK_P.GetObject(sizeof(BITMAP), (LPVOID)&COOK_bmpinfo);
		//자동차1
		CAR1_F.LoadBitmapW(IDB_CAR1_F);
		CAR1_F.GetObject(sizeof(BITMAP), (LPVOID)&CAR1_bmpinfo);
		CAR1_L.LoadBitmapW(IDB_CAR1_L);
		CAR1_L.GetObject(sizeof(BITMAP), (LPVOID)&CAR1_bmpinfo);
		CAR1_H.LoadBitmapW(IDB_CAR1_H);
		CAR1_H.GetObject(sizeof(BITMAP), (LPVOID)&CAR1_bmpinfo);
		//자동차2
		CAR2_F.LoadBitmapW(IDB_CAR2_F);
		CAR2_F.GetObject(sizeof(BITMAP), (LPVOID)&CAR2_bmpinfo);
		CAR2_L.LoadBitmapW(IDB_CAR2_L);
		CAR2_L.GetObject(sizeof(BITMAP), (LPVOID)&CAR2_bmpinfo);
		CAR2_H.LoadBitmapW(IDB_CAR2_H);
		CAR2_H.GetObject(sizeof(BITMAP), (LPVOID)&CAR2_bmpinfo);
		//자동차3
		CAR3_F.LoadBitmapW(IDB_CAR3_F);
		CAR3_F.GetObject(sizeof(BITMAP), (LPVOID)&CAR3_bmpinfo);
		CAR3_L.LoadBitmapW(IDB_CAR3_L);
		CAR3_L.GetObject(sizeof(BITMAP), (LPVOID)&CAR3_bmpinfo);
		CAR3_H.LoadBitmapW(IDB_CAR3_H);
		CAR3_H.GetObject(sizeof(BITMAP), (LPVOID)&CAR3_bmpinfo);
		//자동차4
		CAR4_F.LoadBitmapW(IDB_CAR4_F);
		CAR4_F.GetObject(sizeof(BITMAP), (LPVOID)&CAR4_bmpinfo);
		CAR4_L.LoadBitmapW(IDB_CAR4_L);
		CAR4_L.GetObject(sizeof(BITMAP), (LPVOID)&CAR4_bmpinfo);
		CAR4_H.LoadBitmapW(IDB_CAR4_H);
		CAR4_H.GetObject(sizeof(BITMAP), (LPVOID)&CAR4_bmpinfo);
		//자동차5
		CAR5_F.LoadBitmapW(IDB_CAR5_F);
		CAR5_F.GetObject(sizeof(BITMAP), (LPVOID)&CAR5_bmpinfo);
		CAR5_L.LoadBitmapW(IDB_CAR5_L);
		CAR5_L.GetObject(sizeof(BITMAP), (LPVOID)&CAR5_bmpinfo);
		CAR5_H.LoadBitmapW(IDB_CAR5_H);
		CAR5_H.GetObject(sizeof(BITMAP), (LPVOID)&CAR5_bmpinfo);
		//자동차6
		CAR6_F.LoadBitmapW(IDB_CAR6_F);
		CAR6_F.GetObject(sizeof(BITMAP), (LPVOID)&CAR6_bmpinfo);
		CAR6_L.LoadBitmapW(IDB_CAR6_L);
		CAR6_L.GetObject(sizeof(BITMAP), (LPVOID)&CAR6_bmpinfo);
		CAR6_H.LoadBitmapW(IDB_CAR6_H);
		CAR6_H.GetObject(sizeof(BITMAP), (LPVOID)&CAR6_bmpinfo);


		
		

		memDC.CreateCompatibleDC(&dc);
		textDC.CreateCompatibleDC(&dc);
		mdcOffScreen.CreateCompatibleDC(&dc);

		bmpOffScreen.CreateCompatibleBitmap(&dc, m_Bitmap.bmWidth, m_Bitmap.bmHeight);

		oldbitmap = mdcOffScreen.SelectObject(&bmpOffScreen);


		//int i;
		//memDC.SelectObject(&m_background);
		//mdcOffScreen.SetStretchBltMode(COLORONCOLOR);		
		//mdcOffScreen.StretchBlt(0, 0, m_Bitmap.bmWidth / 2, m_Bitmap.bmHeight / 2, &memDC, 0, 0, m_Bitmap.bmWidth, m_Bitmap.bmHeight, SRCCOPY);
		//dc.StretchBlt(0, 0, m_Bitmap.bmWidth / 2, m_Bitmap.bmHeight / 2, &memDC, 0, 0, m_Bitmap.bmWidth, m_Bitmap.bmHeight, SRCCOPY);
		while (1) {
			memDC.SelectObject(&m_background);
			mdcOffScreen.SetStretchBltMode(COLORONCOLOR);
			mdcOffScreen.StretchBlt(0, 0, m_Bitmap.bmWidth / 2, m_Bitmap.bmHeight / 2, &memDC, 0, 0, m_Bitmap.bmWidth, m_Bitmap.bmHeight, SRCCOPY);
			

			for (int i = 0; i < THREAD_CNT; i++) {
				//memDC.SelectObject(&bmp1);
				//mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), bmpinfo1.bmWidth / 3, bmpinfo1.bmHeight / 3, &memDC, 0, 0, bmpinfo1.bmWidth, bmpinfo1.bmHeight, RGB(0, 255, 0));

				//1번캐릭터
				if (obj_cstmr[i].get_avata() == 1) {
					if (obj_cstmr[i].get_direction() == 1) {
						if (obj_cstmr[i].get_motion() == 1) {
							memDC.SelectObject(&A1_H1);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A1_bmpinfo.bmWidth / 4, A1_bmpinfo.bmHeight / 4, &memDC, 0, 0, A1_bmpinfo.bmWidth, A1_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 2) {
							memDC.SelectObject(&A1_H2);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A1_bmpinfo.bmWidth / 4, A1_bmpinfo.bmHeight / 4, &memDC, 0, 0, A1_bmpinfo.bmWidth, A1_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 3) {
							memDC.SelectObject(&A1_H3);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A1_bmpinfo.bmWidth / 4, A1_bmpinfo.bmHeight / 4, &memDC, 0, 0, A1_bmpinfo.bmWidth, A1_bmpinfo.bmHeight, RGB(0, 255, 0));
						}
					}
				}

				if (obj_cstmr[i].get_avata() == 1) {
					if (obj_cstmr[i].get_direction() == 2) {
						if (obj_cstmr[i].get_motion() == 1) {
							memDC.SelectObject(&A1_L1);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A1_bmpinfo.bmWidth / 4, A1_bmpinfo.bmHeight / 4, &memDC, 0, 0, A1_bmpinfo.bmWidth, A1_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 2) {
							memDC.SelectObject(&A1_L2);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A1_bmpinfo.bmWidth / 4, A1_bmpinfo.bmHeight / 4, &memDC, 0, 0, A1_bmpinfo.bmWidth, A1_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 3) {
							memDC.SelectObject(&A1_L3);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A1_bmpinfo.bmWidth / 4, A1_bmpinfo.bmHeight / 4, &memDC, 0, 0, A1_bmpinfo.bmWidth, A1_bmpinfo.bmHeight, RGB(0, 255, 0));
						}
					}

				}

				if (obj_cstmr[i].get_avata() == 1) {
					if (obj_cstmr[i].get_direction() == 3) {
						if (obj_cstmr[i].get_motion() == 1) {
							memDC.SelectObject(&A1_R1);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A1_bmpinfo.bmWidth / 4, A1_bmpinfo.bmHeight / 4, &memDC, 0, 0, A1_bmpinfo.bmWidth, A1_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 2) {
							memDC.SelectObject(&A1_R2);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A1_bmpinfo.bmWidth / 4, A1_bmpinfo.bmHeight / 4, &memDC, 0, 0, A1_bmpinfo.bmWidth, A1_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 3) {
							memDC.SelectObject(&A1_R3);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A1_bmpinfo.bmWidth / 4, A1_bmpinfo.bmHeight / 4, &memDC, 0, 0, A1_bmpinfo.bmWidth, A1_bmpinfo.bmHeight, RGB(0, 255, 0));
						}
					}
				}
				if (obj_cstmr[i].get_avata() == 1) {
					if (obj_cstmr[i].get_direction() == 4) {
						if (obj_cstmr[i].get_motion() == 1) {
							memDC.SelectObject(&A1_F1);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A1_bmpinfo.bmWidth / 4, A1_bmpinfo.bmHeight / 4, &memDC, 0, 0, A1_bmpinfo.bmWidth, A1_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 2) {
							memDC.SelectObject(&A1_F2);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A1_bmpinfo.bmWidth / 4, A1_bmpinfo.bmHeight / 4, &memDC, 0, 0, A1_bmpinfo.bmWidth, A1_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 3) {
							memDC.SelectObject(&A1_F3);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A1_bmpinfo.bmWidth / 4, A1_bmpinfo.bmHeight / 4, &memDC, 0, 0, A1_bmpinfo.bmWidth, A1_bmpinfo.bmHeight, RGB(0, 255, 0));
						}
					}
				}
				if (obj_cstmr[i].get_avata() == 1) {
					if (obj_cstmr[i].get_direction() == 5) {

						memDC.SelectObject(&A1_SIT);
						mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A1_bmpinfo.bmWidth / 4, A1_bmpinfo.bmHeight / 4, &memDC, 0, 0, A1_bmpinfo.bmWidth, A1_bmpinfo.bmHeight, RGB(0, 255, 0));

					}
				}
				if (obj_cstmr[i].get_avata() == 1) {
					if (obj_cstmr[i].get_direction() == 6) {
						if (obj_cstmr[i].get_motion() == 1) {
							memDC.SelectObject(&A1_CF1);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A1_bmpinfo.bmWidth / 4, A1_bmpinfo.bmHeight / 4, &memDC, 0, 0, A1_bmpinfo.bmWidth, A1_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 2) {
							memDC.SelectObject(&A1_CF2);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A1_bmpinfo.bmWidth / 4, A1_bmpinfo.bmHeight / 4, &memDC, 0, 0, A1_bmpinfo.bmWidth, A1_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 3) {
							memDC.SelectObject(&A1_CF3);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A1_bmpinfo.bmWidth / 4, A1_bmpinfo.bmHeight / 4, &memDC, 0, 0, A1_bmpinfo.bmWidth, A1_bmpinfo.bmHeight, RGB(0, 255, 0));
						}
					}
				}
				if (obj_cstmr[i].get_avata() == 1) {
					if (obj_cstmr[i].get_direction() == 7) {
						if (obj_cstmr[i].get_motion() == 1) {
							memDC.SelectObject(&A1_CR1);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A1_bmpinfo.bmWidth / 4, A1_bmpinfo.bmHeight / 4, &memDC, 0, 0, A1_bmpinfo.bmWidth, A1_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 2) {
							memDC.SelectObject(&A1_CR2);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A1_bmpinfo.bmWidth / 4, A1_bmpinfo.bmHeight / 4, &memDC, 0, 0, A1_bmpinfo.bmWidth, A1_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 3) {
							memDC.SelectObject(&A1_CR3);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A1_bmpinfo.bmWidth / 4, A1_bmpinfo.bmHeight / 4, &memDC, 0, 0, A1_bmpinfo.bmWidth, A1_bmpinfo.bmHeight, RGB(0, 255, 0));
						}
					}
				}
				if (obj_cstmr[i].get_avata() == 1) {
					if (obj_cstmr[i].get_direction() == 8) {
						if (obj_cstmr[i].get_motion() == 1) {
							memDC.SelectObject(&A1_CL1);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A1_bmpinfo.bmWidth / 4, A1_bmpinfo.bmHeight / 4, &memDC, 0, 0, A1_bmpinfo.bmWidth, A1_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 2) {
							memDC.SelectObject(&A1_CL2);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A1_bmpinfo.bmWidth / 4, A1_bmpinfo.bmHeight / 4, &memDC, 0, 0, A1_bmpinfo.bmWidth, A1_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 3) {
							memDC.SelectObject(&A1_CL3);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A1_bmpinfo.bmWidth / 4, A1_bmpinfo.bmHeight / 4, &memDC, 0, 0, A1_bmpinfo.bmWidth, A1_bmpinfo.bmHeight, RGB(0, 255, 0));
						}
					}
				}
				//2번캐릭터


				if (obj_cstmr[i].get_avata() == 2) {
					if (obj_cstmr[i].get_direction() == 1) {
						if (obj_cstmr[i].get_motion() == 1) {
							memDC.SelectObject(&A3_H1);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A3_bmpinfo.bmWidth / 3, A3_bmpinfo.bmHeight / 3, &memDC, 0, 0, A3_bmpinfo.bmWidth, A3_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 2) {
							memDC.SelectObject(&A3_H2);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A3_bmpinfo.bmWidth / 3, A3_bmpinfo.bmHeight / 3, &memDC, 0, 0, A3_bmpinfo.bmWidth, A3_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 3) {
							memDC.SelectObject(&A3_H3);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A3_bmpinfo.bmWidth / 3, A3_bmpinfo.bmHeight / 3, &memDC, 0, 0, A3_bmpinfo.bmWidth, A3_bmpinfo.bmHeight, RGB(0, 255, 0));
						}
					}
				}

				if (obj_cstmr[i].get_avata() == 2) {
					if (obj_cstmr[i].get_direction() == 2) {
						if (obj_cstmr[i].get_motion() == 1) {
							memDC.SelectObject(&A3_L1);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A3_bmpinfo.bmWidth / 3, A3_bmpinfo.bmHeight / 3, &memDC, 0, 0, A3_bmpinfo.bmWidth, A3_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 2) {
							memDC.SelectObject(&A3_L2);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A3_bmpinfo.bmWidth / 3, A3_bmpinfo.bmHeight / 3, &memDC, 0, 0, A3_bmpinfo.bmWidth, A3_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 3) {
							memDC.SelectObject(&A3_L3);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A3_bmpinfo.bmWidth / 3, A3_bmpinfo.bmHeight / 3, &memDC, 0, 0, A3_bmpinfo.bmWidth, A3_bmpinfo.bmHeight, RGB(0, 255, 0));
						}
					}

				}

				if (obj_cstmr[i].get_avata() == 2) {
					if (obj_cstmr[i].get_direction() == 3) {
						if (obj_cstmr[i].get_motion() == 1) {
							memDC.SelectObject(&A3_R1);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A3_bmpinfo.bmWidth / 3, A3_bmpinfo.bmHeight / 3, &memDC, 0, 0, A3_bmpinfo.bmWidth, A3_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 2) {
							memDC.SelectObject(&A3_R2);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A3_bmpinfo.bmWidth / 3, A3_bmpinfo.bmHeight / 3, &memDC, 0, 0, A3_bmpinfo.bmWidth, A3_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 3) {
							memDC.SelectObject(&A3_R3);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A3_bmpinfo.bmWidth / 3, A3_bmpinfo.bmHeight / 3, &memDC, 0, 0, A3_bmpinfo.bmWidth, A3_bmpinfo.bmHeight, RGB(0, 255, 0));
						}
					}
				}
				if (obj_cstmr[i].get_avata() == 2) {
					if (obj_cstmr[i].get_direction() == 4) {
						if (obj_cstmr[i].get_motion() == 1) {
							memDC.SelectObject(&A3_F1);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A3_bmpinfo.bmWidth / 3, A3_bmpinfo.bmHeight / 3, &memDC, 0, 0, A3_bmpinfo.bmWidth, A3_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 2) {
							memDC.SelectObject(&A3_F2);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A3_bmpinfo.bmWidth / 3, A3_bmpinfo.bmHeight / 3, &memDC, 0, 0, A3_bmpinfo.bmWidth, A3_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 3) {
							memDC.SelectObject(&A3_F3);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A3_bmpinfo.bmWidth / 3, A3_bmpinfo.bmHeight / 3, &memDC, 0, 0, A3_bmpinfo.bmWidth, A3_bmpinfo.bmHeight, RGB(0, 255, 0));
						}
					}
				}
				if (obj_cstmr[i].get_avata() == 2) {
					if (obj_cstmr[i].get_direction() == 5) {

						memDC.SelectObject(&A3_SIT);
						mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A3_bmpinfo.bmWidth / 3, A3_bmpinfo.bmHeight / 3, &memDC, 0, 0, A3_bmpinfo.bmWidth, A3_bmpinfo.bmHeight, RGB(0, 255, 0));

					}
				}
				if (obj_cstmr[i].get_avata() == 2) {
					if (obj_cstmr[i].get_direction() == 6) {
						if (obj_cstmr[i].get_motion() == 1) {
							memDC.SelectObject(&A3_CF1);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A3_bmpinfo.bmWidth / 3, A3_bmpinfo.bmHeight / 3, &memDC, 0, 0, A3_bmpinfo.bmWidth, A3_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 2) {
							memDC.SelectObject(&A3_CF2);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A3_bmpinfo.bmWidth / 3, A3_bmpinfo.bmHeight / 3, &memDC, 0, 0, A3_bmpinfo.bmWidth, A3_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 3) {
							memDC.SelectObject(&A3_CF3);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A3_bmpinfo.bmWidth / 3, A3_bmpinfo.bmHeight / 3, &memDC, 0, 0, A3_bmpinfo.bmWidth, A3_bmpinfo.bmHeight, RGB(0, 255, 0));
						}
					}
				}
				if (obj_cstmr[i].get_avata() == 2) {
					if (obj_cstmr[i].get_direction() == 7) {
						if (obj_cstmr[i].get_motion() == 1) {
							memDC.SelectObject(&A3_CR1);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A3_bmpinfo.bmWidth / 3, A3_bmpinfo.bmHeight / 3, &memDC, 0, 0, A3_bmpinfo.bmWidth, A3_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 2) {
							memDC.SelectObject(&A3_CR2);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A3_bmpinfo.bmWidth / 3, A3_bmpinfo.bmHeight / 3, &memDC, 0, 0, A3_bmpinfo.bmWidth, A3_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 3) {
							memDC.SelectObject(&A3_CR3);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A3_bmpinfo.bmWidth / 3, A3_bmpinfo.bmHeight / 3, &memDC, 0, 0, A3_bmpinfo.bmWidth, A3_bmpinfo.bmHeight, RGB(0, 255, 0));
						}
					}
				}
				if (obj_cstmr[i].get_avata() == 2) {
					if (obj_cstmr[i].get_direction() == 8) {
						if (obj_cstmr[i].get_motion() == 1) {
							memDC.SelectObject(&A3_CL1);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A3_bmpinfo.bmWidth / 3, A3_bmpinfo.bmHeight / 3, &memDC, 0, 0, A3_bmpinfo.bmWidth, A3_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 2) {
							memDC.SelectObject(&A3_CL2);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A3_bmpinfo.bmWidth / 3, A3_bmpinfo.bmHeight / 3, &memDC, 0, 0, A3_bmpinfo.bmWidth, A3_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 3) {
							memDC.SelectObject(&A3_CL3);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A3_bmpinfo.bmWidth / 3, A3_bmpinfo.bmHeight / 3, &memDC, 0, 0, A3_bmpinfo.bmWidth, A3_bmpinfo.bmHeight, RGB(0, 255, 0));
						}
					}
				}
				//4번캐릭터
				if (obj_cstmr[i].get_avata() == 3) {
					if (obj_cstmr[i].get_direction() == 1) {
						if (obj_cstmr[i].get_motion() == 1) {
							memDC.SelectObject(&A4_H1);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A4_bmpinfo.bmWidth / 3, A4_bmpinfo.bmHeight / 3, &memDC, 0, 0, A4_bmpinfo.bmWidth, A4_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 2) {
							memDC.SelectObject(&A4_H2);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A4_bmpinfo.bmWidth / 3, A4_bmpinfo.bmHeight / 3, &memDC, 0, 0, A4_bmpinfo.bmWidth, A4_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 3) {
							memDC.SelectObject(&A4_H3);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A4_bmpinfo.bmWidth / 3, A4_bmpinfo.bmHeight / 3, &memDC, 0, 0, A4_bmpinfo.bmWidth, A4_bmpinfo.bmHeight, RGB(0, 255, 0));
						}
					}
				}

				if (obj_cstmr[i].get_avata() == 3) {
					if (obj_cstmr[i].get_direction() == 2) {
						if (obj_cstmr[i].get_motion() == 1) {
							memDC.SelectObject(&A4_L1);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A4_bmpinfo.bmWidth / 3, A4_bmpinfo.bmHeight / 3, &memDC, 0, 0, A4_bmpinfo.bmWidth, A4_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 2) {
							memDC.SelectObject(&A4_L2);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A4_bmpinfo.bmWidth / 3, A4_bmpinfo.bmHeight / 3, &memDC, 0, 0, A4_bmpinfo.bmWidth, A4_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 3) {
							memDC.SelectObject(&A4_L3);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A4_bmpinfo.bmWidth / 3, A4_bmpinfo.bmHeight / 3, &memDC, 0, 0, A4_bmpinfo.bmWidth, A4_bmpinfo.bmHeight, RGB(0, 255, 0));
						}
					}

				}

				if (obj_cstmr[i].get_avata() == 3) {
					if (obj_cstmr[i].get_direction() == 3) {
						if (obj_cstmr[i].get_motion() == 1) {
							memDC.SelectObject(&A4_R1);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A4_bmpinfo.bmWidth / 3, A4_bmpinfo.bmHeight / 3, &memDC, 0, 0, A4_bmpinfo.bmWidth, A4_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 2) {
							memDC.SelectObject(&A4_R2);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A4_bmpinfo.bmWidth / 3, A4_bmpinfo.bmHeight / 3, &memDC, 0, 0, A4_bmpinfo.bmWidth, A4_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 3) {
							memDC.SelectObject(&A4_R3);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A4_bmpinfo.bmWidth / 3, A4_bmpinfo.bmHeight / 3, &memDC, 0, 0, A4_bmpinfo.bmWidth, A4_bmpinfo.bmHeight, RGB(0, 255, 0));
						}
					}
				}
				if (obj_cstmr[i].get_avata() == 3) {
					if (obj_cstmr[i].get_direction() == 4) {
						if (obj_cstmr[i].get_motion() == 1) {
							memDC.SelectObject(&A4_F1);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A4_bmpinfo.bmWidth / 3, A4_bmpinfo.bmHeight / 3, &memDC, 0, 0, A4_bmpinfo.bmWidth, A4_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 2) {
							memDC.SelectObject(&A4_F2);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A4_bmpinfo.bmWidth / 3, A4_bmpinfo.bmHeight / 3, &memDC, 0, 0, A4_bmpinfo.bmWidth, A4_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 3) {
							memDC.SelectObject(&A4_F3);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A4_bmpinfo.bmWidth / 3, A4_bmpinfo.bmHeight / 3, &memDC, 0, 0, A4_bmpinfo.bmWidth, A4_bmpinfo.bmHeight, RGB(0, 255, 0));
						}
					}
				}
				if (obj_cstmr[i].get_avata() == 3) {
					if (obj_cstmr[i].get_direction() == 5) {

						memDC.SelectObject(&A4_SIT);
						mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A4_bmpinfo.bmWidth / 3, A4_bmpinfo.bmHeight / 3, &memDC, 0, 0, A4_bmpinfo.bmWidth, A4_bmpinfo.bmHeight, RGB(0, 255, 0));

					}
				}
				if (obj_cstmr[i].get_avata() == 3) {
					if (obj_cstmr[i].get_direction() == 6) {
						if (obj_cstmr[i].get_motion() == 1) {
							memDC.SelectObject(&A4_CF1);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A4_bmpinfo.bmWidth / 3, A4_bmpinfo.bmHeight / 3, &memDC, 0, 0, A4_bmpinfo.bmWidth, A4_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 2) {
							memDC.SelectObject(&A4_CF2);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A4_bmpinfo.bmWidth / 3, A4_bmpinfo.bmHeight / 3, &memDC, 0, 0, A4_bmpinfo.bmWidth, A4_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 3) {
							memDC.SelectObject(&A4_CF3);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A4_bmpinfo.bmWidth / 3, A4_bmpinfo.bmHeight / 3, &memDC, 0, 0, A4_bmpinfo.bmWidth, A4_bmpinfo.bmHeight, RGB(0, 255, 0));
						}
					}
				}
				if (obj_cstmr[i].get_avata() == 3) {
					if (obj_cstmr[i].get_direction() == 7) {
						if (obj_cstmr[i].get_motion() == 1) {
							memDC.SelectObject(&A4_CR1);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A4_bmpinfo.bmWidth / 3, A4_bmpinfo.bmHeight / 3, &memDC, 0, 0, A4_bmpinfo.bmWidth, A4_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 2) {
							memDC.SelectObject(&A4_CR2);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A4_bmpinfo.bmWidth / 3, A4_bmpinfo.bmHeight / 3, &memDC, 0, 0, A4_bmpinfo.bmWidth, A4_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 3) {
							memDC.SelectObject(&A4_CR3);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A4_bmpinfo.bmWidth / 3, A4_bmpinfo.bmHeight / 3, &memDC, 0, 0, A4_bmpinfo.bmWidth, A4_bmpinfo.bmHeight, RGB(0, 255, 0));
						}
					}
				}
				if (obj_cstmr[i].get_avata() == 3) {
					if (obj_cstmr[i].get_direction() == 8) {
						if (obj_cstmr[i].get_motion() == 1) {
							memDC.SelectObject(&A4_CL1);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A4_bmpinfo.bmWidth / 3, A4_bmpinfo.bmHeight / 3, &memDC, 0, 0, A4_bmpinfo.bmWidth, A4_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 2) {
							memDC.SelectObject(&A4_CL2);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A4_bmpinfo.bmWidth / 3, A4_bmpinfo.bmHeight / 3, &memDC, 0, 0, A4_bmpinfo.bmWidth, A4_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 3) {
							memDC.SelectObject(&A4_CL3);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A4_bmpinfo.bmWidth / 3, A4_bmpinfo.bmHeight / 3, &memDC, 0, 0, A4_bmpinfo.bmWidth, A4_bmpinfo.bmHeight, RGB(0, 255, 0));
						}
					}
				}
				//5번캐릭터 
				if (obj_cstmr[i].get_avata() == 4) {
					if (obj_cstmr[i].get_direction() == 1) {
						if (obj_cstmr[i].get_motion() == 1) {
							memDC.SelectObject(&A5_H1);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A5_bmpinfo.bmWidth / 3, A5_bmpinfo.bmHeight / 3, &memDC, 0, 0, A5_bmpinfo.bmWidth, A5_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 2) {
							memDC.SelectObject(&A5_H2);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A5_bmpinfo.bmWidth / 3, A5_bmpinfo.bmHeight / 3, &memDC, 0, 0, A5_bmpinfo.bmWidth, A5_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 3) {
							memDC.SelectObject(&A5_H3);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A5_bmpinfo.bmWidth / 3, A5_bmpinfo.bmHeight / 3, &memDC, 0, 0, A5_bmpinfo.bmWidth, A5_bmpinfo.bmHeight, RGB(0, 255, 0));
						}
					}
				}

				if (obj_cstmr[i].get_avata() == 4) {
					if (obj_cstmr[i].get_direction() == 2) {
						if (obj_cstmr[i].get_motion() == 1) {
							memDC.SelectObject(&A5_L1);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A5_bmpinfo.bmWidth / 3, A5_bmpinfo.bmHeight / 3, &memDC, 0, 0, A5_bmpinfo.bmWidth, A5_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 2) {
							memDC.SelectObject(&A5_L2);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A5_bmpinfo.bmWidth / 3, A5_bmpinfo.bmHeight / 3, &memDC, 0, 0, A5_bmpinfo.bmWidth, A5_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 3) {
							memDC.SelectObject(&A5_L3);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A5_bmpinfo.bmWidth / 3, A5_bmpinfo.bmHeight / 3, &memDC, 0, 0, A5_bmpinfo.bmWidth, A5_bmpinfo.bmHeight, RGB(0, 255, 0));
						}
					}

				}

				if (obj_cstmr[i].get_avata() == 4) {
					if (obj_cstmr[i].get_direction() == 3) {
						if (obj_cstmr[i].get_motion() == 1) {
							memDC.SelectObject(&A5_R1);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A5_bmpinfo.bmWidth / 3, A5_bmpinfo.bmHeight / 3, &memDC, 0, 0, A5_bmpinfo.bmWidth, A5_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 2) {
							memDC.SelectObject(&A5_R2);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A5_bmpinfo.bmWidth / 3, A5_bmpinfo.bmHeight / 3, &memDC, 0, 0, A5_bmpinfo.bmWidth, A5_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 3) {
							memDC.SelectObject(&A5_R3);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A5_bmpinfo.bmWidth / 3, A5_bmpinfo.bmHeight / 3, &memDC, 0, 0, A5_bmpinfo.bmWidth, A5_bmpinfo.bmHeight, RGB(0, 255, 0));
						}
					}
				}
				if (obj_cstmr[i].get_avata() == 4) {
					if (obj_cstmr[i].get_direction() == 4) {
						if (obj_cstmr[i].get_motion() == 1) {
							memDC.SelectObject(&A5_F1);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A5_bmpinfo.bmWidth / 3, A5_bmpinfo.bmHeight / 3, &memDC, 0, 0, A5_bmpinfo.bmWidth, A5_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 2) {
							memDC.SelectObject(&A5_F2);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A5_bmpinfo.bmWidth / 3, A5_bmpinfo.bmHeight / 3, &memDC, 0, 0, A5_bmpinfo.bmWidth, A5_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 3) {
							memDC.SelectObject(&A5_F3);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A5_bmpinfo.bmWidth / 3, A5_bmpinfo.bmHeight / 3, &memDC, 0, 0, A5_bmpinfo.bmWidth, A5_bmpinfo.bmHeight, RGB(0, 255, 0));
						}
					}
				}
				if (obj_cstmr[i].get_avata() == 4) {
					if (obj_cstmr[i].get_direction() == 5) {

						memDC.SelectObject(&A5_SIT);
						mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A5_bmpinfo.bmWidth / 3, A5_bmpinfo.bmHeight / 3, &memDC, 0, 0, A5_bmpinfo.bmWidth, A5_bmpinfo.bmHeight, RGB(0, 255, 0));

					}
				}
				if (obj_cstmr[i].get_avata() == 4) {
					if (obj_cstmr[i].get_direction() == 6) {
						if (obj_cstmr[i].get_motion() == 1) {
							memDC.SelectObject(&A5_CF1);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A5_bmpinfo.bmWidth / 3, A5_bmpinfo.bmHeight / 3, &memDC, 0, 0, A5_bmpinfo.bmWidth, A5_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 2) {
							memDC.SelectObject(&A5_CF2);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A5_bmpinfo.bmWidth / 3, A5_bmpinfo.bmHeight / 3, &memDC, 0, 0, A5_bmpinfo.bmWidth, A5_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 3) {
							memDC.SelectObject(&A5_CF3);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A5_bmpinfo.bmWidth / 3, A5_bmpinfo.bmHeight / 3, &memDC, 0, 0, A5_bmpinfo.bmWidth, A5_bmpinfo.bmHeight, RGB(0, 255, 0));
						}
					}
				}
				if (obj_cstmr[i].get_avata() == 4) {
					if (obj_cstmr[i].get_direction() == 7) {
						if (obj_cstmr[i].get_motion() == 1) {
							memDC.SelectObject(&A5_CR1);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A5_bmpinfo.bmWidth / 3, A5_bmpinfo.bmHeight / 3, &memDC, 0, 0, A5_bmpinfo.bmWidth, A5_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 2) {
							memDC.SelectObject(&A5_CR2);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A5_bmpinfo.bmWidth / 3, A5_bmpinfo.bmHeight / 3, &memDC, 0, 0, A5_bmpinfo.bmWidth, A5_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 3) {
							memDC.SelectObject(&A5_CR3);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A5_bmpinfo.bmWidth / 3, A5_bmpinfo.bmHeight / 3, &memDC, 0, 0, A5_bmpinfo.bmWidth, A5_bmpinfo.bmHeight, RGB(0, 255, 0));
						}
					}
				}
				if (obj_cstmr[i].get_avata() == 4) {
					if (obj_cstmr[i].get_direction() == 8) {
						if (obj_cstmr[i].get_motion() == 1) {
							memDC.SelectObject(&A5_CL1);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A5_bmpinfo.bmWidth / 3, A5_bmpinfo.bmHeight / 3, &memDC, 0, 0, A5_bmpinfo.bmWidth, A5_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 2) {
							memDC.SelectObject(&A5_CL2);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A5_bmpinfo.bmWidth / 3, A5_bmpinfo.bmHeight / 3, &memDC, 0, 0, A5_bmpinfo.bmWidth, A5_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 3) {
							memDC.SelectObject(&A5_CL3);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A5_bmpinfo.bmWidth / 3, A5_bmpinfo.bmHeight / 3, &memDC, 0, 0, A5_bmpinfo.bmWidth, A5_bmpinfo.bmHeight, RGB(0, 255, 0));
						}
					}
				}
				//6번캐릭터
				if (obj_cstmr[i].get_avata() == 5) {
					if (obj_cstmr[i].get_direction() == 1) {
						if (obj_cstmr[i].get_motion() == 1) {
							memDC.SelectObject(&A6_H1);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A6_bmpinfo.bmWidth / 4, A6_bmpinfo.bmHeight / 4, &memDC, 0, 0, A6_bmpinfo.bmWidth, A6_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 2) {
							memDC.SelectObject(&A6_H2);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A6_bmpinfo.bmWidth / 4, A6_bmpinfo.bmHeight / 4, &memDC, 0, 0, A6_bmpinfo.bmWidth, A6_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 3) {

							memDC.SelectObject(&A6_H3);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A6_bmpinfo.bmWidth / 4, A6_bmpinfo.bmHeight / 4, &memDC, 0, 0, A6_bmpinfo.bmWidth, A6_bmpinfo.bmHeight, RGB(0, 255, 0));
						}
					}
				}

				if (obj_cstmr[i].get_avata() == 5) {
					if (obj_cstmr[i].get_direction() == 2) {
						if (obj_cstmr[i].get_motion() == 1) {
							memDC.SelectObject(&A6_L1);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A6_bmpinfo.bmWidth / 4, A6_bmpinfo.bmHeight / 4, &memDC, 0, 0, A6_bmpinfo.bmWidth, A6_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 2) {
							memDC.SelectObject(&A6_L2);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A6_bmpinfo.bmWidth / 4, A6_bmpinfo.bmHeight / 4, &memDC, 0, 0, A6_bmpinfo.bmWidth, A6_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 3) {
							memDC.SelectObject(&A6_L3);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A6_bmpinfo.bmWidth / 4, A6_bmpinfo.bmHeight / 4, &memDC, 0, 0, A6_bmpinfo.bmWidth, A6_bmpinfo.bmHeight, RGB(0, 255, 0));
						}
					}

				}

				if (obj_cstmr[i].get_avata() == 5) {
					if (obj_cstmr[i].get_direction() == 3) {
						if (obj_cstmr[i].get_motion() == 1) {
							memDC.SelectObject(&A6_R1);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A6_bmpinfo.bmWidth / 4, A6_bmpinfo.bmHeight / 4, &memDC, 0, 0, A6_bmpinfo.bmWidth, A6_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 2) {
							memDC.SelectObject(&A6_R2);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A6_bmpinfo.bmWidth / 4, A6_bmpinfo.bmHeight / 4, &memDC, 0, 0, A6_bmpinfo.bmWidth, A6_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 3) {
							memDC.SelectObject(&A6_R3);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A6_bmpinfo.bmWidth / 4, A6_bmpinfo.bmHeight / 4, &memDC, 0, 0, A6_bmpinfo.bmWidth, A6_bmpinfo.bmHeight, RGB(0, 255, 0));
						}
					}
				}
				if (obj_cstmr[i].get_avata() == 5) {
					if (obj_cstmr[i].get_direction() == 4) {
						if (obj_cstmr[i].get_motion() == 1) {
							memDC.SelectObject(&A6_F1);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A6_bmpinfo.bmWidth / 4, A6_bmpinfo.bmHeight / 4, &memDC, 0, 0, A6_bmpinfo.bmWidth, A6_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 2) {
							memDC.SelectObject(&A6_F2);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A6_bmpinfo.bmWidth / 4, A6_bmpinfo.bmHeight / 4, &memDC, 0, 0, A6_bmpinfo.bmWidth, A6_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 3) {
							memDC.SelectObject(&A6_F3);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A6_bmpinfo.bmWidth / 4, A6_bmpinfo.bmHeight / 4, &memDC, 0, 0, A6_bmpinfo.bmWidth, A6_bmpinfo.bmHeight, RGB(0, 255, 0));
						}
					}
				}
				if (obj_cstmr[i].get_avata() == 5) {
					if (obj_cstmr[i].get_direction() == 5) {

						memDC.SelectObject(&A6_SIT);
						mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A6_bmpinfo.bmWidth / 4, A6_bmpinfo.bmHeight / 4, &memDC, 0, 0, A6_bmpinfo.bmWidth, A6_bmpinfo.bmHeight, RGB(0, 255, 0));

					}
				}
				if (obj_cstmr[i].get_avata() == 5) {
					if (obj_cstmr[i].get_direction() == 6) {
						if (obj_cstmr[i].get_motion() == 1) {
							memDC.SelectObject(&A6_CF1);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A6_bmpinfo.bmWidth / 4, A6_bmpinfo.bmHeight / 4, &memDC, 0, 0, A6_bmpinfo.bmWidth, A6_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 2) {
							memDC.SelectObject(&A6_CF2);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A6_bmpinfo.bmWidth / 4, A6_bmpinfo.bmHeight / 4, &memDC, 0, 0, A6_bmpinfo.bmWidth, A6_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 3) {
							memDC.SelectObject(&A6_CF3);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A6_bmpinfo.bmWidth / 4, A6_bmpinfo.bmHeight / 4, &memDC, 0, 0, A6_bmpinfo.bmWidth, A6_bmpinfo.bmHeight, RGB(0, 255, 0));
						}
					}
				}
				if (obj_cstmr[i].get_avata() == 5) {
					if (obj_cstmr[i].get_direction() == 7) {
						if (obj_cstmr[i].get_motion() == 1) {
							memDC.SelectObject(&A6_CR1);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A6_bmpinfo.bmWidth / 4, A6_bmpinfo.bmHeight / 4, &memDC, 0, 0, A6_bmpinfo.bmWidth, A6_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 2) {
							memDC.SelectObject(&A6_CR2);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A6_bmpinfo.bmWidth / 4, A6_bmpinfo.bmHeight / 4, &memDC, 0, 0, A6_bmpinfo.bmWidth, A6_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 3) {
							memDC.SelectObject(&A6_CR3);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A6_bmpinfo.bmWidth / 4, A6_bmpinfo.bmHeight / 4, &memDC, 0, 0, A6_bmpinfo.bmWidth, A6_bmpinfo.bmHeight, RGB(0, 255, 0));
						}
					}
				}
				if (obj_cstmr[i].get_avata() == 5) {
					if (obj_cstmr[i].get_direction() == 8) {
						if (obj_cstmr[i].get_motion() == 1) {
							memDC.SelectObject(&A6_CL1);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A6_bmpinfo.bmWidth / 4, A6_bmpinfo.bmHeight / 4, &memDC, 0, 0, A6_bmpinfo.bmWidth, A6_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 2) {
							memDC.SelectObject(&A6_CL2);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A6_bmpinfo.bmWidth / 4, A6_bmpinfo.bmHeight / 4, &memDC, 0, 0, A6_bmpinfo.bmWidth, A6_bmpinfo.bmHeight, RGB(0, 255, 0));

						}
						else if (obj_cstmr[i].get_motion() == 3) {
							memDC.SelectObject(&A6_CL3);
							mdcOffScreen.TransparentBlt(obj_cstmr[i].get_point_x(), obj_cstmr[i].get_point_y(), A6_bmpinfo.bmWidth / 4, A6_bmpinfo.bmHeight / 4, &memDC, 0, 0, A6_bmpinfo.bmWidth, A6_bmpinfo.bmHeight, RGB(0, 255, 0));
						}
					}
				}
				// 메뉴 출력 
				CString text(obj_cstmr[i].get_char_buf());
				mdcOffScreen.TextOutW(10, 10, text, text.GetLength());
			}

			//주문받는 직원 이미지 출력
			for (int i = 0; i < 3; i++) {
				if (obj_stf[0].get_direction() == 1) {
					if (obj_stf[0].get_motion() == 1) {
						memDC.SelectObject(&CASH_H1);
						mdcOffScreen.TransparentBlt(obj_stf[0].get_point_x(), obj_stf[0].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));

					}
					else if (obj_stf[0].get_motion() == 2) {
						memDC.SelectObject(&CASH_H2);
						mdcOffScreen.TransparentBlt(obj_stf[0].get_point_x(), obj_stf[0].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));

					}
					else if (obj_stf[0].get_motion() == 3) {
						memDC.SelectObject(&CASH_H3);
						mdcOffScreen.TransparentBlt(obj_stf[0].get_point_x(), obj_stf[0].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));
					}
				}
				if (obj_stf[0].get_direction() == 2) {
					if (obj_stf[0].get_motion() == 1) {
						memDC.SelectObject(&CASH_L1);
						mdcOffScreen.TransparentBlt(obj_stf[0].get_point_x(), obj_stf[0].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));

					}
					else if (obj_stf[0].get_motion() == 2) {
						memDC.SelectObject(&CASH_L2);
						mdcOffScreen.TransparentBlt(obj_stf[0].get_point_x(), obj_stf[0].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));

					}
					else if (obj_stf[0].get_motion() == 3) {
						memDC.SelectObject(&CASH_L3);
						mdcOffScreen.TransparentBlt(obj_stf[0].get_point_x(), obj_stf[0].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));
					}
				}
				if (obj_stf[0].get_direction() == 3) {
					if (obj_stf[0].get_motion() == 1) {
						memDC.SelectObject(&CASH_R1);
						mdcOffScreen.TransparentBlt(obj_stf[0].get_point_x(), obj_stf[0].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));

					}
					else if (obj_stf[0].get_motion() == 2) {
						memDC.SelectObject(&CASH_R2);
						mdcOffScreen.TransparentBlt(obj_stf[0].get_point_x(), obj_stf[0].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));

					}
					else if (obj_stf[0].get_motion() == 3) {
						memDC.SelectObject(&CASH_R3);
						mdcOffScreen.TransparentBlt(obj_stf[0].get_point_x(), obj_stf[0].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));
					}
				}
				if (obj_stf[0].get_direction() == 4) {
					if (obj_stf[0].get_motion() == 1) {
						memDC.SelectObject(&CASH_F1);
						mdcOffScreen.TransparentBlt(obj_stf[0].get_point_x(), obj_stf[0].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));

					}
					else if (obj_stf[0].get_motion() == 2) {
						memDC.SelectObject(&CASH_F2);
						mdcOffScreen.TransparentBlt(obj_stf[0].get_point_x(), obj_stf[0].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));

					}
					else if (obj_stf[0].get_motion() == 3) {
						memDC.SelectObject(&CASH_F3);
						mdcOffScreen.TransparentBlt(obj_stf[0].get_point_x(), obj_stf[0].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));
					}
				}
				if (obj_stf[0].get_direction() == 5) {
					if (obj_stf[0].get_motion() == 1) {
						memDC.SelectObject(&CASH_P);
						mdcOffScreen.TransparentBlt(obj_stf[0].get_point_x(), obj_stf[0].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));

					}

				}
				if (obj_stf[1].get_direction() == 1) {
					if (obj_stf[1].get_motion() == 1) {
						memDC.SelectObject(&CASH_H1);
						mdcOffScreen.TransparentBlt(obj_stf[1].get_point_x(), obj_stf[1].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));

					}
					else if (obj_stf[1].get_motion() == 2) {
						memDC.SelectObject(&CASH_H2);
						mdcOffScreen.TransparentBlt(obj_stf[1].get_point_x(), obj_stf[1].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));

					}
					else if (obj_stf[1].get_motion() == 3) {
						memDC.SelectObject(&CASH_H3);
						mdcOffScreen.TransparentBlt(obj_stf[1].get_point_x(), obj_stf[1].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));
					}
				}
				if (obj_stf[1].get_direction() == 2) {
					if (obj_stf[1].get_motion() == 1) {
						memDC.SelectObject(&CASH_L1);
						mdcOffScreen.TransparentBlt(obj_stf[1].get_point_x(), obj_stf[1].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));

					}
					else if (obj_stf[1].get_motion() == 2) {
						memDC.SelectObject(&CASH_L2);
						mdcOffScreen.TransparentBlt(obj_stf[1].get_point_x(), obj_stf[1].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));

					}
					else if (obj_stf[1].get_motion() == 3) {
						memDC.SelectObject(&CASH_L3);
						mdcOffScreen.TransparentBlt(obj_stf[1].get_point_x(), obj_stf[1].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));
					}
				}
				if (obj_stf[1].get_direction() == 3) {
					if (obj_stf[1].get_motion() == 1) {
						memDC.SelectObject(&CASH_R1);
						mdcOffScreen.TransparentBlt(obj_stf[1].get_point_x(), obj_stf[1].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));

					}
					else if (obj_stf[1].get_motion() == 2) {
						memDC.SelectObject(&CASH_R2);
						mdcOffScreen.TransparentBlt(obj_stf[1].get_point_x(), obj_stf[1].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));

					}
					else if (obj_stf[1].get_motion() == 3) {
						memDC.SelectObject(&CASH_R3);
						mdcOffScreen.TransparentBlt(obj_stf[1].get_point_x(), obj_stf[1].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));
					}
				}
				if (obj_stf[1].get_direction() == 4) {
					if (obj_stf[1].get_motion() == 1) {
						memDC.SelectObject(&CASH_F1);
						mdcOffScreen.TransparentBlt(obj_stf[1].get_point_x(), obj_stf[1].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));

					}
					else if (obj_stf[1].get_motion() == 2) {
						memDC.SelectObject(&CASH_F2);
						mdcOffScreen.TransparentBlt(obj_stf[1].get_point_x(), obj_stf[1].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));

					}
					else if (obj_stf[1].get_motion() == 3) {
						memDC.SelectObject(&CASH_F3);
						mdcOffScreen.TransparentBlt(obj_stf[1].get_point_x(), obj_stf[1].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));
					}
				}
				if (obj_stf[1].get_direction() == 5) {
					if (obj_stf[1].get_motion() == 1) {
						memDC.SelectObject(&CASH_P);
						mdcOffScreen.TransparentBlt(obj_stf[1].get_point_x(), obj_stf[1].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));

					}

				}
				if (obj_stf[2].get_direction() == 1) {
					if (obj_stf[2].get_motion() == 1) {
						memDC.SelectObject(&CASH_H1);
						mdcOffScreen.TransparentBlt(obj_stf[2].get_point_x(), obj_stf[2].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));

					}
					else if (obj_stf[2].get_motion() == 2) {
						memDC.SelectObject(&CASH_H2);
						mdcOffScreen.TransparentBlt(obj_stf[2].get_point_x(), obj_stf[2].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));

					}
					else if (obj_stf[2].get_motion() == 3) {
						memDC.SelectObject(&CASH_H3);
						mdcOffScreen.TransparentBlt(obj_stf[2].get_point_x(), obj_stf[2].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));
					}
				}
				if (obj_stf[2].get_direction() == 2) {
					if (obj_stf[2].get_motion() == 1) {
						memDC.SelectObject(&CASH_L1);
						mdcOffScreen.TransparentBlt(obj_stf[2].get_point_x(), obj_stf[2].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));

					}
					else if (obj_stf[2].get_motion() == 2) {
						memDC.SelectObject(&CASH_L2);
						mdcOffScreen.TransparentBlt(obj_stf[2].get_point_x(), obj_stf[2].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));

					}
					else if (obj_stf[2].get_motion() == 3) {
						memDC.SelectObject(&CASH_L3);
						mdcOffScreen.TransparentBlt(obj_stf[2].get_point_x(), obj_stf[2].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));
					}
				}
				if (obj_stf[2].get_direction() == 3) {
					if (obj_stf[2].get_motion() == 1) {
						memDC.SelectObject(&CASH_R1);
						mdcOffScreen.TransparentBlt(obj_stf[2].get_point_x(), obj_stf[2].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));

					}
					else if (obj_stf[2].get_motion() == 2) {
						memDC.SelectObject(&CASH_R2);
						mdcOffScreen.TransparentBlt(obj_stf[2].get_point_x(), obj_stf[2].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));

					}
					else if (obj_stf[2].get_motion() == 3) {
						memDC.SelectObject(&CASH_R3);
						mdcOffScreen.TransparentBlt(obj_stf[2].get_point_x(), obj_stf[2].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));
					}
				}
				if (obj_stf[2].get_direction() == 4) {
					if (obj_stf[2].get_motion() == 1) {
						memDC.SelectObject(&CASH_F1);
						mdcOffScreen.TransparentBlt(obj_stf[2].get_point_x(), obj_stf[2].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));

					}
					else if (obj_stf[2].get_motion() == 2) {
						memDC.SelectObject(&CASH_F2);
						mdcOffScreen.TransparentBlt(obj_stf[2].get_point_x(), obj_stf[2].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));

					}
					else if (obj_stf[2].get_motion() == 3) {
						memDC.SelectObject(&CASH_F3);
						mdcOffScreen.TransparentBlt(obj_stf[2].get_point_x(), obj_stf[2].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));
					}
				}
				if (obj_stf[2].get_direction() == 5) {
					if (obj_stf[2].get_motion() == 1) {
						memDC.SelectObject(&CASH_P);
						mdcOffScreen.TransparentBlt(obj_stf[2].get_point_x(), obj_stf[2].get_point_y(), CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 2, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));

					}

				}

			}

			/* 캐셔 이미지는 고정, 따라서 for문을 통한 갱신을 하지 않음*/
			memDC.SelectObject(&CASH_P);
			mdcOffScreen.TransparentBlt(210, 375, CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 3, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));

			memDC.SelectObject(&CASH_L2);
			mdcOffScreen.TransparentBlt(185, 350, CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 3, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));

			for (int i = 0; i < 2; i++) {

				memDC.SelectObject(&COOK_P);
				mdcOffScreen.TransparentBlt(350, 215, CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 3, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));


				memDC.SelectObject(&COOK_H2);
				mdcOffScreen.TransparentBlt(420, 175, CASH_bmpinfo.bmWidth / 2, CASH_bmpinfo.bmHeight / 3, &memDC, 0, 0, CASH_bmpinfo.bmWidth, CASH_bmpinfo.bmHeight, RGB(0, 255, 0));

			}


			for (int i = 0; i < THREAD_CNT; i++) {
				if (obj_car[i].get_avata() == 1) {
					if (obj_car[i].get_direction() == 1) {

						memDC.SelectObject(&CAR1_H);
						mdcOffScreen.TransparentBlt(obj_car[i].get_point_x(), obj_car[i].get_point_y(), CAR1_bmpinfo.bmWidth / 3, CAR1_bmpinfo.bmHeight / 3, &memDC, 0, 0, CAR1_bmpinfo.bmWidth, CAR1_bmpinfo.bmHeight, RGB(0, 255, 0));
					}

					else if (obj_car[i].get_direction() == 2) {
						memDC.SelectObject(&CAR1_L);
						mdcOffScreen.TransparentBlt(obj_car[i].get_point_x(), obj_car[i].get_point_y(), CAR1_bmpinfo.bmWidth / 3, CAR1_bmpinfo.bmHeight / 3, &memDC, 0, 0, 312, 198, RGB(0, 255, 0));

					}
					else if (obj_car[i].get_direction() == 3) {
						memDC.SelectObject(&CAR1_F);
						mdcOffScreen.TransparentBlt(obj_car[i].get_point_x(), obj_car[i].get_point_y(), CAR1_bmpinfo.bmWidth / 3, CAR1_bmpinfo.bmHeight / 3, &memDC, 0, 0, CAR1_bmpinfo.bmWidth, CAR1_bmpinfo.bmHeight, RGB(0, 255, 0));
					}
				}
				if (obj_car[i].get_avata() == 2) {
					if (obj_car[i].get_direction() == 1) {

						memDC.SelectObject(&CAR2_H);
						mdcOffScreen.TransparentBlt(obj_car[i].get_point_x(), obj_car[i].get_point_y(), CAR2_bmpinfo.bmWidth / 2, CAR2_bmpinfo.bmHeight / 2, &memDC, 0, 0, CAR2_bmpinfo.bmWidth, CAR2_bmpinfo.bmHeight, RGB(0, 255, 0));
					}

					else if (obj_car[i].get_direction() == 2) {
						memDC.SelectObject(&CAR2_L);
						mdcOffScreen.TransparentBlt(obj_car[i].get_point_x(), obj_car[i].get_point_y(), CAR2_bmpinfo.bmWidth / 2, CAR2_bmpinfo.bmHeight / 2, &memDC, 0, 0, 312, 198, RGB(0, 255, 0));

					}
					else if (obj_car[i].get_direction() == 3) {
						memDC.SelectObject(&CAR2_F);
						mdcOffScreen.TransparentBlt(obj_car[i].get_point_x(), obj_car[i].get_point_y(), CAR2_bmpinfo.bmWidth / 2, CAR2_bmpinfo.bmHeight / 2, &memDC, 0, 0, CAR2_bmpinfo.bmWidth, CAR2_bmpinfo.bmHeight, RGB(0, 255, 0));
					}
				}
				if (obj_car[i].get_avata() == 3) {
					if (obj_car[i].get_direction() == 1) {

						memDC.SelectObject(&CAR3_H);
						mdcOffScreen.TransparentBlt(obj_car[i].get_point_x(), obj_car[i].get_point_y(), CAR3_bmpinfo.bmWidth / 2, CAR3_bmpinfo.bmHeight / 2, &memDC, 0, 0, CAR3_bmpinfo.bmWidth, CAR3_bmpinfo.bmHeight, RGB(0, 255, 0));
					}

					else if (obj_car[i].get_direction() == 2) {
						memDC.SelectObject(&CAR3_L);
						mdcOffScreen.TransparentBlt(obj_car[i].get_point_x(), obj_car[i].get_point_y(), CAR3_bmpinfo.bmWidth / 2, CAR3_bmpinfo.bmHeight / 2, &memDC, 0, 0, 312, 179, RGB(0, 255, 0));

					}
					else if (obj_car[i].get_direction() == 3) {
						memDC.SelectObject(&CAR3_F);
						mdcOffScreen.TransparentBlt(obj_car[i].get_point_x(), obj_car[i].get_point_y(), CAR3_bmpinfo.bmWidth / 2, CAR3_bmpinfo.bmHeight / 2, &memDC, 0, 0, CAR3_bmpinfo.bmWidth, CAR3_bmpinfo.bmHeight, RGB(0, 255, 0));
					}
				}
				if (obj_car[i].get_avata() == 4) {
					if (obj_car[i].get_direction() == 1) {

						memDC.SelectObject(&CAR4_H);
						mdcOffScreen.TransparentBlt(obj_car[i].get_point_x(), obj_car[i].get_point_y(), CAR4_bmpinfo.bmWidth / 2, CAR4_bmpinfo.bmHeight / 2, &memDC, 0, 0, CAR4_bmpinfo.bmWidth, CAR4_bmpinfo.bmHeight, RGB(0, 255, 0));
					}

					else if (obj_car[i].get_direction() == 2) {
						memDC.SelectObject(&CAR4_L);
						mdcOffScreen.TransparentBlt(obj_car[i].get_point_x(), obj_car[i].get_point_y(), CAR4_bmpinfo.bmWidth / 2, CAR4_bmpinfo.bmHeight / 2, &memDC, 0, 0, 312, 179, RGB(0, 255, 0));

					}
					else if (obj_car[i].get_direction() == 3) {
						memDC.SelectObject(&CAR4_F);
						mdcOffScreen.TransparentBlt(obj_car[i].get_point_x(), obj_car[i].get_point_y(), CAR4_bmpinfo.bmWidth / 2, CAR4_bmpinfo.bmHeight / 2, &memDC, 0, 0, CAR4_bmpinfo.bmWidth, CAR4_bmpinfo.bmHeight, RGB(0, 255, 0));
					}
				}
				if (obj_car[i].get_avata() == 5) {
					if (obj_car[i].get_direction() == 1) {

						memDC.SelectObject(&CAR5_H);
						mdcOffScreen.TransparentBlt(obj_car[i].get_point_x(), obj_car[i].get_point_y(), CAR5_bmpinfo.bmWidth / 2, CAR5_bmpinfo.bmHeight / 2, &memDC, 0, 0, CAR5_bmpinfo.bmWidth, CAR5_bmpinfo.bmHeight, RGB(0, 255, 0));
					}

					else if (obj_car[i].get_direction() == 2) {
						memDC.SelectObject(&CAR5_L);
						mdcOffScreen.TransparentBlt(obj_car[i].get_point_x(), obj_car[i].get_point_y(), CAR5_bmpinfo.bmWidth / 2, CAR5_bmpinfo.bmHeight / 2, &memDC, 0, 0, 312, 179, RGB(0, 255, 0));

					}
					else if (obj_car[i].get_direction() == 3) {
						memDC.SelectObject(&CAR5_F);
						mdcOffScreen.TransparentBlt(obj_car[i].get_point_x(), obj_car[i].get_point_y(), CAR5_bmpinfo.bmWidth / 2, CAR5_bmpinfo.bmHeight / 2, &memDC, 0, 0, CAR5_bmpinfo.bmWidth, CAR5_bmpinfo.bmHeight, RGB(0, 255, 0));
					}
				}
				if (obj_car[i].get_avata() == 6) {
					if (obj_car[i].get_direction() == 1) {

						memDC.SelectObject(&CAR6_H);
						mdcOffScreen.TransparentBlt(obj_car[i].get_point_x(), obj_car[i].get_point_y(), CAR6_bmpinfo.bmWidth / 2, CAR6_bmpinfo.bmHeight / 2, &memDC, 0, 0, CAR6_bmpinfo.bmWidth, CAR6_bmpinfo.bmHeight, RGB(0, 255, 0));
					}

					else if (obj_car[i].get_direction() == 2) {
						memDC.SelectObject(&CAR6_L);
						mdcOffScreen.TransparentBlt(obj_car[i].get_point_x(), obj_car[i].get_point_y(), CAR6_bmpinfo.bmWidth / 2, CAR6_bmpinfo.bmHeight / 2, &memDC, 0, 0, 312, 179, RGB(0, 255, 0));

					}
					else if (obj_car[i].get_direction() == 3) {
						memDC.SelectObject(&CAR6_F);
						mdcOffScreen.TransparentBlt(obj_car[i].get_point_x(), obj_car[i].get_point_y(), CAR6_bmpinfo.bmWidth / 2, CAR6_bmpinfo.bmHeight / 2, &memDC, 0, 0, CAR6_bmpinfo.bmWidth, CAR6_bmpinfo.bmHeight, RGB(0, 255, 0));
					}
				}
			}
		
			mdcOffScreen.SetBkMode(TRANSPARENT);

		dc.BitBlt(0, 0, m_Bitmap.bmWidth, m_Bitmap.bmHeight, &mdcOffScreen, 0, 0, SRCCOPY);
		UpdateWindow();
	}
	

		memDC.DeleteDC();
		mdcOffScreen.SelectObject(oldbitmap);
		mdcOffScreen.DeleteDC();
		bmpOffScreen.DeleteObject();

		this->Invalidate();

		CDialogEx::OnPaint();
	}
	
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CComputerSWDesignDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



BOOL CComputerSWDesignDlg::OnEraseBkgnd(CDC* pDC)
{
	
	
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	//return CDialogEx::OnEraseBkgnd(pDC);
	return FALSE;
}


void CComputerSWDesignDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	lpMMI->ptMaxSize.x = 1075;
	lpMMI->ptMaxSize.y = 808;
	lpMMI->ptMinTrackSize.x = lpMMI->ptMaxSize.x;
	lpMMI->ptMinTrackSize.y = lpMMI->ptMaxSize.y;
	lpMMI->ptMaxTrackSize.x = lpMMI->ptMaxSize.x;
	lpMMI->ptMaxTrackSize.y = lpMMI->ptMaxSize.y;
	
	CDialogEx::OnGetMinMaxInfo(lpMMI);
}


void CComputerSWDesignDlg::OnTimer(UINT_PTR nIDEvent)
{
	CDialogEx::OnTimer(nIDEvent);
}
