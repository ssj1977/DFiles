// FileListCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DFiles.h"
#include "FileListCtrl.h"
#include "DFilesDlg.h"
#include <shlwapi.h>
#include <atlpath.h>
#include <winnetwk.h>

/////////////////////////////////////////////////
//파일 아이콘 처리
#include <map>
typedef map<CString, int> CExtMap; //확장자에 해당하는 이미지맵의 번호를 기억
static int nFolderImage = -1;
static CExtMap mapExt;

//해당 파일의 아이콘 정보를 가져온다
CString Get_Ext(CString strFile, BOOL bIsDirectory)
{
	CString strReturn;
	int n = strFile.ReverseFind(_T('.'));
	if (n == -1 || bIsDirectory == TRUE) return _T("");
	strReturn = strFile.Right(strFile.GetLength() - n);
	return strReturn;
}
int GetFileImageIndex(CString strPath)
{
	SHFILEINFO sfi;
	memset(&sfi, 0x00, sizeof(sfi));
	SHGetFileInfo((LPCTSTR)strPath, 0 , &sfi, sizeof(sfi), SHGFI_SYSICONINDEX);
	return sfi.iIcon;
}
int GetFileImageIndexFromMap(CString strPath, BOOL bIsDirectory)
{
	if (bIsDirectory)
	{
		if (nFolderImage == -1) nFolderImage = GetFileImageIndex(_T(""));
		return nFolderImage;
	}
	CString strExt = Get_Ext(strPath, bIsDirectory);
	CExtMap::iterator it = mapExt.find(strExt);
	if (it == mapExt.end())
	{
		int nImage = GetFileImageIndex(strPath);
		mapExt.insert(CExtMap::value_type(strExt, nImage));
		return nImage;
	}
	return (*it).second;
}
/////////////////////////////////////////////////


// CFileListCtrl

IMPLEMENT_DYNAMIC(CFileListCtrl, CMFCListCtrl)

CFileListCtrl::CFileListCtrl()
{
	m_pFont = NULL;
}

CFileListCtrl::~CFileListCtrl()
{
}


BEGIN_MESSAGE_MAP(CFileListCtrl, CMFCListCtrl)
	ON_WM_SIZE()
	ON_NOTIFY(HDN_ITEMCLICKA, 0, &CFileListCtrl::OnHdnItemclick)
	ON_NOTIFY(HDN_ITEMCLICKW, 0, &CFileListCtrl::OnHdnItemclick)
	ON_WM_DROPFILES()
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, &CFileListCtrl::OnLvnBegindrag)
END_MESSAGE_MAP()

CString CFileListCtrl::GetCurrentFolder()
{
	return m_strFolder;
}

void CFileListCtrl::OpenSelectedFile()
{
	NMITEMACTIVATE nmia;
	nmia.hdr.code = NM_DBLCLK;
	nmia.hdr.hwndFrom = this->m_hWnd;
	nmia.hdr.idFrom = IDC_LIST_FILE;
	nmia.iItem = GetNextItem(-1, LVNI_SELECTED);
	nmia.iSubItem = 0;
	nmia.lParam = 0;
	nmia.ptAction = CPoint(0,0);
	nmia.uChanged = 0;
	nmia.uKeyFlags = 0;
	nmia.uNewState = 0;
	nmia.uOldState = 0;
	SendMessage(WM_NOTIFY, (WPARAM)IDC_LIST_FILE, (LPARAM)&nmia);
}

void CFileListCtrl::OpenParentFolder()
{
	if (m_strFolder.IsEmpty()) return;
	CPath path = CPath(m_strFolder);
	if (path.IsRoot())
	{
		return;
	}
	CString strParent = m_strFolder;
	int nPos = strParent.ReverseFind(_T('\\'));
	if (nPos = strParent.GetLength() - 1)
	{
		strParent = strParent.Left(nPos);
		nPos = strParent.ReverseFind(_T('\\'));
	}
	if (nPos <= 0) return;
	strParent = strParent.Left(nPos);
	DisplayFolder(strParent);
}

void CFileListCtrl::ResizeColumns()
{
	CRect rcThis;
	GetClientRect(rcThis);

	/*int nColWidthSum = 0;
	for (int i = 0; i < GetHeaderCtrl().GetItemCount(); i++)
	{
		nColWidthSum += GetColumnWidth(i);
	}
	if (nColWidthSum < rcThis.Width())
	{
		int nW = GetColumnWidth(0);
		SetColumnWidth(0, nW + rcThis.Width() - nColWidthSum);
	}*/
}

#define COL_NAME 0
#define COL_DATE 1
#define COL_SIZE 2

void CFileListCtrl::InitColumns()
{
	int nCount = GetHeaderCtrl().GetItemCount();
	for (int i = nCount -1 ; i >= 0; i--)
		DeleteColumn(i);
	InsertColumn(COL_NAME, _T("Name"), LVCFMT_LEFT, 300);
	InsertColumn(COL_DATE, _T("Date"), LVCFMT_RIGHT, 200);
	InsertColumn(COL_SIZE, _T("Size"), LVCFMT_RIGHT, 150);
}

#define ITEM_TYPE_DOTS 0
#define ITEM_TYPE_DIRECTORY 1
#define ITEM_TYPE_FILE 2

CString EnumerateFunc(LPNETRESOURCE lpnr);
CString DisplayStruct(int i, LPNETRESOURCE lpnrLocal);


void CFileListCtrl::DisplayFolder(CString strFolder)
{
	if (strFolder.IsEmpty()) return;
	CPath path = CPath(strFolder);
	m_strFolder = (CString)path;
	path.AddBackslash();
	DeleteAllItems();
	InitColumns();
/*	if (path.IsUNC())
	{
		NETRESOURCE nr;
		//AfxMessageBox(EnumerateFunc(lpnr));
		//return;
		nr.dwDisplayType = RESOURCEDISPLAYTYPE_SHARE;
		nr.dwScope = RESOURCE_GLOBALNET;
		nr.dwType = RESOURCETYPE_DISK;
		nr.dwUsage = RESOURCEUSAGE_CONTAINER;
		nr.lpComment = NULL;
		nr.lpLocalName = NULL;
		nr.lpProvider = NULL;
		nr.lpRemoteName = (LPTSTR)(LPCTSTR)path;
		DWORD dwResult, dwResultEnum;
		HANDLE hEnum;
		DWORD cbBuffer = 16384;     // 16K is a good size
		DWORD cEntries = -1;        // enumerate all possible entries
		LPNETRESOURCE lpnrLocal;    // pointer to enumerated structures
		DWORD i;
		dwResult = WNetOpenEnum(RESOURCE_GLOBALNET, RESOURCETYPE_DISK, RESOURCEUSAGE_CONTAINER, &nr, &hEnum);    // handle to the resource
		if (dwResult != NO_ERROR) return;
		lpnrLocal = (LPNETRESOURCE)GlobalAlloc(GPTR, cbBuffer);
		if (lpnrLocal == NULL) return;
		do 
		{
			ZeroMemory(lpnrLocal, cbBuffer);
			dwResultEnum = WNetEnumResource(hEnum, &cEntries, lpnrLocal, &cbBuffer);
			if (dwResultEnum == NO_ERROR) 
			{
				for (i = 0; i < cEntries; i++) 
				{
					CString strName = (LPCTSTR)lpnrLocal[i].lpRemoteName;
					int nItem = InsertItem(GetItemCount(), strName, GetFileImageIndexFromMap(_T(""), TRUE));
					SetItemData(nItem, ITEM_TYPE_DIRECTORY);
				}
			}
		}
		while (dwResultEnum != ERROR_NO_MORE_ITEMS);
		GlobalFree((HGLOBAL)lpnrLocal);
		dwResult = WNetCloseEnum(hEnum);
		return;
	}*/
	CFileFind find;
	BOOL b = find.FindFile(path + _T("*.*"));
	CString strName, strSize, strDate, strType;
	DWORD itemData = 0;
	int nItem;
	while (b)
	{
		b = find.FindNextFileW();
		strName = find.GetFileName();
		CTime tTemp;
		find.GetLastWriteTime(tTemp);
		strDate = tTemp.Format(_T("%Y-%m-%d %H:%M:%S"));
		if (find.IsDots())
		{
			itemData = ITEM_TYPE_DOTS; //Dots
			strSize.Empty();
		}
		else if (find.IsDirectory())
		{ 
			itemData = ITEM_TYPE_DIRECTORY;  //Directory
			strSize.Empty();
		}
		else
		{
			itemData = ITEM_TYPE_FILE;  //File
			strSize.Format(_T("%I64u"), find.GetLength());
		}
		if (itemData != ITEM_TYPE_DOTS)
		{
			nItem = InsertItem(GetItemCount(), strName, GetFileImageIndexFromMap(find.GetFilePath(), find.IsDirectory()));
			SetItemData(nItem, itemData);
			SetItemText(nItem, COL_DATE, strDate);
			if (!strSize.IsEmpty()) SetItemText(nItem, COL_SIZE, strSize);
		}
	}
	GetParent()->SendMessage(WM_COMMAND, IDM_UPDATE_TABCTRL, (DWORD_PTR)this);
	Sort(GetHeaderCtrl().GetSortColumn(), GetHeaderCtrl().IsAscending());
}

void CFileListCtrl::OnSize(UINT nType, int cx, int cy)
{
	CMFCListCtrl::OnSize(nType, cx, cy);
	ResizeColumns();
}

void CFileListCtrl::OnHdnItemclick(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	Default();
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDFilesDlg* pParent = (CDFilesDlg*)GetParent();
	pParent->UpdateSortColumn(GetHeaderCtrl().GetSortColumn(), GetHeaderCtrl().IsAscending());

	*pResult = 0;
}


BOOL CFileListCtrl::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_LBUTTONDBLCLK)
	{
		if (GetNextItem(-1, LVNI_SELECTED) == -1)
		{
			OpenParentFolder();
			return TRUE;
		}
	}
	return CMFCListCtrl::PreTranslateMessage(pMsg);
}


void CFileListCtrl::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CMFCListCtrl::OnDropFiles(hDropInfo);
}


void CFileListCtrl::OnLvnBegindrag(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	NM_LISTVIEW* pNMListView = pNMLV;

	CRect rectSrc;
	int   nItem = pNMListView->iItem;
	// Get the bounding rectangle of the entire item.
	GetItemRect(nItem, rectSrc, LVIR_BOUNDS);

	CRectTracker tr(&rectSrc, CRectTracker::dottedLine);
	if (tr.Track(this, pNMListView->ptAction)) {
		// Get the mouse position and convert it to the target list
		// coordinates.
		CPoint pt;
		GetCursorPos(&pt);
		CWnd* pWnd = WindowFromPoint(pt);

		if (pWnd != this)
		{
			AfxMessageBox(GetItemText(nItem, 0));
		}
	}
	*pResult = 0;
}


int CFileListCtrl::OnCompareItems(LPARAM lParam1, LPARAM lParam2, int iColumn)
{
//	if (iColumn == 1)//assuming SetItemData was called earlier
//		return lParam1 - lParam2;
	int nRet = 0;
	DWORD type1, type2;
	ULONGLONG size1, size2;
	CString str1, str2;

	switch (iColumn)
	{
	case COL_NAME: //by Name
		type1 = GetItemData(lParam1);
		type2 = GetItemData(lParam2);
		if (type1 != type2)
		{
			nRet = type1 - type2;
			break;
		}
	case COL_DATE: //by Date
		str1 = GetItemText(lParam1, iColumn);
		str2 = GetItemText(lParam2, iColumn);
		nRet = StrCmpLogicalW(str1.GetBuffer(), str2.GetBuffer());
		str1.ReleaseBuffer();
		str2.ReleaseBuffer();
		break;
	case COL_SIZE: //by Size
		size1 = _wcstoui64(GetItemText(lParam1, iColumn), NULL, 10);
		size2 = _wcstoui64(GetItemText(lParam2, iColumn), NULL, 10);
		if (size1 == size2) nRet = 0;
		else if (size1 > size2) nRet = -1;
		else if (size1 < size2) nRet = 1;
		break;
	}
	return nRet;
}

void CFileListCtrl::Sort(int iColumn, BOOL bAscending, BOOL bAdd)
{
	/*if (iColumn == 1) // Using Item Data : File size
	{
		CMFCListCtrl::Sort(iColumn, bAscending, bAdd);
		return;
	}*/
	CWaitCursor wait;
	GetHeaderCtrl().SetSortColumn(iColumn, bAscending, bAdd);
	m_iSortedColumn = iColumn;
	m_bAscending = bAscending;
	SortItemsEx(CompareProc, (LPARAM)this);
}

CString EnumerateFunc(LPNETRESOURCE lpnr)
{
	DWORD dwResult, dwResultEnum;
	HANDLE hEnum;
	DWORD cbBuffer = 16384;     // 16K is a good size
	DWORD cEntries = -1;        // enumerate all possible entries
	LPNETRESOURCE lpnrLocal;    // pointer to enumerated structures
	DWORD i;
	CString strTemp;
	//
	// Call the WNetOpenEnum function to begin the enumeration.
	//
	dwResult = WNetOpenEnum(RESOURCE_GLOBALNET, // all network resources
		RESOURCETYPE_ANY,   // all resources
		0,  // enumerate all resources
		lpnr,       // NULL first time the function is called
		&hEnum);    // handle to the resource

	if (dwResult != NO_ERROR) {
		printf("WnetOpenEnum failed with error %d\n", dwResult);
		return strTemp;
	}
	//
	// Call the GlobalAlloc function to allocate resources.
	//
	lpnrLocal = (LPNETRESOURCE)GlobalAlloc(GPTR, cbBuffer);
	if (lpnrLocal == NULL) {
		printf("WnetOpenEnum failed with error %d\n", dwResult);
		//      NetErrorHandler(hwnd, dwResult, (LPSTR)"WNetOpenEnum");
		return strTemp;
	}
	do {
		//
		// Initialize the buffer.
		//
		ZeroMemory(lpnrLocal, cbBuffer);
		//
		// Call the WNetEnumResource function to continue
		//  the enumeration.
		//
		dwResultEnum = WNetEnumResource(hEnum,  // resource handle
			&cEntries,      // defined locally as -1
			lpnrLocal,      // LPNETRESOURCE
			&cbBuffer);     // buffer size
//
// If the call succeeds, loop through the structures.
//
		if (dwResultEnum == NO_ERROR) {
			for (i = 0; i < cEntries; i++) {
				// Call an application-defined function to
				//  display the contents of the NETRESOURCE structures.
				//
				strTemp += DisplayStruct(i, &lpnrLocal[i]);

				// If the NETRESOURCE structure represents a container resource, 
				//  call the EnumerateFunc function recursively.

				if (RESOURCEUSAGE_CONTAINER == (lpnrLocal[i].dwUsage
					& RESOURCEUSAGE_CONTAINER))
					//          if(!EnumerateFunc(hwnd, hdc, &lpnrLocal[i]))
					if (!EnumerateFunc(&lpnrLocal[i]))
						printf("EnumerateFunc returned FALSE\n");
				//            TextOut(hdc, 10, 10, "EnumerateFunc returned FALSE.", 29);
			}
		}
		// Process errors.
		//
		else if (dwResultEnum != ERROR_NO_MORE_ITEMS) {
			printf("WNetEnumResource failed with error %d\n", dwResultEnum);

			//      NetErrorHandler(hwnd, dwResultEnum, (LPSTR)"WNetEnumResource");
			break;
		}
	}
	//
	// End do.
	//
	while (dwResultEnum != ERROR_NO_MORE_ITEMS);
	//
	// Call the GlobalFree function to free the memory.
	//
	GlobalFree((HGLOBAL)lpnrLocal);
	//
	// Call WNetCloseEnum to end the enumeration.
	//
	dwResult = WNetCloseEnum(hEnum);

	if (dwResult != NO_ERROR) {
		//
		// Process errors.
		//
		printf("WNetCloseEnum failed with error %d\n", dwResult);
		//    NetErrorHandler(hwnd, dwResult, (LPSTR)"WNetCloseEnum");
		return strTemp;
	}

	return strTemp;
}

CString DisplayStruct(int i, LPNETRESOURCE lpnrLocal)
{
	CString str, strTemp; 
	strTemp.Format(_T("NETRESOURCE[%d] Scope: "), i);
	str += strTemp;
	switch (lpnrLocal->dwScope) {
	case (RESOURCE_CONNECTED):
		str+=_T("connected\n");
		break;
	case (RESOURCE_GLOBALNET):
		str += _T("all resources\n");
		break;
	case (RESOURCE_REMEMBERED):
		str += _T("remembered\n");
		break;
	default:
		strTemp.Format(_T("unknown scope %d\n"), lpnrLocal->dwScope);
		str += strTemp;
		break;
	}

	strTemp.Format(_T("NETRESOURCE[%d] Type: "), i);
	str += strTemp;
	switch (lpnrLocal->dwType) {
	case (RESOURCETYPE_ANY):
		str += _T("any\n");
		break;
	case (RESOURCETYPE_DISK):
		str += _T("disk\n");
		break;
	case (RESOURCETYPE_PRINT):
		str += _T("print\n");
		break;
	default:
		strTemp.Format(_T("unknown type %d\n"), lpnrLocal->dwType);
		str += strTemp;
		break;
	}

	printf("NETRESOURCE[%d] DisplayType: ", i);
	switch (lpnrLocal->dwDisplayType) {
	case (RESOURCEDISPLAYTYPE_GENERIC):
		str += _T("generic\n");
		break;
	case (RESOURCEDISPLAYTYPE_DOMAIN):
		str += _T("domain\n");
		break;
	case (RESOURCEDISPLAYTYPE_SERVER):
		str += _T("server\n");
		break;
	case (RESOURCEDISPLAYTYPE_SHARE):
		str += _T("share\n");
		break;
	case (RESOURCEDISPLAYTYPE_FILE):
		str += _T("file\n");
		break;
	case (RESOURCEDISPLAYTYPE_GROUP):
		str += _T("group\n");
		break;
	case (RESOURCEDISPLAYTYPE_NETWORK):
		str += _T("network\n");
		break;
	default:
		strTemp.Format(_T("unknown display type %d\n"), lpnrLocal->dwDisplayType);
		str += strTemp;
		break;
	}

	strTemp.Format(_T("NETRESOURCE[%d] Usage: 0x%x = "), i, lpnrLocal->dwUsage);
	str += strTemp;
	if (lpnrLocal->dwUsage & RESOURCEUSAGE_CONNECTABLE)
		str += _T("connectable ");
	if (lpnrLocal->dwUsage & RESOURCEUSAGE_CONTAINER)
		str += _T("container ");
	str += _T("\n");

	strTemp.Format(_T("NETRESOURCE[%d] Localname: %S\n"), i, lpnrLocal->lpLocalName);
	str += strTemp;
	strTemp.Format(_T("NETRESOURCE[%d] Remotename: %S\n"), i, lpnrLocal->lpRemoteName);
	str += strTemp;
	strTemp.Format(_T("NETRESOURCE[%d] Comment: %S\n"), i, lpnrLocal->lpComment);
	str += strTemp;
	strTemp.Format(_T("NETRESOURCE[%d] Provider: %S\n"), i, lpnrLocal->lpProvider);
	str += strTemp;
	str += _T("\n");
	return str;
}
