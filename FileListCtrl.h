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
	void OpenSelectedItem();
	void OpenParentFolder();
	void ResizeColumns();
	void DisplayFolder(CString strFolder);
	void InitColumns(int nType);
	void ShowContextMenu(CPoint pt);
	CString GetItemFullPath(int nIndex);
	CString GetCurrentFolder();
	CString m_strFolder;
	int m_nType;
	int CompareItemByType(LPARAM item1, LPARAM item2, int nCol, int nType);

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
	afx_msg void OnNMDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMRClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMReturn(NMHDR* pNMHDR, LRESULT* pResult);
};


