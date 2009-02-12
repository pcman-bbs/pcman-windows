// TermView.cpp : implementation of the CTermView class
//
#include "stdafx.h"

#include "WinUtils.h"

#include "TermView.h"
#include "ListDlg.h"
#include "MainFrm.h"
#include "SetBkDlg.h"
#include "CustomizeDlg.h"
#include "StringDlg.h"
#include "MemIniFile.h"
#include "StrUtils.h"

#include "SitePage.h"
#include "AutoReplyPage.h"
#include "GeneralPage.h"
#include "OtherPage.h"
#include "HyperLinkPage.h"
#include "ConnectPage.h"

#include "Clipboard.h"
#include "InputNameDlg.h"
#include "SearchPlugin.h"
#include "MouseCTL.h"

#include "OleImage.h"

#if defined	_COMBO_
#include "../Combo/WebPageDlg.h"
#include "../Combo/WebConn.h"
#include "../Combo/WebCfgPage.h"
#endif

#include <wininet.h>
#include <afxtempl.h>

#if _MFC_VER >= 0x0700	// MFC 7.0 and 4.2 are not compatible
#include <atlimage.h>
#endif

#include <malloc.h>
#include <Imm.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFont fnt;
extern CUcs2Conv g_ucs2conv;

#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "Imm32.lib")

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#pragma warning(disable:4786)

/////////////////////////////////////////////////////////////////////////////
// CTermView

const UINT WM_FINDREPLACE = RegisterWindowMessage(FINDMSGSTRING);

inline void swap(long& v1, long& v2)		{	long t;	t = v1;	v1 = v2;	v2 = t;	}

BEGIN_MESSAGE_MAP(CTermView, CWnd)
	ON_MESSAGE(WM_SOCKET, OnSocket)
	//{{AFX_MSG_MAP(CTermView)
	ON_WM_ERASEBKGND()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_DESTROY()
	ON_WM_VSCROLL()
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_PASTEFILE, OnEditPastefile)
	ON_COMMAND(ID_RECONNECT, OnReconnect)
	ON_WM_CREATE()
	ON_COMMAND(ID_SETBK, OnSetBkgnd)
	ON_COMMAND(ID_RIGHTNODBCS, OnRightNoDBCS)
	ON_COMMAND(ID_LEFTNODBCS, OnLeftNoDBCS)
	ON_COMMAND(ID_BACKNODBCS, OnBackspaceNoDBCS)
	ON_COMMAND(ID_SELALL_BUF, OnSelAllBuf)
	ON_WM_DESTROYCLIPBOARD()
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_SMOOTH_DRAW, OnSmoothDraw)
	ON_COMMAND(ID_CURCON_SETTINGS, OnCurConSettings)
	ON_COMMAND(ID_UPDATE_BBSLIST, OnUpdateBBSList)
	ON_COMMAND(ID_DISCONNECT, OnDisConnect)
	ON_COMMAND(ID_ANSICOPY, OnAnsiCopy)
	ON_COMMAND(ID_ANSIEDITOR, OnAnsiEditor)
	ON_COMMAND(ID_ANSI_OPEN, OnAnsiOpen)
	ON_COMMAND(ID_ANSI_SAVE, OnAnsiSave)
	ON_COMMAND(ID_ANSI_SAVEAS, OnAnsiSaveAs)
	ON_COMMAND(ID_ANSI_CLS, OnAnsiCls)
	ON_COMMAND(ID_ANSI_INS, OnAnsiIns)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_IME_CHAR, OnImeChar)
	ON_MESSAGE(WM_IME_COMPOSITION, OnImeComposition)
	ON_MESSAGE(WM_INPUTLANGCHANGE, OnInputLangChange)
	ON_MESSAGE(WM_DNSLOOKUP_END, OnDNSLookupEnd)
	ON_WM_MOUSEWHEEL()
	ON_COMMAND_RANGE(ID_EDIT_OPENURL, ID_EDIT_OPENURL_FTP, OnEditOpenURL)
	ON_COMMAND_RANGE(ID_FIRST_HISTORY, ID_LAST_HISTORY, OnHistory)	//連線紀錄
	ON_REGISTERED_MESSAGE(WM_FINDREPLACE, OnFind)
	ON_COMMAND_RANGE(CSearchPluginCollection::ID_SEARCHPLUGIN00, CSearchPluginCollection::ID_SEARCHPLUGIN31, OnSearchPlugin)
	ON_COMMAND(CSearchPluginCollection::ID_TRANSLATION, OnTranslation)
END_MESSAGE_MAP()

CFont fnt;

/////////////////////////////////////////////////////////////////////////////
// CTermView construction/destruction


CPtrArray CTermView::all_telnet_conns;

CTermView::CTermView()
{
	CConn::view = this;

	blight = 0;
	parent = NULL;
	bk = NULL;
	doflash = 0;
	left_margin = 0;
	top_margin = 0;

	caret_vis = 0;
	lineh = 12;
	chw = 0;

	bk = NULL;
	memset(&font_info, 0, sizeof(font_info));
	draw_bmp = NULL;
	key_processed = 0;

	telnet = NULL;

	auto_switch = 0;
	blight = 0;

	holdobj = NULL;
	memdc = NULL;

	hand_cursor = NULL;
	paste_block = 0;

	pfinddlg = NULL;

	cp_id = ::GetACP();
	ime_prop = ImmGetProperty(GetKeyboardLayout(0), IGP_PROPERTY);
	os_ver_nt = ::GetVersion() < 0x80000000 ? TRUE : FALSE;

#if defined _COMBO_
	con = NULL;
	autosort_favorite = 0;
#endif
}

CTermView::~CTermView()
{
#if defined	_COMBO_
//	CloseHandle(lock);
#endif
	if (memdc)
	{
		DeleteDC(memdc);
		SelectObject(memdc, holdobj);
	}
	if (bk)
		DeleteObject(bk);
	telnet = NULL;
}

LPSTR CTermView::ctviewcls = "BBS_View";

BOOL CTermView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_VSCROLL;
	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	RegWndClass(ctviewcls, AfxGetAfxWndProc(), NULL, NULL, AfxGetApp()->LoadStandardCursor(IDC_ARROW), CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS);
	cs.lpszClass = ctviewcls;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CTermView drawing

void CTermView::OnInitialUpdate()
{
	SCROLLINFO info;
	GetScrollInfo(SB_VERT, &info);
	info.nMin = 0;
	info.nPos = info.nMax = 0;
	info.fMask = SIF_RANGE | SIF_POS | SIF_DISABLENOSCROLL;
	SetScrollInfo(SB_VERT, &info);

	SetTimer(ID_MAINTIMER, 1000, NULL);

	if (!parent->LoadUI())
	{
		MessageBox(LoadString(IDS_IMPORTANT_FILE_LOST), NULL, MB_ICONSTOP | MB_OK);
		return;
	}

	if (!AppConfig.auto_font)	//如果不使用動態字體調整
	{
		fnt.DeleteObject();
		fnt.CreateFontIndirect(&AppConfig.font_info);
		CWindowDC dc(this);
		CGdiObject* old = dc.SelectObject(&fnt);
		CSize& sz = dc.GetTextExtent(LoadString(IDS_DOUBLE_SPACE_CHAR), 2);
		dc.SelectObject(&old);
		chw = sz.cx / 2;
		lineh = sz.cy;
	}

	UpdateBkgnd();

	SetScrollBar();
}

/////////////////////////////////////////////////////////////////////////////
// CTermView message handlers

BOOL CTermView::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

UINT CTermView::SetDCColors(CDC *dc, BYTE color, BOOL invirt)	//傳回 draw_opt: ETO_OPAQUE or 0
{
	BYTE fg = GetAttrFgColor(color), bk = GetAttrBkColor(color);

	if (bk > 7)
		bk = 0;

	COLORREF fgc = AppConfig.colormap[fg];
	COLORREF bkc = AppConfig.colormap[bk];
	if (invirt)
	{
		fgc = 0x00ffffff & ~fgc;
		bkc = 0x00ffffff & ~bkc;
	}

	dc->SetTextColor(fgc);
	if (bk == 0 && AppConfig.bktype)
	{
		dc->SetBkMode(TRANSPARENT);
		return 0;
	}
	else
	{
		dc->SetBkMode(OPAQUE);
		dc->SetBkColor(bkc);
		return ETO_OPAQUE;
	}
}

void CTermView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (!telnet)
		return;

	if (key_processed)
		return;

	BYTE ch;
	ch = (BYTE)nChar;
	if (telnet->site_settings.localecho)
		telnet->LocalEcho(&ch, 1);
	telnet->Send(&ch, 1);
}


void CTermView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	DBG_PRINT("OnKeyDown(0x%X('%c'), 0x08%X(Scan:0x%02X('%c') Ext:%d)\r\n",
			  nChar, nChar, nFlags, nFlags&0xFF, nFlags&0xFF, (nFlags >> 8)&0x1);

	if (!telnet)
	{
		if (nChar == VK_RETURN && AppConfig.nocon_enter_reconnect)
			OnHistory(ID_FIRST_HISTORY);
		return;
	}

	const char* keystr = NULL;
	if (telnet->key_map)
		keystr = telnet->key_map->FindKey(nChar, nFlags);

	char dbcmd[32];
	LPSTR curstr = telnet->screen[telnet->cursor_pos.y];

	if (!telnet->is_ansi_editor)
	{
		if (!keystr)
		{
			key_processed = false;
			return;
		}
		key_processed = true;

		UINT l = strlen(keystr);
		switch (nChar)
		{
		case VK_LEFT:
			if (telnet->site_settings.auto_dbcs_arrow && telnet->cursor_pos.x > 1 && IsBig5(curstr, telnet->cursor_pos.x - 2))
			{
				strcpy(dbcmd, keystr);
				strcpy(dbcmd + l, keystr);
				telnet->Send(dbcmd, l*2);
			}
			else
				telnet->Send(keystr, l);
			break;
		case VK_RIGHT:
			if (telnet->site_settings.auto_dbcs_arrow && IsBig5(curstr, telnet->cursor_pos.x))
			{
				strcpy(dbcmd, keystr);
				strcpy(dbcmd + l, keystr);
				telnet->Send(dbcmd, l*2);
			}
			else
				telnet->Send(keystr, l);
			break;
		case VK_BACK:
			if (telnet->cursor_pos.x > 1 && telnet->site_settings.auto_dbcs_backspace	&& IsBig5(curstr, telnet->cursor_pos.x - 2))
			{
				strcpy(dbcmd, keystr);
				strcpy(dbcmd + l, keystr);
				telnet->Send(dbcmd, l*2);
				if (telnet->site_settings.localecho)
					telnet->Back(2);
			}
			else
			{
				if (telnet->site_settings.localecho)
					telnet->Back();
				telnet->Send(keystr, l);
			}
			break;
		case VK_DELETE:
			if (telnet->site_settings.auto_dbcs_del && IsBig5(curstr, telnet->cursor_pos.x))
			{
				strcpy(dbcmd, keystr);
				strcpy(dbcmd + l, keystr);
				telnet->Send(dbcmd, l*2);
				if (telnet->site_settings.localecho)
					telnet->Delete(2);
			}
			else
			{
				if (telnet->site_settings.localecho)
					telnet->Delete(1);
				telnet->Send(keystr, l);
			}
			break;
		case VK_RETURN:
			if (telnet->is_disconnected && AppConfig.enter_reconnect)
				ReConnect(telnet);
			else
			{
				if (telnet->site_settings.localecho)
					telnet->LocalEcho("\r\n", 2);
				telnet->Send(keystr, l);
			}
			break;
		default:
			telnet->Send(keystr, l);
		}
	}
	else
	{
		key_processed = true;
		BOOL update = 0;
		switch (nChar)
		{
		case VK_UP:
			telnet->GoUp(1);
			break;
		case VK_DOWN:
			telnet->GoDown(1);
			break;
		case VK_LEFT:
			if (telnet->site_settings.auto_dbcs_arrow && telnet->cursor_pos.x > 1 && IsBig5(curstr, telnet->cursor_pos.x - 2))
				telnet->GoLeft(2);
			else
				telnet->GoLeft(1);
			break;
		case VK_RIGHT:
			if (telnet->site_settings.auto_dbcs_arrow && IsBig5(curstr, telnet->cursor_pos.x))
				telnet->GoRight(2);
			else
				telnet->GoRight(1);
			break;
		case VK_TAB:
			telnet->Send("    ", 4);
			update = TRUE;
			break;
		case VK_PRIOR:	//page up
			OnVScroll(SB_PAGEUP, 0, NULL);
			break;
		case VK_NEXT:	//page down
			OnVScroll(SB_PAGEDOWN, 0, NULL);
			break;
		case VK_HOME:
			telnet->Home();
			break;
		case VK_END:
			telnet->End();
			break;
		case VK_INSERT:
			OnAnsiIns();
			break;
		case VK_DELETE:
			if (telnet->site_settings.auto_dbcs_del && IsBig5(curstr, telnet->cursor_pos.x))
				telnet->Delete(2);
			else
				telnet->Delete();
			update = TRUE;
			break;
		case VK_BACK:
			if (telnet->cursor_pos.x > 1 && telnet->site_settings.auto_dbcs_backspace	&& IsBig5(curstr, telnet->cursor_pos.x - 2))
				telnet->Back(2);
			else
				telnet->Back();
			update = TRUE;
			break;
		default:
			key_processed = 0;
		}

		if (update)
			telnet->UpdateLine(telnet->cursor_pos.y);
	}
}


bool find_link(char* type, char* str, int& start, int& end)
{
	char *ppos = strstr(str, type);
	if (!ppos)
		return 0;
	int pos = ppos - str;
	start = pos;
	end = pos;
	while (str[end] > 32 && str[end] < 127 && str[end] != '>')
		end++;

	return 1;
}


void CTermView::OnLButtonDblClk(UINT nFlags, CPoint point_In)
{
	if (!telnet || !AppConfig.dblclk_select)
		return;

	CPoint point;

	point = point_In;

	int x, y;
	PtToLineCol(point, x, y, false);
	y += telnet->scroll_pos;

	if (telnet->sel_end.x != telnet->sel_start.x || telnet->sel_end.y != telnet->sel_start.y)
	{
		telnet->sel_end.x = telnet->sel_start.x;
		telnet->sel_end.y = telnet->sel_start.y;
		Invalidate(FALSE);
	}

	telnet->sel_start.x = x;
	telnet->sel_start.y = y;
	telnet->sel_end.x = x;
	telnet->sel_end.y = y;
	LPSTR curstr = telnet->screen[y];
	int tmpx;
	if (IsBig5(curstr, x) || IsBig5(curstr, x - 1))
	{
		if (IsBig5(curstr, x - 1)) x--;
		tmpx = x;
		while (tmpx >= 0)
		{
			if (IsBig5(curstr, tmpx))
			{
				telnet->sel_start.x = tmpx;
				tmpx -= 2;
			}
			else
				break;
		}
		tmpx = x;
		while (tmpx < telnet->site_settings.cols_per_page)
		{
			if (IsBig5(curstr, tmpx))
			{
				telnet->sel_end.x = tmpx + 2;
				tmpx += 2;
			}
			else
				break;
		}
	}
	else
	{
		tmpx = x;
		while (tmpx >= 0)
		{
			if (((curstr[tmpx] >= '0' && curstr[tmpx] <= '9') ||
				 (curstr[tmpx] >= 'A' && curstr[tmpx] <= 'Z') ||
				 (curstr[tmpx] >= 'a' && curstr[tmpx] <= 'z')) && !IsBig5(curstr, tmpx - 1))
			{
				telnet->sel_start.x = tmpx;
				tmpx--;
			}
			else
				break;
		}
		tmpx = x;
		while (tmpx < telnet->site_settings.cols_per_page)
		{
			if ((curstr[tmpx] >= '0' && curstr[tmpx] <= '9') ||
				(curstr[tmpx] >= 'A' && curstr[tmpx] <= 'Z') ||
				(curstr[tmpx] >= 'a' && curstr[tmpx] <= 'z'))
			{
				telnet->sel_end.x = tmpx + 1;
				tmpx++;
			}
			else
				break;
		}
	}
	Invalidate(FALSE);

	if (CanUseMouseCTL())
		MouseCTL_OnLButtonDblClk(m_hWnd, nFlags, point_In);

}


void CTermView::OnLButtonDown(UINT nFlags, CPoint point_In)
{
	SetFocus();
	if (!telnet)
		return;

	CPoint point;

	point = point_In;
	/*
	//測試用---------
		CWindowDC dc(this);
		DWORD tc=GetTickCount();
		for(int i=0;i<100;i++)
		{
			OldDrawScreen(dc);
		}
		tc=GetTickCount()-tc;
		char result[10];
		ltoa(tc,result,10);
		MessageBox(result);
	//---------------
	*/

	SetCapture();

	int x, y;
	PtToLineCol(point, x, y);
	y += telnet->scroll_pos;
	mouse_sel_timer = SetTimer(ID_MOUSE_SEL_TIMER, 100, NULL);
	telnet->sel_block = !!(nFlags & MK_SHIFT);

	if (telnet->sel_end.x != telnet->sel_start.x || telnet->sel_end.y != telnet->sel_start.y)
	{
		telnet->sel_end.x = telnet->sel_start.x;
		telnet->sel_end.y = telnet->sel_start.y;
		Invalidate(FALSE);
	}

	telnet->sel_start.x = x;
	telnet->sel_start.y = y;
	telnet->sel_end.x = x;
	telnet->sel_end.y = y;

	if (telnet->is_ansi_editor)
	{
		telnet->cursor_pos.x = x;
		telnet->cursor_pos.y = y;
		telnet->UpdateCursorPos();
	}

	if (CanUseMouseCTL())
		MouseCTL_OnLButtonDown(m_hWnd, nFlags, point_In);

}

void CTermView::OnDestroy()
{
	KillTimer(ID_MAINTIMER);
	KillTimer(ID_MOVIETIMER);
	DestroyCaret();
	parent->ansi_bar.DestroyWindow();
	CWnd::OnDestroy();
}

void CTermView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (!telnet)
		return;
	SCROLLINFO info;
	GetScrollInfo(SB_VERT, &info);
	int oldpos = info.nPos;
	info.fMask = SIF_POS;
	switch (nSBCode)
	{
	case SB_LINEUP:
		if (info.nPos == 0)
			return;
		info.nPos--;
		break;
	case SB_LINEDOWN:
		if (info.nPos >= (int)(info.nMax - info.nPage))
			return;
		info.nPos++;
		break;
	case SB_PAGEUP:
		info.nPos -= telnet->site_settings.lines_per_page;
		if (info.nPos < 0)
			info.nPos = 0;
		break;
	case SB_PAGEDOWN:
		info.nPos += telnet->site_settings.lines_per_page;
		if (info.nPos > (int)(info.nMax - info.nPage))
			info.nPos = info.nMax - info.nPage;
		break;
	case SB_THUMBTRACK:
		info.nPos = nPos;
		if (info.nPos > (int)(info.nMax - info.nPage))
			info.nPos = info.nMax - info.nPage;
		break;
	}
	if (info.nPos == oldpos)
		return;

	SetScrollInfo(SB_VERT, &info);
	telnet->scroll_pos = info.nPos;
	if (telnet->is_ansi_editor)
		telnet->UpdateCursorPos();
	Invalidate(FALSE);
}

void CTermView::OnTimer(UINT nIDEvent)
{
	if (CanUseMouseCTL())
		MouseCTL_OnTimer(m_hWnd, nIDEvent);

	if (nIDEvent == ID_MAINTIMER)
	{
		if (AppConfig.flash_window && doflash)
			parent->FlashWindow(TRUE);

		if (0 == all_telnet_conns.GetSize())
			return;

#ifdef	_COMBO_
		if (con && con->is_connected)
#else
		if (telnet && telnet->is_connected)
#endif
			parent->UpdateStatus();

		blight = !blight;
		if (telnet && telnet->sel_start == telnet->sel_end)
			DrawBlink();

		CTelnetConn** pitem = (CTelnetConn**)all_telnet_conns.GetData();
		CTelnetConn** plast_item = pitem + all_telnet_conns.GetSize();
		for (; pitem < plast_item ; pitem++)
		{
			CTelnetConn* item = *pitem;
			if (item->is_connected)
			{
				item->time++;
				item->idle_time++;
				if (!(item->idle_time % item->site_settings.idle_interval) && item->site_settings.prevent_idle)
				{
					CString idlestr = UnescapeControlChars(item->site_settings.idle_str);
					item->Send((LPCTSTR)idlestr, idlestr.GetLength());
				}

				//Delay Send
				POSITION pos = item->delay_send.GetHeadPosition();
				while (pos)
				{
					CTelnetConnDelayedSend& ds = item->delay_send.GetAt(pos);
					if (ds.time > 1)
					{
						ds.time--;
						item->delay_send.GetNext(pos);
					}
					else
					{
						CString dsstr = ds.str;
						POSITION _pos = pos;
						item->delay_send.GetNext(pos);
						item->delay_send.RemoveAt(_pos);
						item->SendMacroString(dsstr);
					}
				}
			}
			else if (item->is_disconnected)
			{
				//如果設定自動重連，而且在時間內被斷線，且間隔時間已到
				if (item->site_settings.auto_reconnect)
				{
					if (item->time <= item->site_settings.connect_interval)
					{
						if (item->site_settings.reconnect_interval > 0)
							item->site_settings.reconnect_interval--;
						else
							ReConnect(item);	//重新連線
					}
				}
			}
		}
	}
	else if (nIDEvent == ID_MOVIETIMER && telnet && telnet->is_connected)
	{
		CString txt = telnet->screen[telnet->last_line];
		if (telnet->IsEndOfArticleReached())
			KillTimer(ID_MOVIETIMER);
		else
			telnet->SendString(" ");
	}
}


void CTermView::OnLButtonUp(UINT nFlags, CPoint point_In)
{
	DWORD dw1;

	CPoint point;

	point = point_In;

	if (CanUseMouseCTL())
	{
		dw1 = MouseCTL_OnLButtonUp_PreProcess(m_hWnd, nFlags, point_In);
		if (dw1 == FALSE)
			return ;
	}

	ReleaseCapture();


	::KillTimer(NULL, mouse_sel_timer);
	if (!telnet)
		return;
//	----------如果有選取區，考慮是否有自動複製------------
	if (telnet->sel_start.x != telnet->sel_end.x || telnet->sel_start.y != telnet->sel_end.y)
	{
		if (AppConfig.auto_copy)
			CopySelText();
	}
	else
	{
//	----------如果不是在選取文字，就處理超連結--------------
		int x, y;
		PtToLineCol(point, x, y, false);	//換算成終端機螢幕座標
		int l;	char* url;
		if ((url = HyperLinkHitTest(x, y, l)))	//如果滑鼠點選到超連結
		{
			char tmp;	tmp = url[l];	url[l] = 0;
			//	呼叫程式開啟超連結
			AppConfig.hyper_links.OpenURL(url);
			url[l] = tmp;
		}
		else
		{
			if (CanUseMouseCTL())
				MouseCTL_OnLButtonUp(m_hWnd, nFlags, point_In);
		}
//	-------------------------------------------------
	}

	if (CanUseMouseCTL())
		MouseCTL_OnLButtonUp_PostProcess(m_hWnd, nFlags, point_In);
}

void CTermView::OnMouseMove(UINT nFlags, CPoint point_In)
{
	if (!telnet)
		return;

	CPoint point;
	int lx, ly;

	point = point_In;

	PtToLineCol(point, lx, ly, false);

	BOOL bsel = (nFlags & MK_LBUTTON && ::GetCapture() == m_hWnd);		//是否正在選取?
	point.x -= left_margin;	point.y -= top_margin;
	if (point.x < 0)	point.x = 0;

	int cx = 0;	int cy = (point.y / lineh);
	if (cy > telnet->site_settings.lines_per_page - 1)
	{
		cy = telnet->site_settings.lines_per_page - 1;
		if (bsel)
			OnVScroll(SB_LINEDOWN, 0, NULL);	//跨頁選取，捲動
	}
	if (point.y < 0)
	{
		cy = 0;
		if (bsel)
			OnVScroll(SB_LINEUP, 0, NULL);	//跨頁選取，捲動
	}

	int y2 = telnet->scroll_pos + cy;
	LPSTR curstr = telnet->screen[y2];
	LPSTR tmpstr = curstr;

	cx = (point.x / chw);
	if (telnet->site_settings.auto_dbcs_mouse)
	{
		//-----------新的支援中文的座標計算-----------
		if (cx > 0 && IsBig5(tmpstr, cx - 1))	//如果選擇中文後半段，就選取下一個字
			cx++;
		else if (!IsBig5(tmpstr, cx) && (point.x % chw)*2 > chw)	//如果也不是中文前半，才是英文
			cx++;
	}
	else
	{
		//------------不考慮雙位元組的座標計算-----------
		if ((point.x % chw)*2 > chw)
			cx++;
	}

	if (cx > telnet->site_settings.cols_per_page)
		cx = telnet->site_settings.cols_per_page;

	if (point.x < chw)
		cx = 0;

	if (bsel)
	{
		blight = 1;
		CRect urc;
		GetClientRect(urc);
		urc.left = left_margin;
		int selendy = telnet->sel_end.y - telnet->scroll_pos;
		int selstarty = telnet->sel_start.y - telnet->scroll_pos;

		if (telnet->sel_block)
		{
			int selstartx, selendx;
			if (telnet->sel_end.x > telnet->sel_start.x)
				selstartx = telnet->sel_start.x, selendx = telnet->sel_end.x;
			else
				selstartx = telnet->sel_end.x, selendx = telnet->sel_start.x;

			if (selendy < selstarty)
			{
				int t = selendy;
				selendy = selstarty;
				selstarty = t;
			}
			urc.top = (selstarty < cy ? selstarty : cy) * lineh + top_margin;
			urc.bottom = (selendy > cy ? selendy : cy) * lineh + lineh + top_margin;
			urc.right = ((selendx > cx ? selendx : cx) + 1) * chw + left_margin;
			urc.left = (selstartx < cx ? selstartx : cx) * chw + left_margin;
		}
		else
		{
			if (cy > selendy)
			{
				urc.top = selendy * lineh + top_margin;
				urc.bottom = cy * lineh + top_margin;
				InvalidateRect(urc, FALSE);
				urc.right = chw * cx + left_margin;
				urc.bottom += lineh + 2;
			}
			else if (cy < selendy)
			{
				urc.top = cy * lineh + top_margin;
				urc.bottom = selendy * lineh + lineh + 2 + top_margin;
				InvalidateRect(urc, FALSE);
				urc.left += cx * chw;
			}
			else
			{
				urc.top = cy * lineh + top_margin;
				urc.bottom = urc.top + lineh + 2;
				urc.left = telnet->sel_end.x * chw + left_margin;
				urc.right = cx * chw + left_margin;
				if (urc.left > urc.right)
				{
					int tmp = urc.left;
					urc.left = urc.right;
					urc.right = tmp;
				}
			}
		}
		telnet->sel_end.x = cx;
		telnet->sel_end.y = y2;
		InvalidateRect(urc, FALSE);

		if (telnet->is_ansi_editor)
		{
			telnet->cursor_pos.x = cx;
			telnet->cursor_pos.y = y2;
			telnet->UpdateCursorPos();
		}
	}
	else
	{
		int len;
		if (HyperLinkHitTest(lx, ly, len))
		{
			if (MouseCTL_GetCurrentMouseCursor() == NULL)
				SetCursor(hand_cursor);
		}
	}

	if (CanUseMouseCTL())
		MouseCTL_OnMouseMove(m_hWnd, nFlags, point_In);
}

void CTermView::OnContextMenu(CWnd* pWnd, CPoint point)
{
//	超連結選單
	/*	char text[32];
		MENUITEMINFO inf;
		inf.cbSize=sizeof(inf);
		inf.fMask=MIIM_TYPE|MIIM_SUBMENU|MIIM_ID;
		inf.dwTypeData=text;
		inf.cch=sizeof(text);
		GetMenuItemInfo(edit,4,TRUE,&inf);
		HMENU sub=inf.hSubMenu;	UINT wID=inf.wID;
	*/

	CString sel;
	try
	{
		sel = GetSelText();
	}
	catch (...)
	{
		sel = _T("");
	}

	if (sel.GetLength() > 0)
	{
		MENUITEMINFO search_menuiteminfo = { sizeof(MENUITEMINFO) };
		search_menuiteminfo.fMask =  MIIM_ID | MIIM_DATA | MIIM_TYPE | MIIM_SUBMENU;
		search_menuiteminfo.wID = CSearchPluginCollection::ID_SEARCHPLUGIN_MENU;
		search_menuiteminfo.hSubMenu = SearchPluginCollection.CreateSearchMenu();
		CString web_search;
		web_search.LoadString(IDS_WEB_SEARCH);
		search_menuiteminfo.dwTypeData = (LPTSTR)LPCTSTR(web_search);
		InsertMenuItem(parent->edit_menu, 5, TRUE, &search_menuiteminfo);

		SearchPluginCollection.MaxTranLength = AppConfig.max_translation_length;
		if (sel.GetLength() <= SearchPluginCollection.MaxTranLength)
		{
			MENUITEMINFO tran_menuiteminfo = { sizeof(MENUITEMINFO) };
			tran_menuiteminfo.fMask =  MIIM_ID | MIIM_DATA | MIIM_TYPE | MIIM_SUBMENU;
			tran_menuiteminfo.wID = CSearchPluginCollection::ID_SEARCHPLUGIN_MENU;
			tran_menuiteminfo.hSubMenu = SearchPluginCollection.CreateTranMenu(sel);
			CString web_search;
			web_search.LoadString(IDS_TRANSLATION);
			tran_menuiteminfo.dwTypeData = (LPTSTR)LPCTSTR(web_search);
			InsertMenuItem(parent->edit_menu, 6, TRUE, &tran_menuiteminfo);
		}
	}

	char* link = NULL;
	int len;
	if (telnet)
	{
		int x, y;
		CPoint pt = point;
		ScreenToClient(&pt);
		PtToLineCol(pt, x, y, false);

		if ((link = HyperLinkHitTest(x, y, len)))	//如果偵測到是超連結
		{
			InsertMenu(parent->edit_menu, 0, MF_SEPARATOR | MF_BYPOSITION, 0, 0);
			InsertMenu(parent->edit_menu, 0, MF_STRING | MF_BYPOSITION, ID_EDIT_COPYURL, LoadString(IDS_COPY_URL));
		}
	}

	int c = GetMenuItemCount(parent->edit_menu);
	AppendMenu(parent->edit_menu, MF_SEPARATOR, 0, NULL);
	HMENU config = GetSubMenu(parent->main_menu, 2);
	char str[40];
	GetMenuString(config, ID_VIEW_FULLSCR, str, sizeof(str), MF_BYCOMMAND);
	AppendMenu(parent->edit_menu, MF_STRING, ID_VIEW_FULLSCR, str);

	UINT cmd =::TrackPopupMenu(parent->edit_menu, TPM_RETURNCMD | TPM_LEFTALIGN | TPM_RIGHTBUTTON,
							   point.x, point.y, 0, parent->m_hWnd, NULL);

	RemoveMenu(parent->edit_menu, c, MF_BYPOSITION);
	RemoveMenu(parent->edit_menu, c, MF_BYPOSITION);
	if (link)
	{
		RemoveMenu(parent->edit_menu, 0, MF_BYPOSITION);
		RemoveMenu(parent->edit_menu, 0, MF_BYPOSITION);
		if (cmd == ID_EDIT_COPYURL)
		{
			CClipboard::SetText(m_hWnd, link, len);
			return;
		}
	}

	if (sel.GetLength() > 0)
	{
		DeleteMenu(parent->edit_menu, 5, MF_BYPOSITION);
		if (sel.GetLength() <= SearchPluginCollection.MaxTranLength)
		{
			DeleteMenu(parent->edit_menu, 5, MF_BYPOSITION);
		}
	}
	if (cmd > 0)
		AfxGetMainWnd()->SendMessage(WM_COMMAND, cmd, 0);
}


void CTermView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	if (AppConfig.smooth_draw)
	{
		drawdc.CreateCompatibleDC(&dc);
		GetClientRect(view_rect);
		draw_bmp =::CreateCompatibleBitmap(dc.m_hDC, view_rect.right, view_rect.bottom);
		HGDIOBJ h_old = SelectObject(drawdc.m_hDC, draw_bmp);
		int x = dc.m_ps.rcPaint.left , y = dc.m_ps.rcPaint.top;
		DrawScreen(drawdc);
		dc.BitBlt(x, y, dc.m_ps.rcPaint.right - x, dc.m_ps.rcPaint.bottom - y, &drawdc, x, y, SRCCOPY);
//		dc.BitBlt(x,y,view_rect.right,view_rect.bottom,&drawdc,x,y,SRCCOPY);
		SelectObject(drawdc.m_hDC, h_old);
		DeleteObject(draw_bmp);
		drawdc.DeleteDC();
		return;
	}
	DrawScreen(dc);
}

void CTermView::OnSize(UINT nType, int cx, int cy)
{
	if (AppConfig.bktype > 2)
		UpdateBkgnd();
	AdjustFont(cx, cy);
}

void CTermView::SetScrollBar()
{
	if (!telnet)
	{
		ShowScrollBar(SB_VERT, FALSE);
		EnableScrollBar(SB_VERT, ESB_DISABLE_BOTH);
		return;
	}

	SCROLLINFO info;
	GetScrollInfo(SB_VERT, &info);
	info.nMin = 0;
	info.nMax = telnet->site_settings.line_count;
	info.nPos = telnet->scroll_pos;
	info.nPage = telnet->site_settings.lines_per_page;
	info.fMask = SIF_RANGE | SIF_POS | SIF_PAGE;
	SetScrollInfo(SB_VERT, &info);
	ShowScrollBar(SB_VERT, telnet->site_settings.showscroll);

	if (telnet->site_settings.line_count <= telnet->site_settings.lines_per_page)
		EnableScrollBar(SB_VERT, ESB_DISABLE_BOTH);
	else
		EnableScrollBar(SB_VERT, ESB_ENABLE_BOTH);
}

void CTermView::OnDisConnect()
{
#if defined	_COMBO_
	if (!con)
		return;

	if (!telnet)
	{
		((CWebConn*)con)->web_browser.wb_ctrl.Stop();
		return;
	}
#else
	if (!telnet)
		return;
#endif

	if (telnet->is_connecting || telnet->is_lookup_host)
	{
		parent->OnConnectClose();
		return;
	}

	if (!telnet->is_ansi_editor)
	{
		telnet->Shutdown();
		telnet->Close();
		telnet->ClearAllFlags();
		telnet->is_disconnected = true;
		telnet->site_settings.auto_reconnect = 0;

		int idx = 0;
		TCITEM tcitem;
		tcitem.mask = TCIF_IMAGE;
		tcitem.iImage = 1;
		idx = parent->ConnToIndex(telnet);
		parent->tab.SetItem(idx, &tcitem);
	}
}

void CTermView::OnSetFocus(CWnd* pOldWnd)
{
	if (CanUseMouseCTL())
		MouseCTL_OnSetFocus(m_hWnd);

	CWnd::OnSetFocus(pOldWnd);
	CreateCaret();
	ShowCaret();
	if (telnet)
		telnet->UpdateCursorPos();
	else
		SetCaretPos(CPoint(left_margin, lineh + top_margin - 2));
}

void CTermView::OnKillFocus(CWnd* pNewWnd)
{
	DestroyCaret();
	caret_vis = FALSE;
	CWnd::OnKillFocus(pNewWnd);
}


LRESULT CTermView::OnImeChar(WPARAM wparam, LPARAM lparam)
{
	if (!telnet)
		return 0;

	WORD db = (WORD)wparam;
	BYTE ch[] = "  ";
	ch[0] = (BYTE) HIBYTE(db);
	ch[1] = (BYTE) LOBYTE(db);

	if (!ch[0])
	{
		if (telnet->site_settings.localecho)
			telnet->LocalEcho(ch + 1, 1);
		telnet->Send(ch + 1, 1);
	}
	else
	{
		if (telnet->site_settings.localecho)
			telnet->LocalEcho(ch, 2);
		switch (telnet->site_settings.text_input_conv)	// Encoding Conversion
		{
		case BIG52GB:
			chi_conv.Big52GB((const char*)ch, (char*)ch, 2);
			break;
		case GB2BIG5:
			chi_conv.GB2Big5((const char*)ch, (char*)ch, 2);
		}
		telnet->Send(ch, 2);
	}
	return 0;
}

LRESULT CTermView::OnInputLangChange(WPARAM wparam, LPARAM lparam)
{
	if (!telnet)
		return 0;

	ime_prop = ImmGetProperty((HKL)lparam, IGP_PROPERTY);
	return 0;
}

LRESULT CTermView::OnImeComposition(WPARAM wparam, LPARAM lparam)
{
	if (lparam & GCS_RESULTSTR)
	{
		if (!telnet)
			return 0;

		if (ime_prop & IME_PROP_UNICODE)
			return _OnImeCompositionW(wparam, lparam);
		else
			return _OnImeCompositionA(wparam, lparam);
	}

	else
	{
		return DefWindowProc(WM_IME_COMPOSITION, wparam, lparam);
	}
}

LRESULT CTermView::_OnImeCompositionW(WPARAM wparam, LPARAM lparam)
{
	HIMC hIMC;
	if (!(hIMC = ImmGetContext(GetSafeHwnd())))
		return 0;

	DWORD size =
		ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, NULL, 0) + (sizeof(wchar_t));

	if (size < 1 || size > 1024)
		return 0;

	wchar_t* pwcsData = NULL;
	pwcsData = (wchar_t*)_alloca(size);
	memset(pwcsData, 0, size);

	unsigned char* pszData = NULL;
	pszData = (unsigned char*)_alloca(size);
	memset(pszData, 0, size);

	if (pwcsData && pszData)
	{
		ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, pwcsData, size);
//			ImmSetCompositionStringW(hIMC, SCS_SETSTR, L"", NULL, L"", NULL);

		if (*pwcsData)
		{
			UINT cp_id = GetCodePage();
			if (cp_id == 950)
			{
				g_ucs2conv.Ucs22Big5(pwcsData, (char*)pszData);
			}
			else
			{
				::WideCharToMultiByte(cp_id, 0, pwcsData, -1,
									  (char*)pszData, size, NULL, NULL);
			}

			for (; *pszData; ++pszData)
			{
				UINT in = 0;
				unsigned char ch1 = BYTE(*pszData);
				if (ch1 > 0x7e)
				{
					in = ch1 << 8 | (*(pszData + 1));
					++pszData;
				}
				else
				{
					in = ch1;
				}
				this->OnImeChar(in, 0);
			}
		}
	}
	ImmReleaseContext(GetSafeHwnd(), hIMC);
	return 0;
}

LRESULT CTermView::_OnImeCompositionA(WPARAM wparam, LPARAM lparam)
{
	HIMC hIMC;
	if (!(hIMC = ImmGetContext(GetSafeHwnd())))
		return 0;

	DWORD size =
		ImmGetCompositionStringA(hIMC, GCS_RESULTSTR, NULL, 0) + (sizeof(char));

	if (size < 1 || size > 1024)
		return 0;

	unsigned char* pszData = NULL;
	pszData = (unsigned char*)_alloca(size);
	memset(pszData, 0, size);

	if (pszData)
	{
		ImmGetCompositionStringA(hIMC, GCS_RESULTSTR, pszData, size);
//			ImmSetCompositionStringA(hIMC, SCS_SETSTR, "", NULL, "", NULL);

		if (*pszData)
		{
			for (; *pszData; ++pszData)
			{
				UINT in = 0;
				unsigned char ch1 = BYTE(*pszData);
				if (ch1 > 0x7e)
				{
					in = ch1 << 8 | (*(pszData + 1));
					++pszData;
				}
				else
				{
					in = ch1;
				}
				this->OnImeChar(in, 0);
			}
		}
	}
	ImmReleaseContext(GetSafeHwnd(), hIMC);
	return 0;
}

BOOL CTermView::Connect(CString address, CString name, unsigned short port, LPCTSTR cfg_path)
{
	if (name.IsEmpty())
		return FALSE;
#if defined	_COMBO_
	if (port > 0 && address.Find("telnet://") == -1)
		address = "telnet://" + address;
#endif

	CConn* ncon = NewConn(address, name, port, cfg_path);	//產生了新的連線畫面，完成所有設定
	if (!ncon)
		return FALSE;

	parent->SwitchToConn(ncon);
	if ((ncon->is_ansi_editor))
		return TRUE;

//開始連接socket
	ConnectSocket((CTelnetConn*)ncon);
	return TRUE;
}

LRESULT CTermView::OnDNSLookupEnd(WPARAM found, LPARAM lparam)
{
	DNSLookupData* data = (DNSLookupData*)lparam;
	CTelnetConn* new_telnet = data->new_telnet;
	SOCKADDR_IN& sockaddr = data->sockaddr;
//設定好相關資訊
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons((u_short)new_telnet->port);
//	sockaddr.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;

	if (found)
	{
		if (new_telnet->is_cancelled)
			delete new_telnet;
		else
		{
			new_telnet->ClearAllFlags();
			new_telnet->is_connecting = true;
			new_telnet->Connect((SOCKADDR*)&sockaddr, sizeof(SOCKADDR_IN));
		}
	}
	else
	{
		if (new_telnet->is_cancelled)
			delete new_telnet;
		else
			new_telnet->OnConnect(WSAEADDRNOTAVAIL);	//Connection failed
	}
	WaitForSingleObject(data->hTask, INFINITE);
	CloseHandle(data->hTask);

	delete data;
	return 0;
}

void CTermView::OnEditPastefile()
{
	if (!telnet)
		return;
	if (!telnet->is_connected && !telnet->is_ansi_editor)
		return;

	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING, LoadString(IDS_TXT_FILTER));
	if (dlg.DoModal() == IDOK)
	{
		CFile file;
		if (file.Open(dlg.GetPathName(), CFile::modeRead))
		{
			int len = file.GetLength();
			char *data = new char[len+1];
			file.Read(data, len);
			data[len] = 0;
			file.Abort();
			telnet->SendString(data);
			delete data;
		}
	}
}


void CTermView::ReConnect(CTelnetConn *retelnet)
{
	if (retelnet->is_disconnected)
	{
		int idx = parent->ConnToIndex(retelnet);
		TCITEM item;
		item.mask = TCIF_IMAGE;
		item.iImage = 7;
		parent->tab.SetItem(idx, &item);
		retelnet->time = 0;
		retelnet->ClearAllFlags();
		retelnet->is_lookup_host = true;
		retelnet->Close();
		retelnet->ClearScreen(2);
		retelnet->site_settings.Load(retelnet->cfg_path);
		//不用重新載入鍵盤對應

		ConnectSocket(retelnet);
	}
	else
	{
		Connect(retelnet->address, retelnet->name, retelnet->port, retelnet->cfg_path);
	}
}

void CTermView::OnReconnect()
{
	if (telnet)
		ReConnect(telnet);
#if defined	_COMBO_
	else if (con)
		((CWebConn*)con)->web_browser.wb_ctrl.Refresh();
#endif
}

void CTermView::OnHistory(UINT id)
{
	if (AppConfig.favorites.history.GetSize() > 0)
	{
		CString str = AppConfig.favorites.history[id-ID_FIRST_HISTORY];
		int p = str.Find('\t');
		p = str.Find('\t', p + 1);
		if (-1 == p)
			ConnectStr(str, afxEmptyString);
		else
			ConnectStr(str.Left(p), str.Mid(p + 1));
	}
}

void CTermView::OnAnsiCopy()
{
	if (!telnet)
		return;

	CString data = GetSelAnsi();
	if (CClipboard::SetText(m_hWnd, data))
		paste_block = telnet->sel_block;

	if (AppConfig.auto_cancelsel)
	{
		telnet->sel_end = telnet->sel_start;
		Invalidate(FALSE);
	}
}

void CTermView::OnAnsiEditor()
{
	Connect(LoadString(IDS_NOT_SAVED), LoadString(IDS_ANSI_EDIT), 0);
}


int CTermView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	hand_cursor = AfxGetApp()->LoadCursor(IDC_HAND_CUR);
	MouseCTL_Init(m_hWnd);

	chi_conv.SetTablePath(AppPath + "Conv\\");
	return 0;
}


void CTermView::OnAnsiOpen()
{
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING, LoadString(IDS_TXT_AND_ANS_FILTER), parent);
	if (dlg.DoModal() == IDOK)
	{
		POSITION pos = dlg.GetStartPosition();
		while (pos)
			OpenAnsFile(dlg.GetNextPathName(pos));
	}
}

void CTermView::OnAnsiSaveAs()
{
	if (!telnet || !telnet->is_ansi_editor)
		return;

	CFileDialog dlg(FALSE, "ans", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING, LoadString(IDS_ANS_FILTER));
	if (dlg.DoModal() == IDOK)
	{
		CFile file;
		if (file.Open(dlg.GetPathName(), CFile::modeCreate | CFile::modeWrite))
		{
			OnSelAllBuf();
			CString str = GetSelAnsi();
			telnet->sel_end = telnet->sel_start;
			file.Write(LPCTSTR(str), str.GetLength());
			file.Abort();
			telnet->address = dlg.GetPathName();
			telnet->name = dlg.GetFileName();

			parent->tab.SetItemText(parent->tab.GetCurSel(), telnet->name);
			parent->UpdateStatus();
			parent->UpdateAddressBar();
		}
	}
}

CString CTermView::GetSelAnsi()
{
	CString data;
	if (telnet->sel_end.x != telnet->sel_start.x || telnet->sel_end.y != telnet->sel_start.y)
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

		long selstarty = telnet->sel_start.y;
		long selendy = telnet->sel_end.y;

		data = "\x1b[m";
		LPSTR str = telnet->screen[selstarty] + telnet->sel_start.x;
		LPSTR strend;
		LPBYTE attr = telnet->GetLineAttr(selstarty) + telnet->sel_start.x;
		BYTE tmpatb = *attr;
		if (tmpatb != 7)
		{
			data += AttrToStr(7, tmpatb);
		}
		if (selstarty == selendy)	//if a single line is selected
		{
			strend = telnet->screen[selendy] + telnet->sel_end.x - 1;
			BYTE *atbend = telnet->GetLineAttr(selendy) + telnet->sel_end.x - 1;
			while (*strend == ' ' && GetAttrBkColor(*atbend) == 0)
				strend--, atbend--;

			while (str <= strend)
			{
				if (*attr != tmpatb)
				{
					data += AttrToStr(tmpatb, *attr);
					tmpatb = *attr;
				}
				if (*str)
					data += *str;
				str++;
				attr++;
			}
			if (tmpatb != 7)
				data += "\x1b[m";
		}
		else	//select several line
		{
			int x, x2;
			if (telnet->sel_block)	//使用區塊選取
			{
				x = telnet->sel_start.x;
				x2 = telnet->sel_end.x;
				if (telnet->sel_end.x >= telnet->site_settings.cols_per_page)
					x2--;
			}
			else
			{
				x = 0;
				x2 = telnet->site_settings.cols_per_page - 1;
			}

			strend = telnet->screen[selstarty] + x2;
			LPBYTE atbend = telnet->GetLineAttr(selstarty) + x2;
			while (*strend == ' ' && GetAttrBkColor(*atbend) == 0)
				strend--, atbend--;

			while (str <= strend)
			{
				if (*attr != tmpatb)
				{
					data += AttrToStr(tmpatb, *attr);
					tmpatb = *attr;
				}
				if (*str)
					data += *str;
				str++;
				attr++;
			}
			if (tmpatb != 7)
				data += "\x1b[m";
			data += "\x0d\x0a";

			for (int i = selstarty + 1;i < selendy;i++)
			{
				str = telnet->screen[i] + x;
				attr = telnet->GetLineAttr(i) + x;
				strend = str + x2;
				atbend = attr + x2;
				while (*strend == ' ' && GetAttrBkColor(*atbend) == 0)
					strend--, atbend--;

				tmpatb = 7;
				while (str <= strend)
				{
					if (*attr != tmpatb)
					{
						data += AttrToStr(tmpatb, *attr);
						tmpatb = *attr;
					}
					if (*str)
						data += *str;
					str++;
					attr++;
				}
				if (tmpatb != 7)
					data += "\x1b[m";
				data += "\x0d\x0a";
			}
			str = telnet->screen[telnet->sel_end.y];
			attr = telnet->GetLineAttr(telnet->sel_end.y);
			strend = str + telnet->sel_end.x - 1;
			atbend = attr + telnet->sel_end.x - 1;
			str += x;
			attr += x;
			while (*strend == ' ' && GetAttrBkColor(*atbend) == 0)
				strend--, atbend--;

			tmpatb = 7;
			while (str <= strend)
			{
				if (*attr != tmpatb)
				{
					data += AttrToStr(tmpatb, *attr);
					tmpatb = *attr;
				}
				if (*str)
					data += *str;
				str++;
				attr++;
			}
			if (tmpatb != 7)
				data += "\x1b[m";
		}
	}
	return data;
}

void CTermView::OnAnsiSave()
{
	if (!telnet)
		return;

	if (!telnet->address.Compare(LoadString(IDS_NOT_SAVED)))
	{
		OnAnsiSaveAs();
	}
	else
	{
		CFile file;
		if (file.Open(telnet->address, CFile::modeCreate | CFile::modeWrite))
		{
			OnSelAllBuf();
			while (telnet->sel_end.y > telnet->sel_start.y && telnet->IsEmptyLine(telnet->screen[telnet->sel_end.y], telnet->site_settings.cols_per_page))
				telnet->sel_end.y--;
			CString str = GetSelAnsi();
			telnet->sel_end = telnet->sel_start;
			file.Write(LPCTSTR(str), str.GetLength());
			file.Abort();
		}
	}
}

void CTermView::OnAnsiCls()
{
	if (telnet && telnet->is_ansi_editor)
	{
		telnet->cursor_pos.x = 0;
		telnet->cursor_pos.y = 0;
		telnet->UpdateCursorPos();
		telnet->ReSizeBuffer(48, telnet->site_settings.cols_per_page, telnet->site_settings.lines_per_page);
		for (int i = 0;i < telnet->last_line;i++)
			telnet->InitNewLine(telnet->screen[i]);
		Invalidate(FALSE);
	}
}

void CTermView::OnAnsiIns()
{
	if (telnet && telnet->is_ansi_editor)
	{
		telnet->insert_mode = !telnet->insert_mode;
		parent->UpdateStatus();
	}
}

void CTermView::OnSetBkgnd()
{
	CSetBkDlg dlg(this);
	if (dlg.DoModal() == IDOK)
	{
		if (AppConfig.bktype == 1)
		{
			parent->CloseWindow();
			Sleep(500);
			UpdateBkgnd();
			parent->ShowWindow(SW_RESTORE);
			parent->ShowWindow(SW_SHOW);
		}
		else if (AppConfig.bktype == 0)
		{
			DeleteObject(bk);
			bk = NULL;
			Invalidate(FALSE);
		}
		else
		{
			UpdateBkgnd();
			Invalidate(FALSE);
		}
	}
}

void CTermView::UpdateBkgnd()
{
	if (memdc)
	{
		SelectObject(memdc, holdobj);
		DeleteDC(memdc), memdc = NULL;
	}
	if (bk)
	{
		DeleteObject(bk);
		bk = NULL;
	}
	if (AppConfig.bktype == 0 || AppConfig.bktype > 4)
		return;

	HDC hdc = NULL;
	BITMAP bmp;

	hdc =::GetDC(m_hWnd);
	memdc =::CreateCompatibleDC(hdc);
	::ReleaseDC(m_hWnd, hdc);

	if (AppConfig.bktype > 1)
	{
		if (AppConfig.bkpath.GetLength() > 3)
		{
			LPCTSTR ext = LPCTSTR(AppConfig.bkpath) + AppConfig.bkpath.GetLength() - 3;
			if (0 == stricmp(ext, "bmp"))	// *.bmp
				bk = (HBITMAP)LoadImage(AfxGetInstanceHandle(), AppConfig.bkpath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTCOLOR);
			else	// *.gif, *.jpg
			{
				COleImage img;
				COleImage::Initialize();
				img.LoadFromFile(AppConfig.bkpath);
				bk = img.CopyBitmap();
				COleImage::Finalize();
			}
		}

		if (!bk)
		{
			MessageBox(LoadString(IDS_LOAD_PIC_FAILED), LoadString(IDS_ERR), MB_OK | MB_ICONSTOP);
			AppConfig.bktype = 0;
			AppConfig.bkpath.Empty();
			return;
		}
	}
	else
	{
		hdc =::GetDC(::GetDesktopWindow());
		bk = CreateCompatibleBitmap(hdc, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
		holdobj = SelectObject(memdc, bk);
		BitBlt(memdc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), hdc, 0, 0, SRCCOPY);
		::ReleaseDC(::GetDesktopWindow(), hdc);
		SelectObject(memdc, holdobj);
	}
	GetObject(bk, sizeof(bmp), &bmp);

	BLENDFUNCTION blf;
	blf.BlendOp = AC_SRC_OVER;
	blf.BlendFlags = 0;
	blf.AlphaFormat = 0;
	blf.SourceConstantAlpha = 255 * AppConfig.bkratio / 10;


	CRect rc;
	switch (AppConfig.bktype)
	{
	case 1:	// desktop capture
	case 2:	// tile
		rc.left = 0;	rc.right = GetSystemMetrics(SM_CXSCREEN);
		rc.top = 0;	rc.bottom = GetSystemMetrics(SM_CYSCREEN);
		break;
	case 3:	// center
	case 4:	// stretch
		GetClientRect(rc);
	}

	HDC dc =::GetDC(m_hWnd);
	HDC memdc2 =::CreateCompatibleDC(dc);
	HBITMAP tmp = bk;
	bk = CreateCompatibleBitmap(dc, rc.right, rc.bottom);
	::ReleaseDC(m_hWnd, dc);
	holdobj = SelectObject(memdc, bk);
	HGDIOBJ old2 = SelectObject(memdc2, tmp);

	if (AppConfig.bktype == 1 || AppConfig.bktype == 4)
	{
		AlphaBlend(memdc, 0, 0, rc.Width(), rc.Height(),
				   memdc2, 0, 0, bmp.bmWidth, bmp.bmHeight, blf);
	}
	else if (AppConfig.bktype == 2)	// tile
	{
		while (rc.left < rc.right)
		{
			while (rc.top < rc.bottom)
			{
				::FillRect(memdc, rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
				AlphaBlend(memdc, rc.left, rc.top, bmp.bmWidth, bmp.bmHeight,
						   memdc2, 0, 0, bmp.bmWidth, bmp.bmHeight, blf);
				rc.top += bmp.bmHeight;
			}
			rc.top = 0;
			rc.left += bmp.bmWidth;
		}
	}
	else if (AppConfig.bktype == 3)
	{
		int left = (rc.Width() - bmp.bmWidth) / 2;
		int top = (rc.Height() - bmp.bmHeight) / 2;
		AlphaBlend(memdc, left, top, bmp.bmWidth, bmp.bmHeight,
				   memdc2, 0, 0, bmp.bmWidth, bmp.bmHeight, blf);

	}
	SelectObject(memdc, holdobj);
	SelectObject(memdc2, old2);
	DeleteDC(memdc2);
	DeleteObject(tmp);

	holdobj = SelectObject(memdc, bk);
}

inline void CTermView::FillBk(CDC &dc)
{
	GetClientRect(view_rect);
	if (!AppConfig.bktype)
	{
		dc.FillSolidRect(view_rect, AppConfig.colormap[0]);
		return;
	}
	BITMAP bmp;
	GetObject(bk, sizeof(BITMAP), &bmp);
	if (AppConfig.bktype == 1)
	{
		ClientToScreen(view_rect);
		::BitBlt(dc.m_hDC, 0, 0, view_rect.Width(), view_rect.Height(), memdc, view_rect.left, view_rect.top, SRCCOPY);
	}
	else
		::BitBlt(dc.m_hDC, 0, 0, bmp.bmWidth, bmp.bmHeight, memdc, 0, 0, SRCCOPY);
}

void CTermView::OnRightNoDBCS()
{
	if (telnet)
		telnet->Send("\x1b[C", 3);
}

void CTermView::OnLeftNoDBCS()
{
	if (telnet)
		telnet->Send("\x1b[D", 3);
}

void CTermView::OnBackspaceNoDBCS()
{
	if (telnet)
	{
		if (telnet->is_ansi_editor)
			telnet->Back();
		else
		{
			const char* back = NULL;
			if (telnet->key_map && (back = telnet->key_map->FindKey(VK_BACK, 0)))
				telnet->Send(back, strlen(back));
		}
	}
}

CConn* CTermView::NewConn(CString address, CString name, unsigned short port, LPCTSTR cfg_path)
{
	CConn* newcon = NULL;
	newcon = new CTelnetConn;
	newcon->address = address;
	newcon->name = name;

#if defined	_COMBO_
	if (!newcon->is_web)	//如果是連線BBS,新開BBS畫面
	{
#endif
		CTelnetConn* new_telnet = (CTelnetConn*)newcon;
		new_telnet->port = port;
		new_telnet->cfg_path = cfg_path;

		//為新的socket載入設定值
		if (!new_telnet->site_settings.Load(new_telnet->cfg_path))	//如果載入設定發生錯誤
		{
			//這很有可能會在密碼輸入錯誤的時候發生!
			delete new_telnet;
			return NULL;
		}
		new_telnet->key_map = CKeyMap::Load(new_telnet->site_settings.key_map_name);

		if (new_telnet->site_settings.text_input_conv || new_telnet->site_settings.text_output_conv)
			chi_conv.AddRef();

		if (0 == port)  // port = 0 代表 ansi editor ( dirty hack :( )
		{
			new_telnet->ClearAllFlags();
			new_telnet->is_ansi_editor = true;
			new_telnet->site_settings.line_count = AppConfig.ed_lines_per_page * 2;
			new_telnet->site_settings.cols_per_page = AppConfig.ed_cols_per_page;
			new_telnet->site_settings.lines_per_page = AppConfig.ed_lines_per_page;
			new_telnet->site_settings.showscroll = TRUE;
		}
		new_telnet->CreateBuffer();

#if defined	_COMBO_
	}
#endif
	parent->NewTab(newcon);
	all_telnet_conns.Add(newcon);
	return newcon;
}

DWORD CTermView::DNSLookupThread(LPVOID param)
{
	DNSLookupData* host = (DNSLookupData*)param;
	CTelnetConn* new_telnet = host->new_telnet;
	LPHOSTENT lphost = gethostbyname(host->address);
	if (lphost == NULL)
	{
		WSASetLastError(WSAEINVAL);
		new_telnet->view->PostMessage(WM_DNSLOOKUP_END, FALSE, (LPARAM)param);
		ExitThread(1);
	}
	else
	{
		host->sockaddr.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
	}
	new_telnet->view->PostMessage(WM_DNSLOOKUP_END, TRUE, (LPARAM)param);
	ExitThread(0);
	return 0;
}

LRESULT CTermView::OnSocket(WPARAM wparam, LPARAM lparam)
{
	if (!all_telnet_conns.GetData())
		return 0;

	CTelnetConn** pptelnet = (CTelnetConn**)all_telnet_conns.GetData();
	CTelnetConn** plasttelnet = pptelnet + all_telnet_conns.GetSize();
	for (; pptelnet < plasttelnet; pptelnet++)
	{
		CTelnetConn* ptelnet = *pptelnet;
		if (ptelnet->is_telnet && ptelnet->telnet == (SOCKET)wparam)
		{
			WORD err = HIWORD(lparam);

			switch (LOWORD(lparam))
			{
			case FD_READ:
				ptelnet->OnReceive(err);
				break;
			case FD_CONNECT:
				ptelnet->OnConnect(err);
				break;
			case FD_CLOSE:
				ptelnet->OnClose(err);
			}
		}
	} //end for
	return 0;
}

void CTermView::ConnectSocket(CTelnetConn *new_telnet)
{
	if (InternetAttemptConnect(0) != ERROR_SUCCESS)
	{
		new_telnet->OnConnect(WSAENOTCONN);
		return;
	}

	new_telnet->Create();

#if defined	_COMBO_
	const LPCTSTR paddress = LPCTSTR(new_telnet->address) + 9;
#else
	CString &paddress = new_telnet->address;
#endif
	SOCKADDR_IN sockaddr;
	memset(&sockaddr, 0, sizeof(SOCKADDR_IN));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons((u_short) new_telnet->port);
	sockaddr.sin_addr.s_addr = inet_addr(paddress);
	if (sockaddr.sin_addr.s_addr != INADDR_NONE)
	{
		new_telnet->ClearAllFlags();
		new_telnet->is_connecting = true;
		new_telnet->Connect((SOCKADDR*)&sockaddr, sizeof(SOCKADDR_IN));
		return;
	}
//	如果不是IP，必須尋找主機
//	加入新執行緒
	DNSLookupData *newfind = new DNSLookupData;
	newfind->new_telnet = new_telnet;
	newfind->address = paddress;
	DWORD tid;
//	開始新的執行緒
	memset(&sockaddr, 0, sizeof(SOCKADDR_IN));
	newfind->hTask = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DNSLookupThread,
								  newfind, 0, &tid);
}


void CTermView::OnSelAllBuf()
{
	if (!telnet)
		return;
	telnet->sel_start.x = 0;
	telnet->sel_start.y = 0;
	telnet->sel_end.x = telnet->site_settings.cols_per_page;
	telnet->sel_end.y = telnet->site_settings.line_count - 1;
	Invalidate(FALSE);
}

BOOL CTermView::OnMouseWheel(UINT nFlags, short zDelta, CPoint point_In)
{
	if (!telnet)
		return TRUE;

	CPoint pt;

	pt = point_In;

	if (CanUseMouseCTL())
	{
		MouseCTL_OnMouseWheel(m_hWnd, nFlags, zDelta, point_In);
		return TRUE;
	}

	SCROLLINFO info;
	GetScrollInfo(SB_VERT, &info);
	if ((zDelta > 0 && info.nPos > 0) || (zDelta < 0 && info.nPos < info.nMax))
	{
		int oldpos = info.nPos;

		info.fMask = SIF_POS;
		info.nPos -= zDelta / WHEEL_DELTA;
		int pos = info.nPos;
		if (info.nPos < 0)
			info.nPos = 0;
		else if (info.nPos > (int)(info.nMax - info.nPage))
			info.nPos = info.nMax - info.nPage;

		if (info.nPos != oldpos)
		{
			SetScrollInfo(SB_VERT, &info);
			telnet->scroll_pos = info.nPos;
			if (telnet->is_ansi_editor)
				telnet->UpdateCursorPos();

			//		telnet->CheckHyperLinks();
			Invalidate(FALSE);
		}
	}

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

inline void CTermView::DrawScreen(CDC &dc)
{
	if (!telnet) { FillBk(dc); return; }

	GetClientRect(view_rect);
	if (AppConfig.bktype == 1)
		ClientToScreen(view_rect);

	int y = top_margin;
	HANDLE fold = SelectObject(dc.m_hDC, fnt.m_hObject);

	//計算選取區，將倒置的選取區還原
	long selstarty = telnet->sel_start.y;		long selendy = telnet->sel_end.y;
	long selstartx = telnet->sel_start.x;		long selendx = telnet->sel_end.x;
	if (telnet->sel_block)
	{
		if (selstarty > selendy)
			swap(selstarty, selendy);
		if (selstartx > selendx)
			swap(selstartx, selendx);
	}
	else
	{
		if (selstarty > selendy)
		{
			swap(selstarty, selendy);
			swap(selstartx, selendx);
		}
		else if (selstarty == selendy && selstartx > selendx)
			swap(selstartx, selendx);
	}

	BYTE* pline_selstart;	//設為最大
	BYTE* pline_selend;	//設為最小
	int last_line = telnet->scroll_pos + telnet->site_settings.lines_per_page;
	for (int i = telnet->scroll_pos ; i < last_line; i++)
	{
		LPBYTE atbline = telnet->GetLineAttr(i);	//attribs
		if (i >= selstarty && i <= selendy)	//如果在選取區內
		{
			pline_selstart = (BYTE*)0xffffffff;	//設為最大
			pline_selend	= (BYTE*)0x00000000;	//設為最小
			if (telnet->sel_block)	//區塊選取
			{
				pline_selstart = atbline + selstartx;
				pline_selend = atbline + selendx;
			}
			else
			{
				pline_selstart = (i == selstarty) ? (atbline + selstartx) : atbline;
				pline_selend = (i == selendy) ? (atbline + selendx) : atbline + telnet->site_settings.cols_per_page;
			}
		}
		else
			pline_selstart = pline_selend = NULL;

		if (AppConfig.old_textout)
			DrawLineOld(dc, telnet->screen[i], pline_selstart, pline_selend, y);
		else
			DrawLine(dc, telnet->screen[i], pline_selstart, pline_selend, y);

		if (AppConfig.link_underline)
			DrawLink(dc, telnet->screen[i], telnet->GetLineAttr(i), y);
		y += lineh;
	}
	SelectObject(dc.m_hDC, fold);

//-----------填滿畫面周圍-------------
	int right_margin = left_margin + telnet->site_settings.cols_per_page * chw;
	CRect rc;	GetClientRect(rc);
	int t = rc.bottom;	rc.bottom = top_margin;	FillBkRect(dc, rc, 0);	//上
	rc.bottom = t;	rc.top = y;	FillBkRect(dc, rc, 0);	//下
	t = rc.right;		rc.right = left_margin;	rc.top = top_margin;	rc.bottom = y;	FillBkRect(dc, rc, 0);	//左
	rc.right = t;		rc.left = right_margin;	FillBkRect(dc, rc, 0);	//右
}


BOOL IsDBCS(char* head, char* str)
{
	int w;
	while (head <= str)
	{
		w = get_chw(head);
		head += w;
	}
	return (head >= str && w == 2);
}


void CTermView::DrawLine(CDC &dc, LPCSTR line, BYTE* pline_selstart, BYTE* pline_selend, int y)
{
	LPCSTR eol = line + telnet->site_settings.cols_per_page;
	CRect rc(left_margin, y, 0, y + lineh);
	UINT draw_opt = SetDCColors(&dc, 7);
	BYTE prevatb = 7;		bool prevbsel = false, textout = true;

	while (line < eol)
	{
		int w = get_chw(line);
		LPBYTE patb = telnet->GetLineAttr(line);
		bool bsel = (patb >= pline_selstart && patb < pline_selend);

		int dx = chw * w;
		rc.right = rc.left + dx;
		if (w == 2)	//如果是雙位元組字，檢查是否為雙色字
		{
			bool bsel2 = ((patb + 1) >= pline_selstart && (patb + 1) < pline_selend);
			//如果是雙色字，或是中文前後半選取狀態不同
			if (patb[0] != patb[1] || bsel != bsel2)	//先輸出整個文字，再輸出前半
			{
				if (patb[1] != prevatb || bsel2 != prevbsel)
				{
					draw_opt = SetDCColors(&dc, (prevatb = patb[1]), (prevbsel = bsel2));
					textout = (!IsAttrBlink(patb[1]) || blight || telnet->sel_end != telnet->sel_start);
				}
				if (!(draw_opt & ETO_OPAQUE))
					FillBkRect(dc, rc, prevatb, prevbsel);
				ExtTextOut(dc, rc.left, rc.top, draw_opt, rc, line, (textout ? 2 : 0));
				rc.right -= chw;
				draw_opt = ETO_CLIPPED;
			}
		}
		if (patb[0] != prevatb || bsel != prevbsel)
		{
			draw_opt &= ETO_CLIPPED;
			draw_opt |= SetDCColors(&dc, (prevatb = patb[0]) , (prevbsel = bsel));
			textout = (!IsAttrBlink(patb[0]) || blight || telnet->sel_end != telnet->sel_start);
		}

		if (!(draw_opt & ETO_OPAQUE))
			FillBkRect(dc, rc, prevatb, prevbsel);

		ExtTextOut(dc, rc.left, rc.top, draw_opt, rc, line, (textout ? w : 0));

		rc.left += dx;
		line += w;
	}
}


void CTermView::DrawLineBlink(CDC &dc, LPCSTR line, int y)
{
	LPCSTR eol = line + telnet->site_settings.cols_per_page;
	CRect rc(left_margin, y, 0, y + lineh);
	UINT draw_opt = SetDCColors(&dc, 7);
	BYTE prevatb = 7;
	while (line < eol)
	{
		int w = get_chw(line);
		LPBYTE patb = telnet->GetLineAttr(line);

		int dx = chw * w;
		rc.right = rc.left + dx;
		bool update = IsAttrBlink(patb[0]);

		if (w == 2)	//如果是雙位元組字，檢查是否為雙色字
		{
			//如果是雙色字
			if (patb[0] != patb[1])	//先輸出整個文字，再輸出前半
			{
				if (IsAttrBlink(patb[1]))
				{
					if (patb[1] != prevatb)
						draw_opt = SetDCColors(&dc, (prevatb = patb[1]));

					if (!(draw_opt & ETO_OPAQUE))
						FillBkRect(dc, rc, prevatb);
					ExtTextOut(dc, rc.left, rc.top, draw_opt, rc, line, (blight ? 2 : 0));
					update = true;
				}
				rc.right -= chw;
				draw_opt = ETO_CLIPPED;
			}
		}

		if (update)
		{
			if (patb[0] != prevatb)
			{
				draw_opt &= ETO_CLIPPED;
				draw_opt |= SetDCColors(&dc, (prevatb = patb[0]));
			}
			if (!(draw_opt & ETO_OPAQUE))
				FillBkRect(dc, rc, prevatb);

			ExtTextOut(dc, rc.left, rc.top, draw_opt, rc, line,
					   ((!IsAttrBlink(patb[0]) || blight) ? w : 0));
		}
		rc.left += dx;
		line += w;
	}
}


inline void CTermView::DrawLink(CDC &dc, LPSTR line, LPBYTE atbline, int y)
{
	if (!telnet->GetHyperLink(line))
		return;

	int start, len;
	HPEN pen = NULL;
	y += lineh - 1;
	const char* plink = line;
	const char* eol = line + telnet->site_settings.cols_per_page;
	while ((plink < eol) && (plink = AppConfig.hyper_links.FindHyperLink(plink, len)))
	{
		start = (int(plink) - int(line)) * chw + left_margin;
		char tmp = plink[len];	((char*)plink)[len] = 0;
		int t = AppConfig.hyper_links.GetURLType(plink);
		((char*)plink)[len] = tmp;
		if (t >= 0)
		{
			pen = CreatePen(PS_SOLID, 1, AppConfig.hyper_links.links[t].color);
			HGDIOBJ h_old = SelectObject(dc.m_hDC, pen);
			dc.MoveTo(start, y);
			dc.LineTo(start + len*chw, y);
			SelectObject(dc.m_hDC, h_old);
			DeleteObject(pen);
		}
		plink += len;
	}
}

inline void CTermView::DrawBlink()
{
	if (!telnet)
		return;
	CClientDC dc(this);

	GetClientRect(view_rect);
	if (AppConfig.bktype == 1)
		ClientToScreen(view_rect);

	LPSTR* curline = telnet->screen + telnet->scroll_pos;
	LPSTR* lastline = curline + telnet->site_settings.lines_per_page;
	int y = top_margin;
	CFont* fold = dc.SelectObject(&fnt);

	if (AppConfig.old_textout)
	{
		for (;curline < lastline; curline++, y += lineh)
			DrawLineBlinkOld(dc, *curline, y);
	}
	else
	{
		for (;curline < lastline; curline++, y += lineh)
			DrawLineBlink(dc, *curline, y);
	}
	dc.SelectObject(fold);
}


inline LPBYTE find_blink(LPBYTE atbline, LPBYTE eoatb, int& len)
{
	LPBYTE patb; for (patb = atbline;!IsAttrBlink(*patb) && patb < eoatb; patb++);
	if (patb >= eoatb)
		return NULL;
	atbline = patb;
	while (IsAttrBlink(*patb) && *patb == *atbline && patb < eoatb)
		patb++;
	len = int(patb - atbline);
	return atbline;
}

inline void CTermView::FillBkRect(CDC &dc, CRect &rc, BYTE attr, BOOL bkinvirt/* =0 */)
{
	int i = GetAttrBkColor(attr);
	if (AppConfig.bktype == 0 || i)
	{
		dc.SetBkColor(AppConfig.colormap[i]);
		dc.ExtTextOut(rc.left, rc.top, ETO_OPAQUE, rc, NULL, 0, NULL);
	}
	else
		::BitBlt(dc.m_hDC, rc.left, rc.top, rc.Width(), rc.Height(), memdc, view_rect.left + rc.left, rc.top + view_rect.top, bkinvirt ? NOTSRCCOPY : SRCCOPY);
}

BOOL CTermView::OpenAnsFile(LPCTSTR filepath)
{
	CFile file;
	if (file.Open(filepath, CFile::modeRead))
	{
		OnAnsiEditor();
		char* title = (char*)_mbsrchr((BYTE*)LPCTSTR(filepath), '\\');
		if (title)	title++;
		parent->tab.SetItemText(parent->tab.GetCurSel(), title);
		telnet->name = title;

		int len = file.GetLength();
		char *data = new char[len];
		file.Read(data, len);
		char *pdata = data, *eod = data + len;
		if (*pdata == 0x0a)
		{
			*pdata = 0x0d;
			pdata++;
		}

		while (pdata < eod)
		{
			if (*pdata == 0x0a && *(pdata - 1) != 0x0d)
				*pdata = 0x0d;
			pdata++;
		}

		file.Abort();
		LockWindowUpdate();
		telnet->Send(data, len);
		telnet->scroll_pos = 0;
		telnet->cursor_pos.x = telnet->cursor_pos.y = 0;
		telnet->UpdateCursorPos();
		SetScrollBar();
		UnlockWindowUpdate();
		delete []data;
		telnet->address = filepath;
		parent->UpdateStatus();
		parent->UpdateAddressBar();
		return TRUE;
	}
	return FALSE;
}


void CTermView::SendAnsiString(CString data)	//送出ESC轉換後的ANSI字串
{
//使用彩色貼上含有控制碼，要暫時關閉自動換行功能
	BOOL tmp_autowrap = AppConfig.site_settings.paste_autowrap;
	AppConfig.site_settings.paste_autowrap = 0;

	if (!telnet->is_ansi_editor && !telnet->site_settings.esc_convert.IsEmpty())
	{
		char* pdata;
		char* buf = pdata = new char[data.GetLength()+1];
		memcpy(pdata, data, data.GetLength() + 1);
		data.Empty();
		while (*pdata)
		{
			if (*pdata == '\x1b' && *(pdata + 1) == '[')
			{
				data += telnet->site_settings.esc_convert;
				data += '[';
				pdata += 2;
				continue;
			}
			data += *pdata;
			pdata++;
		}
		delete []buf;
	}
	telnet->SendString(data);

//重新恢復自動換行
	AppConfig.site_settings.paste_autowrap = tmp_autowrap;
}

void CTermView::OnDestroyClipboard()
{
	CWnd::OnDestroyClipboard();
	if (paste_block)
		paste_block = 0;
}

void CTermView::OnSmoothDraw()
{
	if (!AppConfig.smooth_draw)
	{
		if (MessageBox(LoadString(IDS_SMOOTH_DRAW_INFO)
					   , LoadString(IDS_HELP), MB_OKCANCEL | MB_ICONEXCLAMATION) == IDOK)
			AppConfig.smooth_draw = TRUE;
	}
	else
		AppConfig.smooth_draw = FALSE;
	Invalidate(FALSE);
}

#if defined	_COMBO_
BOOL CTermView::SetWindowPos(const CWnd *pWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags)
{
	if (con && !telnet)
		((CWebConn*)con)->web_browser.SetWindowPos(pWndInsertAfter, x, y, cx, cy, nFlags);
	return CWnd::SetWindowPos(pWndInsertAfter, x, y, cx, cy, nFlags);
}

void CTermView::MoveWindow(int x, int y, int nWidth, int nHeight, BOOL bRepaint)
{
	CWnd::MoveWindow(x, y, nWidth, nHeight, bRepaint);
	if (con && !telnet)
		((CWebConn*)con)->web_browser.MoveWindow(x, y, nWidth, nHeight, bRepaint);
}

CWebConn* CTermView::ConnectWeb(CString address, BOOL act)
{
	CWebConn* newcon = new CWebConn;
	newcon->web_browser.view = this;
	newcon->name = newcon->address = address;
	newcon->web_browser.parent = parent;
	newcon->web_browser.Create(NULL, NULL, WS_CHILD, CRect(0, 0, 0, 0), parent, 0);
	newcon->web_browser.wb_ctrl.put_RegisterAsBrowser(TRUE);
	newcon->web_browser.wb_ctrl.put_RegisterAsDropTarget(TRUE);
	parent->NewTab(newcon);

	if (!address.IsEmpty())
	{
		COleVariant v;
		COleVariant url = address;
		newcon->web_browser.wb_ctrl.Navigate2(&url, &v, &v, &v, &v);
	}
	if (act)
		parent->SwitchToConn(newcon);
	else
		newcon->web_browser.EnableWindow(FALSE);

	parent->FilterWebConn(newcon);
	return newcon;
}
#endif

void CMainFrame::SendFreqStr(CString str, BYTE inf)
{
	if (inf & 1 << 6)	//control
		str = UnescapeControlChars(str);

	if (inf & 1 << 7)
	{
		if (!(inf & 1 << 6))	//沒做控制碼轉換
			str.Replace("^[[", "\x1b[");		//轉換 ^[ 成 ESC

		view.SendAnsiString(str);
	}
	else
		view.telnet->SendString(str);
}

typedef char*(*strstrfunc)(const char*, const char*);
typedef char*(*strnrstrfunc)(const char*, const char*, int);
LRESULT CTermView::OnFind(WPARAM w, LPARAM l)
{
	if (!pfinddlg)	return 0;

	if (pfinddlg->IsTerminating())
	{
		pfinddlg = NULL;
		return 0;
	}

	if (!telnet)	return 0;

	CString find_str = pfinddlg->GetFindString();

	char* line;		char* found = NULL;
	RECT rc;	long y;	int startx;	int endx;

	rc.left = left_margin + telnet->sel_start.x * chw;
	rc.right = rc.left + find_text.GetLength() * chw;
	rc.top = top_margin + (telnet->sel_start.y - telnet->scroll_pos) * lineh;
	rc.bottom = rc.top + lineh;

	bool first_time = (telnet->sel_start.x == telnet->sel_end.x) || (find_str != find_text);

	int first_line = pfinddlg->MatchWholeWord() ? 0 : telnet->first_line;
	if (pfinddlg->SearchDown())	//向後搜尋
	{
		if (first_time)	//第一次尋找
		{
			telnet->sel_start.y = telnet->sel_end.y = first_line;
			telnet->sel_start.x = telnet->sel_end.x = 0;
		}

		y = telnet->sel_start.y;
		startx = telnet->sel_start.x;
		endx = telnet->sel_end.x;
		line = telnet->screen[y] + endx;

		if (pfinddlg->MatchCase())
		{
			found = strstr(line, find_str);
		}
		else
		{
			found = strstri(line, find_str);
		}

		while (!found)
		{
			y++;
			if (y >= telnet->site_settings.line_count)
				break;
			endx = 0;
			line = telnet->screen[y];
		}
	}
	else	//向前搜尋
	{
		strnrstrfunc pstrnrstr = pfinddlg->MatchCase() ? strnrstr : strnrstri;
		if (first_time)	//第一次尋找
		{
			telnet->sel_start.y = telnet->sel_end.y = telnet->last_line;
			telnet->sel_start.x = telnet->sel_end.x = telnet->site_settings.cols_per_page;
		}
		y = telnet->sel_end.y;
		startx = telnet->sel_start.x;
		endx = telnet->sel_end.x;
		line = telnet->screen[y];

		while (!(found = (*pstrnrstr)(line, find_str, startx)) && y > first_line)
		{
			y--;
			startx = telnet->site_settings.cols_per_page;
			line = telnet->screen[y];
		}
	}

	if (found)	//如果有找到
	{
		InvalidateRect(&rc, FALSE);	//清除原本的舊選取區
		startx = (found - telnet->screen[y]);
		telnet->sel_start.x = startx;
		telnet->sel_end.y = telnet->sel_start.y = y;
		telnet->sel_end.x = startx + find_str.GetLength();

		if (telnet->scroll_pos > y || y >= telnet->scroll_pos + telnet->site_settings.lines_per_page)	//如果不在目前視野內
		{
			int pg = (y / telnet->site_settings.lines_per_page);	//找出所在的頁數
			telnet->scroll_pos = pg * telnet->site_settings.lines_per_page;
			SetScrollPos(SB_VERT, telnet->scroll_pos);
			Invalidate(FALSE);
		}
	}

	rc.left = left_margin + startx * chw;
	rc.right = rc.left + find_str.GetLength() * chw;
	rc.top = top_margin + (y - telnet->scroll_pos) * lineh;
	rc.bottom = rc.top + lineh;
	InvalidateRect(&rc, FALSE);

	if (!found)
		MessageBox(LoadString(IDS_NOT_FOUND) + " " + find_str);

	find_text = find_str;
	return 0;
}

#include <dlgs.h>

//	AUTOCHECKBOX    "Match &whole word only", chx1, 4, 26, 100, 12, WS_GROUP
//	偷偷借用FindDialog的Match Case作為尋找全部緩衝區的選項 :)

void CTermView::FindStart()
{
	telnet->sel_end.x = 0;
	telnet->sel_start = telnet->sel_end;
	if (!pfinddlg)
	{
		pfinddlg = new CFindReplaceDialog;
		pfinddlg->m_fr.lpstrFindWhat = (LPTSTR)(LPCTSTR)find_text;
		pfinddlg->Create(TRUE, NULL, NULL, FR_DOWN, this);
		pfinddlg->GetDlgItem(chx1)->SetWindowText(LoadString(IDS_FIND_IN_ALL_BUF));	//"尋找整個緩衝區(&B)"
		pfinddlg->ShowWindow(SW_SHOW);
	}
	else
		pfinddlg->SetForegroundWindow();
}

inline void CTermView::PtToLineCol(POINT pt, int &x, int &y, bool adjust_x)
{
	pt.x -= left_margin;	pt.y -= top_margin;
	if (pt.x < 0)	pt.x = 0;
	if (pt.y < 0)	pt.y = 0;

	x = (pt.x / chw);	y = (pt.y / lineh);
	if (y >= telnet->site_settings.lines_per_page)
		y = telnet->site_settings.lines_per_page - 1;

	LPSTR curstr = telnet->screen[telnet->scroll_pos+y];

	if (adjust_x)
	{
		if (telnet->site_settings.auto_dbcs_mouse)
		{
			//-----------新的支援中文的座標計算-----------
			if (x > 0 && IsBig5(curstr, x - 1))	//如果選擇中文後半段，就選取下一個字
				x++;
			else if (!IsBig5(curstr, x) && (pt.x % chw)*2 > chw)	//如果也不是中文前半，才是英文
				x++;
		}
		else
		{
			//------------不考慮雙位元組的座標計算-----------
			if ((pt.x % chw)*2 > chw)
				x++;
		}
	}

	if (x > telnet->site_settings.cols_per_page)
		x = telnet->site_settings.cols_per_page;

	if (pt.x < chw)
		x = 0;
}


inline char* CTermView::HyperLinkHitTest(int x, int y, int& len)	//用來測試畫面上某點是否為超連結
//x,y為終端機行列座標，而不是滑鼠座標
{
	y += telnet->scroll_pos;
	const char* plink = telnet->screen[y];

	if (!telnet->GetHyperLink(plink))
		return NULL;

	const char* eol = plink + telnet->site_settings.cols_per_page;
	while (plink = AppConfig.hyper_links.FindHyperLink(plink, len))
	{
		char* px = telnet->screen[y] + x;
		if (px >= plink && px < plink + len)
			return (char*)plink;
		plink += len;
	}
	return NULL;
}

void CTermView::OnCurConSettings()
{
	CPropertySheet dlg(IDS_CUR_CON_SETTINGS);
	CSiteSettings tmpset;

	if (!telnet->cfg_path.IsEmpty())	//嘗試載入個別設定
		tmpset.Load(telnet->cfg_path);
	tmpset = telnet->site_settings;	//並使用目前設定值覆蓋無法載入的部分

	CSitePage page1;
	page1.psettings = &tmpset;

	CAutoReplyPage page2;
	page2.triggers = &tmpset.triggers;
	dlg.AddPage(&page1);
	dlg.AddPage(&page2);
	if (dlg.DoModal() == IDOK)
	{
		if (!telnet->cfg_path.IsEmpty())
			tmpset.Save(telnet->cfg_path);

		//重新載入字串觸發
		telnet->site_settings.triggers.CopyFrom(tmpset.triggers);

		if (strncmp(telnet->site_settings.key_map_name, tmpset.key_map_name, 10))	//如果鍵盤對映更改
		{
			telnet->key_map->Release();
			telnet->key_map = CKeyMap::Load(tmpset.key_map_name);
			telnet->site_settings.key_map_name = tmpset.key_map_name;
		}
		CRect rc;	GetClientRect(rc);
		telnet->ReSizeBuffer(tmpset.line_count, tmpset.cols_per_page, tmpset.lines_per_page);
		telnet->site_settings = tmpset;
		SetScrollBar();
		telnet->SendNaws();
		AdjustFont(rc.right, rc.bottom);
		Invalidate(FALSE);
	}
}

void CTermView::OnEditOpenURL(UINT id)
{
	CString url = GetSelText();
	url.Replace("\x0d\x0a", "");
	url.Replace(" ", "");
	telnet->sel_start = telnet->sel_end;
	Invalidate(FALSE);
	switch (id)
	{
	case ID_EDIT_OPENURL_HTTP:
		url = "http://" + url;
		break;
	case ID_EDIT_OPENURL_FTP:
		url = "ftp://" + url;
		break;
	case ID_EDIT_OPENURL_TELNET:
		if (0 != strnicmp(url, "telnet:", 7))
			url = "telnet://" + url;
		parent->OnNewConnectionAds(url);
		return;
	}
	AppConfig.hyper_links.OpenURL(url);
}

void CTermView::OnUpdateBBSList()
{
	ShellExecute(AfxGetMainWnd()->m_hWnd, "open", AppPath + "UBL.exe", AppPath + "sites.dat", AppPath, SW_SHOW);
}


CString CTermView::GetSelText()
{
	CString ret;
	if (telnet->sel_end.x != telnet->sel_start.x || telnet->sel_end.y != telnet->sel_start.y)
	{
		UINT tmp;

		bool bottom2top = (telnet->sel_end.y < telnet->sel_start.y);
		if (bottom2top)
		{
			tmp = telnet->sel_end.y;
			telnet->sel_end.y = telnet->sel_start.y;
			telnet->sel_start.y = tmp;
			//x也需要倒過來
			tmp = telnet->sel_end.x;
			telnet->sel_end.x = telnet->sel_start.x;
			telnet->sel_start.x = tmp;
		}

		long selstarty = telnet->sel_start.y;
		long selendy = telnet->sel_end.y;

		bool single_line_sel = (selstarty == selendy);

		if ((telnet->sel_block || single_line_sel) && telnet->sel_end.x < telnet->sel_start.x)
		{
			tmp = telnet->sel_end.x;
			telnet->sel_end.x = telnet->sel_start.x;
			telnet->sel_start.x = tmp;
		}

		LPSTR data = NULL;
		int len = 0;

		if (single_line_sel)	//if a single line is selected
		{
			len = telnet->sel_end.x - telnet->sel_start.x;
			data = ret.GetBuffer(len + 1);
			memset(data, 0, len + 1);
			strncpy(data, telnet->screen[selstarty] + telnet->sel_start.x, len);
			if (telnet->sel_block)	//如果區塊選取
				paste_block = TRUE;
			strstriptail(data);
		}
		else	//select several line
		{
			char crlf[] = {13, 10, 0};
			if (telnet->sel_block)	//如果區塊選取
			{
				paste_block = TRUE;
				int oll = telnet->sel_end.x - telnet->sel_start.x;

				len = (oll + 2) * (telnet->sel_end.y - telnet->sel_start.y + 1) + 1;
				data = ret.GetBuffer(len);
				char* pdata = data;
				for (int i = selstarty;i <= selendy;i++)
				{
					memcpy(pdata, telnet->screen[i] + telnet->sel_start.x, oll);
					*(pdata + oll) = 0;
					pdata = strstriptail(pdata);
					memcpy(pdata, crlf, 2);
					pdata += 2;
				}
				*(pdata - 2) = 0;
			}
			else
			{
				len = (telnet->site_settings.cols_per_page + 2) - telnet->sel_start.x;
				len += (telnet->site_settings.cols_per_page + 2) * (selendy - selstarty);
				len += telnet->sel_end.x + 1;
				data = ret.GetBuffer(len + 10);
				memset(data, 0, len + 10);
				strcpy(data, telnet->screen[selstarty] + telnet->sel_start.x);
				strstriptail(data);
				strcat(data, crlf);
				int i;
				for (i = selstarty + 1; i < selendy; ++i)
				{
					strcat(data, telnet->screen[i]);
					strstriptail(data);
					strcat(data, crlf);
				}
				len = strlen(data);
				strncpy(data + len, telnet->screen[i], telnet->sel_end.x);
				strstriptail(data);
			}
		}
		ret.ReleaseBuffer();
	}
	return ret;
}

LRESULT CTermView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (AppConfig.lock_pcman &&
		message != WM_TIMER &&
		message != WM_SOCKET &&
		message != WM_DNSLOOKUP_END)
		return 0;
	return CWnd::WindowProc(message, wParam, lParam);
}


void CTermView::AdjustFont(int cx, int cy)
{
//-----動態字體調整------
	int cols_per_page = telnet ? telnet->site_settings.cols_per_page : AppConfig.site_settings.cols_per_page;
	int lines_per_page = telnet ? telnet->site_settings.lines_per_page : AppConfig.site_settings.lines_per_page;
	if (AppConfig.auto_font)
	{
		int x = cx / (cols_per_page / 2);
		int y = cy / lines_per_page;

//		if(AppConfig.old_textout)
		{
			if (x % 2)
				x--;
			if (y % 2)
				y--;
		}
		AppConfig.font_info.lfHeight = y;
		chw = AppConfig.font_info.lfWidth = x / 2;
		fnt.DeleteObject();
		fnt.CreateFontIndirect(&AppConfig.font_info);

		CWindowDC dc(this);
		CGdiObject* pold = dc.SelectObject(&fnt);
		CSize &sz = dc.GetTextExtent("　", 2);
		chw = sz.cx / 2;
//		lineh=y;
		lineh = sz.cy;
//---------------------
	}
	left_margin = (cx - chw * cols_per_page) / 2;
	top_margin = (cy - lineh * lines_per_page) / 2;

	CreateCaret();
	ShowCaret();
	if (telnet)
		telnet->UpdateCursorPos();
	else
		SetCaretPos(CPoint(left_margin, top_margin + lineh - 2));
}

void CTermView::ConnectStr(CString name, CString dir)
{
	CString address;
	unsigned short port;
	int i = name.Find('\t');
	address = name.Mid(i + 1);
	char type = name[0];
	name = name.Mid(1, i - 1);

#if defined(_COMBO_)
	if (type != 's')
	{
		ConnectWeb(address, TRUE);
		return;
	}
	if (0 == strncmp("telnet:", address, 7))
	{
		LPCTSTR p = address;
		p += 7;
		while (*p && *p == '/')
			++p;
		address = p;
	}
#endif

	i = address.ReverseFind(':');
	if (i == -1)
		port = 23;
	else
	{
		port = (unsigned short)atoi(LPCTSTR(address.Mid(i + 1)));
		address = address.Left(i);
	}
	SetFocus();

	CString conf;
	if (dir.IsEmpty())
		conf = afxEmptyString;
	else
	{
		conf = dir;
		conf += name;
	}
	Connect(address, name, port, conf);
}


void CTermView::DrawLineOld(CDC &dc, LPSTR line,
							BYTE* pline_selstart, BYTE* pline_selend, int y)
{
	LPBYTE atbline = telnet->GetLineAttr(line);
	LPSTR hol = line;	//head of line
	LPSTR eol = line + telnet->site_settings.cols_per_page;	//end of line
	LPBYTE patb = atbline;	//attributes
	int l;	//每次輸出的文字長度
	int x = left_margin;	//x position
	CRect rc;	//用來輸出文字的rect
	rc.top = y;	rc.bottom = y + lineh;

//	開始輸出文字
	while (line < eol)
	{
		BYTE attr;	BOOL bsel;
		for (attr = *patb, bsel = (patb >= pline_selstart && patb < pline_selend);
			 attr == *patb && bsel == (patb >= pline_selstart && patb < pline_selend);
			 patb++);	//如果是相同屬性就向後移動

		l = patb - atbline;	//字串長度
		UINT drawopt = AppConfig.bktype ? 0 : ETO_OPAQUE;
		bool textout = (!IsAttrBlink(attr) || blight || telnet->sel_end != telnet->sel_start);
		if (IsBig5(hol, line + l - 1))	//最後一個字如果是中文前半段
		{
			int x2 = x + l * chw - chw;
			rc.left = x2;
			rc.right = x2 + chw * 2;
			BOOL bsel2 = (patb >= pline_selstart && patb < pline_selend);
			SetDCColors(&dc, *patb, bsel2);
			if (AppConfig.bktype)
			{
				if (GetAttrBkColor(*patb))
					drawopt |= ETO_OPAQUE;
				else
					FillBkRect(dc, rc, 0, bsel2);
			}
			if (!IsAttrBlink(*patb) || blight || telnet->sel_end != telnet->sel_start)
				ExtTextOut(dc, x2, y, drawopt, rc, line + l - 1, 2);	//先輸出整個中文，再切掉前半
			else // only draw background
				dc.ExtTextOut(x2, y, drawopt, rc, NULL, 0, NULL);

			drawopt = ETO_CLIPPED;
			rc.left = x, rc.right = x + chw * l;
			l++;
			SetDCColors(&dc, attr, bsel);
			if (AppConfig.bktype == 0 || GetAttrBkColor(attr))
				drawopt |= ETO_OPAQUE;
			else
				FillBkRect(dc, rc, 0, bsel);

			if (textout) // output text
				ExtTextOut(dc, x, y, drawopt, rc, line, l);
			else // only background
				dc.ExtTextOut(x, y, drawopt, rc, NULL, 0, NULL);

			line += l;
			patb++;
		}
		else
		{
			SetDCColors(&dc, attr, bsel);
			CRect rc(x, y, x + l*chw, y + lineh);
			if (AppConfig.bktype == 0 || GetAttrBkColor(attr))
				drawopt |= ETO_OPAQUE;
			else
			{
				FillBkRect(dc, rc, 0, bsel);
			}
			if (textout) // output text
				ExtTextOut(dc, x, y, drawopt, rc, line, l);
			else // only background
				dc.ExtTextOut(x, y, drawopt, rc, NULL, 0, NULL);

			line += l;
		}
		atbline = patb;
		x += chw * l;
	}
}

inline void CTermView::DrawLineBlinkOld(CDC &dc, LPSTR line, int y)
{
	LPBYTE atbline = telnet->GetLineAttr(line);
	LPSTR hol = line;
	LPBYTE eoatb = atbline + telnet->site_settings.cols_per_page;
	LPBYTE pblink;
	int l = 0;
	int x = left_margin;
	while ((pblink = find_blink(atbline, eoatb, l)))
	{
		int pos = int(pblink - atbline);
		x += chw * pos;
		line += pos;
		//開始輸出文字
		CRect rc(x, y, 0, y + lineh);
		UINT drawopt = 0;
		//如果第一個字是中文後半
		if (hol != line && IsBig5(hol, line - 1))
		{
			rc.right = x + chw;
			rc.left -= chw;
			SetDCColors(&dc, *pblink);
			if (AppConfig.bktype == 0 || GetAttrBkColor(*pblink))
				drawopt = ETO_OPAQUE;
			if (blight)
				ExtTextOut(dc, rc.left, y, drawopt, rc, line - 1, 2);
			else
				FillBkRect(dc, rc, *pblink);
			rc.right -= chw;
			drawopt = ETO_CLIPPED;
			BYTE tmpatb = *(pblink - 1);
			SetDCColors(&dc, tmpatb);
			if (AppConfig.bktype == 0 || GetAttrBkColor(tmpatb))
				drawopt |= ETO_OPAQUE;

			FillBkRect(dc, rc, tmpatb);
			if (blight || !IsAttrBlink(tmpatb))
				ExtTextOut(dc, rc.left, y, drawopt, rc, line - 1, 2);

			//略過第一個字的屬性色彩和文字內容
			line++;
			x += chw;
			l--;
			pblink++;
		}

		drawopt = 0;
		//如果最後一個字是中文前半
		if (IsBig5(hol, line + l - 1))
		{
			rc.left = x + chw * l - chw;
			rc.right = rc.left + chw * 2;
			BYTE tmpatb = *(pblink + l);
			SetDCColors(&dc, tmpatb);
			if (AppConfig.bktype == 0 || GetAttrBkColor(tmpatb))
				drawopt = ETO_OPAQUE;

			if (blight || !IsAttrBlink(tmpatb))
				ExtTextOut(dc, rc.left, y, drawopt, rc, line + l - 1, 2);
			else
				FillBkRect(dc, rc, tmpatb);

			drawopt = ETO_CLIPPED;
			rc.right -= chw;
			l++;
		}
		else
			rc.right = x + l * chw;

		rc.left = x;
		SetDCColors(&dc, *pblink);
		if (AppConfig.bktype == 0 || GetAttrBkColor(*pblink))
		{
			drawopt |= ETO_OPAQUE;
			if (blight)
				ExtTextOut(dc, rc.left, y, drawopt, rc, line, l);
			else
				FillBkRect(dc, rc, *pblink);
		}
		else
		{
			FillBkRect(dc, rc, *pblink);
			if (blight)
				ExtTextOut(dc, rc.left, y, drawopt, rc, line, l);
		}

		//輸出文字結束

		atbline = pblink + l;
		line += l;
		x += chw * l;
	}
}

BOOL CTermView::ExtTextOut(CDC& dc, int x, int y, UINT nOptions, LPCRECT lpRect, LPCTSTR lpszString, UINT nCount)
{
	char buf[161];	// maximum of every line on the bbs screen.
	wchar_t wbuf[161]/* = {0}*/;

	if (nCount > 1)
	{
		// FIXME: support unicode addon here?
		// We can convert characters not defined in big5 to unicode with our own table
		// gotten from UAO project.
		// ::ExtTextOutW() can be used to draw unicode strings, even in Win 98.

		switch (telnet->site_settings.text_output_conv)
		{
		case 0:
			break;
		case GB2BIG5:
			chi_conv.GB2Big5(lpszString, buf, nCount);
			lpszString = buf;
			break;
		case BIG52GB:
			chi_conv.Big52GB(lpszString, buf, nCount);
			lpszString = buf;
		};
	}
	UINT cp_id = this->GetCodePage();
	if (cp_id != 950)
	{
		// in全為dbcs, in[n]=out[n/2+1]; in全為acsii, in[n]=out[n+1]
		memset(wbuf, 0, (nCount + 1)*sizeof(wchar_t)); // <-- in bytes
		::MultiByteToWideChar(cp_id, 0, lpszString, nCount, wbuf, nCount + 1);
		return ::ExtTextOutW(dc.GetSafeHdc(), x, y, nOptions, lpRect, wbuf, wcslen(wbuf) , NULL);
	}

	size_t size = g_ucs2conv.Big52Ucs2(lpszString, wbuf, nCount);
	wbuf[size] = 0;
	return ::ExtTextOutW(dc.GetSafeHdc(), x, y, nOptions, lpRect, wbuf, wcslen(wbuf) , NULL);
}


void CTermView::CopySelText()
{
	CString seltext = GetSelText();

	if (!seltext.IsEmpty())
	{
		if (IsWinNT())
		{
			UINT cp_id = GetCodePage();
			int len = seltext.GetLength() + 1 ;
			wchar_t* pwbuf = new wchar_t[len];
			memset(pwbuf, 0, len * sizeof(wchar_t));

			if (cp_id == 950)
			{
				g_ucs2conv.Big52Ucs2((const char*)seltext , pwbuf);
				CClipboard::SetTextW(m_hWnd, pwbuf);
				paste_block = telnet->sel_block;
			}
			else
			{
				::MultiByteToWideChar(cp_id, 0, seltext,
									  len, pwbuf, len);

				CClipboard::SetTextW(m_hWnd, pwbuf);
				paste_block = telnet->sel_block;
			}
			if (pwbuf)
				delete [] pwbuf;
		}
		// !WinNT
		else
		{
			CClipboard::SetText(m_hWnd, seltext);
			paste_block = telnet->sel_block;
		}
	}

	if (AppConfig.auto_cancelsel)
	{
		telnet->sel_end = telnet->sel_start;
		Invalidate(FALSE);
	}
}

void CTermView::OnSearchPlugin(UINT id)
{
	id -= CSearchPluginCollection::ID_SEARCHPLUGIN00;
	AppConfig.hyper_links.OpenURL(SearchPluginCollection.UrlForSearch(id, GetSelText()));
}

void CTermView::OnTranslation()
{
	AppConfig.hyper_links.OpenURL(SearchPluginCollection.UrlForTranslate(GetSelText()));
}

void CTermView::OnRButtonDblClk(UINT nFlags, CPoint point_In)
{
	if (CanUseMouseCTL())
		MouseCTL_OnRButtonDblClk(m_hWnd, nFlags, point_In);
}

void CTermView::OnRButtonDown(UINT nFlags, CPoint point_In)
{
	if (CanUseMouseCTL())
		MouseCTL_OnRButtonDown(m_hWnd, nFlags, point_In);
}

void CTermView::OnRButtonUp(UINT nFlags, CPoint point_In)
{
	CPoint point2;

	if (CanUseMouseCTL())
	{
		MouseCTL_OnRButtonUp(m_hWnd, nFlags, point_In);
	}
	else
	{
		POINT pt;
		CPoint point2;

		pt = point_In;
		ClientToScreen(&pt);
		point2.x = pt.x;
		point2.y = pt.y;
		OnContextMenu(this, point2);
	}
}

void CTermView::OnMButtonDown(UINT nFlags, CPoint point_In)
{
	if (CanUseMouseCTL())
		MouseCTL_OnMButtonDown(m_hWnd, nFlags, point_In);
}

void CTermView::OnMButtonUp(UINT nFlags, CPoint point_In)
{
	if (CanUseMouseCTL())
		MouseCTL_OnMButtonUp(m_hWnd, nFlags, point_In);
}

void CTermView::OnMButtonDblClk(UINT nFlags, CPoint point_In)
{
	if (CanUseMouseCTL())
		MouseCTL_OnMButtonDblClk(m_hWnd, nFlags, point_In);
}

BOOL CTermView::CanUseMouseCTL()
{
	BOOL xRet;

	xRet = FALSE;

	if (AppConfig.use_MouseCTL && telnet && telnet->is_ansi_editor == FALSE)
		xRet = TRUE;

	return xRet;
}
