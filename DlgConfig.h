#pragma once
#include "afxwin.h"


// DlgConfig ��ȭ �����Դϴ�.

class CDlgConfig : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgConfig)

public:
	CDlgConfig(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgConfig();
	int fontsize;
	int iconsize;

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CONFIG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CComboBox m_cbIconSize;
	CEdit m_editFontSize;
	virtual void OnCancel();
	virtual void OnOK();
};
