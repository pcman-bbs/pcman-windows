// SearchBar.cpp : implementation file
//

#include "../Lite/stdafx.h"

#include "SearchBar.h"
#include "../Lite/MainFrm.h"
#include "../Lite/SearchPlugin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSearchBar

CSearchBar::CSearchBar() : old_search_bar_proc(NULL), img_list(NULL)
{
}

CSearchBar::~CSearchBar()
{
	ImageList_Destroy( img_list );
}

BEGIN_MESSAGE_MAP(CSearchBar, CToolBar)
	//{{AFX_MSG_MAP(CSearchBar)
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(TBN_DROPDOWN, OnMenuDropDown)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchBar message handlers

static TCHAR btn_text[] = "Google!";
static TBBUTTON searchbar_btns[]={
	{-1, ID_WEB_SEARCH,TBSTATE_ENABLED,TBSTYLE_BUTTON|TBSTYLE_DROPDOWN, 0, 0 }
};

BOOL CSearchBar::Create( CWnd* pParent )
{
	return CToolBar::CreateEx( pParent, TBSTYLE_AUTOSIZE|/*TBSTYLE_LIST|*/TBSTYLE_FLAT, TBSTYLE_TOOLTIPS|WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP|WS_CLIPCHILDREN,
                               CRect(0, 0, 0, 0), IDC_SEARCHBAR );
}

void CSearchBar::OnSetFocus(CWnd* pOldWnd) 
{
	::SetFocus( hedit );
}

LRESULT CALLBACK CSearchBar::EditProc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam)
{
	CSearchBar* This = (CSearchBar*)::GetWindowLong( hwnd, GWL_USERDATA );
	switch( msg )
	{
	case WM_KEYDOWN:
		switch(wparam)
		{
		case VK_RETURN:
			This->GetParent()->PostMessage(WM_COMMAND, ID_WEB_SEARCH, 0);
			break;
		case VK_ESCAPE:
			return 0;
		}
		break;
/*
	case WM_SETFOCUS:
		{
			LPTSTR engine = SearchPluginCollection.GetField( AppConfig.search_engine, CSearchPluginCollection::SHORTNAME );
			CString term;
			int len = ::GetWindowTextLength( hwnd );
			::GetWindowText( hwnd, term.GetBuffer( len + 1 ), len + 1 );
			term.ReleaseBuffer();
			if( term == engine )
				::SetWindowText( hwnd, NULL );
			break;
		}
	case WM_KILLFOCUS:
		{
			CString term;
			int len = ::GetWindowTextLength( hwnd );
			::GetWindowText( hwnd, term.GetBuffer( len + 1 ), len + 1 );
			term.ReleaseBuffer();
			if( term.IsEmpty() )
				::SetWindowText(hwnd, SearchPluginCollection.GetField( AppConfig.search_engine, CSearchPluginCollection::SHORTNAME ));
		}
		break;
*/
	case WM_GETDLGCODE:
		return DLGC_WANTALLKEYS;
	}
	return CallWindowProc( This->old_search_bar_proc, hwnd, msg, wparam, lparam );
}

void CSearchBar::OnSize(UINT nType, int cx, int cy) 
{
	CToolBar::OnSize(nType, cx, cy);
	CRect rc;
	GetItemRect( 0, rc );

	::MoveWindow( hedit, 1, 1, cx - rc.Width() - 6, cy-2, TRUE );
	GetToolBarCtrl().SetIndent( cx - rc.Width() - 4 );
}

void CSearchBar::SetFont(CFont *font)
{
	CToolBar::SetFont(font);
	::SendMessage( hedit, WM_SETFONT, (WPARAM)font->m_hObject, MAKELPARAM(TRUE, 0) );
}

void CSearchBar::OnMenuDropDown(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTOOLBAR nmtb=(LPNMTOOLBAR)pNMHDR;
	RECT rc;
	SendMessage( TB_GETRECT, nmtb->iItem, (LPARAM)&rc );
	::MapWindowPoints( m_hWnd, NULL, LPPOINT(&rc), 2 );
	if(nmtb->iItem == ID_WEB_SEARCH)
	{
		HMENU pop = SearchPluginCollection.CreateSearchMenu();
		if( AppConfig.search_engine >= 0 )
		{
			SetMenuDefaultItem( pop, AppConfig.search_engine, MF_BYPOSITION );
			CheckMenuItem( pop, AppConfig.search_engine, MF_BYPOSITION|MF_CHECKED );
		}
		UINT cmd = TrackPopupMenu( pop, TPM_RETURNCMD|TPM_RIGHTALIGN|TPM_TOPALIGN, rc.right, rc.bottom, 0, m_hWnd, NULL);
		DestroyMenu( pop );
		if( cmd > 0 )
		{
			int idx = (int)cmd - CSearchPluginCollection::ID_SEARCHPLUGIN00;
			if( AppConfig.search_engine != idx )
			{
				AppConfig.search_engine = idx;
				UpdateBtn();
			}
		}
	}

}

void CSearchBar::UpdateBtn()
{
	int img = -1;
	if( AppConfig.search_engine >= 0 
		&& AppConfig.search_engine < SearchPluginCollection.GetCount()
		&& SearchPluginCollection.GetImage(AppConfig.search_engine) )
	{
		int i;
		for( i = 0; i <= AppConfig.search_engine; ++i )
		{
			if( SearchPluginCollection.GetImage(i) )
				++img;
		}
	}

	TBBUTTONINFO inf={0};
	inf.cbSize = sizeof(inf);
	inf.dwMask = TBIF_IMAGE;
	inf.iImage = img;
	GetToolBarCtrl().SetButtonInfo( ID_WEB_SEARCH, &inf );
	GetToolBarCtrl().AutoSize();
//	GetToolBarCtrl().SetIndent( rc.Width() - btn_rc.Width() - 4 );

/*
	TBBUTTONINFO inf={0};
	inf.cbSize = sizeof(inf);
	inf.dwMask = TBIF_IMAGE|TBIF_TEXT|TBIF_SIZE;
	inf.iImage = img;
	LPCTSTR text = SearchPluginCollection.GetField(AppConfig.search_engine,
 		                                           CSearchPluginCollection::SHORTNAME);
	if( text )
	{
		CWindowDC dc(this);
		CGdiObject* oldf = dc.SelectObject(GetFont());
		CSize sz = dc.GetTextExtent( text, strlen(text) );
		dc.SelectObject( oldf );
		inf.cx = sz.cx + 44;
	}
	else
		inf.cx = 44;

	inf.pszText = (LPTSTR)text;
	GetToolBarCtrl().SetButtonInfo( ID_WEB_SEARCH, &inf );
	GetToolBarCtrl().AutoSize();

	CRect rc;
	GetWindowRect(rc);
	CRect btn_rc;
	GetItemRect( 0, btn_rc );

	::MoveWindow( hedit, 1, 1, rc.Width() - btn_rc.Width() - 6, rc.Height()-2, TRUE );
	GetToolBarCtrl().SetIndent( rc.Width() - btn_rc.Width() - 4 );
*/
}

void CSearchBar::OnTimer(UINT nIDEvent) 
{
	UpdateBtn();
	CToolBar::OnTimer(nIDEvent);
	KillTimer( nIDEvent );
}

CString CSearchBar::GetSearchTerm()
{
	CString term;
	int len = ::GetWindowTextLength(hedit) + 1;
	if( len > 0 )
	{
		::GetWindowText( hedit, term.GetBuffer( len ), len );
		term.ReleaseBuffer();
	}
	return term;
}

int CSearchBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CToolBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	hedit = CreateWindowEx( WS_EX_CLIENTEDGE, "EDIT", NULL, ES_AUTOHSCROLL|WS_CHILD|WS_VISIBLE,
		                    0, 0, 0, 0, m_hWnd, NULL, AfxGetInstanceHandle(), NULL );
	old_search_bar_proc=(WNDPROC)::GetWindowLong(hedit,GWL_WNDPROC);
	::SetWindowLong(hedit,GWL_WNDPROC,(LONG)EditProc);
	::SetWindowLong(hedit,GWL_USERDATA,(LONG)this);

	CToolBarCtrl& toolbar = GetToolBarCtrl();

	toolbar.AddButtons( 1, searchbar_btns );
	toolbar.SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);
	toolbar.SetIndent( 100 );
	toolbar.AutoSize();

	int n = SearchPluginCollection.GetCount();
	if( n > 0 )
	{
		for( int i = 0; i < n; ++i )
		{
			HBITMAP hbmp = SearchPluginCollection.GetImage(i);
			if( hbmp )
			{
				CBitmap* bmp = CBitmap::FromHandle(hbmp);
				toolbar.AddBitmap( 0, bmp );
			}
		}
	}

	// FIXME: Icon cannot be correctly set here due to bugs of Windows.
	//        Use WM_TIMER to force asynchronized update.
	// UpdateBtn();
	SetTimer( 1, 0, NULL );
	EnableToolTips();
	return 0;
}


