#include "stdafx.h"
#include "CtrlEdit.h"
#include "Clipboard.h"
#include "Ucs2Conv.h"
#include "FrequentText.h"

extern CUcs2Conv g_ucs2conv;

bool CCtrlEdit::Install(HWND parent, UINT id)
{
	// MFC's ANSI subclass proc would thunk WM_GETTEXT/EM_REPLACESEL via ACP.
	// Attach the handle only; intercept paste with the native Unicode subclass.
	HWND old = ::GetDlgItem(parent, id);
	if (!old) return false;
	RECT r;
	::GetWindowRect(old, &r);
	::MapWindowPoints(NULL, parent, reinterpret_cast<POINT*>(&r), 2);
	HWND h = ::CreateWindowExW(::GetWindowLong(old, GWL_EXSTYLE), L"EDIT", L"",
		::GetWindowLong(old, GWL_STYLE), r.left, r.top, r.right-r.left, r.bottom-r.top,
		parent, reinterpret_cast<HMENU>(static_cast<UINT_PTR>(id)), AfxGetInstanceHandle(), NULL);
	if (!h) return false;
	if (!SetWindowSubclass(h, EditProc, 1, reinterpret_cast<DWORD_PTR>(this)))
	{ ::DestroyWindow(h); return false; }
	::SendMessageW(h, WM_SETFONT, ::SendMessage(old, WM_GETFONT, 0, 0), FALSE);
	::SetWindowPos(h, old, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	::DestroyWindow(old);
	Attach(h);
	return true;
}

LRESULT CALLBACK CCtrlEdit::EditProc(HWND h, UINT msg, WPARAM w, LPARAM l, UINT_PTR id, DWORD_PTR data)
{
	CCtrlEdit* self = reinterpret_cast<CCtrlEdit*>(data);
	if (msg == WM_PASTE || (msg == WM_CHAR && w == 22) ||
		(msg == WM_KEYDOWN && w == VK_INSERT && (::GetKeyState(VK_SHIFT) < 0)))
	{ self->Paste(); return 0; }
	if (msg == WM_NCDESTROY)
	{
		RemoveWindowSubclass(h, EditProc, id);
		if (self->GetSafeHwnd() == h) self->Detach();
	}
	return DefSubclassProc(h, msg, w, l);
}

CStringW CCtrlEdit::Display(const CString& bytes, bool ansi) const
{
	return FrequentText::Display(bytes, ansi, codePage, g_ucs2conv);
}

void CCtrlEdit::ShowBytes(const CString& bytes, bool ansi)
{
	original = bytes;
	shown = Display(bytes, ansi);
	::SetWindowTextW(m_hWnd, shown);
}

bool CCtrlEdit::ReadBytes(CString& bytes) const
{
	CStringW value;
	const int size = ::GetWindowTextLengthW(m_hWnd) + 1;
	const int n = ::GetWindowTextW(m_hWnd, value.GetBuffer(size), size);
	value.ReleaseBuffer(n);
	if (value == shown) { bytes = original; return true; }
	return FrequentText::Encode(value, codePage, g_ucs2conv, bytes);
}

void CCtrlEdit::Paste()
{
	if (GetStyle() & ES_READONLY) return;
	CString raw;
	CStringW text;
	const bool station = CClipboard::GetStationText(raw, codePage) != FALSE;
	bool ansi = false;
	if (station)
	{
		ansi = raw.Find("\x1b[") >= 0;
		if (ansi) raw.Replace("\x1b[", "^[[");
		text = Display(raw, ansi);
	}
	else
	{
		wchar_t* wide = NULL;
		if (CClipboard::GetTextW(&wide)) { text = wide; delete [] wide; }
		else if (CClipboard::GetText(raw)) text = FrequentText::Decode(raw, codePage, g_ucs2conv);
		ansi = text.Replace(L"\x1b[", L"^[[") != 0;
	}
	if (text.IsEmpty()) return;
	DWORD begin = 0, end = 0;
	::SendMessageW(m_hWnd, EM_GETSEL, reinterpret_cast<WPARAM>(&begin), reinterpret_cast<LPARAM>(&end));
	const bool replacesAll = begin == 0 && end == static_cast<DWORD>(::GetWindowTextLengthW(m_hWnd));
	::SendMessageW(m_hWnd, EM_REPLACESEL, TRUE, reinterpret_cast<LPARAM>(text.GetString()));
	if (station && replacesAll) { original = raw; shown = text; }
	if (ansi) ::CheckDlgButton(::GetParent(m_hWnd), IDC_ANSI, BST_CHECKED);
}
