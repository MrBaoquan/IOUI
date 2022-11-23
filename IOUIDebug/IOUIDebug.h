
// IOUIDebug.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CIOUIDebugApp: 
// �йش����ʵ�֣������ IOUIDebug.cpp
//

class CIOUIDebugApp : public CWinApp
{
public:
	CIOUIDebugApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CIOUIDebugApp theApp;