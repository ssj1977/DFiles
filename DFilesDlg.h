
// DFilesDlg.h : ��� ����
//

#pragma once
#include "afxcmn.h"
#include "afxshelllistctrl.h"
#include "afxeditbrowsectrl.h"
#include "FileListCtrl.h"
#include "TabInfo.h"

using namespace std;

// CDFilesDlg ��ȭ ����
class CDFilesDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CDFilesDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.
	CRect m_rcMain;
	CString m_strINIPath;
	int m_nSelectedCount;
	void UpdateSortColumn(int iSortColumn, BOOL bSortAscend);
	CArray<CTabInfo> m_aTabInfo;
	int m_nCurrentTab;
	BOOL m_bEnable_UpdateCurrentPathByClick;
	CToolBar m_tool;
	void UpdateBottomBar();
	int fontsize;
	int iconsize;
	CFont m_font;
// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DFILES_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:

	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);

//	CFileListCtrl m_listFile;
	CTabCtrl m_tabPath;
	CMFCEditBrowseCtrl m_editPath;

	afx_msg void OnBnClickedBtnPath();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTcnSelchangeTabPath(NMHDR *pNMHDR, LRESULT *pResult);
	void InitTabs();
	void ResizeDlgItems();
	void INISave(CString strFile);
	void INILoad(CString strFile);
	void InitINIPath();
	void InitToolBar();
	void UpdateCurrentPathByClick();
	void UpdateCurrentPathByString();
	void UpdateFontSize();
	void UpdateIconSize();
	void SetCurrentTab(int nIndex);
	BOOL TabCreateList(int iTabIndex);
	int TabAddNew(CString strPath);
	void TabClose(int nIndex);
	CFileListCtrl* GetCurrentListCtrl();
	int FindTabIndex(CFileListCtrl* pList);
	void SetTabTitle(int nIndex, CString strTitle);
	void InitFont();

protected:
	afx_msg LRESULT OnAfxWmChangeCurrentFolder(WPARAM wParam, LPARAM lParam);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
