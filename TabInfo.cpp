#include "stdafx.h"
#include "TabInfo.h"



CTabInfo::CTabInfo()
{
	pWnd = NULL;
}

CTabInfo::CTabInfo(CString strPath, int iSortColumn, BOOL bSortAscend)
{
	SetTabInfo(strPath, iSortColumn, bSortAscend);
	pWnd = NULL;
}

void CTabInfo::SetTabInfo(CString strPath, int iSortColumn, BOOL bSortAscend)
{
	this->strPath = strPath;
	this->iSortColumn = iSortColumn;
	this->bSortAscend = bSortAscend;
};

CTabInfo::~CTabInfo()
{
}
