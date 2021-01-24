#pragma once
#ifndef CTabInfo
class CTabInfo
{
public:
	CTabInfo();
	CTabInfo(CString strPath, int iSortColumn, BOOL bSortAscend);
	~CTabInfo();
	void SetTabInfo(CString strPath, int iSortColumn, BOOL bSortAscend);
	CWnd* pWnd;
	CString strPath;
	int iSortColumn;
	BOOL bSortAscend;
};
#endif