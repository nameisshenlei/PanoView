
// PanoView.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CPanoViewApp: 
// �йش����ʵ�֣������ PanoView.cpp
//

class CPanoViewApp : public CWinApp
{
public:
	CPanoViewApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CPanoViewApp theApp;