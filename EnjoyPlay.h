
// EnjoyPlay.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CEnjoyPlayApp:
// �йش����ʵ�֣������ EnjoyPlay.cpp
//

class CEnjoyPlayApp : public CWinApp
{
public:
	CEnjoyPlayApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CEnjoyPlayApp theApp;