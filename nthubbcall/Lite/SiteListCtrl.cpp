// SiteListCtrl.cpp: implementation of the CSiteListCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcman.h"
#include "SiteListCtrl.h"
#include "ListDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const char SEPARATOR[] = "        ";
const int SEPARATOR_LEN = 8;

CSiteListCtrl::CSiteListCtrl()
{
	SetAutoExpandTarget(1000);
	changed = 0;
}

CSiteListCtrl::~CSiteListCtrl()
{

}

BEGIN_MESSAGE_MAP(CSiteListCtrl, CDragTreeCtrl)
	//{{AFX_MSG_MAP(CSiteListCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSiteListCtrl::MoveItem(HTREEITEM item, HTREEITEM target, bool up, bool bcopy)
{
	if (!GetParentItem(item))
		return;

	changed = TRUE;
	HTREEITEM parent = GetParentItem(target);
	if (!parent)	parent = target;
	int img;	GetItemImage(target, img, img);
	if (img == 3)
	{
		CMenu mnu;	mnu.LoadMenu(IDR_POPUP);
		CPoint pt;	GetCursorPos(&pt);
		UINT ret = mnu.GetSubMenu(3)->TrackPopupMenu(TPM_RETURNCMD, pt.x, pt.y, this);
		if (!ret)
			return;
		if (ret == ID_SITE_MOVE_INTO_FOLDER)
			parent = target;
	}

	HTREEITEM after;
	if (up)
	{
		after = GetPrevSiblingItem(target);
		if (!after || parent == target)	after = TVI_FIRST;
	}
	else
	{
		after = target;
		if (parent == target)	after = TVI_LAST;
	}

	CString oldpath = GetItemPath(item);
	HTREEITEM newitem = CopyTo(item, parent, after, !!bcopy);
	CString newpath = GetItemPath(newitem);

	if (!bcopy)
		DeleteItem(item, oldpath != newpath);
	SelectItem(newitem);
}

HTREEITEM CSiteListCtrl::CopyTo(HTREEITEM from, HTREEITEM parent, HTREEITEM insert_after, bool prevent_dup)
{
	HTREEITEM oldparent = GetParentItem(from);
	HTREEITEM child = GetChildItem(parent);

	changed = TRUE;
	HTREEITEM newitem;
	TVINSERTSTRUCT tvitem;	tvitem.item.mask = TVIF_CHILDREN | TVIF_HANDLE | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE | TVIF_STATE | TVIF_TEXT;
	tvitem.item.cchTextMax = 256;
	char tmp[256];
	tvitem.item.pszText = tmp;
	tvitem.item.hItem = from;
	GetItem(&tvitem.item);
	tvitem.hInsertAfter = insert_after;
	tvitem.hParent = parent;

	char* ptab = strstr(tmp, SEPARATOR);
	CString address;
	char* ptmp;
	if (ptab)
	{
		address = ptab;
		*ptab = 0;
		ptmp = ptab;
	}
	else
		ptmp = tmp + strlen(tmp);

	int i = 2;
	while (prevent_dup && FindChildItem(parent, tmp))
	{
		itoa(i, ptmp, 10);
		i++;
	}
	if (ptab)
		strcat(tmp, address);

	newitem = CTreeCtrl::InsertItem(&tvitem);

	CString oldpath = GetItemFilePath(from);
	if (tvitem.item.iImage != 3 && IsFileExist(oldpath))
	{
		CString newdirname = GetItemFilePath(newitem);
		CopyFile(oldpath, newdirname, FALSE);
	}

	for (child = GetChildItem(from); child; child = GetNextSiblingItem(child))
		CopyTo(child, newitem, TVI_LAST);
	return newitem;
}

BOOL CSiteListCtrl::CanDrag(HTREEITEM item)
{
	return !!GetParentItem(item);
}

CString CSiteListCtrl::GetItemPath(HTREEITEM item, char separator, bool strip)
{
	CString path = CDragTreeCtrl::GetItemPath(item, separator);
	if (strip)
	{
		int i = path.Find(SEPARATOR);
		if (i != -1)
			path = path.Left(i);
	}
	return path;
}

void CSiteListCtrl::DeleteItem(HTREEITEM item, BOOL bDelFile)
{
	int img;	GetItemImage(item, img, img);
	CString fpath = GetItemFilePath(item);
	HTREEITEM parent = GetParentItem(item);
	if (img == 3)
	{
		for (HTREEITEM child = GetChildItem(item); child; child = GetNextSiblingItem(child))
			DeleteItem(child, bDelFile);
	}
	else if (bDelFile)
		DeleteFile(fpath);
	CDragTreeCtrl::DeleteItem(item);
}

CString CSiteListCtrl::GetItemFilePath(HTREEITEM item)
{
	CString fp = GetItemPath(item);

	if (IsItemDir(item))
		return (ConfigPath + fp);
	return CSiteSettings::GetFilePath(fp);
}

HTREEITEM CSiteListCtrl::FindChildItem(HTREEITEM parent, LPCTSTR key)
{
	HTREEITEM item;
	for (item = GetChildItem(parent);item; item = GetNextSiblingItem(item))
	{
		CString txt = GetItemText(item);
		int i = txt.Find(SEPARATOR);
		if (i != -1)
			txt = txt.Left(i);
		if (txt.CompareNoCase(key) == 0)
			break;
	}
	return item;
}

HTREEITEM CSiteListCtrl::GetTopParent(HTREEITEM item)
{
	HTREEITEM parent;
	while (parent = GetParentItem(item))
		item = parent;
	return item;
}
