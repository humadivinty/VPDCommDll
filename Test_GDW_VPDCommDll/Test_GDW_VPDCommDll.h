
// Test_GDW_VPDCommDll.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTest_GDW_VPDCommDllApp: 
// �йش����ʵ�֣������ Test_GDW_VPDCommDll.cpp
//

class CTest_GDW_VPDCommDllApp : public CWinApp
{
public:
	CTest_GDW_VPDCommDllApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTest_GDW_VPDCommDllApp theApp;