
// ComputerSWDesignDlg.h : ��� ����
//

#pragma once

class Customer;
class Staff;
class Cashier;
class Car;

// CComputerSWDesignDlg ��ȭ ����
class CComputerSWDesignDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CComputerSWDesignDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_COMPUTERSWDESIGN_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
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
