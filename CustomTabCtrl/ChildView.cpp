// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "CustomTabCtrl.h"
#include "ChildView.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildView

CChildView::CChildView()
{
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	//{{AFX_MSG_MAP(CChildView)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
									   ::LoadCursor(NULL, IDC_ARROW), (HBRUSH)GetStockObject(BLACK_BRUSH), NULL);

	return TRUE;
}

void CChildView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// TODO: Add your message handler code here

	// Do not call CWnd::OnPaint() for painting messages
}


int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd ::OnCreate(lpCreateStruct) == -1)
		return -1;

	img.Create(IDR_MAINFRAME, 16, 0, RGB(192, 192, 192));

	tab.Create(WS_VISIBLE | WS_CHILD | TCS_SINGLELINE | TCS_FLATBUTTONS | TCS_HOTTRACK, CRect(0, 0, 0, 0), this, 10);
	tab.SetImageList(&img);

	char text[60]; int i;
	for (i = 1; i <= 12; ++i)
	{
		sprintf(text, "Tab Item %d", i);
		tab.AppendItem(text, i % 6);
		tab.SetItemTooltip(i - 1, text);
	}
	tab.SetMinTabSize(40);
	tab.SetCurSel(2);

	tab2.Create(WS_VISIBLE | WS_CHILD/*|TCS_BUTTONS*/ | TCS_TOOLTIPS | TCS_SINGLELINE | TCS_FLATBUTTONS | TCS_HOTTRACK, CRect(0, 0, 0, 0), this, 11);
	for (i = 0; i < 12; ++i)
	{
		sprintf(text, "Tab Item %d", i + 1);
		tab2.InsertItem(i, text, -1);
	}
	return 0;
}

void CChildView::OnSize(UINT nType, int cx, int cy)
{
	CWnd ::OnSize(nType, cx, cy);
	tab.MoveWindow(0, 0, cx, 32);
	tab2.MoveWindow(0, 64, cx, 32);
}

BOOL CChildView::OnToolTipNeedText(UINT id, NMHDR *nmhdr, LRESULT *r)
{
	NMTTDISPINFO* ptt = (NMTTDISPINFO*)nmhdr;
	ptt->lpszText = (LPSTR)(LPCTSTR)"TEST!!!!";

	return TRUE;
}
