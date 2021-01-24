#include "stdafx.h"
#include "DCommon.h"

CString CutName(CString strFile)
{
	CString strName;
	strName=strFile.Right(strFile.GetLength()-strFile.ReverseFind(_T('\\'))-1);
	strName=strName.Left(strName.ReverseFind(_T('.')));
	return strName;
}

int GetLine(CString& strText, int nPos, CString& strLine, CString strToken)
{
	if (strText.IsEmpty()) 
	{
		strLine.Empty();
		return -1;
	}
	int nPosNext = -1;
	
	if (strToken.GetLength()==1)
	{
		TCHAR c=strToken.GetAt(0);
		nPosNext=strText.Find(c,nPos);
	}
	else
	{
		nPosNext=strText.Find(strToken,nPos);
	}

	if (nPosNext!=-1)
	{
		strLine = strText.Mid(nPos,nPosNext-nPos);
		nPosNext+=strToken.GetLength();
	}
	else
	{
		nPosNext= strText.GetLength();
		strLine = strText.Mid(nPos,nPosNext-nPos);
		return -1;
	}
	return nPosNext;
}

BOOL WriteCStringToFile(CString strFile, CString& strContent)
{
	try
	{
		CFile file;
		if (file.Open(strFile, CFile::modeCreate|CFile::modeWrite)==FALSE) return FALSE;
#ifdef _UNICODE
		BYTE UnicodeIdentifier[] = {0xff,0xfe};
		file.Write(UnicodeIdentifier, 2);
#endif 
		if (strContent.IsEmpty()==FALSE)
		{
			file.Write(strContent.GetBuffer(0), strContent.GetLength()*sizeof(TCHAR));
			strContent.ReleaseBuffer();
		}
		file.Flush();
		file.Close();
	}
	catch(CFileException* e)
	{
		e->Delete(); 
		return FALSE;
	}
	return TRUE;
}

BOOL ReadFileToCString(CString strFile, CString& strData)
{
	//Unicode 식별해서 읽기
	try
	{
		CFile file;
		if (file.Open(strFile, CFile::modeRead)==FALSE) return FALSE;
		size_t filesize = file.GetLength();
		if (filesize > 2)
		{
			BYTE uidf[2];
			file.Read(uidf, 2);
			if (uidf[0]==0xff && uidf[1]==0xfe)	
			{
				filesize-=2;
#ifdef _UNICODE
				int nStrLen = ( filesize / sizeof(TCHAR) ) + 1;
				TCHAR* pBuf=strData.GetBufferSetLength(nStrLen);
				memset(pBuf, 0, filesize + sizeof(TCHAR));
				file.Read(pBuf, filesize);
				strData.ReleaseBuffer();
				file.Close();
#else
				int nStrLen = ( filesize / sizeof(WCHAR) ) + 1;
				WCHAR* pBuf=new WCHAR[nStrLen];
				memset(pBuf, 0, filesize + sizeof(WCHAR));
				file.Read(pBuf, filesize);
				file.Close();
				strData=pBuf;
				delete[] pBuf;
#endif 
			}
			else								
			{
				file.SeekToBegin();
				char* pBuf=new char[filesize + 1];
				memset(pBuf, 0, filesize + 1);
				file.Read(pBuf, filesize);
				strData = pBuf;	
				file.Close();
				delete[] pBuf;
			}
		}
	}
	catch(CFileException* e)
	{
		e->Delete(); 
		return FALSE;
	}
	return TRUE;
}

void GetToken(CString& strLine, CString& str1, CString& str2, TCHAR cSplit, BOOL bReverseFind)
{
	int n;
	if (bReverseFind==FALSE)	n=strLine.Find(cSplit);
	else						n=strLine.ReverseFind(cSplit);
	if (n==-1)	
	{
		str1=strLine;
		str2.Empty();
	}
	else		
	{
		str1 = strLine.Left(n);
		if ( (strLine.GetLength()-n-1) < 1 ) str2.Empty();
		else str2 = strLine.Right(strLine.GetLength()-n-1);
	}
	str1.TrimLeft(); str1.TrimRight();
	str2.TrimLeft(); str2.TrimRight();
}

CString INTtoSTR(int n)
{
	CString str; str.Format(_T("%d"), n);
	return str;
}

CString GetRealPath(CString strPath, CString strExt)
{
	CFileFind find; //In case of no path execution
	CString strReturn;
	strPath.Remove(_T('\"'));	strPath.TrimLeft();	strPath.TrimRight();
	if (strExt.IsEmpty()==FALSE) //Make d:\test\SMIEdit -> d:\test\SMIEdit.exe 
	{
		if (strPath.GetLength()<strExt.GetLength()) strPath+=_T('.')+strExt;
		else
		{
			CString strTemp=strPath.Right(strExt.GetLength());
			strTemp.MakeLower(); strExt.MakeLower();
			if (strTemp!=strExt) strPath+=_T('.')+strExt;
		}
	}
	if (strPath.IsEmpty()==FALSE)	
	{
		if (find.FindFile(strPath))
		{
			find.FindNextFile();
			strReturn=find.GetFilePath();
		}
	}
	return strReturn;
}

void MakeStringArray(CString& strLine, CStringArray& strData, TCHAR nSplit)
{
	int nLen=strLine.GetLength();
	int n1=-1;	
	int n2=-1;
	CString strToken;
	strData.RemoveAll();
	strLine.TrimLeft();
	strLine.TrimRight();
	while(TRUE)
	{
		n2=strLine.Find(nSplit,n1+1);
		if (n2==-1) n2=nLen;
		strToken = strLine.Mid(n1+1, n2-n1-1);
		n1=n2;
		strToken.TrimLeft();
		strToken.TrimRight();
		if (strToken.IsEmpty()==FALSE) strData.Add(strToken);
		if (n2==nLen) break;
	}
}