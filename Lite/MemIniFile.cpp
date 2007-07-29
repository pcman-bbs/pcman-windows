// MemIniFile.cpp: implementation of the CMemIniFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcman.h"
#include "MemIniFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void SaveString(CFile& file, LPCTSTR str)
{
	DWORD len = strlen(str) + 1;
	file.Write(&len, sizeof(DWORD));
	file.Write(str, len);
}


CMemIniFile::CMemIniFile()
{
	data = pdata = eof = NULL;
	hfile = hmap = INVALID_HANDLE_VALUE;
}

CMemIniFile::~CMemIniFile()
{
	Close();
}

UINT CMemIniFile::Read(void *lpBuf, UINT nCount)
{
	if (pdata + nCount > eof)
		nCount = eof - pdata;
	memcpy(lpBuf, pdata, nCount);
	pdata += nCount;
	return nCount;
}

BOOL CMemIniFile::Open(LPCTSTR lpszFileName, UINT nOpenFlags)
{
	UINT flags = 0;
	if (nOpenFlags == CFile::modeRead)
		flags = (GENERIC_READ | GENERIC_WRITE);
	if (nOpenFlags & CFile::modeWrite)
		flags |= GENERIC_WRITE;
	if (nOpenFlags & CFile::modeCreate)
		flags |= CREATE_ALWAYS;

	hfile = CreateFile(lpszFileName, flags | GENERIC_WRITE, 0,
					   NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hfile != INVALID_HANDLE_VALUE)
	{
		if (nOpenFlags == CFile::modeRead)
		{
			// FIXME: Using file Mapping here is not well-tested and may potentially be buggy.
			//        Besides, can small files be benefit from using file mapping?
			DWORD hi, l = GetFileSize(hfile, &hi);
			hmap = CreateFileMapping(hfile, NULL, PAGE_READWRITE , hi, l, NULL);
			if (hmap)
			{
				pdata = data = (BYTE*)MapViewOfFile(hmap, PAGE_READONLY, 0, 0, 0);
				eof = data + l;
			}
		}
		return TRUE;
	}
	else
	{
		data = NULL;
		DWORD err = GetLastError();
	}
	return FALSE;
}

void CMemIniFile::Close()
{
	if (hfile != INVALID_HANDLE_VALUE)
	{
		if (hmap)
		{
			if (data)
			{
				UnmapViewOfFile(data);
				data = pdata = eof = NULL;
			}
			CloseHandle(hmap);
			hmap = NULL;
		}
		CloseHandle(hfile);
		hfile = INVALID_HANDLE_VALUE;
	}
}

UINT CMemIniFile::Read4(void *lpBuf)
{
	if (pdata + 4 > eof)
		return 0;
	*(DWORD*)lpBuf = *(DWORD*)pdata;
	pdata += 4;
	return 4;
}
