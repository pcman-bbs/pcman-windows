// Ucs2Conv.h: interface for the CUcs2Conv class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UCS2CONV_H__6D4B7931_4162_4FE1_8060_F5B2ADE116EB__INCLUDED_)
#define AFX_UCS2CONV_H__6D4B7931_4162_4FE1_8060_F5B2ADE116EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afx.h>

#define B2U_TAB_SIZE 39564 +2
#define U2B_TAB_SIZE 130814+2

class CUcs2Conv
{
public:
	void Big52Ucs2(const char* text_in, wchar_t* text_out);
	size_t Big52Ucs2(const char* text_in, wchar_t* text_out, int len);
	void Ucs22Big5(const wchar_t* text_in, char* text_out);
	CUcs2Conv();
	virtual ~CUcs2Conv();
	void SetTablePath(CString tab_dir);

protected:
	CString m_TabDirPath;
	unsigned char *m_Big52Ucs2_Tab;
	unsigned char *m_Ucs22Big5_Tab;
	size_t m_Ucs2TabSize;

public:   /* FIXME */
	BOOL InitBig52Ucs2Tab();
	BOOL InitUcs22Big5Tab();
};

#endif // !defined(AFX_CHICONV_H__6D4B7931_4162_4FE1_8060_F5B2ADE116EB__INCLUDED_)