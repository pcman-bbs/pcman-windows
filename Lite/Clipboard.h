// Clipboard.h: interface for the CClipboard class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIPBOARD_H__13CC6589_8FB7_446D_AA42_2BD1AAF0883E__INCLUDED_)
#define AFX_CLIPBOARD_H__13CC6589_8FB7_446D_AA42_2BD1AAF0883E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CClipboard  
{
public:
	static BOOL SetText(HWND owner, LPCTSTR str,int len);
	static BOOL SetText(HWND owner, LPCTSTR str);
	static BOOL GetText(CString& str);
};

inline BOOL CClipboard::SetText(HWND owner, LPCTSTR str)
{
	return SetText(owner,str,strlen(str));
}

#endif // !defined(AFX_CLIPBOARD_H__13CC6589_8FB7_446D_AA42_2BD1AAF0883E__INCLUDED_)
