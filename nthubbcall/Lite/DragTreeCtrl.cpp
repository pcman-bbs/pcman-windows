// DragTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "pcman.h"
#include "DragTreeCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDragTreeCtrl

CDragTreeCtrl::CDragTreeCtrl()
{
	dragitem = NULL;
	dragimg = NULL;
	hoveritem = NULL;
	auto_expand_time = -1;
	hovertime = 0;
}

CDragTreeCtrl::~CDragTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CDragTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CDragTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBeginDrag)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnExpanded)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
	ON_WM_TIMER()
	ON_WM_KEYUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDragTreeCtrl message handlers

void CDragTreeCtrl::MoveItem(HTREEITEM item, HTREEITEM target, bool up, bool bcopy)
{

}

void CDragTreeCtrl::OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	dragitem = pNMTreeView->itemNew.hItem;
	dragimg = CreateDragImage(dragitem);

	this->ModifyStyle(0, TVS_NOTOOLTIPS);

	dragimg->BeginDrag(0, CPoint(-12, -8));
	SetCapture();
	cursor = CanDrag(dragitem) ? AfxGetApp()->LoadCursor(IDC_DRAG_CUR)
			 : AfxGetApp()->LoadStandardCursor(IDC_NO);
	SetCursor(cursor);

	if (auto_expand_time > 0)
		SetTimer(1, 200, NULL);
	*pResult = 0;
}

void CDragTreeCtrl::OnExpanded(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (dragimg)
		dragimg->DragShowNolock(TRUE);
}

void CDragTreeCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	UINT flag;
	HTREEITEM newitem = HitTest(point, &flag);
	if (flag & TVHT_ONITEMLABEL || flag & TVHT_ONITEMICON)
		if (newitem && dragitem && newitem != dragitem)
		{
			dragimg->DragShowNolock(FALSE);
			if (CanDrag(dragitem))
			{
				SelectItem(newitem);
				MoveItem(dragitem, newitem,
						 GetIndex(dragitem, GetRootItem()) > GetIndex(newitem, GetRootItem()),
						 !!HIBYTE(GetKeyState(VK_CONTROL)));
			}
		}
	if (dragitem)
		EndDrag();
	SelectDropTarget(NULL);
	CTreeCtrl::OnLButtonUp(nFlags, point);
}

void CDragTreeCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if (dragitem)
	{
//		dragimg->DragShowNolock(FALSE);
		SetCursor(cursor);
		CRect rc;
		GetClientRect(rc);
		if (point.y > rc.bottom)
		{
			dragimg->DragShowNolock(FALSE);
			SendMessage(WM_VSCROLL, MAKEWPARAM(SB_LINEDOWN, 0), 0);
		}
		else if (point.y < 0)
		{
			dragimg->DragShowNolock(FALSE);
			SendMessage(WM_VSCROLL, MAKEWPARAM(SB_LINEUP, 0), 0);
		}
		UINT flag;
		HTREEITEM newitem = HitTest(point, &flag);
		if (flag & TVHT_ONITEMLABEL || flag & TVHT_ONITEMICON)
		{
			dragimg->DragShowNolock(FALSE);
			if (newitem != hoveritem)	//如果移動到其他item，重新計時
				hovertime = 0;
			hoveritem = newitem;
			SelectDropTarget(newitem);
		}
		else
		{
			if (GetDropHilightItem())
			{
				dragimg->DragShowNolock(FALSE);
				SelectDropTarget(NULL);
			}
		}

		ClientToScreen(&point);
		dragimg->DragShowNolock(TRUE);
		dragimg->DragMove(point);
	}
	CTreeCtrl::OnMouseMove(nFlags, point);
}

void CDragTreeCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_CONTROL)
	{
		if (CanDrag(dragitem))
		{
			cursor = AfxGetApp()->LoadCursor(IDC_COPY_CUR);
			SetCursor(cursor);
		}
	}
	if (!dragitem)
		CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
	else if (nChar == VK_ESCAPE)
	{
		SelectDropTarget(NULL);
		EndDrag();
	}
}

UINT CDragTreeCtrl::OnGetDlgCode()
{
	if (dragitem)
		return DLGC_WANTALLKEYS;
	return CTreeCtrl::OnGetDlgCode();
}

void CDragTreeCtrl::EndDrag()
{
	dragitem = NULL;
	dragimg->EndDrag();
	delete dragimg;
	dragimg = NULL;

	this->ModifyStyle(TVS_NOTOOLTIPS, 0);
	ReleaseCapture();
	if (auto_expand_time > 0)
		KillTimer(1);
}

void CDragTreeCtrl::SetAutoExpandTarget(int time)
{
	auto_expand_time = time;
}

void CDragTreeCtrl::OnTimer(UINT nIDEvent)
{
	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);
	UINT flag;	HTREEITEM newitem = HitTest(point, &flag);
	if (flag & TVHT_ONITEMLABEL || flag & TVHT_ONITEMICON)
	{
		if (newitem != hoveritem)	//如果移動到其他item，重新計時
		{
			hovertime = 0;
			hoveritem = newitem;
		}
		else if (hovertime <= auto_expand_time)
		{
			hovertime += 200;
			if (hovertime > auto_expand_time &&
				ItemHasChildren(hoveritem))
			{
				dragimg->DragShowNolock(FALSE);
				Expand(hoveritem, TVE_EXPAND);
			}
		}
	}
	else
		hovertime = 0;

	CTreeCtrl::OnTimer(nIDEvent);
}

int CDragTreeCtrl::GetIndex(HTREEITEM item, HTREEITEM from)
{
	int i = 0;
	for (HTREEITEM _item = from; _item != item && _item; _item = GetNextSiblingItem(_item))
		if (ItemHasChildren(_item))
			i += GetIndex(item, GetChildItem(_item));
		else
			i++;
	return i;
}


void CDragTreeCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_CONTROL)
	{
		cursor = !dragitem ? AfxGetApp()->LoadStandardCursor(IDC_ARROW) :
				 (CanDrag(dragitem) ? AfxGetApp()->LoadCursor(IDC_DRAG_CUR) :
				  AfxGetApp()->LoadStandardCursor(IDC_NO));
		SetCursor(cursor);
	}
	CTreeCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
}

HTREEITEM CDragTreeCtrl::PathToItem(LPCTSTR path, HTREEITEM root, TCHAR separator)
{
	static char* path_buf = 0;
	if (!path_buf)
	{
		int l = strlen(path) + 1;
		path_buf = new char[l];
		memcpy(path_buf, path, l);
		path = path_buf;
	}

	HTREEITEM item;	LPSTR pch;
	for (pch = LPSTR(path) ; *pch; pch = CharNextExA(CP_ACP, pch, 0))
	{
		if (*pch == separator)
			break;
	}
	char ch = *pch;	*pch = 0;
	for (item = GetChildItem(root); item; item = GetNextSiblingItem(item))
	{
		CString text = GetItemText(item);
		if (text.CompareNoCase(path) == 0)
			break;
	}
	if (ch == separator && item)	//如果還沒到終點
		return PathToItem(pch + 1, item, separator);

	delete []path_buf;
	path_buf = NULL;
	return item;
}

CString CDragTreeCtrl::GetItemPath(HTREEITEM item, TCHAR separator)
{
	char path[2048];
	char* pbuf = path + 2048;
	CString text;
	for (; item; item = GetParentItem(item))
	{
		text = GetItemText(item);
		char* pbuf2 = pbuf - (text.GetLength() + 1);
		strcpy(pbuf2, text);
		*(pbuf - 1) = separator;
		pbuf = pbuf2;
	}
	path[2048-1] = 0;
	return CString(pbuf);
}
