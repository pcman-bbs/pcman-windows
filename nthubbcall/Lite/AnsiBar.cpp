// AnsiBar.cpp : implementation file
//

#include "stdafx.h"
#include "pcman.h"
#include "AnsiBar.h"
#include "Mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAnsiBar dialog

CAnsiBar::CAnsiBar(CWnd* pParent /*=NULL*/)
{
	//{{AFX_DATA_INIT(CAnsiBar)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	fg = 7;
	bk = 0;
	is_blink = 0;
	bchild = 0;
}


BEGIN_MESSAGE_MAP(CAnsiBar, CDialogBar)
	//{{AFX_MSG_MAP(CAnsiBar)
	ON_WM_DRAWITEM()
	ON_WM_MEASUREITEM()
	ON_WM_ERASEBKGND()
	ON_WM_MOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnsiBar message handlers

void CAnsiBar::OnBk()
{
	CMenu mnu;
	mnu.CreatePopupMenu();
	for (int i = 10;i < 18;i++)
	{
		mnu.AppendMenu(MF_OWNERDRAW, i, "");
	}
	CRect rc;
	::GetWindowRect(::GetDlgItem(m_hWnd, IDC_ANSIBAR_BK), rc);
	UINT id = mnu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RETURNCMD, rc.left, rc.bottom, this);
	if (id > 9)
	{
		id -= 10;
		bk = id;
		Invalidate(FALSE);
		::SetForegroundWindow(::GetParent(m_hWnd));
		::SetFocus(view->m_hWnd);
		UpdateColor(UC_BK);
	}
}

void CAnsiBar::OnFg()
{
	CMenu mnu;
	mnu.CreatePopupMenu();
	for (int i = 10;i < 26;i++)
	{
		mnu.AppendMenu(MF_OWNERDRAW, i, "");
	}
	CRect rc;
	::GetWindowRect(::GetDlgItem(m_hWnd, IDC_ANSIBAR_FG), rc);
	UINT id = mnu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RETURNCMD, rc.left, rc.bottom, this);
	if (id > 9)
	{
		id -= 10;
		fg = id;
		Invalidate(FALSE);
		::SetForegroundWindow(view->parent->m_hWnd);
		::SetFocus(view->m_hWnd);
		UpdateColor(UC_FG);
	}
}

void CAnsiBar::OnBlink()
{
	is_blink = !is_blink;
	::SetFocus(view->m_hWnd);
	UpdateColor(UC_BLINK);
}

void CAnsiBar::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);
	COLORREF clr, txtclr = 0;
	if (nIDCtl)	//Button
	{
		switch (nIDCtl)
		{
		case IDC_ANSIBAR_SENDDROP:
			dc.DrawFrameControl(&lpDrawItemStruct->rcItem, DFC_SCROLL, DFCS_SCROLLDOWN | (lpDrawItemStruct->itemState&ODS_SELECTED ? DFCS_PUSHED : 0));
			break;
		default:	//前景背景按鈕
			clr = AppConfig.colormap[(nIDCtl==IDC_ANSIBAR_FG?fg:bk)];
			dc.FillSolidRect(&lpDrawItemStruct->rcItem, clr);
			if (GetRValue(clr) <= 128 && GetGValue(clr) <= 128 && GetBValue(clr) <= 128)
				txtclr = 0xffffff;
			dc.SetTextColor(txtclr);
			char txt[5];
			dc.DrawEdge(&lpDrawItemStruct->rcItem, lpDrawItemStruct->itemState&ODS_SELECTED ? EDGE_SUNKEN : EDGE_RAISED, BF_RECT);
			::GetWindowText(lpDrawItemStruct->hwndItem, txt, 5);
			dc.DrawText(txt, 4, &lpDrawItemStruct->rcItem, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
		}
	}
	else	//Menu
	{
		clr = AppConfig.colormap[(lpDrawItemStruct->itemID-10)];
		dc.FillSolidRect(&lpDrawItemStruct->rcItem, clr);
		dc.DrawEdge(&lpDrawItemStruct->rcItem, lpDrawItemStruct->itemState&ODS_SELECTED ? EDGE_RAISED : 0, BF_RECT | BF_ADJUST);
	}
	dc.Detach();
}


void CAnsiBar::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	CDialogBar::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
	lpMeasureItemStruct->itemHeight = 16;
	lpMeasureItemStruct->itemWidth = 50;
}

void CAnsiBar::UpdateColor(UINT type)
{
	if (!view->telnet)
		return;

	if (! view->telnet->is_ansi_editor)
		return;

	BYTE attr = bk << 4;
	attr &= 127;	//0111,1111b=127b;
	if (fg > 7)
	{
		attr |= (fg - 8);
		attr |= 8;	//00001000b=8d
	}
	else
		attr |= fg;

	if (is_blink)
		attr |= 128;	//10000000b=128d

	CTelnetConn* telnet = view->telnet;
	telnet->cur_attr = attr;

	if (telnet->sel_start != telnet->sel_end)
	{
		UINT tmp;
		if (telnet->sel_end.x < telnet->sel_start.x)
		{
			tmp = telnet->sel_end.x;
			telnet->sel_end.x = telnet->sel_start.x;
			telnet->sel_start.x = tmp;
		}
		if (telnet->sel_end.y < telnet->sel_start.y)
		{
			tmp = telnet->sel_end.y;
			telnet->sel_end.y = telnet->sel_start.y;
			telnet->sel_start.y = tmp;
		}

		if (telnet->sel_block || telnet->sel_start.y == telnet->sel_end.y)	//如果使用區塊選取或只選取單行
		{
			for (int i = telnet->sel_start.y;i <= telnet->sel_end.y;i++)
				for (int p = telnet->sel_start.x;p < telnet->sel_end.x;p++)
					AddAttr(telnet->GetLineAttr(i)[p], type);
		}
		else	//不使用區塊並選取多行
		{
			int i, p;
			for (p = telnet->sel_start.x;p < telnet->site_settings.cols_per_page;p++)
				AddAttr(telnet->GetLineAttr(telnet->sel_start.y)[p], type);

			for (i = telnet->sel_start.y + 1;i < telnet->sel_end.y;i++)
			{
				for (p = 0;p < telnet->site_settings.cols_per_page;p++)
					AddAttr(telnet->GetLineAttr(i)[p], type);
			}

			for (p = 0;p < telnet->sel_end.x;p++)
				AddAttr(telnet->GetLineAttr(telnet->sel_end.y)[p], type);
		}
		CRect rc;
		rc.top = telnet->sel_start.y * view->lineh + view->top_margin;
		rc.bottom = (telnet->sel_end.y + 1) * view->lineh + view->top_margin;
		rc.left = view->left_margin;
		rc.right = view->chw * telnet->site_settings.cols_per_page + rc.left;
		view->InvalidateRect(rc, FALSE);
	}
}

void CAnsiBar::OnSend()
{
	Send(0);	//全部格式
}

void CAnsiBar::AddAttr(BYTE &attr, UINT type)
{
	switch (type)
	{
	case UC_FG:
		attr &= 240;	//1111,0000b=240d
		if (fg > 7)
		{
			attr |= (fg - 8);
			attr |= 8;
		}
		else
			attr |= fg;
		break;
	case UC_BK:
		BYTE _bk;
		_bk = (bk << 4) & 127;
		attr &= 143;	//1000,1111b=143d
		attr |= _bk;
		break;
	case UC_BLINK:
		if (is_blink)
			attr |= 128;
		else
			attr &= 127;

		break;
	}
}

void CAnsiBar::OnSendDropDown()
{
	CRect rc;
	::GetWindowRect(::GetDlgItem(m_hWnd, IDC_ANSIBAR_SEND), rc);
	CMenu _pop;
	_pop.LoadMenu(IDR_POPUP);
	CMenu* pop = _pop.GetSubMenu(2);

	UINT id = pop->TrackPopupMenu(TPM_LEFTALIGN | TPM_RETURNCMD, rc.left, rc.bottom, this);
	switch (id)
	{
	case ID_SEND_RESETCLR:	//送出色彩還原碼
		Send(4);
		break;
	case ID_SEND_FG:	//送出前景
		Send(1);
		break;
	case ID_SEND_BK:	//送出背景
		Send(2);
		break;
	}
	::SetForegroundWindow(view->parent->m_hWnd);
	view->SetFocus();
}

void CAnsiBar::Send(DWORD type)
{
	CString escstr = view->telnet->site_settings.esc_convert;
	escstr += '[';
	BYTE attr;
	char tmp[4];
	switch (type)
	{
	case 1:	//送出前景
		if (is_blink)
			escstr += "5;";
		attr = fg;
		if (attr > 7)
		{
			attr -= 8;
			escstr += "1;";
		}
		itoa(attr + 30, tmp, 10);
		strcat(tmp, "m");
		escstr += tmp;
		break;
	case 2:	//送出背景
		itoa(bk + 40, tmp, 10);
		strcat(tmp, "m");
		escstr += tmp;
		break;
	case 4:	//送出還原碼
		escstr += "m";
		break;
	default:
		attr = (bk << 4);
		attr &= 127;	//0111,1111b=127b;
		if (fg > 7)
		{
			attr |= (fg - 8);
			attr |= 8;	//00001000b=8d
		}
		else
			attr |= fg;

		if (is_blink)
			attr |= 128;	//10000000b=128d

		//01110111b=119d
		escstr += LPCTSTR(AttrToStr((~attr) & 119, attr)) + 2;
	}
	::SetForegroundWindow(view->parent->m_hWnd);
	view->SetFocus();
	view->telnet->Send(LPCTSTR(escstr), escstr.GetLength());
}

BOOL CAnsiBar::OnEraseBkgnd(CDC* pDC)
{
	// dirty hack used to draw background properly under XP & Vista. :-(
	// http://msdn2.microsoft.com/en-us/library/17750y97(VS.80).aspx
	HWND parent = ::GetParent(m_hWnd);
	CPoint pt(0, 0);
	::MapWindowPoints(m_hWnd, parent, &pt, 1);
	pt = pDC->OffsetWindowOrg(pt.x, pt.y);
	LRESULT ret = ::SendMessage(parent, WM_ERASEBKGND, (WPARAM)pDC->m_hDC, 0);
	pDC->SetWindowOrg(pt.x, pt.y);
	return ret;
}

void CAnsiBar::OnMove(int x, int y)
{
	// dirty hack used to draw background properly under XP & Vista. :-(
	// http://msdn2.microsoft.com/en-us/library/17750y97(VS.80).aspx
	Invalidate();
}


