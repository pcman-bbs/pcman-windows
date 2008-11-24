// CtrlEdit.cpp : implementation file
//

#include "stdafx.h"
#include "pcman.h"
#include "CtrlEdit.h"
#include "StringDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCtrlEdit

CCtrlEdit::CCtrlEdit()
{
}

CCtrlEdit::~CCtrlEdit()
{
}

LRESULT CCtrlEdit::OnPaste(WPARAM w, LPARAM l)
{
	if ((GetStyle() & ES_READONLY) || !OpenClipboard())
		return 0;

	HANDLE hlocmem = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, sizeof(LCID));
	if (hlocmem)
	{
		PLCID lcid = (PLCID)GlobalLock(hlocmem);
		*lcid = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL), SORT_CHINESE_BIG5);
		GlobalUnlock(hlocmem);
		SetClipboardData(CF_LOCALE, hlocmem);
	}
	HANDLE hmem = GetClipboardData(CF_TEXT);
	if (hmem != NULL)
	{
		LPSTR data = (LPSTR)GlobalLock(hmem);
		if (strstr(data, "\x1b["))	//如果含有色彩控制碼
		{
			CString tmp = data;
			tmp.Replace("\x1b[", "^[[");
			ReplaceSel(tmp, FALSE);
			CStringDlg* pdlg = (CStringDlg*)GetParent();
			if (pdlg)
			{
				pdlg->ansi.SetCheck(1);	//勾選控制碼選項
			}
		}
		else
			ReplaceSel(data, TRUE);

		GlobalUnlock(hmem);
	}
	CloseClipboard();
	return 0;
}

BEGIN_MESSAGE_MAP(CCtrlEdit, CEdit)
	//{{AFX_MSG_MAP(CCtrlEdit)
	ON_WM_CANCELMODE()
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_PASTE , CCtrlEdit::OnPaste)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCtrlEdit message handlers

void CCtrlEdit::OnCancelMode()
{
	//CEdit::OnCancelMode();
}

//DEL LRESULT CCtrlEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
//DEL {
//DEL 	if(!enabled)
//DEL 	{
//DEL 		if( (message>=WM_KEYFIRST && message<=WM_KEYLAST)
//DEL 		|| (message>=WM_MOUSEFIRST && message<=WM_MOUSELAST) )
//DEL 			return 0;
//DEL 	}
//DEL 	return CEdit::WindowProc(message, wParam, lParam);
//DEL }

//DEL BOOL CCtrlEdit::EnableWindow(BOOL bEnable)
//DEL {
//DEL 	BOOL p=enabled;
//DEL 	if(bEnable)
//DEL 		ModifyStyle(0,WS_TABSTOP);
//DEL 	else
//DEL 		ModifyStyle(WS_TABSTOP,0);
//DEL 	enabled=bEnable;
//DEL 	return p;
//DEL }

HBRUSH CCtrlEdit::CtlColor(CDC* pDC, UINT nCtlColor)
{
	if (nCtlColor != CTLCOLOR_STATIC)
		return NULL;
	pDC->SetBkColor(GetSysColor(COLOR_WINDOW));
	return GetSysColorBrush(COLOR_WINDOW);
}
