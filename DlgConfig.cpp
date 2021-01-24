// DlgConfig.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DFiles.h"
#include "DlgConfig.h"
#include "afxdialogex.h"


// DlgConfig 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgConfig, CDialogEx)

CDlgConfig::CDlgConfig(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CONFIG, pParent)
{
	fontsize = 12;
	iconsize = SHIL_EXTRALARGE;
}

CDlgConfig::~CDlgConfig()
{
}

void CDlgConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ICONSIZE, m_cbIconSize);
	DDX_Control(pDX, IDC_EDIT_FONTSIZE, m_editFontSize);
}


BEGIN_MESSAGE_MAP(CDlgConfig, CDialogEx)
END_MESSAGE_MAP()


// DlgConfig 메시지 처리기입니다.


BOOL CDlgConfig::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_cbIconSize.InsertString(0, L"16 x 16"); 
	m_cbIconSize.SetItemData(0, SHIL_SMALL);
	m_cbIconSize.InsertString(1, L"32 x 32");
	m_cbIconSize.SetItemData(1, SHIL_LARGE);
	m_cbIconSize.InsertString(2, L"48 x 48");
	m_cbIconSize.SetItemData(2, SHIL_EXTRALARGE);
	m_cbIconSize.InsertString(3, L"256 x 256");
	m_cbIconSize.SetItemData(3, SHIL_JUMBO);
	m_cbIconSize.InsertString(4, L"No Icon");
	m_cbIconSize.SetItemData(4, -1);
	switch (iconsize)
	{
	case SHIL_SMALL: m_cbIconSize.SetCurSel(0); break;
	case SHIL_LARGE: m_cbIconSize.SetCurSel(1); break;
	case SHIL_EXTRALARGE: m_cbIconSize.SetCurSel(2); break;
	case SHIL_JUMBO: m_cbIconSize.SetCurSel(3); break;
	case -1: m_cbIconSize.SetCurSel(4); break;
	}

	CString strText;
	strText.Format(L"%d", fontsize);
	m_editFontSize.SetWindowTextW(strText);


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgConfig::OnCancel()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CDialogEx::OnCancel();
}


void CDlgConfig::OnOK()
{
	CString strFontSize;
	m_editFontSize.GetWindowText(strFontSize);
	fontsize = _ttoi(strFontSize);
	iconsize = m_cbIconSize.GetItemData(m_cbIconSize.GetCurSel());

	CDialogEx::OnOK();
}
