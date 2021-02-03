
// DFilesDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "DFiles.h"
#include "DFilesDlg.h"
#include "DlgConfig.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <CommonControls.h>
#include <shlwapi.h>
#include "DCommon.h"

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.
# define BTNW 60

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnSize(UINT nType, int cx, int cy);
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
//	ON_WM_SIZE()
END_MESSAGE_MAP()

CString GetPathName(CString strPath);

// CDFilesDlg 대화 상자



CDFilesDlg::CDFilesDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DFILES_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nCurrentTab = 0;
	m_rcMain = CRect(0, 0, 0, 0);
	m_bEnable_UpdateCurrentPathByClick = FALSE;
	m_nSelectedCount = 0;
	m_fontsize = 12;
	m_pSysImgList = NULL;
	m_iconsize = SHIL_EXTRALARGE;
}

void CDFilesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_PATH, m_tabPath);
	DDX_Control(pDX, IDC_EDIT_PATH, m_editPath);
}

BEGIN_MESSAGE_MAP(CDFilesDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_PATH, &CDFilesDlg::OnBnClickedBtnPath)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_PATH, &CDFilesDlg::OnTcnSelchangeTabPath)
	ON_REGISTERED_MESSAGE(AFX_WM_CHANGE_CURRENT_FOLDER, &CDFilesDlg::OnAfxWmChangeCurrentFolder)
	ON_WM_CREATE()
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CDFilesDlg 메시지 처리기
void CDFilesDlg::InitFont()
{
	CFont* pFont = GetFont();
	LOGFONT lf;
	pFont->GetLogFont(&lf);
	lf.lfHeight = m_fontsize * 10 * -1;//-1 * MulDiv(m_fontsize, GetDeviceCaps(GetDC()->GetSafeHdc(), LOGPIXELSY), 72);
	m_font.DeleteObject();
	m_font.CreatePointFontIndirect(&lf);
}


BOOL CDFilesDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	InitFont();
	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.
	InitToolBar();
	InitINIPath();
	UpdateIconSize();
	INILoad(m_strINIPath);
	m_tabPath.SetFont(&m_font);
	m_editPath.SetFont(&m_font);
	InitTabs();
	m_editPath.EnableFolderBrowseButton();
	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	if (!m_rcMain.IsRectEmpty())
	{
		CRect rcWindow;
		CRect rcTemp;
		GetDesktopWindow()->GetClientRect(rcWindow);
		rcTemp.IntersectRect(rcWindow, m_rcMain);
		if (rcTemp.Width() < 100 || rcTemp.Height() < 100)
		{
			m_rcMain = CRect(0, 0, 400, 300);
		}
		MoveWindow(m_rcMain, TRUE);
	}
	ResizeDlgItems();
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CDFilesDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CDFilesDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CDFilesDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CDFilesDlg::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	//CDialogEx::OnOK();
}


void CDFilesDlg::OnCancel()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	GetWindowRect(m_rcMain);
	INISave(m_strINIPath);
	for (int i = 0; i < m_aTabInfo.GetSize(); i++)
	{
		if (m_aTabInfo[i].pWnd != NULL) delete m_aTabInfo[i].pWnd;
	}
	m_font.DeleteObject();
	CDialogEx::OnCancel();
}


void CDFilesDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	ResizeDlgItems();
}


void CDFilesDlg::InitTabs()
{
	if (m_aTabInfo.GetSize() == 0)
	{
		CTabInfo tabInfo(L"", 0, TRUE);
		m_aTabInfo.Add(tabInfo);
	}

	for (int i = 0; i < m_aTabInfo.GetSize(); i++)
	{
		m_tabPath.InsertItem(i, GetPathName(m_aTabInfo[i].strPath));
	}
	SetCurrentTab(m_nCurrentTab);
}


void CDFilesDlg::ResizeDlgItems()
{
	if (::IsWindow(m_tabPath.GetSafeHwnd()) && GetCurrentListCtrl()!=NULL)
	{
		LOGFONT lf;
		m_font.GetLogFont(&lf);
		int SU = abs(lf.lfHeight) * 1.5;//m_fontsize * 2;

		CRect rc, rcWnd;
		GetClientRect(rcWnd);
		if (::IsWindow(m_editPath.GetSafeHwnd()))
		{
			rc = CRect(rcWnd.left + 2, rcWnd.top + 2, rcWnd.right - BTNW, rcWnd.top + SU + 2);
			m_editPath.MoveWindow(rc);
		}
		if (::IsWindow(GetDlgItem(IDC_BTN_PATH)->GetSafeHwnd()))
		{
			rc = CRect(rcWnd.right - BTNW, rcWnd.top + 2, rcWnd.right - 2, rcWnd.top + 2 + SU);
			GetDlgItem(IDC_BTN_PATH)->MoveWindow(rc);
		}
		if (::IsWindow(m_tabPath.GetSafeHwnd()))
		{
			rc = CRect(rcWnd.left + 2, rcWnd.top + SU + 2 + 2, rcWnd.right, rcWnd.top + SU + 2 + SU + 9);
			m_tabPath.MoveWindow(rc);
		}
		if (GetCurrentListCtrl() !=NULL && ::IsWindow(GetCurrentListCtrl()->GetSafeHwnd()))
		{
			rc = CRect(rcWnd.left + 2, rcWnd.top + SU + 2 + SU + 9, rcWnd.right - BTNW + 2, rcWnd.bottom - SU);
			for (int i = 0; i < m_aTabInfo.GetSize(); i++)
			{
				if (m_aTabInfo[i].pWnd!=NULL) m_aTabInfo[i].pWnd->MoveWindow(rc);
			}
		}
		if (::IsWindow(m_tool.GetSafeHwnd()))
		{
			rc = CRect(rcWnd.right - BTNW + 2, rcWnd.top + SU + 2 + SU + 9, rcWnd.right, rcWnd.bottom - SU);
			m_tool.MoveWindow(rc);
		}
		if (::IsWindow(GetDlgItem(IDC_STATIC_BAR)->GetSafeHwnd()))
		{
			rc = CRect(rcWnd.left, rcWnd.bottom - SU, rcWnd.right, rcWnd.bottom);
			GetDlgItem(IDC_STATIC_BAR)->MoveWindow(rc);
		}
	}
}


void CDFilesDlg::INISave(CString strFile)
{
	CString strData, strLine, str1, str2;
	strLine.Format(_T("MainRectLeft=%d\r\n"), m_rcMain.left);		strData += strLine;
	strLine.Format(_T("MainRectTop=%d\r\n"), m_rcMain.top);		strData += strLine;
	strLine.Format(_T("MainRectRight=%d\r\n"), m_rcMain.right);	strData += strLine;
	strLine.Format(_T("MainRectBottom=%d\r\n"), m_rcMain.bottom);	strData += strLine;
	strLine.Format(_T("CurrentTab=%d\r\n"), m_nCurrentTab);	strData += strLine;
	strLine.Format(_T("FontSize=%d\r\n"), m_fontsize);	strData += strLine;
	strLine.Format(_T("IconSize=%d\r\n"), m_iconsize);	strData += strLine;

	for (int i = 0; i < m_aTabInfo.GetSize(); i++)
	{
		strLine.Format(_T("Tab_Path=%s\r\n"), m_aTabInfo[i].strPath);	strData += strLine;
		strLine.Format(_T("Tab_SortCol=%d\r\n"), m_aTabInfo[i].iSortColumn);	strData += strLine;
		strLine.Format(_T("Tab_SortAscend=%d\r\n"), m_aTabInfo[i].bSortAscend);	strData += strLine;
	}
	WriteCStringToFile(strFile, strData);
}


void CDFilesDlg::INILoad(CString strFile)
{
	CString strData, strLine, str1, str2, strTemp;
	m_aTabInfo.RemoveAll();
	ReadFileToCString(strFile, strData);
	int nPos = 0;
	int nTabCount = -1;
	while (nPos != -1)
	{
		nPos = GetLine(strData, nPos, strLine, _T("\r\n"));
		GetToken(strLine, str1, str2, _T('='), FALSE);
		if (str1.CompareNoCase(_T("MainRectLeft")) == 0) m_rcMain.left = _ttoi(str2);
		else if (str1.CompareNoCase(_T("MainRectTop")) == 0) m_rcMain.top = _ttoi(str2);
		else if (str1.CompareNoCase(_T("MainRectRight")) == 0) m_rcMain.right = _ttoi(str2);
		else if (str1.CompareNoCase(_T("MainRectBottom")) == 0) m_rcMain.bottom = _ttoi(str2);
		else if (str1.CompareNoCase(_T("CurrentTab")) == 0) m_nCurrentTab = _ttoi(str2);
		else if (str1.CompareNoCase(_T("FontSize")) == 0) m_fontsize = _ttoi(str2);
		else if (str1.CompareNoCase(_T("IconSize")) == 0) m_iconsize = _ttoi(str2);
		else if (str1.CompareNoCase(_T("Tab_Path")) == 0)
		{
			CTabInfo tabInfo(str2, 0, TRUE);
			nTabCount = m_aTabInfo.Add(tabInfo);
		}
		else if (str1.CompareNoCase(_T("Tab_SortCol")) == 0 && nTabCount != -1) m_aTabInfo[nTabCount].iSortColumn = _ttoi(str2);
		else if (str1.CompareNoCase(_T("Tab_SortAscend")) == 0 && nTabCount != -1) m_aTabInfo[nTabCount].bSortAscend = _ttoi(str2);
	}
}


void CDFilesDlg::InitINIPath()
{
	CString str0;
#ifdef _UNICODE
	int pNumArg;
	LPWSTR* pArgv = CommandLineToArgvW(GetCommandLine(), &pNumArg);
	if (pNumArg>0) str0 = pArgv[0];
	GlobalFree(pArgv);
#else
	CCommandLineInfo cmd;
	ParseCommandLine(cmd);
	str1 = cmd.m_strFileName;
	TCHAR szBuff[MAX_PATH];
	GetModuleFileName(m_hInstance, szBuff, MAX_PATH);
	str0 = szBuff;
#endif 
	if (str0.IsEmpty() == FALSE)
	{
		str0 = GetRealPath(str0, _T("exe"));
		m_strINIPath = str0.Left(str0.ReverseFind(_T('.')));
		if (!m_strINIPath.IsEmpty()) m_strINIPath += _T(".ini");
	}
}


BOOL CDFilesDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
			case VK_RETURN: 
				if (pMsg->hwnd == m_editPath.GetSafeHwnd())
				{
					UpdateCurrentPathByString();
					return TRUE;
				}
				break;
			case VK_BACK:
				if (pMsg->hwnd == GetCurrentListCtrl()->GetSafeHwnd())
				{
					OnCommand(IDM_OPENPARENT, 0);
					return TRUE;
				}
				break;

		}
	}
	if (GetCurrentListCtrl()->GetSelectedCount() != m_nSelectedCount)
	{
		UpdateBottomBar();
		m_nSelectedCount = GetCurrentListCtrl()->GetSelectedCount();
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CDFilesDlg::OnTcnSelchangeTabPath(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
	SetCurrentTab(m_tabPath.GetCurSel());
}


void CDFilesDlg::OnBnClickedBtnPath()
{
//	UpdateCurrentPathByString();

	if (m_aTabInfo.GetSize() <= m_nCurrentTab || GetCurrentListCtrl() == NULL) return;

	CString strPath, strName;
	GetDlgItemText(IDC_EDIT_PATH, strPath);

	CFileFind find;
	BOOL b = find.FindFile(strPath + _T("\\*.*"));
	int count = 0;
	while (b == TRUE)
	{
		b = find.FindNextFileW();
		count++;
	}
	AfxMessageBox(INTtoSTR(count));
}

void CDFilesDlg::InitToolBar()
{
	///////////////////////////////////////////////////////////////////	
	//툴바 처리 
	//m_bmpToolBG = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_TOOLBG));

	m_tool.CreateEx(this, TBSTYLE_FLAT, WS_BORDER | WS_CHILD | WS_VISIBLE | CBRS_LEFT
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC | CCS_VERT);
	m_tool.LoadToolBar(IDR_TB_MAIN);

}

int CDFilesDlg::TabAddNew(CString strPath)
{
	int iNewTabIndex = -1;
	CTabInfo tbTemp(strPath, TRUE, 0);
	iNewTabIndex = m_aTabInfo.Add(tbTemp);
	m_tabPath.InsertItem(iNewTabIndex, GetPathName(m_aTabInfo[iNewTabIndex].strPath));
	return iNewTabIndex;
}

BOOL CDFilesDlg::TabCreateList(int iTabIndex)
{
	if (iTabIndex < 0 || iTabIndex >= m_aTabInfo.GetSize()) return FALSE;
	CTabInfo& ti = m_aTabInfo[iTabIndex];
	CFileListCtrl* pList = new CFileListCtrl;
	CRect rc, rcWnd;
	GetClientRect(rcWnd);
	int SU = m_fontsize * 2;
	rc = CRect(rcWnd.left + 2, rcWnd.top + SU * 2 + 11, rcWnd.right - BTNW + 2, rcWnd.bottom- SU);
	m_bEnable_UpdateCurrentPathByClick = FALSE;
	if (pList->Create(WS_CHILD | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SHAREIMAGELISTS, rc, this, IDC_LIST_FILE) == FALSE)
	{
		delete pList;
		m_bEnable_UpdateCurrentPathByClick = TRUE;
		return FALSE;
	}
	pList->SetExtendedStyle(LVS_EX_FULLROWSELECT | WS_EX_CLIENTEDGE);
	pList->SetFont(&m_font);
	//pList->EnableShellContextMenu();
	pList->SetSortColumn(ti.iSortColumn, ti.bSortAscend);
	if (m_pSysImgList) ListView_SetImageList(pList->GetSafeHwnd(), m_pSysImgList, LVSIL_SMALL);
	ti.pWnd = (CWnd*)pList;
	pList->DisplayFolder(ti.strPath);
	m_bEnable_UpdateCurrentPathByClick = TRUE;
	return TRUE;
}

void CDFilesDlg::TabClose(int iTabIndex)
{
	if (m_aTabInfo.GetSize() <= 1) return;
	if (m_aTabInfo[iTabIndex].pWnd!=NULL) delete m_aTabInfo[iTabIndex].pWnd;
	m_aTabInfo.RemoveAt(iTabIndex);
	m_tabPath.DeleteItem(iTabIndex);
	if (iTabIndex == m_nCurrentTab)
	{
		if (iTabIndex > 0) iTabIndex--;
		SetCurrentTab(iTabIndex);
	}
}

CFileListCtrl * CDFilesDlg::GetCurrentListCtrl()
{
	CFileListCtrl* pList = NULL;
	if (m_aTabInfo.GetSize() > 0 && m_nCurrentTab < m_aTabInfo.GetSize() && m_nCurrentTab >= 0)
	{
		pList = (CFileListCtrl*)m_aTabInfo[m_nCurrentTab].pWnd;
	}
	return pList;
}

int CDFilesDlg::FindTabIndex(CFileListCtrl* pList)
{
	for (int i = 0; i < m_aTabInfo.GetSize(); i++)
	{
		if (m_aTabInfo[i].pWnd == pList) return i;
	}
	return -1;
}

void CDFilesDlg::SetTabTitle(int nIndex, CString strTitle)
{
	if (!strTitle.IsEmpty() && nIndex < m_tabPath.GetItemCount() )
	{
		TCITEM item;
		item.mask = TCIF_TEXT;
		item.pszText = strTitle.GetBuffer();
		m_tabPath.SetItem(nIndex, &item);
		strTitle.ReleaseBuffer();
	}
}

BOOL CDFilesDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	switch (wParam)
	{
	case IDM_OPENFILE:
		if (GetCurrentListCtrl()) GetCurrentListCtrl()->OpenSelectedItem();
		break;
	case IDM_OPENPARENT:
		if (GetCurrentListCtrl()) GetCurrentListCtrl()->OpenParentFolder();
		break;
	case IDM_REFRESH:
		if (GetCurrentListCtrl()) UpdateCurrentPathByString();
		break;
	case IDM_ADDTAB: 
		SetCurrentTab(TabAddNew(L""));
		break;
	case IDM_CLOSETAB:
		TabClose(m_nCurrentTab);
		break;
	case IDM_CONFIG:
		{
			CDlgConfig dlg;
			dlg.iconsize = m_iconsize;
			dlg.fontsize = m_fontsize;
			if (dlg.DoModal() == IDOK)
			{
				if (m_fontsize != dlg.fontsize)
				{
					m_fontsize = dlg.fontsize;
					UpdateFontSize();
				}
				if (m_iconsize != dlg.iconsize)
				{
					m_iconsize = dlg.iconsize;
					UpdateIconSize();
				}
			}
		}
		break;
	case IDM_UPDATE_TABCTRL:
		{
			CFileListCtrl* pList = (CFileListCtrl*)lParam;
			int nIndex = FindTabIndex(pList);
			if (nIndex != -1)
			{
				m_aTabInfo[nIndex].strPath = pList->GetCurrentFolder();
				SetTabTitle(nIndex, GetPathName(pList->GetCurrentFolder()));
				if (nIndex == m_nCurrentTab)
				{
					m_editPath.SetWindowTextW(pList->GetCurrentFolder());
				}
			}
		}
		return TRUE;
	}

	return CDialogEx::OnCommand(wParam, lParam);
}

void CDFilesDlg::UpdateCurrentPathByClick()
{
	if (m_aTabInfo.GetSize() <= m_nCurrentTab || GetCurrentListCtrl()==NULL) return;
	CString strEdit, strPath, strName;
	GetCurrentListCtrl()->SetRedraw(FALSE);
	GetDlgItemText(IDC_EDIT_PATH, strEdit);
	strPath = GetCurrentListCtrl()->GetCurrentFolder();
	strName = GetPathName(strPath);
	if (strEdit.CompareNoCase(strPath) != 0)
	{
		m_aTabInfo[m_nCurrentTab].strPath = strPath;
		if (!strPath.IsEmpty()) m_editPath.SetWindowText(strPath);
		else if (!strName.IsEmpty()) m_editPath.SetWindowText(strName);
		SetTabTitle(m_nCurrentTab, strName);
	}
	GetCurrentListCtrl()->Sort(m_aTabInfo[m_nCurrentTab].iSortColumn, m_aTabInfo[m_nCurrentTab].bSortAscend);
	GetCurrentListCtrl()->SetRedraw(TRUE);
	UpdateBottomBar();
}

void CDFilesDlg::UpdateCurrentPathByString()
{
	if (m_aTabInfo.GetSize() <= m_nCurrentTab || GetCurrentListCtrl() == NULL) return;
	m_bEnable_UpdateCurrentPathByClick = FALSE;
	GetCurrentListCtrl()->SetRedraw(FALSE);
	CString strPath, strName;
	GetDlgItemText(IDC_EDIT_PATH, strPath);
	GetCurrentListCtrl()->DisplayFolder(strPath);
	strName = GetPathName(strPath);
	SetTabTitle(m_nCurrentTab, strName);
	GetCurrentListCtrl()->Sort(m_aTabInfo[m_nCurrentTab].iSortColumn, m_aTabInfo[m_nCurrentTab].bSortAscend);
	GetCurrentListCtrl()->SetRedraw(TRUE);
	UpdateBottomBar();
	m_bEnable_UpdateCurrentPathByClick = TRUE;
}

void CDFilesDlg::UpdateFontSize()
{
	m_tabPath.SetFont(&m_font);
	m_editPath.SetFont(&m_font);
	for(int i = 0; i < m_aTabInfo.GetSize(); i++)
	{
		CFileListCtrl* pList = (CFileListCtrl*)m_aTabInfo[i].pWnd;
		if (pList && ::IsWindow(pList->GetSafeHwnd())) pList->SetFont(&m_font);
	}
	ResizeDlgItems();
}

void CDFilesDlg::UpdateIconSize()
{
	m_pSysImgList = NULL;
	SHGetImageList(m_iconsize, IID_IImageList, (void**)&m_pSysImgList);
	if (m_pSysImgList)
	{
		for (int i = 0; i < m_aTabInfo.GetSize(); i++)
		{
			CFileListCtrl* pList = (CFileListCtrl*)m_aTabInfo[i].pWnd;
			ListView_SetImageList(pList->GetSafeHwnd(), m_pSysImgList, LVSIL_SMALL);
		}
	}
}

void CDFilesDlg::SetCurrentTab(int iNewTab)
{
	m_bEnable_UpdateCurrentPathByClick = FALSE;
	if (iNewTab >= 0 && iNewTab < m_aTabInfo.GetSize())
	{
		if (GetCurrentListCtrl() != NULL && ::IsWindow(GetCurrentListCtrl()->GetSafeHwnd()))
		{
			GetCurrentListCtrl()->ShowWindow(SW_HIDE);
		}
		m_nCurrentTab = iNewTab; //GetCurrentListCtrl() may return a different value
		if (m_aTabInfo[iNewTab].pWnd == NULL)
		{
			TabCreateList(iNewTab);
			UpdateBottomBar();
		}
		if (m_tabPath.GetCurSel()!=iNewTab) m_tabPath.SetCurSel(iNewTab);
		CString strName;
		if (GetCurrentListCtrl() != NULL && ::IsWindow(GetCurrentListCtrl()->GetSafeHwnd()))
		{
			strName = GetPathName(GetCurrentListCtrl()->GetCurrentFolder());
			if (m_aTabInfo[iNewTab].strPath.IsEmpty())
			{
				m_editPath.SetWindowTextW(strName);
			}
			else
			{
				m_editPath.SetWindowTextW(m_aTabInfo[iNewTab].strPath);
			}
			GetCurrentListCtrl()->ShowWindow(SW_SHOW);
		}
	}
	m_bEnable_UpdateCurrentPathByClick = TRUE;
}



afx_msg LRESULT CDFilesDlg::OnAfxWmChangeCurrentFolder(WPARAM wParam, LPARAM lParam)
{
	if (GetCurrentListCtrl()!=NULL && ::IsWindow(GetCurrentListCtrl()->m_hWnd) && m_bEnable_UpdateCurrentPathByClick)
	{
		UpdateCurrentPathByClick();
	}
	return 0;
}

void CDFilesDlg::UpdateSortColumn(int iSortColumn, BOOL bSortAscend)
{
	m_aTabInfo[m_nCurrentTab].iSortColumn = iSortColumn;
	m_aTabInfo[m_nCurrentTab].bSortAscend = bSortAscend;
}

void CDFilesDlg::UpdateBottomBar()
{
	CString strText;
	strText.Format(L"%d / %d ", GetCurrentListCtrl()->GetSelectedCount(), GetCurrentListCtrl()->GetItemCount());
	SetDlgItemText(IDC_STATIC_BAR, strText);
}


int CDFilesDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1) return -1;

	return 0;
}
