// CustomToolBar.cpp : implementation file
//

#include "stdafx.h"
#include "pcman.h"
#include "CustomToolBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCustomToolBar

CCustomToolBar::CCustomToolBar()
{
}

CCustomToolBar::~CCustomToolBar()
{
}


BEGIN_MESSAGE_MAP(CCustomToolBar, CToolBar)
	//{{AFX_MSG_MAP(CCustomToolBar)
	ON_NOTIFY_REFLECT(TBN_GETBUTTONINFO, OnToolBarGetButtonInfo)
	ON_NOTIFY_REFLECT(TBN_BEGINADJUST, OnToolBarBeginAdjust)
	ON_NOTIFY_REFLECT(TBN_ENDADJUST, OnToolBarEndAdjust)
	ON_NOTIFY_REFLECT(TBN_QUERYDELETE, OnToolBarQueryDelete)
	ON_NOTIFY_REFLECT(TBN_QUERYINSERT, OnToolBarQueryInsert)
	ON_NOTIFY_REFLECT(TBN_TOOLBARCHANGE, OnToolBarChange)
	ON_NOTIFY_REFLECT(TBN_RESET, OnToolBarReset)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
	ON_COMMAND(1000, OnCustomize)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomToolBar message handlers

void CCustomToolBar::OnToolBarGetButtonInfo(NMHDR *pnmhdr, LRESULT *r)
{
	NMTOOLBAR* nmt = (NMTOOLBAR*)pnmhdr;
	if (nmt->iItem < 0 || nmt->iItem >= inf->allcount)
	{
		*r = FALSE;
		return;
	}
	memcpy(&nmt->tbButton, &inf->pbtns[nmt->iItem], sizeof(TBBUTTON));
	CString str;
	str.LoadString(inf->pbtns[nmt->iItem].idCommand);
	strncpy(nmt->pszText, LPCTSTR(str) + 1, nmt->cchText);
	*r = TRUE;
}

void CCustomToolBar::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CMenu pop;
	pop.CreatePopupMenu();
	pop.AppendMenu(MF_STRING, 1000, LoadString(IDS_CUSTOMIZE_TOOLBAR));
	pop.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
}

void CCustomToolBar::OnCustomize()
{	GetToolBarCtrl().Customize();	}

void CCustomToolBar::OnToolBarQueryDelete(NMHDR *pnmhdr, LRESULT *r)
{	*r = TRUE;	}

void CCustomToolBar::OnToolBarQueryInsert(NMHDR *pnmhdr, LRESULT *r)
{	*r = TRUE;	}

void CCustomToolBar::OnToolBarChange(NMHDR *pnmhdr, LRESULT *r)
{
	GetParentFrame()->RecalcLayout();
}

void CCustomToolBar::OnToolBarEndAdjust(NMHDR *pnmhdr, LRESULT *r)
{
	inf->GetState(*this);
}

void CCustomToolBar::OnToolBarBeginAdjust(NMHDR *pnmhdr, LRESULT *r)
{

}

void CCustomToolBar::OnToolBarReset(NMHDR *pnmhdr, LRESULT *r)
{
	DeleteAllButtons();
	LoadToolBar(inf);
}

BOOL CCustomToolBar::LoadToolBar(CCustomToolBarInfo* _inf)
{
	inf = _inf;
	for (int i = 0;i < inf->count;i++)
		GetToolBarCtrl().AddButtons(1, &inf->pbtns[ inf->index[i] ]);
	GetToolBarCtrl().AutoSize();
	return TRUE;
}

void CCustomToolBar::Default()
{
	GetToolBarCtrl().AddButtons(inf->allcount, inf->pbtns);
}

void CCustomToolBar::DeleteAllButtons()
{
	int c = GetToolBarCtrl().GetButtonCount();
	while (c--)
		GetToolBarCtrl().DeleteButton(0);
}
