
// ComputerSWDesignDlg.cpp : ���� ����
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


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
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


// CComputerSWDesignDlg ��ȭ ����



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


// CComputerSWDesignDlg �޽��� ó����

BOOL CComputerSWDesignDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	// ���� ���̾�α� �ܺ� ���� ������ �ʱ�ȭ
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

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
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

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	// �ʿ��� �׸��� �ε��Ѵ�.
	m_background.LoadBitmapW(IDB_BACKGROUND);
	m_background.GetObject(sizeof(BITMAP), (LPVOID)&m_Bitmap);

	/* Create a semaphore */

	// [ī����] ��������� �մ��� 20�� �����ϵ��� �����Ѵ�.
	smp_gate = CreateSemaphore(NULL, MAX_CUSTOMER_CNT, MAX_CUSTOMER_CNT, NULL);
	if (smp_gate == NULL) {
		_tprintf(_T("CreateSemaphore _gate_ error : %d\n"), GetLastError());
		return 1;
	}

	for (i = 0; i < THREAD_CNT; i++) {
		// [����ȭ] �մ��� �������� ���� �� ������ �ֹ��� �޴´�.
		smp_staff[i] = CreateSemaphore(NULL, 0, 1, NULL);
		if (smp_staff == NULL) {
			_tprintf(_T("CreateSemaphore _staff_ error : %d\n"), GetLastError());
			return 1;
		}
	}

	for (i = 0; i < THREAD_CNT; i++) {
		// [����ȭ] �մ��� ĳ�ſ��� ���� �� ĳ�Ű� ����� ���ش�.
		smp_cashier[i] = CreateSemaphore(NULL, 0, 1, NULL);
		if (smp_cashier[i] == NULL) {
			_tprintf(_T("CreateSemaphore _cashier_ error : %d\n"), GetLastError());
			return 1;
		}
	}

	for (i = 0; i < THREAD_CNT; i++) {
		// [����ȭ] ������ �մԿ��� �ֹ��� �Ϸ�Ǿ����� �˷��ش�(�մ��� ĳ�ſ��� ����).
		smp_wait_order[i] = CreateSemaphore(NULL, 0, 1, NULL);
		if (smp_wait_order[i] == NULL) {
			_tprintf(_T("CreateSemaphore _wait_order_ error : %d\n"), GetLastError());
			return 1;
		}
	}

	for (i = 0; i < THREAD_CNT; i++) {
		// [����ȭ] ĳ�Ű� �մԿ��� �ܹ��Ű� �� ����������� �˷��ش�(�ܹ��Ÿ� �մԿ��� �ش�).
		smp_burger[i] = CreateSemaphore(NULL, 0, 1, NULL);
		if (smp_burger[i] == NULL) {
			_tprintf(_T("CreateSemaphore _burger_ error : %d\n"), GetLastError());
			return 1;
		}
	}
	// [ī����] �մ��� ���� �� �ִ� ���̺��� ���� ���ѵǾ� �ִ�(�Ļ��� �� �ִ� ���� ���ѵȴ�).
	smp_table = CreateSemaphore(NULL, TABLE_CNT, TABLE_CNT, NULL);
	if (smp_table == NULL) {
		_tprintf(_T("CreateSemaphore _table_ error : %d\n"), GetLastError());
		return 1;
	}

	for (i = 0; i < THREAD_CNT; i++) {
		// [����ȭ] ������ �丮�翡�� �ܹ��Ÿ� ������ ��û�Ѵ�.
		smp_ck_req[i] = CreateSemaphore(NULL, 0, 1, NULL);
		if (smp_ck_req[i] == NULL) {
			_tprintf(_T("CreateSemaphore _ck_req_ error : %d\n"), GetLastError());
			return 1;
		}
	}

	for (i = 0; i < THREAD_CNT; i++) {
		// [����ȭ] �丮�簡 ĳ�ſ��� �ܹ��Ű� �� ��������ٰ� �˷��ش�(�ܹ��Ÿ� ĳ�ſ��� �ش�).
		smp_ck_resp[i] = CreateSemaphore(NULL, 0, 1, NULL);
		if (smp_ck_resp[i] == NULL) {
			_tprintf(_T("CreateSemaphore _ck_resp_ error : %d\n"), GetLastError());
			return 1;
		}
	}

	// �������� customer_number�� ���� ��ȣ������ �����ϴ� �������� 
	smp_mutex1 = CreateSemaphore(NULL, 1, 1, NULL);
	if (smp_mutex1 == NULL) {
		_tprintf(_T("CreateSemaphore _mutex1_ error : %d\n"), GetLastError());
		return 1;
	}

	// �������� customer_number�� ���� ��ȣ������ �����ϴ� �������� 
	smp_wait_mutex = CreateSemaphore(NULL, 1, 1, NULL);
	if (smp_wait_mutex == NULL) {
		_tprintf(_T("CreateSemaphore smp_wait_mutex_ error : %d\n"), GetLastError());
		return 1;
	}

	// ���� ť�� ���� ��ȣ������ �����ϴ� ��������
	smp_qmutex1 = CreateSemaphore(NULL, 1, 1, NULL);
	if (smp_qmutex1 == NULL) {
		_tprintf(_T("CreateSemaphore _qmutex1_ error : %d\n"), GetLastError());
		return 1;
	}

	// ���� ť�� ���� ��ȣ������ �����ϴ� ��������
	smp_qmutex2 = CreateSemaphore(NULL, 1, 1, NULL);
	if (smp_qmutex2 == NULL) {
		_tprintf(_T("CreateSemaphore _qmutex2_ error : %d\n"), GetLastError());
		return 1;
	}

	// ���� ť�� ���� ��ȣ������ �����ϴ� ��������
	smp_qmutex3 = CreateSemaphore(NULL, 1, 1, NULL);
	if (smp_qmutex3 == NULL) {
		_tprintf(_T("CreateSemaphore _qmutex3_ error : %d\n"), GetLastError());
		return 1;
	}

	// enter_Mcd���� dc�� ���� ��ȣ������ �����ϴ� ��������
	smp_dc_eMcd = CreateSemaphore(NULL, 1, 1, NULL);
	if (smp_dc_eMcd == NULL) {
		_tprintf(_T("CreateSemaphore _smp_dc_eMcd_ error : %d\n"), GetLastError());
		return 1;
	}

	// order���� dc�� ���� ��ȣ������ �����ϴ� ��������
	smp_dc_order = CreateSemaphore(NULL, 1, 1, NULL);
	if (smp_dc_order == NULL) {
		_tprintf(_T("CreateSemaphore _smp_dc_order_ error : %d\n"), GetLastError());
		return 1;
	}

	// drive thru�� �ڵ��� �� �븸 ������ �� �ֵ��� ī������ �����ϴ� ��������
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

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CComputerSWDesignDlg::OnPaint()
{

	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		
		// �������� �׸��ϴ�.
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
		
		//1�� ĳ����
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
		//3��ĳ����
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
		//4��ĳ����
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
		//5��ĳ����
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
		//6��ĳ����
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
		//ĳ��
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
		//�丮��
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
		//�ڵ���1
		CAR1_F.LoadBitmapW(IDB_CAR1_F);
		CAR1_F.GetObject(sizeof(BITMAP), (LPVOID)&CAR1_bmpinfo);
		CAR1_L.LoadBitmapW(IDB_CAR1_L);
		CAR1_L.GetObject(sizeof(BITMAP), (LPVOID)&CAR1_bmpinfo);
		CAR1_H.LoadBitmapW(IDB_CAR1_H);
		CAR1_H.GetObject(sizeof(BITMAP), (LPVOID)&CAR1_bmpinfo);
		//�ڵ���2
		CAR2_F.LoadBitmapW(IDB_CAR2_F);
		CAR2_F.GetObject(sizeof(BITMAP), (LPVOID)&CAR2_bmpinfo);
		CAR2_L.LoadBitmapW(IDB_CAR2_L);
		CAR2_L.GetObject(sizeof(BITMAP), (LPVOID)&CAR2_bmpinfo);
		CAR2_H.LoadBitmapW(IDB_CAR2_H);
		CAR2_H.GetObject(sizeof(BITMAP), (LPVOID)&CAR2_bmpinfo);
		//�ڵ���3
		CAR3_F.LoadBitmapW(IDB_CAR3_F);
		CAR3_F.GetObject(sizeof(BITMAP), (LPVOID)&CAR3_bmpinfo);
		CAR3_L.LoadBitmapW(IDB_CAR3_L);
		CAR3_L.GetObject(sizeof(BITMAP), (LPVOID)&CAR3_bmpinfo);
		CAR3_H.LoadBitmapW(IDB_CAR3_H);
		CAR3_H.GetObject(sizeof(BITMAP), (LPVOID)&CAR3_bmpinfo);
		//�ڵ���4
		CAR4_F.LoadBitmapW(IDB_CAR4_F);
		CAR4_F.GetObject(sizeof(BITMAP), (LPVOID)&CAR4_bmpinfo);
		CAR4_L.LoadBitmapW(IDB_CAR4_L);
		CAR4_L.GetObject(sizeof(BITMAP), (LPVOID)&CAR4_bmpinfo);
		CAR4_H.LoadBitmapW(IDB_CAR4_H);
		CAR4_H.GetObject(sizeof(BITMAP), (LPVOID)&CAR4_bmpinfo);
		//�ڵ���5
		CAR5_F.LoadBitmapW(IDB_CAR5_F);
		CAR5_F.GetObject(sizeof(BITMAP), (LPVOID)&CAR5_bmpinfo);
		CAR5_L.LoadBitmapW(IDB_CAR5_L);
		CAR5_L.GetObject(sizeof(BITMAP), (LPVOID)&CAR5_bmpinfo);
		CAR5_H.LoadBitmapW(IDB_CAR5_H);
		CAR5_H.GetObject(sizeof(BITMAP), (LPVOID)&CAR5_bmpinfo);
		//�ڵ���6
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

				//1��ĳ����
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
				//2��ĳ����


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
				//4��ĳ����
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
				//5��ĳ���� 
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
				//6��ĳ����
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
				// �޴� ��� 
				CString text(obj_cstmr[i].get_char_buf());
				mdcOffScreen.TextOutW(10, 10, text, text.GetLength());
			}

			//�ֹ��޴� ���� �̹��� ���
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

			/* ĳ�� �̹����� ����, ���� for���� ���� ������ ���� ����*/
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

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CComputerSWDesignDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



BOOL CComputerSWDesignDlg::OnEraseBkgnd(CDC* pDC)
{
	
	
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	//return CDialogEx::OnEraseBkgnd(pDC);
	return FALSE;
}


void CComputerSWDesignDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
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
