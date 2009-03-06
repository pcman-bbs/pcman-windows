// SearchBar.cpp : implementation file
//

#include "../Lite/stdafx.h"

#include "SearchBar.h"
#include "../Lite/MainFrm.h"
#include "../Lite/SearchPlugin.h"
#include "../Lite/AppConfig.h"

#include <windowsx.h>
#include <Richedit.h>	// for richedit 2.0
#include "../Resource/resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static HMODULE riched20 = NULL;

WCHAR* RichEdit20_GetText(HWND edit);

/////////////////////////////////////////////////////////////////////////////
// CSearchBar

CSearchBar::CSearchBar() : old_search_bar_proc(NULL), img_list(NULL)
{
	if (AppConfig.use_riched20)
		riched20 = LoadLibraryA("RICHED20.DLL");
}

CSearchBar::~CSearchBar()
{
	ImageList_Destroy(img_list);

	if (riched20)
	{
		FreeLibrary(riched20);
		riched20 = NULL;
	}
}

BEGIN_MESSAGE_MAP(CSearchBar, CToolBar)
	//{{AFX_MSG_MAP(CSearchBar)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(TBN_DROPDOWN, OnMenuDropDown)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchBar message handlers

static TBBUTTON searchbar_btns[] =
{
	{ -1, ID_WEB_SEARCH, TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_DROPDOWN, 0, 0 }
};

BOOL CSearchBar::Create(CWnd* pParent)
{
	return CToolBar::CreateEx(pParent, TBSTYLE_TRANSPARENT | TBSTYLE_AUTOSIZE |/*TBSTYLE_LIST|*/TBSTYLE_FLAT, TBSTYLE_TOOLTIPS | WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP | WS_CLIPCHILDREN,
							  CRect(0, 0, 0, 0), IDC_SEARCHBAR);
}

LRESULT CALLBACK CSearchBar::EditProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	CSearchBar* This = (CSearchBar*)::GetWindowLong(hwnd, GWL_USERDATA);
	switch (msg)
	{
	case WM_KEYDOWN:
		switch (wparam)
		{
		case VK_RETURN:
			This->GetParent()->PostMessage(WM_COMMAND, ID_WEB_SEARCH, 0);
			break;
		case VK_ESCAPE:
			return 0;
		}
		break;
	case WM_CONTEXTMENU:
		if (riched20)
		{
			HMENU menu = ::LoadMenu(AfxGetResourceHandle(), LPCTSTR(IDR_POPUP2));
			HMENU popup = ::GetSubMenu(menu, 2);
			TrackPopupMenu(popup, TPM_LEFTALIGN, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), 0, hwnd, NULL);
			DestroyMenu(menu);
			return 0;
		}
		break;
	case WM_COMMAND:
		if (riched20)
		{
			switch (LOWORD(wparam))
			{
			case ID_EDIT_UNDO:
				::SendMessage(hwnd, WM_UNDO, 0, 0);
				return 0;
			case ID_EDIT_CUT:
				::SendMessage(hwnd, WM_CUT, 0, 0);
				return 0;
			case ID_EDIT_COPY:
				::SendMessage(hwnd, WM_COPY, 0, 0);
				return 0;
			case ID_EDIT_PASTE:
				::SendMessage(hwnd, WM_PASTE, 0, NULL);
				return 0;
			case ID_EDIT_DELETE:
				::SendMessage(hwnd, WM_CLEAR, 0, 0);
				return 0;
			case ID_EDIT_SELALL:
				{
					CHARRANGE cr = {0, -1};
					::SendMessage(hwnd, EM_EXSETSEL, 0, (LPARAM)&cr);
					return 0;
				}
			}
		}
		break;
	case WM_SETFOCUS:
		CallWindowProc(This->old_search_bar_proc, hwnd, msg, wparam, lparam);
		if (riched20)
		{
			CHARRANGE cr = {0, -1};
			::SendMessage(hwnd, EM_EXSETSEL, 0, (LPARAM)&cr);
		}
		else
			::SendMessage(hwnd, EM_SETSEL, 0, -1);
		return 0;
	case WM_GETDLGCODE:
		return DLGC_WANTALLKEYS;
	case WM_SETCURSOR:
		if (riched20)
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_IBEAM));
		break;
	}
	return CallWindowProc(This->old_search_bar_proc, hwnd, msg, wparam, lparam);
}

void CSearchBar::OnSize(UINT nType, int cx, int cy)
{
	CToolBar::OnSize(nType, cx, cy);
	CRect rc;
	GetItemRect(0, rc);

	::MoveWindow(hedit, 1, 1, cx - rc.Width() - 6, cy - 2, TRUE);
	GetToolBarCtrl().SetIndent(cx - rc.Width() - 4);
}

void CSearchBar::SetFont(CFont *font)
{
	CToolBar::SetFont(font);
	::SendMessage(hedit, WM_SETFONT, (WPARAM)font->m_hObject, MAKELPARAM(TRUE, 0));
}

void CSearchBar::OnMenuDropDown(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTOOLBAR nmtb = (LPNMTOOLBAR)pNMHDR;
	RECT rc;
	SendMessage(TB_GETRECT, nmtb->iItem, (LPARAM)&rc);
	::MapWindowPoints(m_hWnd, NULL, LPPOINT(&rc), 2);
	if (nmtb->iItem == ID_WEB_SEARCH)
	{
		HMENU pop = SearchPluginCollection.CreateSearchMenu();
		if (AppConfig.search_engine >= 0)
		{
			SetMenuDefaultItem(pop, AppConfig.search_engine, MF_BYPOSITION);
			CheckMenuItem(pop, AppConfig.search_engine, MF_BYPOSITION | MF_CHECKED);
		}
		UINT cmd = TrackPopupMenu(pop, TPM_RETURNCMD | TPM_RIGHTALIGN | TPM_TOPALIGN, rc.right, rc.bottom, 0, m_hWnd, NULL);
		DestroyMenu(pop);
		if (cmd > 0)
		{
			int idx = (int)cmd - CSearchPluginCollection::ID_SEARCHPLUGIN00;
			if (AppConfig.search_engine != idx)
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
	if (AppConfig.search_engine >= 0
		&& AppConfig.search_engine < SearchPluginCollection.GetCount()
		&& SearchPluginCollection.GetImage(AppConfig.search_engine))
	{
		int i;
		for (i = 0; i <= AppConfig.search_engine; ++i)
		{
			if (SearchPluginCollection.GetImage(i))
				++img;
		}
	}

	TBBUTTONINFO inf = {0};
	inf.cbSize = sizeof(inf);
	inf.dwMask = TBIF_IMAGE;
	inf.iImage = img;
	GetToolBarCtrl().SetButtonInfo(ID_WEB_SEARCH, &inf);
	GetToolBarCtrl().AutoSize();
}

bool CSearchBar::GetSearchTerm(CString& term)
{
	if (riched20)
	{
		WCHAR* tmp = ::RichEdit20_GetText(hedit);
		int len = 0;
		if (tmp)
		{
			int wlen = wcslen(tmp) + 1;
			len = WideCharToMultiByte(CP_UTF8, 0, tmp, wlen, NULL, 0, NULL, NULL);
			char* buf = term.GetBuffer(len);
			len = WideCharToMultiByte(CP_UTF8, 0, tmp, wlen, buf, len, NULL, NULL);
			term.ReleaseBuffer();
			delete tmp;
			if(AppConfig.searchbar_cleanup)
				::SetWindowText(hedit, "");
		}
		if (len <= 0)
			term.Empty();

		return true;
	}
	else
	{
		int len = ::GetWindowTextLength(hedit) + 1;
		if (len > 0)
		{
			::GetWindowText(hedit, term.GetBuffer(len), len);
			term.ReleaseBuffer();
		}
	}
	return false;
}

int CSearchBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CToolBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	GetToolBarCtrl().SetBitmapSize(CSize(16, 16));

	LPCTSTR cls_name;
	if (riched20)
		cls_name = "RichEdit20A";
	else
		cls_name = "EDIT";

	hedit = CreateWindowEx(WS_EX_CLIENTEDGE, cls_name, NULL, ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE,
						   0, 0, 0, 0, m_hWnd, NULL, AfxGetInstanceHandle(), NULL);

	if (riched20)
	{
		::SendMessage(hedit, EM_SETTEXTMODE, TM_PLAINTEXT | TM_MULTICODEPAGE, 0);
		::SendMessage(hedit, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(2, 2));
		::SendMessage(hedit, EM_SETUNDOLIMIT, 1, 0);
	}

	old_search_bar_proc = (WNDPROC)::GetWindowLong(hedit, GWL_WNDPROC);
	::SetWindowLong(hedit, GWL_WNDPROC, (LONG)EditProc);
	::SetWindowLong(hedit, GWL_USERDATA, (LONG)this);

	CToolBarCtrl& toolbar = GetToolBarCtrl();

	toolbar.AddButtons(1, searchbar_btns);
	toolbar.SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);
	toolbar.SetIndent(100);
	toolbar.AutoSize();

	int n = SearchPluginCollection.GetCount();
	if (n > 0)
	{
		for (int i = 0; i < n; ++i)
		{
			HBITMAP hbmp = SearchPluginCollection.GetImage(i);
			if (hbmp)
			{
				TBADDBITMAP tbab;
				tbab.hInst = NULL;
				tbab.nID = (UINT)hbmp;
				::SendMessage(toolbar.m_hWnd, TB_ADDBITMAP, 1, (LPARAM)&tbab);
			}
		}
	}

	UpdateBtn();
	EnableToolTips();
	return 0;
}

WCHAR* RichEdit20_GetText(HWND edit)
{
	WCHAR *buf;
	GETTEXTLENGTHEX gtl = { GTL_USECRLF | GTL_CLOSE, 1200 };
	DWORD len = SendMessage(edit, EM_GETTEXTLENGTHEX, (WPARAM) & gtl, (LPARAM)0);
	if (len <= 0)
		return NULL;
	GETTEXTEX gt = {0};
	gt.codepage = 1200;	// unicode
	gt.flags = GT_USECRLF;
	gt.cb = len * sizeof(WCHAR);
	buf = new WCHAR[len + 1];
	len = SendMessage(edit, EM_GETTEXTEX, (WPARAM) & gt, (LPARAM)buf);
	return buf;
}

void CSearchBar::SetEditFocus()
{
	::SetFocus(hedit);
}

void CSearchBar::OnSetFocus(CWnd* pOldWnd)
{
	CToolBar::OnSetFocus(pOldWnd);
	SetEditFocus();
}
