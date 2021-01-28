// FileListCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DFiles.h"
#include "FileListCtrl.h"
#include "DFilesDlg.h"
#include <shlwapi.h>
#include <atlpath.h>
#include <lm.h>
//#include <fileapi.h>

#pragma comment(lib, "Netapi32.lib")

/////////////////////////////////////////////////
//파일 아이콘 처리
#include <map>
typedef map<CString, int> CExtMap; //확장자에 해당하는 이미지맵의 번호를 기억
static int nFolderImage = -1;
static CExtMap mapExt;

//기본 이미지맵 번호
#define SI_UNKNOWN 0 //Unknown File Type
#define SI_DEF_DOCUMENT	1 //Default document
#define SI_DEF_APPLICATION 2//Default application
#define SI_FOLDER_CLOSED 3	//Closed folder
#define SI_FOLDER_OPEN 4	//Open folder
#define SI_FLOPPY_514 5	//5 1/4 floppy
#define SI_FLOPPY_35 6	//3 1/2 floppy
#define SI_REMOVABLE 7	//Removable drive
#define SI_HDD 8	//Hard disk drive
#define SI_NETWORKDRIVE 9	//Network drive
#define SI_NETWORKDRIVE_DISCONNECTED 10 //network drive offline
#define SI_CDROM 11	//CD drive
#define SI_RAMDISK 12	//RAM disk
#define SI_NETWORK 13	//Entire network
#define SI_MOUSEGLOBE //14		?
#define SI_MYCOMPUTER 15	//My Computer
#define SI_PRINTMANAGER 16	//Printer Manager
#define SI_NETWORK_NEIGHBORHOOD	17//Network Neighborhood
#define SI_NETWORK_WORKGROUP 18	//Network Workgroup
#define SI_STARTMENU_PROGRAMS 19	//Start Menu Programs
#define SI_STARTMENU_DOCUMENTS 20	//Start Menu Documents
#define SI_STARTMENU_SETTINGS 21	//Start Menu Settings
#define SI_STARTMENU_FIND 22	//Start Menu Find
#define SI_STARTMENU_HELP 23	//Start Menu Help
#define SI_STARTMENU_RUN 24	//Start Menu Run
#define SI_STARTMENU_SUSPEND 25	//Start Menu Suspend
#define SI_STARTMENU_DOCKING 26	//Start Menu Docking
#define SI_STARTMENU_SHUTDOWN 27	//Start Menu Shutdown
#define SI_SHARE 28	//Sharing overlay (hand)
#define SI_SHORTCUT 29	//Shortcut overlay (small arrow)
#define SI_PRINTER_DEFAULT 30	//Default printer overlay (small tick)
#define SI_RECYCLEBIN_EMPTY 31	//Recycle bin empty
#define SI_RECYCLEBIN_FULL 32	//Recycle bin full
#define SI_DUN 33	//Dial-up Network Folder
#define SI_DESKTOP 34	//Desktop
#define SI_CONTROLPANEL 35	//Control Panel
#define SI_PROGRAMGROUPS 36	//Program Group
#define SI_PRINTER 37	//Printer
#define SI_FONT 38	//Font Folder
#define SI_TASKBAR 39	//Taskbar
#define SI_AUDIO_CD 40	//Audio CD
#define SI_TREE 41		//?
#define SI_PCFOLDER 42	//?
#define SI_FAVORITES 43	//IE favorites
#define SI_LOGOFF 44	//Start Menu Logoff
#define SI_FOLDERUPLOAD 45		?
#define SI_SCREENREFRESH 46		?
#define SI_LOCK 47	//Lock
#define SI_HIBERNATE 48	//Hibernate


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
	m_strFolder = L"";
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

void CFileListCtrl::DisplayVolumes()
{
	DWORD drives = GetLogicalDrives();
	DeleteAllItems();
	InitColumns();
	DWORD flag = 1;
	int nItem = 0;
	TCHAR c = L'A';
	CString strTemp;
	for (int i = 0; i < 32; i++)
	{
		if (drives & flag)
		{
			strTemp = (TCHAR)(c + i);
			nItem = InsertItem(GetItemCount(), strTemp, SI_HDD ); //GetFileImageIndexFromMap(L"", TRUE)
		}
		flag = flag * 2;
	}

}


void CFileListCtrl::DisplayFolder(CString strFolder)
{
	if (strFolder.IsEmpty())
	{
		DisplayVolumes();
		return;
	}
	CPath path = CPath(strFolder);
	m_strFolder = (CString)path;
	DeleteAllItems();
	InitColumns();
	if (path.IsUNCServer())
	{
		PSHARE_INFO_0 pBuffer, pTemp;
		NET_API_STATUS res;
		DWORD er = 0, tr = 0, resume = 0, i;
		LPTSTR lpszServer = strFolder.GetBuffer();
		do // begin do
		{
			res = NetShareEnum(lpszServer, 0, (LPBYTE*)&pBuffer, MAX_PREFERRED_LENGTH, &er, &tr, &resume);
			if (res == ERROR_SUCCESS || res == ERROR_MORE_DATA)
			{
				pTemp = pBuffer;
				for (i = 1; i <= er; i++)
				{
					CString strTemp = pTemp->shi0_netname;
					InsertItem(GetItemCount(), strTemp, SI_NETWORKDRIVE);
					pTemp++;
				}
				NetApiBufferFree(pBuffer);
			}
			else
			{
				AfxMessageBox(L"Err");
			}
		}
		while (res == ERROR_MORE_DATA);
		strFolder.ReleaseBuffer();
		return;
	}
	path.AddBackslash();
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
