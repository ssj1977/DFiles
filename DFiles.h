
// DFiles.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CDFilesApp:
// �� Ŭ������ ������ ���ؼ��� DFiles.cpp�� �����Ͻʽÿ�.
//

class CDFilesApp : public CWinApp
{
public:
	CDFilesApp();
// �������Դϴ�.
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CDFilesApp theApp;
