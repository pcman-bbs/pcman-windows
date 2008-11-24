// AutoComplete.cpp : implementation file
//

#include "stdafx.h"
#include "pcman.h"
#include "AutoComplete.h"
#include "StrUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAutoComplete

CAutoComplete::CAutoComplete()
{
}

CAutoComplete::~CAutoComplete()
{
}

BEGIN_MESSAGE_MAP(CAutoComplete, CListCtrl)
	//{{AFX_MSG_MAP(CAutoComplete)
	ON_WM_MOUSEACTIVATE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
	ON_WM_NCACTIVATE()
	ON_WM_ACTIVATEAPP()
	ON_WM_ACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoComplete message handlers

void CAutoComplete::AttachEdit(HWND hedit, CStringList *src, HWND hCombo/*=NULL*/)
{
	CWnd::CreateEx(WS_EX_TOOLWINDOW, WC_LISTVIEW, NULL, WS_CLIPCHILDREN/*|WS_HSCROLL*/ |
				   WS_VSCROLL | LVS_OWNERDRAWFIXED | LVS_OWNERDATA | LVS_SINGLESEL |
				   LVS_NOCOLUMNHEADER | LVS_REPORT | WS_BORDER | WS_POPUP,
				   0, 0, 0, 0, AfxGetMainWnd()->m_hWnd, NULL);
	SetExtendedStyle(LVS_EX_FULLROWSELECT);

	InsertColumn(0, LPCTSTR(NULL));
	::SetWindowLong(hedit, GWL_USERDATA, LONG(this));
	m_OldEditProc = (WNDPROC)::SetWindowLong(hedit, GWL_WNDPROC, LONG(EditProc));
	m_hEdit = hedit;
	m_hCombo = hCombo;
	m_pSrc = src;
}

int CAutoComplete::OnMouseActivate(CWnd* pDetelnetopWnd, UINT nHitTest, UINT message)
{	return MA_NOACTIVATE;	}

void CAutoComplete::DrawItem(LPDRAWITEMSTRUCT lpds)
{
	CDC dc;	dc.Attach(lpds->hDC);
	int i = lpds->itemID;
	if (i != -1)
	{
		if (lpds->itemState& ODS_SELECTED)
		{
			COLORREF bkclr = GetSysColor(COLOR_HIGHLIGHT);
			dc.FillSolidRect(&lpds->rcItem, bkclr);
			dc.SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
			dc.SetBkColor(bkclr);
		}
		else
		{
			COLORREF bkclr = GetSysColor(COLOR_WINDOW);
			dc.FillSolidRect(&lpds->rcItem, bkclr);
			dc.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
			dc.SetBkColor(bkclr);
		}
		dc.DrawText(GetDisplayedItemText(i), &lpds->rcItem, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	}
	else
		dc.FillSolidRect(&lpds->rcItem, GetSysColor(COLOR_WINDOW));

	dc.Detach();
}

void CAutoComplete::OnMouseMove(UINT nFlags, CPoint point)
{
	int i = HitTest(point);
	if (i != -1)
		SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
}

LRESULT CALLBACK CAutoComplete::EditProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	CAutoComplete* pthis = (CAutoComplete*)::GetWindowLong(hwnd, GWL_USERDATA);

	LRESULT r = CallWindowProc(pthis->m_OldEditProc, hwnd, msg, wp, lp);
	switch (msg)
	{
	case WM_KEYDOWN:
		{
			switch (wp)
			{
			case VK_DELETE:
				break;
			case VK_UP:
			case VK_DOWN:
				if (::IsWindowVisible(hwnd))
				{
					::SendMessage(pthis->m_hWnd, msg, wp, lp);
					pthis->UpdateEditText();
				}
				return r;
			case VK_RETURN:
				{
					int sel = (int)pthis->GetFirstSelectedItemPosition() - 1;
					if (sel >= 0)
					{
						CString text = pthis->GetDisplayedItemText(sel);
						::SetWindowText(hwnd, text);
						::SendMessage(hwnd, EM_SETSEL, 0, -1);
					}
				}
			case VK_ESCAPE:
				pthis->Close();
			default:
				return r;
			}
			break;
		}
	case WM_CHAR:
		if (wp < ' ' && wp != VK_BACK)
			return r;
		switch (wp)
		{
		case VK_ESCAPE:
		case VK_RETURN:
			pthis->Close();
			return r;
		}
		break;
	case WM_PASTE:
	case EM_REPLACESEL:
		break;
	case WM_KILLFOCUS:
		pthis->Close();
		return r;
	case WM_DESTROY:
		pthis->DetachEdit();
	default:
		return r;
	}

	pthis->FilterDisplayedItems();
	return r;
}

void CAutoComplete::DetachEdit()
{
	::SetWindowLong(m_hEdit, GWL_WNDPROC, LONG(m_OldEditProc));
	DestroyWindow();
}

HHOOK CAutoComplete::m_MouseHook = NULL;
CAutoComplete* CAutoComplete::m_pOpenedList = NULL;

void CAutoComplete::Open()
{
	if (m_MouseHook)
		return;

	m_pOpenedList = this;
	m_MouseHook = SetWindowsHookEx(WH_MOUSE, MouseProc, AfxGetInstanceHandle(), 0);
	CRect rc;
	::GetWindowRect(m_hEdit, rc);
	rc.top = rc.bottom + 4;	rc.bottom = rc.top + 320;
	MoveWindow(rc);
	SetColumnWidth(0, rc.Width());
	ShowWindow(SW_SHOWNA);
	if (m_hCombo)
		::SendMessage(m_hCombo, CB_SHOWDROPDOWN, FALSE, 0);
}

void CAutoComplete::Close()
{
	ShowWindow(SW_HIDE);
	m_pOpenedList = NULL;
	m_DisplayedItems.RemoveAll();
	SetItemCountEx(0);
	if (m_MouseHook)
	{
		UnhookWindowsHookEx(m_MouseHook);
		m_MouseHook = NULL;
	}
}

LRESULT CALLBACK CAutoComplete::MouseProc(int code, WPARAM w, LPARAM l)
{
	MOUSEHOOKSTRUCT* mp = (MOUSEHOOKSTRUCT*)l;
	if (mp->hwnd != m_pOpenedList->m_hWnd)
	{
		switch (w)
		{
		case WM_MOUSEMOVE:
		case WM_NCHITTEST:
		case WM_NCMOUSEMOVE:
		case WM_MOUSEWHEEL:
			break;
		default:
			m_pOpenedList->Close();
		}
	}
	LRESULT r = CallNextHookEx(m_MouseHook, code, w, l);
	return r;
}

void CAutoComplete::OnLButtonDown(UINT nFlags, CPoint point)
{
	int i = HitTest(point);
	if (i != -1)
	{
		SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
		UpdateEditText();
		::SendMessage(m_hEdit, WM_KEYDOWN, VK_RETURN, 0);
		Close();
	}
}


void CAutoComplete::OnSize(UINT nType, int cx, int cy)
{
	SetColumnWidth(0, cx - GetSystemMetrics(SM_CXVSCROLL));
}

BOOL CAutoComplete::OnNcActivate(BOOL bActive)
{
	if (!bActive)
		Close();
	return TRUE;
}

#if _MFC_VER >= 0x0700	// MFC 7.0 and 4.2 are not compatible
void CAutoComplete::OnActivateApp(BOOL bActive, DWORD hTask)
#else
void CAutoComplete::OnActivateApp(BOOL bActive, HTASK hTask)
#endif
{
	CListCtrl::OnActivateApp(bActive, hTask);
	if (!bActive)
		Close();
}

void CAutoComplete::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	if (WA_INACTIVE == nState)
		Close();
}

void CAutoComplete::UpdateEditText()
{
	int sel = int(GetFirstSelectedItemPosition()) - 1;
	if (sel >= 0)
	{
		CString& text = m_pSrc->GetAt(m_DisplayedItems[sel].strpos);
		::SetWindowText(m_hEdit, text);
		::SendMessage(m_hEdit, EM_SETSEL, text.GetLength(),
					  text.GetLength());
	}
}

int CAutoComplete::CompareItem(const void *p1, const void *p2)
{
	ListItemData *item1 = (ListItemData*)p1;
	ListItemData *item2 = (ListItemData*)p2;
	int r = item1->strstrpos - item2->strstrpos;
	if (0 != r)
		return r;
	return strcmp(m_pOpenedList->m_pSrc->GetAt(item1->strpos),
				  m_pOpenedList->m_pSrc->GetAt(item2->strpos));
}

void CAutoComplete::FilterDisplayedItems()
{
	CStringList& src = *m_pSrc;
	CArray<ListItemData, ListItemData>& cur = m_DisplayedItems;
	CString str;	int len =::GetWindowTextLength(m_hEdit);
	if (len)
	{
		::GetWindowText(m_hEdit, str.GetBuffer(len + 1), len + 1);
		str.ReleaseBuffer();
	}

	cur.RemoveAll();
	DeleteAllItems();
	if (!str.IsEmpty())
	{
		for (POSITION pos = src.GetHeadPosition(); pos ;src.GetNext(pos))
		{
			CString& str2 = src.GetAt(pos);
			char* pstrstrpos = strnstri(str2, str, str2.GetLength());
			if (pstrstrpos)
			{
				cur.Add(ListItemData(pos, int(pstrstrpos - LPCTSTR(str2))));
				InsertItem(0, NULL);
			}
		}
	}

	if (cur.GetSize() > 0)
	{
		m_pOpenedList = this;
		qsort(cur.GetData(), cur.GetSize(), sizeof(ListItemData), CompareItem);
		Open();
	}
	else
		Close();
}
