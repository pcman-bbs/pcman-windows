// MemIniFile.h: interface for the CMemIniFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMINIFILE_H__C82E3AA1_4916_11D7_992C_0040F427D59A__INCLUDED_)
#define AFX_MEMINIFILE_H__C82E3AA1_4916_11D7_992C_0040F427D59A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afx.h>

class CMemIniFile
{
public:
	UINT Read4(void *lpBuf);
	void Close();
	BOOL Open(LPCTSTR lpszFileName, UINT nOpenFlags);
	UINT Read(void* lpBuf, UINT nCount);
	UINT Write(void* lpBuf, UINT nCount)
	{
		DWORD written = 0;
		if (WriteFile(hfile, lpBuf, nCount, &written, NULL))
			return written;
		return -1;
	}
	CMemIniFile();
	virtual ~CMemIniFile();

protected:
	BYTE* eof;
	BYTE* pdata;
	BYTE* data;
	HANDLE hfile, hmap;
};

#endif // !defined(AFX_MEMINIFILE_H__C82E3AA1_4916_11D7_992C_0040F427D59A__INCLUDED_)
