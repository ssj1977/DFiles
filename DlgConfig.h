#pragma once
#include "afxwin.h"


// DlgConfig 대화 상자입니다.

class CDlgConfig : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgConfig)

public:
	CDlgConfig(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgConfig();
	int fontsize;
	int iconsize;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CONFIG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CComboBox m_cbIconSize;
	CEdit m_editFontSize;
	virtual void OnCancel();
	virtual void OnOK();
};
