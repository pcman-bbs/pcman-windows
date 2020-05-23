// CustomTabCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "pcman.h"
#include "CustomTabCtrl.h"
#include "MainFrm.h"
#include "TelnetConn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCustomTabCtrl

CCustomTabCtrl::CCustomTabCtrl()
{
	dragimg = NULL;
}

CCustomTabCtrl::~CCustomTabCtrl()
{
//	DestroyCursor(drag_cursor);
}


BEGIN_MESSAGE_MAP(CCustomTabCtrl, CTabCtrl)
	//{{AFX_MSG_MAP(CCustomTabCtrl)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_MBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomTabCtrl message handlers

void CCustomTabCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CTabCtrl::OnLButtonDblClk(nFlags, point);
	if (!AppConfig.dblclk_close)
		return;

	TCHITTESTINFO hti;
	hti.pt = point;
	sel = HitTest(&hti);
	if (sel != -1)
		parent->CloseConn(sel, true);
}

void CCustomTabCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	TCHITTESTINFO hti;
	hti.pt = point;
	if ((sel = HitTest(&hti)) != -1)
	{
		md = 1;	//Left Button Down
		oldp = point;
	}
	CTabCtrl::OnLButtonDown(nFlags, point);
}

void CCustomTabCtrl::OnMButtonDown(UINT nFlags, CPoint point)
{
	TCHITTESTINFO hti;
	hti.pt = point;
	sel = HitTest(&hti);
	if (sel != -1)
		parent->CloseConn(sel, true);
}

void CCustomTabCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	TCHITTESTINFO hti;
	hti.pt = point;
	int newsel = HitTest(&hti);
	if (md == 2)
	{
		if (newsel != -1 && sel != -1 && newsel != sel)
		{
			TCITEM item;
			item.mask = TCIF_PARAM | TCIF_IMAGE;
			GetItem(sel, &item);
			CTelnetConn* oldc = (CTelnetConn*)item.lParam;

			TCITEM tmp;
			tmp.mask = TCIF_PARAM;
			//���R���A�s�W
			int c = GetItemCount();
			DeleteItem(sel);
			/*			if(oldc->pprev)
							oldc->pprev->pnext=oldc->pnext;
						if(oldc->pnext)
							oldc->pnext->pprev=oldc->pprev;
			*/
			CString txt = oldc->name;
			item.mask |= TCIF_TEXT;
			item.pszText = (LPSTR)(LPCTSTR)txt;
			InsertItem(newsel, &item);

			/*			if(newsel>0)	//�p�G���J�ᤣ�O�Ĥ@��,���e�@��
						{
							GetItem(newsel-1,&tmp);
							oldc->pprev=(CTelnetConn*)tmp.lParam;
							oldc->pprev->pnext=oldc;
						}
						else
							oldc->pprev=NULL;

						if( newsel<c-1 )	//�p�G���J�ᤣ�O�̥���,����@��
						{
							GetItem(newsel+1,&tmp);
							oldc->pnext=(CTelnetConn*)tmp.lParam;
							oldc->pnext->pprev=oldc;
						}
						else
							oldc->pnext=NULL;
			*/
		}
		else
			CTabCtrl::OnLButtonUp(nFlags, point);
	}
	else
		CTabCtrl::OnLButtonUp(nFlags, point);

	md = 0;
	ReleaseCapture();
	dragimg->EndDrag();
	if (dragimg)
	{
		dragimg->DeleteImageList();
		delete dragimg;
	}
	dragimg = NULL;
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}

void CCustomTabCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if (nFlags&MK_LBUTTON && md == 1)
	{
		if (abs(point.x - oldp.x) > 4)
		{
			if (::GetCapture() != m_hWnd)
				::SetCapture(m_hWnd);
//			SetCursor(CTermView::hand_cursor);
			md = 2;
			BeginDrag(point);
		}
	}

	if (dragimg)
	{
		ClientToScreen(&point);
		dragimg->DragShowNolock(TRUE);
		dragimg->DragMove(point);
	}
	CTabCtrl::OnMouseMove(nFlags, point);
}

int CCustomTabCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTabCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	scaler.Update(this);
	return 0;
}

void CCustomTabCtrl::OnSize(UINT nType, int cx, int cy)
{
	CTabCtrl::OnSize(nType, cx, cy);
	// This is to set the height of the tab bar.
	// Combo uses fixed size tab, so we specify width here.
	SetItemSize(CSize(scaler.CalcX(128), cy));
}

//DEL void CCustomTabCtrl::ReCalcSize(int dc)
//DEL {
//DEL /*	int c=GetItemCount()+dc;
//DEL 	CRect rc;
//DEL 	GetClientRect(rc);
//DEL 	int cx=rc.Width()-20;
//DEL 	if(c>0)
//DEL 	{
//DEL 		cx/=c;
//DEL 		if(cx>120)
//DEL 			cx=120;
//DEL 		if(cx<80)
//DEL 			cx=80;
//DEL 		SetItemSize(CSize(cx-10,rc.Height()));
//DEL 	}
//DEL */
//DEL }

BOOL CCustomTabCtrl::DeleteItem(int nItem)
{
	BOOL r = CTabCtrl::DeleteItem(nItem);
	if (AppConfig.tab_add_number)
		UpdateNumberFrom(nItem);

	// FIXME: Dirty hacks used to force-resize the tabs and make the scroll buttons visible.
	CRect rc;
	GetWindowRect(rc);
	--rc.right;
	::SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, rc.Width(), rc.Height(), SWP_NOMOVE | SWP_NOZORDER);
	++rc.right;
	::SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, rc.Width(), rc.Height(), SWP_NOMOVE | SWP_NOZORDER);

	return r;
}

BOOL CCustomTabCtrl::InsertItem(int nItem, TCITEM* pTabCtrlItem)
{
	BOOL r;
	if (!AppConfig.tab_add_number)
		return CTabCtrl::InsertItem(nItem, pTabCtrlItem);

	CString name;
	name.Format("%d.%s", nItem + 1, pTabCtrlItem->pszText);
	pTabCtrlItem->pszText = (LPSTR)(LPCTSTR)name;
	r = CTabCtrl::InsertItem(nItem, pTabCtrlItem);
	UpdateNumberFrom(nItem);

	// FIXME: Dirty hacks used to force-resize the tabs and make the scroll buttons visible.
	CRect rc;
	GetWindowRect(rc);
	--rc.right;
	::SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, rc.Width(), rc.Height(), SWP_NOMOVE | SWP_NOZORDER);
	++rc.right;
	::SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, rc.Width(), rc.Height(), SWP_NOMOVE | SWP_NOZORDER);
	return r;
}

BOOL CCustomTabCtrl::InsertItem(UINT nMask, int nItem, LPCTSTR lpszItem, int nImage, LPARAM lParam)
{
	TCITEM item;
	item.mask = nMask;
	item.pszText = (LPSTR)lpszItem;
	item.iImage = nImage;
	item.lParam = lParam;
	nItem = InsertItem(nItem, &item);
	return nItem;
}

void CCustomTabCtrl::UpdateNumberFrom(int idx)
{
	int c = GetItemCount();
	BOOL addnum = AppConfig.tab_add_number;
	while (c > idx)
	{
		TCITEM item;
		item.mask = TCIF_PARAM;
		GetItem(c - 1, &item);
		CTelnetConn* telnet = (CTelnetConn*)item.lParam;
		CString name;
		if (addnum)
		{
			name.Format("%d.%s", c, telnet->name);
			item.pszText = (LPSTR)(LPCTSTR)name;
		}
		else
			item.pszText = (LPSTR)(LPCTSTR)telnet->name;

		item.mask = TCIF_TEXT;
		SetItem(c - 1, &item);
		c--;
	}
}

void CCustomTabCtrl::DrawItem(LPDRAWITEMSTRUCT lpds)
{
	CDC dc;
	dc.Attach(lpds->hDC);

	DWORD style = GetStyle();

	TCITEM item;	char text[100];
	item.mask = TCIF_IMAGE | TCIF_TEXT;
	item.pszText = text;	item.cchTextMax = 100;
	GetItem(lpds->itemID, &item);
	CConn* con = (CConn*)lpds->itemData;
	COLORREF bkcolor, textcolor;
	dc.SetBkMode(TRANSPARENT);
//�]�w��r�M�I���C��
#ifdef	_COMBO_
	//������T�A�Ϊ̺����U�������A�ϥմ���!
	if (item.iImage == 5 && !(lpds->itemState & ODS_SELECTED))
#else
	if (item.iImage == 5 && !(lpds->itemState & ODS_SELECTED))	//������T�A�ϥմ���!
#endif

	{
		textcolor = GetSysColor(COLOR_HIGHLIGHTTEXT);
		bkcolor = GetSysColor(COLOR_HIGHLIGHT);
	}
	else
	{
		bkcolor = GetSysColor(COLOR_BTNFACE);

		if (lpds->itemState & ODS_SELECTED)
			textcolor = AppConfig.active_tab_textcolor;
		else
			textcolor = GetSysColor(COLOR_WINDOWTEXT);
	}
//---------------------------------------------------------------

	int icox, icoy;
//	��R�I��
	if (style&TCS_BUTTONS)		//�OButton�Φ���
	{
		//	���n�A�d�\CDC�������A CDC::GetHalftoneBrush()
		//	���ݭnDeleteObject�A�]����ӵ{���@�Τ@��brush
		//	����MFC�������ѡA�ƹ�W�٬O�إ�Bitmap�M��CreatePatternBrush
		if (lpds->itemState & ODS_SELECTED)
		{
			CBrush *cbsh = CDC::GetHalftoneBrush();
			//	^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
			dc.SetTextColor(GetSysColor(COLOR_3DHILIGHT));
			dc.SetBkColor(GetSysColor(COLOR_BTNFACE));
			FillRect(dc.m_hDC, &lpds->rcItem, (HBRUSH)cbsh->m_hObject);
			lpds->rcItem.top += scaler.CalcY(2);
			lpds->rcItem.left += scaler.CalcX(2);
		}
		else
			dc.FillSolidRect(&lpds->rcItem, bkcolor);
	}
	else	//Tab �Φ�
	{
		scaler.InflateRect(&lpds->rcItem, -2, -1);
	}
	//�e�X icon
	static const int icon_size = 16;
	icox = lpds->rcItem.left + scaler.CalcX(2);
	icoy = lpds->rcItem.top + (lpds->rcItem.bottom - lpds->rcItem.top - icon_size) / 2;
	GetImageList()->Draw(&dc, item.iImage, CPoint(icox, icoy), ILD_TRANSPARENT);

	//��X��r
	dc.SetTextColor(textcolor);
	lpds->rcItem.left = icox + icon_size + scaler.CalcX(2);
	lpds->rcItem.top += (lpds->rcItem.bottom - lpds->rcItem.top - scaler.CalcY(12)) / 2;
//#if defined(_COMBO_)
//	lpds->rcItem.right-=3;
//#endif
	dc.DrawText(text, strlen(text), &lpds->rcItem, DT_SINGLELINE | DT_END_ELLIPSIS);
	dc.Detach();
}


CImageList* CCustomTabCtrl::CreateDragImage(int i)
{
	TCITEM item;	char text[100];
	item.mask = TCIF_IMAGE | TCIF_TEXT;
	item.pszText = text;	item.cchTextMax = 100;
	GetItem(i, &item);

//	Prepare to Paint
	CImageList* img = new CImageList;
	HDC hdc =::GetDC(m_hWnd);
	HDC memdc = CreateCompatibleDC(hdc);
	HGDIOBJ holdf = SelectObject(memdc, GetFont()->m_hObject);
//	Begin Paint

	CRect rc;	rc.top = rc.left = 0;
	int l = strlen(item.pszText);
	DrawText(memdc, item.pszText, l, rc, DT_LEFT | DT_SINGLELINE | DT_CALCRECT);
	HBITMAP bmp = CreateCompatibleBitmap(hdc, rc.right + 20, rc.bottom);
	img->Create(rc.right + 20, rc.bottom, ILC_COLOR32 | ILC_MASK, 1, 0);
	HGDIOBJ holdobj = SelectObject(memdc, bmp);
//	�M�ŭI��
	SetBkColor(memdc, 0xc0c0c0);
	ExtTextOut(memdc, 0, 0, ETO_OPAQUE, rc, NULL, 0, NULL);

	ImageList_Draw(GetImageList()->m_hImageList, item.iImage, memdc, 0, (rc.bottom - 16) / 2, ILD_TRANSPARENT);
	//��X��r
	SetTextColor(memdc, GetSysColor(COLOR_WINDOWTEXT));
	rc.OffsetRect(20, 0);
	DrawText(memdc, item.pszText, l, rc, DT_SINGLELINE);

//	End	Paint
	::ReleaseDC(m_hWnd, hdc);
	SelectObject(memdc, holdobj);
	SelectObject(memdc, holdf);
	ImageList_AddMasked(img->m_hImageList, bmp, 0xc0c0c0);
	DeleteObject(bmp);
	DeleteDC(memdc);
	return img;
}

inline void CCustomTabCtrl::BeginDrag(CPoint point)
{
	dragimg = CreateDragImage(sel);
	dragimg->BeginDrag(0, CPoint(-12, -4));
	SetCursor(AfxGetApp()->LoadCursor(IDC_DRAG_CUR));
}

void CCustomTabCtrl::SetItemText(int i, CString Text)
{
	TCITEM item;	item.mask = TCIF_TEXT;	CString Title;
	if (AppConfig.tab_add_number)
		Title.Format("%d.%.12s", i + 1, Text);
	else
		Title = Text.Left(12);
	item.pszText = (LPSTR)(LPCTSTR)Title;
	SetItem(i, &item);
}
