#ifndef _DCOMMON_

int GetLine(CString& strText, int nPos, CString& strLine, CString strToken);
BOOL WriteCStringToFile(CString strFile, CString& strContent);
BOOL ReadFileToCString(CString strFile, CString& strData);
void GetToken(CString& strLine, CString& str1, CString& str2, TCHAR cSplit, BOOL bReverseFind);
CString INTtoSTR(int n);
CString GetRealPath(CString strPath, CString strExt=_T(""));
CString CutName(CString strFile);

#endif 

