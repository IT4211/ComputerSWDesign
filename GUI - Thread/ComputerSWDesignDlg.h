
// ComputerSWDesignDlg.h : 헤더 파일
//

#pragma once

class Customer;
class Staff;
class Cashier;
class Car;

// CComputerSWDesignDlg 대화 상자
class CComputerSWDesignDlg : public CDialogEx
{
// 생성입니다.
public:
	CComputerSWDesignDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_COMPUTERSWDESIGN_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CBitmap m_background;
	BITMAP m_Bitmap;
	Customer *obj_cstmr;
	Staff *obj_stf;
	//Staff *obj_stf1;
	//Staff *obj_stf2;
	//Staff *obj_stf3;
	Car *obj_car;
};
