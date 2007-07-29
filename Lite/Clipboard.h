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
	static BOOL SetText(HWND owner, LPCTSTR str, int len);
	static BOOL SetText(HWND owner, LPCTSTR str);
	static BOOL SetTextW(HWND owner, const wchar_t* str, int len);
	static BOOL SetTextW(HWND owner, const wchar_t* str);
	static BOOL GetText(CString& str);
	static BOOL GetTextW(wchar_t** ppwstr);
};

inline BOOL CClipboard::SetText(HWND owner, LPCTSTR str)
{
	return SetText(owner, str, strlen(str));
}

inline BOOL CClipboard::SetTextW(HWND owner, const wchar_t* str)
{
	return SetTextW(owner, str, wcslen(str)*sizeof(wchar_t)); //in bytes
}
#endif // !defined(AFX_CLIPBOARD_H__13CC6589_8FB7_446D_AA42_2BD1AAF0883E__INCLUDED_)
