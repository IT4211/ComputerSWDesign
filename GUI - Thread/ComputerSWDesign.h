
// ComputerSWDesign.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CComputerSWDesignApp:
// �� Ŭ������ ������ ���ؼ��� ComputerSWDesign.cpp�� �����Ͻʽÿ�.
//

class CComputerSWDesignApp : public CWinApp
{
public:
	CComputerSWDesignApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
	ULONG_PTR m_gdiplusToken;
	virtual int ExitInstance();
};

extern CComputerSWDesignApp theApp;