// CustomTabCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "CustomTabCtrl.h"
#include "CustomTabCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#undef	IDC_SPIN
#define	IDC_SPIN		1

/////////////////////////////////////////////////////////////////////////////
// CCustomTabCtrl

CCustomTabCtrl::CCustomTabCtrl()
{
	cur_item = -1;	// current active item
	hot_item = -1;	// current hovered item
	scroll_pos = 0;	// scroll position
	min_size = -1;	// minimal size of tab
	max_size = -1;	// maximal size of tab
	tab_height = 0;	// height of all tabs
	homogeneous = false;	// whether all tabs use the same size or not
	can_reorder = false;	// whether the order of tabs can be changed by the user or not
	show_number = false;	// show numbers on the tabs
	close_btn = CTC_CLOSE_BTN_NONE;	// whether to show close button or not
	image_list = NULL;	// image list providing icons for items
	spin = NULL;	// spin button used to scroll the tab bar
	tooltips = NULL;
}

CCustomTabCtrl::~CCustomTabCtrl()
{
}

CCustomTabCtrl::ItemData::ItemData(CString txt, int img, LPVOID dat)
		: text(txt), image(img), data(dat), desired_size(0), size(0), hilight(false)
{

}



BEGIN_MESSAGE_MAP(CCustomTabCtrl, CWnd)
	//{{AFX_MSG_MAP(CCustomTabCtrl)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN, OnScrolled)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCustomTabCtrl message handlers

BOOL CCustomTabCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;
//	cs.dwExStyle |= WS_EX_DLGMODALFRAME;
	cs.style |= WS_CLIPCHILDREN;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
									   ::LoadCursor(NULL, IDC_ARROW), HBRUSH(COLOR_BTNFACE + 1), NULL);
	return TRUE;
}

BOOL CCustomTabCtrl::Create(DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT nID)
{
	return CWnd::Create(NULL, NULL, dwStyle, rect, pParentWnd, nID);
}

int CCustomTabCtrl::InsertItem(int idx, LPCTSTR text, int image, void *data)
{
	ItemData* item = new ItemData(text, image, data);

	if (IsLegalIndex(idx))
		items.InsertAt(idx, item);
	else
		items.Add(item);

	CClientDC dc(this);
	CSize size = dc.GetTextExtent(item->text, item->text.GetLength());

	// FIXME: vertical tabs??
	item->desired_size = size.cx + 8;	// FIXME: padding?

	if (image_list && item->image >= 0)
	{
		int imgx, imgy;
		if (ImageList_GetIconSize(image_list->GetSafeHandle(), &imgx, &imgy))
		{
			item->desired_size += (imgx + 4);	// padding = 2?
		}
	}

	RecalcLayout();
	return -1;
}

BOOL CCustomTabCtrl::DeleteItem(int idx)
{
	if (IsLegalIndex(idx))
	{
		ItemData* item = (ItemData*)items[idx];
		items.RemoveAt(idx);
		delete item;
		// last item was removed
		if (idx == cur_item && cur_item == GetItemCount())
			--cur_item;	// set cur_item to previous item (the new last item)
		Invalidate();	//FIXME: don't update the whole client area
	}
	return TRUE;
}

BOOL CCustomTabCtrl::DeleteAllItems()
{
	int size = items.GetSize();

	if (size == 0)
		return TRUE;

	for (int i = 0; i < size; ++i)
		delete(ItemData*)items[i];

	items.RemoveAll();

	if (GetSafeHwnd())
		Invalidate();

	return TRUE;
}

int CCustomTabCtrl::HitTest(int x, int y, int *place)
{
	CRect item_rc;
	GetClientRect(item_rc);
//	item_rc.DeflateRect( 2, 2 );
	item_rc.top += 6;	// FIXME: shouldn't be hard-coded
	item_rc.bottom -= 2;

	// FIXME: consider vertical tabs?
	if (y > item_rc.bottom || y < item_rc.top)
		return -1;

	int c = GetItemCount();
	for (int i = scroll_pos; i < c; ++i)
	{
		ItemData* item = (ItemData*)items[i];
		item_rc.right = item_rc.left + item->size;

		if (x > item_rc.left && x < item_rc.right)
		{
			if (place)	// FIXME
			{

			}
			return i;
		}

		item_rc.left = item_rc.right;
	}
	return -1;
}

BOOL CCustomTabCtrl::HighlightItem(int idx, BOOL high)
{
	if (IsLegalIndex(idx))
	{
		((ItemData*)items[idx])->hilight = high;
		return TRUE;
	}
	return FALSE;
}

BOOL CCustomTabCtrl::GetItemRect(int idx, LPRECT rc) const
{
	if (! IsLegalIndex(idx))
		return FALSE;

	int left = 0;
	ItemData* item;
	CRect item_rc;
	GetClientRect(item_rc);
	item_rc.DeflateRect(2, 2);	// FIXME: padding = 2 ??

	if (idx >= scroll_pos)
	{
		item = (ItemData*)items[0];
		for (int i = scroll_pos; i < idx; ++i)
		{
			item = (ItemData*)items[i];
			left += item->size;
		}
		item_rc.left = left;
		item_rc.right = left + item->size;
	}
	else	// idx < scroll_pos
	{
		item = (ItemData*)items[0];
		for (int i = scroll_pos; i >= idx; --i)
		{
			item = (ItemData*)items[i];
			left -= item->size;
		}
		item_rc.left = left;
		item_rc.right = left + item->size;
	}

	*rc = item_rc;
	return TRUE;
}

int CCustomTabCtrl::SetCurSel(int idx)
{
	// According to MSDN:
	// A tab control does not send a TCN_SELCHANGING or TCN_SELCHANGE notification message when a tab is
	// selected using the TCM_SETCURSEL message.

	if (idx == cur_item)
		return cur_item;

	int old_idx = cur_item;

	if (IsLegalIndex(idx))
	{
		cur_item = idx;

		RedrawItem(idx);
		RedrawItem(old_idx);
	}
	return old_idx;
}

CImageList* CCustomTabCtrl::SetImageList(CImageList *pImageList)
{
	CImageList* old = image_list;
	if (image_list != pImageList)
	{
		image_list = pImageList;
		RecalcLayout();	// Invalidate() is called inside RecalcLayout()
	}
	return image_list;
}

LPVOID CCustomTabCtrl::SetItemData(int idx, LPVOID data)
{
	LPVOID old_data = NULL;
	ItemData* item = GetItem(idx);
	if (item)
	{
		old_data = item->data;
		item->data = data;
	}
	return old_data;
}

LPVOID CCustomTabCtrl::GetItemData(int idx) const
{
	if (IsLegalIndex(idx))
	{
		return ((ItemData*)items[idx])->data;
	}
	return NULL;
}

void CCustomTabCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	int c = items.GetSize();
	if (c == 0)
		return;

	dc.SetBkMode(TRANSPARENT);

	CRect item_rc;
	GetClientRect(item_rc);
	item_rc.top += 6;
	item_rc.bottom -= 2;

	for (int i = scroll_pos; i < c; ++i)
	{
		ItemData* item = (ItemData*)items[i];
		item_rc.right = item_rc.left + item->size;

		if (i == cur_item)
		{
			item_rc.top -= 2;
//			dc.SetTextColor( GetSysColor( COLOR_HIGHLIGHT ) );
//			dc.FillSolidRect( item_rc, GetSysColor( COLOR_WINDOW ) );
			dc.DrawEdge(item_rc, EDGE_RAISED, BF_TOP | BF_LEFT | BF_RIGHT);
		}
		else
		{
			dc.SetTextColor(GetSysColor(COLOR_BTNTEXT));
			COLORREF clr = GetSysColor(COLOR_BTNFACE);
			clr = RGB(GetRValue(clr) * 0.9, GetGValue(clr) * 0.9, GetBValue(clr) * 0.9);
			dc.FillSolidRect(item_rc, clr);
			dc.Draw3dRect(item_rc, GetSysColor(COLOR_BTNHILIGHT), GetSysColor(COLOR_BTNSHADOW));
		}

		int tmp_left = item_rc.left;
		item_rc.left += 4;

		if (image_list && item->image >= 0)
		{
			int imgx, imgy;
			if (ImageList_GetIconSize(image_list->GetSafeHandle(), &imgx, &imgy))
			{
				image_list->DrawIndirect(&dc, item->image,
										 CPoint(item_rc.left, item_rc.top + (item_rc.Height() - imgy) / 2),
										 CSize(imgx, imgy),
										 CPoint(0, 0),
										 ILD_TRANSPARENT);
				item_rc.left += (imgx + 2);
			}
		}

		dc.DrawText(item->text, item->text.GetLength(), item_rc,
					DT_SINGLELINE | DT_VCENTER | DT_WORD_ELLIPSIS | DT_END_ELLIPSIS);
//		item_rc.left -= 4;
		item_rc.left = tmp_left;

		if (i == cur_item)
			item_rc.top += 2;

		item_rc.left = item_rc.right;
	}
}


void CCustomTabCtrl::PostNcDestroy()
{
	DeleteAllItems();
	if (spin)
	{
		delete spin;
		spin = NULL;
	}
	if (tooltips)
	{
		delete tooltips;
		tooltips = NULL;
	}
	CWnd::PostNcDestroy();
}

void CCustomTabCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	RecalcLayout();
}

void CCustomTabCtrl::RecalcLayout()
{
	int c = items.GetSize();
	if (c == 0)
		return;

	CRect rc;
	GetClientRect(rc);
	int avail_space = rc.Width();	// FIXME: should use rc.Height() for vertical tabs

	int desired_size = 0;
	for (int i = 0; i < c; ++i)
	{
		ItemData* item = (ItemData*)items[i];
		desired_size += item->desired_size;
	}

	// if space is not enough and min_size is set, try to compress tabs
	if (desired_size > avail_space && min_size > 0)
	{
		float ratio = ((float)avail_space) / desired_size;
		int compressed_size = 0;
		// try to compress
		for (int i = 0; i < c; ++i)
		{
			ItemData* item = (ItemData*)items[i];
			item->size = (int)(item->desired_size * ratio);
			if (item->size < min_size)
				item->size = min_size;
			compressed_size += item->size;
		}

		// compressed tabs fit in one page
		if (compressed_size < avail_space)
		{
			scroll_pos = 0;	// no scroll needed
			if (spin)
			{
				delete spin;
				spin = NULL;
			}
		}
		else
		{
			EnsureSpinButton();	// create the spin button used to scroll the tab bar
		}
	}
	else // all tabs use their original desired size
	{
		for (int i = 0; i < c; ++i)
		{
			ItemData* item = (ItemData*)items[i];
			item->size = item->desired_size;
		}

		if (desired_size < avail_space)  // all tabs fit in one page
		{
			scroll_pos = 0;	// no scroll is needed
			if (spin)
			{
				delete spin;
				spin = NULL;
			}
		}
		else
		{
			EnsureSpinButton();	// create the spin button used to scroll the tab bar
		}
	}

	Invalidate();

	if (tooltips)	// update tooltips
		UpdateTooltips();
}

void CCustomTabCtrl::SetMinTabSize(int min)
{
	if (min_size == min)
		return;

	min_size = min;
	RecalcLayout();
}

void CCustomTabCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	int place = 0;
	int idx = HitTest(point.x, point.y, &place);
	if (idx >= 0)
	{
		if (! ParentNotify(TCN_SELCHANGING))
		{
			SetCurSel(idx);
			ParentNotify(TCN_SELCHANGE);
		}
	}
	else
	{
		// scroll bar?
	}
	CWnd::OnLButtonDown(nFlags, point);
}

void CCustomTabCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{

	CWnd::OnLButtonUp(nFlags, point);
}

void CCustomTabCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	int old = hot_item;
	hot_item = HitTest(point.x, point.y);
	if (hot_item != old)
	{
		if (hot_item >= 0)
			ParentNotify(TTN_NEEDTEXT, hot_item);
	}
	CWnd::OnMouseMove(nFlags, point);
}

BOOL CCustomTabCtrl::RedrawItem(int idx)
{
	CRect rc;
	if (GetItemRect(idx, rc))
	{
		InvalidateRect(rc);
		return TRUE;
	}
	return FALSE;
}

void CCustomTabCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// FIXME: Only for test
	int idx = HitTest(point.x, point.y, NULL);
	DeleteItem(idx);

	CWnd::OnLButtonDblClk(nFlags, point);
}

LRESULT CCustomTabCtrl::ParentNotify(int code, int id /*= -1*/)
{
	NMHDR nmhdr;
	nmhdr.code = code;
	nmhdr.hwndFrom = m_hWnd;
	UINT ctrl_id = ::GetWindowLong(m_hWnd, GWL_ID);
	if (id == -1)
		nmhdr.idFrom = ctrl_id;
	return ::SendMessage(::GetParent(m_hWnd), WM_NOTIFY, ctrl_id, (LPARAM)&nmhdr);
}

void CCustomTabCtrl::EnsureSpinButton()
{
	// FIXME: consider vertical tab bar?
	CRect rc;
	GetClientRect(rc);
	rc.top += 4;
	rc.bottom -= 4;
	rc.left = rc.right - rc.Height() * 2;

	if (! spin)
	{
		spin = new CSpinButtonCtrl;
		spin->Create(UDS_HORZ | WS_VISIBLE | WS_CHILD, rc, this, IDC_SPIN);
	}
	else
		spin->MoveWindow(rc);
	spin->SetRange(0, GetItemCount() - 1);	// FIXME: Too many scrollable items
}

void CCustomTabCtrl::OnScrolled(NMHDR* nmhdr, LRESULT* result)
{
	NMUPDOWN* ud = (NMUPDOWN*)nmhdr;
	int new_pos = ud->iPos + ud->iDelta;
	if (new_pos >= GetItemCount())
	{
		new_pos = GetItemCount() - 1;
		return;
	}
	else if (new_pos < 0)
	{
		new_pos = 0;
		return;
	}
	if (new_pos != scroll_pos)
	{
		scroll_pos = new_pos;
		Invalidate();
		UpdateTooltips();
	}
}

void CCustomTabCtrl::EnsureTooltips()
{
	if (!tooltips)
	{
		tooltips = new CToolTipCtrl;
		tooltips->Create(this);
		tooltips->Activate(TRUE);
	}
}

void CCustomTabCtrl::SetItemTooltip(int idx, LPCTSTR text)
{
	ItemData* item = GetItem(idx);
	if (item)
	{
		if (text)
		{
			EnsureTooltips();
			item->tooltip = text;
			tooltips->AddTool(this, item->tooltip, CRect(0, 0, 0, 0), idx + 1);
		}
		else
		{
			item->tooltip.Empty();
			if (tooltips)
				tooltips->DelTool(this, idx + 1);
		}
	}
}

CString CCustomTabCtrl::GetItemTooltip(int idx)
{
	ItemData* item = GetItem(idx);
	if (item)
		return item->tooltip;
	return AfxGetEmptyString();
}

BOOL CCustomTabCtrl::PreTranslateMessage(MSG* pMsg)
{
	if (tooltips)
	{
		switch (pMsg->message)
		{
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
			tooltips->RelayEvent(pMsg);
		}
	}
	return CWnd::PreTranslateMessage(pMsg);
}

void CCustomTabCtrl::UpdateTooltips()
{
	// FIXME: this is extremely inefficient!!!!
	int c = GetItemCount();
	for (int i = 0 ; i < c; ++i)
	{
		ItemData* item = (ItemData*)items[i];
		tooltips->UpdateTipText(item->tooltip, this, i + 1);
		CRect rc;
		GetItemRect(i, rc);
		tooltips->SetToolRect(this, i + 1, rc);
	}
}
