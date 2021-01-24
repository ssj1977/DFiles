#pragma once

#include "TabInfo.h"
// CFileListCtrl

#define IDC_LIST_FILE 50000
#define IDM_UPDATE_TABCTRL 50001

class CFileListCtrl : public CMFCListCtrl
{
	DECLARE_DYNAMIC(CFileListCtrl)

public:
	CFileListCtrl();
	virtual ~CFileListCtrl();
	void OpenSelectedFile();
	void OpenParentFolder();
	void ResizeColumns();
	void DisplayFolder(CString strFolder);
	void InitColumns();
	CString GetCurrentFolder();
	CFont* m_pFont;
	CString m_strFolder;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHdnItemclick(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual int OnCompareItems(LPARAM lParam1, LPARAM lParam2, int iColumn);
	virtual void Sort(int iColumn, BOOL bAscending = TRUE, BOOL bAdd = FALSE);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnLvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
};


