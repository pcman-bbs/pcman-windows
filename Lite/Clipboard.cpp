// Clipboard.cpp: implementation of the CClipboard class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcman.h"
#include "Clipboard.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL CClipboard::GetText(CString &str)
{
	if (!OpenClipboard(NULL))
		return FALSE;

	HANDLE hlocmem = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, sizeof(LCID));
	if (hlocmem)
	{
		PLCID lcid = (PLCID)GlobalLock(hlocmem);
		*lcid = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL), SORT_CHINESE_BIG5);
		GlobalUnlock(hlocmem);
		SetClipboardData(CF_LOCALE, hlocmem);
	}

	HANDLE hmem = GetClipboardData(CF_TEXT);
	if (hmem)
	{
		LPSTR data = (LPSTR)GlobalLock(hmem);
		str = data;
		GlobalUnlock(hmem);
	}
	CloseClipboard();
	return TRUE;
}

// Unicode for other applications; exact station bytes for a lossless PCMan
// round trip (including old-style SGRs embedded between Big5 lead/trail).
static UINT StationFormat()
{
	static const UINT format = RegisterClipboardFormat(_T("PCMan.StationText.v1"));
	return format;
}

BOOL CClipboard::SetStationText(HWND owner, const CStringW& unicode, const CString& bytes, UINT cp)
{
	if (unicode.IsEmpty() || bytes.IsEmpty()) return FALSE;
	const SIZE_T wideSize = (static_cast<SIZE_T>(unicode.GetLength()) + 1) * sizeof(wchar_t);
	const UINT count = bytes.GetLength();
	HGLOBAL wide = GlobalAlloc(GMEM_MOVEABLE, wideSize);
	HGLOBAL raw = GlobalAlloc(GMEM_MOVEABLE, sizeof(UINT) * 2 + count);
	void* w = wide ? GlobalLock(wide) : NULL;
	UINT* r = raw ? static_cast<UINT*>(GlobalLock(raw)) : NULL;
	if (w) { memcpy(w, unicode.GetString(), wideSize); GlobalUnlock(wide); }
	if (r) { r[0] = cp; r[1] = count; memcpy(r + 2, bytes.GetString(), count); GlobalUnlock(raw); }
	BOOL ok = FALSE;
	if (w && OpenClipboard(owner))
	{
		if (EmptyClipboard() && SetClipboardData(CF_UNICODETEXT, wide))
		{
			wide = NULL; ok = TRUE;
			const UINT format = StationFormat();
			if (r && format && SetClipboardData(format, raw)) raw = NULL;
		}
		CloseClipboard();
	}
	if (raw) GlobalFree(raw);
	if (wide) GlobalFree(wide);
	return ok;
}

BOOL CClipboard::GetStationText(CString& bytes, UINT cp)
{
	const UINT format = StationFormat();
	if (!format || !IsClipboardFormatAvailable(format) || !OpenClipboard(NULL)) return FALSE;
	HGLOBAL h = GetClipboardData(format);
	const SIZE_T size = h ? GlobalSize(h) : 0;
	const UINT* r = size >= 2 * sizeof(UINT) ? static_cast<const UINT*>(GlobalLock(h)) : NULL;
	BOOL ok = FALSE;
	if (r)
	{
		if (r[0] == cp && r[1] && r[1] < INT_MAX && r[1] <= size - 2 * sizeof(UINT))
		{
			bytes.SetString(reinterpret_cast<const char*>(r + 2), static_cast<int>(r[1]));
			ok = TRUE;
		}
		GlobalUnlock(h);
	}
	CloseClipboard();
	return ok;
}

BOOL CClipboard::GetTextW(wchar_t** ppwstr)
{
	if (!OpenClipboard(NULL))
		return FALSE;

	HANDLE hmem = GetClipboardData(CF_UNICODETEXT);
	if (hmem)
	{
		wchar_t* data = (wchar_t*)GlobalLock(hmem);
		*ppwstr = new wchar_t[ wcslen(data) +1];
		wcscpy(*ppwstr, data);
		GlobalUnlock(hmem);
		CloseClipboard();
		return TRUE;
	}
	else
	{
		CloseClipboard();
		return FALSE;
	}
}

BOOL CClipboard::SetText(HWND owner, LPCTSTR str, int len)
{
	if (str && *str && OpenClipboard(owner))
	{
		EmptyClipboard();
		HANDLE hmem = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, len + 1);
		HANDLE hlocmem = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, sizeof(LCID));
		if (hmem)
		{
			LPSTR buf = (LPSTR)GlobalLock(hmem);
			PLCID lcid = (PLCID)GlobalLock(hlocmem);
			memcpy(buf, str, len + 1);
			// FIXME: Big5 should not be hard-coded since we have GB2312 version.
			*lcid = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL), SORT_CHINESE_BIG5);
			GlobalUnlock(hmem);
			GlobalUnlock(hlocmem);
			SetClipboardData(CF_TEXT, hmem);
			SetClipboardData(CF_LOCALE, hlocmem);
		}
		CloseClipboard();
		return TRUE;
	}
	return FALSE;
}

BOOL CClipboard::SetTextW(HWND owner, const wchar_t* str, int len)
{
	if (str && *str && OpenClipboard(owner))
	{
		EmptyClipboard();
		HANDLE hmem = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, len + 4);
		if (hmem)
		{
			wchar_t* buf = (wchar_t*)GlobalLock(hmem);
			memcpy(buf, str, len);
			buf[len / sizeof(wchar_t)] = L'\0';
			GlobalUnlock(hmem);
			SetClipboardData(CF_UNICODETEXT, hmem);
		}
		CloseClipboard();
		return TRUE;
	}
	return FALSE;
}
