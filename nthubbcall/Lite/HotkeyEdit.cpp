// HotkeyEdit.cpp : implementation file
//

#include "stdafx.h"
#include "pcman.h"
#include "HotkeyEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHotkeyEdit

CHotkeyEdit::CHotkeyEdit()
{
	fVirt = 0;
	key = 0;
	accepted = FALT | FSHIFT | FCONTROL;
}

CHotkeyEdit::~CHotkeyEdit()
{
}


BEGIN_MESSAGE_MAP(CHotkeyEdit, CEdit)
	//{{AFX_MSG_MAP(CHotkeyEdit)
	ON_WM_KEYDOWN()
	ON_WM_CONTEXTMENU()
	ON_WM_CHAR()
	ON_WM_KEYUP()
	ON_WM_SYSKEYDOWN()
	ON_WM_SYSKEYUP()
	ON_WM_GETDLGCODE()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHotkeyEdit message handlers

void CHotkeyEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (key)
	{
		fVirt = 0;
		key = 0;
	}

	if (accepted & FNUMPAD)	//如果要特別區分Extended和numpad
	{
		switch (nChar)
		{
		case VK_PRIOR:
		case VK_NEXT:
		case VK_END:
		case VK_HOME:
		case VK_LEFT:
		case VK_UP:
		case VK_RIGHT:
		case VK_DOWN:
		case VK_INSERT:
		case VK_DELETE:
			if (!(nFlags & (1 << 8)))	//如果不是extended，就是Num pad
			{
				fVirt |= FNUMPAD;
				break;
			}
		}
	}

	switch (nChar)
	{
	case VK_CONTROL:
		if (accepted & FCONTROL)
			fVirt |= FCONTROL;
		break;
	case VK_MENU:
		if (accepted & FALT)
			fVirt |= FALT;
		break;
	case VK_SHIFT:
		if (accepted & FSHIFT)
			fVirt |= FSHIFT;
		break;
	case VK_BACK:
		if (!fVirt && !(accepted&FBACK))
			break;
	default:
		key = nChar;
		::PostMessage(::GetParent(m_hWnd), WM_HOTKEYDONE, 0, 0);
	}
	SetWindowText(HotkeyToStr(fVirt, key));
}

void CHotkeyEdit::OnContextMenu(CWnd* pWnd, CPoint point)
{
}

void CHotkeyEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}

void CHotkeyEdit::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (!key)
	{
		switch (nChar)
		{
		case VK_CONTROL:
			fVirt &= ~FCONTROL;
			break;
		case VK_MENU:
			fVirt &= ~FALT;
			break;
		case VK_SHIFT:
			fVirt &= ~FSHIFT;
			break;
		default:
			if (!fVirt)
				key = 0;
		}
	}
	SetWindowText(HotkeyToStr(fVirt, key));
}

void CHotkeyEdit::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	OnKeyDown(nChar, nRepCnt, nFlags);
}

void CHotkeyEdit::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	OnKeyUp(nChar, nRepCnt, nFlags);
}

UINT CHotkeyEdit::OnGetDlgCode()
{
	return DLGC_WANTALLKEYS;
}

void CHotkeyEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);
	if (!key)
	{
		fVirt = 0;
		SetWindowText(::HotkeyToStr(0, 0));
	}
}
