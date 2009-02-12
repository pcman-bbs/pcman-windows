// TelnetConn.cpp : implementation file
//

#include "stdafx.h"
#include "TelnetConn.h"

#include "TermView.h"
#include "listdlg.h"
#include "mainfrm.h"
#include "StrUtils.h"
#include "WinUtils.h"
#include "Clipboard.h"

#include <mmsystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
typedef void (CTelnetConn::*ansi_func)(int);
typedef void (CTelnetConn::*ansi_func2)(int,int);
typedef void (CTelnetConn::*ansi_funcs)(CString);
struct ANSI_TAB
{
	char code;
	char param_type;	//-1表示直接傳入LPCTSTR
	char d1;	//-1表示無效
	char d2;	//-1表示無效
	ansi_func pfunc;	//ansi function
};

struct ANSI_TAB ansi_tab[]={
	{'m',-1,-1,-1,(ansi_func)CTelnetConn::SetCurrentAttributes},	//color
	{'K',1,0,-1,CTelnetConn::ClearCurrentLine},	//clear current line
	{'H',2,1,1,(ansi_func)CTelnetConn::GotoXY},	//move cursor
	{'F',2,1,1,(ansi_func)CTelnetConn::GotoXY},	//move cursor
	{'r',2,-1,-1,(ansi_func)CTelnetConn::SetScrollRange},	//set scroll range
	{'J',1,-1,-1,CTelnetConn::ClearScreen},	//clear screen
	{'E',1,1,-1,CTelnetConn::LineFeed},	//line feed
	{'L',1,1,-1,CTelnetConn::InsertLines},	//insert line
	{'A',1,1,-1,CTelnetConn::GoUp},	//up
	{'B',1,1,-1,CTelnetConn::GoDown},	//down
	{'C',1,1,-1,CTelnetConn::GoRight},	//right
	{'D',1,1,-1,CTelnetConn::GoLeft},	//left
	//新增的ANSI支援
	{'s',1,-1,-1,CTelnetConn::SaveCursorPos},
	{'u',1,-1,-1,CTelnetConn::RestoreCursorPos},
	{'@',1,1,-1,CTelnetConn::InsertChar},	//insert char
	{'M',1,1,-1,CTelnetConn::DeleteLines},	//delete n line
	{'P',1,1,-1,CTelnetConn::DeleteChar},	//delete char
	{'U',1,1,-1,CTelnetConn::NextPages},	//next n page
	{'V',1,1,-1,CTelnetConn::PrevPages},	//previous n page
	{'Z',1,1,-1,CTelnetConn::BackTab},	//cursor back tab
	{'h',1,-1,-1,CTelnetConn::SetMode},	//set mode
	{'l',1,-1,-1,CTelnetConn::ResetMode},	//reset mode
	{'n',1,-1,-1,CTelnetConn::DeviceStatusReport},	//Device Status Report
	{0,0,-1,-1,0}	//null
};
*/

/////////////////////////////////////////////////////////////////////////////
// CTelnetConn

BYTE CTelnetConn::buffer[];
CString CTelnetConn::downloaded_article;
int CTelnetConn::current_download_line;

CTelnetConn::CTelnetConn()
{
	screen = NULL;
	idle_time = 0;
	time = 0;
	cur_attr = 7;
	attr_flags = 0;

	ansi_mode = 0;
	pansi_param = ansi_param;
	insert_mode = 1;

	is_getting_article = 0;
	get_article_with_ansi = 0;
	get_article_in_editor = 0;

	old_cursor_pos.x = old_cursor_pos.y = cursor_pos.x = cursor_pos.y = 0;

	is_lookup_host = true;
	is_telnet = true;

	scr_top = 0;
	scr_bottom = 23;

	buf = NULL;
	last_byte = NULL;

	sel_start.x = 0;
	sel_start.y = 0;
	sel_end.x = 0;
	sel_end.y = 0;
	sel_block = false;

	site_settings.localecho = 0;
	*ansi_param = 0;
	pansi_param = ansi_param;

	site_settings.paste_autowrap = 0;

	key_map = NULL;
}

CTelnetConn::~CTelnetConn()
{
	if (is_connected)
		Shutdown();
	if (telnet)
		Close();

	for (int i = 0; i < CTermView::all_telnet_conns.GetSize(); i++)
	{
		if (CTermView::all_telnet_conns[i] == (void*)this)
		{
			CTermView::all_telnet_conns.RemoveAt(i);
			break;
		}
	}

	if (screen)
	{
		for (long i = 0;i < site_settings.line_count;i++)
			delete []screen[i];
		delete []screen;
	}

	if (key_map)
		key_map->Release();

	if (site_settings.text_input_conv || site_settings.text_output_conv)
		view->chi_conv.Release();

}


/////////////////////////////////////////////////////////////////////////////
// CTelnetConn member functions

void CTelnetConn::OnReceive(int nErrorCode)
{
	if (view->telnet != this)
	{
		int idx = view->parent->ConnToIndex(this);
		TCITEM tcitem;
		tcitem.mask = TCIF_IMAGE;
		view->parent->tab.GetItem(idx, &tcitem);

		if (tcitem.iImage != 5)
		{
			tcitem.iImage = 2;
			view->parent->tab.SetItem(idx, &tcitem);
		}
	}
	else
	{
		SCROLLINFO info;
		view->GetScrollInfo(SB_VERT, &info);
		if (info.nPos != (int)(info.nMax - info.nPage))
		{
			scroll_pos = info.nPos = (info.nMax - info.nPage);
			info.fMask = SIF_POS;
			view->SetScrollInfo(SB_VERT, &info);
			view->Invalidate(FALSE);
		}
	}
	ReceiveData();
}

inline void CTelnetConn::OnIAC()
{
	BYTE ret[24];
	BYTE* pbuf = (BYTE*)ansi_param + 1;
	switch (*pbuf)
	{
	case WILL:
		pbuf++;
		ret[0] = IAC;
		ret[2] = *buf;
		switch (*pbuf)
		{
		case ECHO:
		case SUPRESS_GO_AHEAD:
			ret[1] = DO;
			break;
		default:
			ret[1] = DONT;
		}
		Send(ret, 3);
		break;
	case WONT:
		return;
	case DO:
		pbuf++;
		ret[0] = IAC;
		ret[2] = *buf;
		switch (*pbuf)
		{
		case TERMINAL_TYPE:
			ret[1] = WILL;
			Send(ret, 3);
			break;
		case NAWS:
			ret[1] = WILL;
			Send(ret, 3);
			SendNaws();
			break;
		default:
			ret[1] = WONT;
			Send(ret, 3);
		}
		break;
	case DONT:
		break;
	case SB:
		{
			pbuf++;
			switch (*pbuf)
			{
			case TERMINAL_TYPE:
				pbuf++;
				if (*pbuf == SEND)
				{
					int l = site_settings.termtype.GetLength() + 6;
					BYTE *type = new BYTE[l];
					type[0] = IAC;	type[1] = SB;	type[2] = TERMINAL_TYPE;	type[3] = IS;
					strcpy((char*)type + 4, LPCTSTR(site_settings.termtype));	type[l-2] = IAC;	type[l-1] = SE;
					Send(type, l);
					delete []type;
				}
				break;
			default:
				break;
			}	//end switch
			return;
		}
	default:
		return;
	}
}

class CDownloadArticleDlg : public CDialog
{
public:
	CDownloadArticleDlg(CTelnetConn* _telnet)
			: CDialog(IDD_DOWNLOADING), telnet(_telnet)
	{
	}

	BOOL OnInitDialog()
	{
		CDialog::OnInitDialog();
		return TRUE;
	}

	void OnCancel()
	{
		telnet->CopyArticleComplete(true);
		CDialog::OnCancel();
	}

	void PostNcDestroy();
protected:
	CTelnetConn* telnet;
};

static CDownloadArticleDlg* download_article_dlg = NULL;

void CDownloadArticleDlg::PostNcDestroy()
{
	CDialog::PostNcDestroy();
	if (download_article_dlg)
	{
		download_article_dlg = NULL;
		delete this;
	}
}

void CTelnetConn::OnText()
{
	while (buf < last_byte)
	{
		if (*buf == IAC)
		{
			buf--;
			break;
		}

		if (ansi_mode)
		{
			if (*ansi_param != '[')	//如果是ESC ' '類控制碼
			{
				if (*buf == 0x1b)
					buf++;
				if (buf >= last_byte)
					return;
				if (*buf != '[')
					ansi_mode = 0;
			}
			else
			{
				if (*buf >= '@' && *buf <= '~')	//如果已經結束ANSI mode
					ansi_mode = 0;

				if (*buf == 0x0A)
				{
					ansi_mode = 0;
					buf--;

				}
			}

			if ((pansi_param - ansi_param) < 63)	//檢查是否已經超出ansi_buffer,64th byte是結尾0
			{
				*pansi_param = *buf;
				pansi_param++;
			}

			if (ansi_mode == 0)	//如果已經結束ANSI mode，開始處理控制碼
			{
				*pansi_param = 0;
				ProcessAnsiEscapeSequence();
				*ansi_param = 0;	//清空ANSI參數
				pansi_param = ansi_param;
			}
		}	//end if(ansi_mode)
		else
			switch (*buf)
			{
			case 13:	//Carrige Return
				if (is_ansi_editor)
					EditorCarriageRetiurn();
				cursor_pos.x = 0;
				break;
			case 10:	//line feed
				if (!is_ansi_editor)
					LineFeed();
				break;
			case 8:
				Back();
				break;
			case 7:
				if (::GetForegroundWindow() != view->parent->m_hWnd)
					view->doflash = 1;

				switch (AppConfig.sound)
				{
				case 1:
					//	From Putty
//				if ( IsWinNT() )		// Windows NT
//					Beep(400,40);
//				else								//Windows 9X
					MessageBeep(0xffffffff);
					break;
				case 2:
					PlaySound(NULL, NULL, SND_ASYNC);
					PlaySound(AppConfig.wavepath, NULL, SND_FILENAME | SND_ASYNC);
					break;
				}

				if (view->telnet != this)
				{
					int idx = view->parent->ConnToIndex(this);
					TCITEM tcitem;
					tcitem.mask = TCIF_IMAGE;
					tcitem.iImage = 5;
					view->parent->tab.SetItem(idx, &tcitem);
				}
				if (view->auto_switch)
				{
					view->parent->SwitchToConn(this);
					view->parent->SetForegroundWindow();
					view->parent->BringWindowToTop();
				}
				break;
			case 0x1b:	//ansi control code
				ansi_mode = 1;
				break;
			case 9:
				{
					UINT x = (cursor_pos.x / 8) * 8 + 8;
					cursor_pos.x = x;
					if (cursor_pos.x > site_settings.cols_per_page - 1)
						cursor_pos.x = site_settings.cols_per_page - 1;
					break;
				}
			default:	//normal plain text
				if (cursor_pos.y > last_line || cursor_pos.y < first_line)
					break;

				if (site_settings.line_wrap &&
					cursor_pos.x >= site_settings.cols_per_page
					&& !is_ansi_editor)
				{
					cursor_pos.x = 0;
					LineFeed();
				}

				if (cursor_pos.x < site_settings.cols_per_page)
				{
					LPSTR curstr = screen[cursor_pos.y];
					LPBYTE pcuratb = GetLineAttr(curstr);

					if (*buf >= ' ')
					{
						if (is_ansi_editor && insert_mode)
						{
							for (int i = site_settings.cols_per_page - 1;i > cursor_pos.x;i--)
							{
								curstr[i] = curstr[i-1];
								pcuratb[i] = pcuratb[i-1];
							}
							SetUpdateWholeLine(cursor_pos.y);
						}
						else
						{
							// reduce unnecessary update.  does this work?
							if (curstr[cursor_pos.x] != *buf || pcuratb[cursor_pos.x] != cur_attr)
								SetUpdateLine(cursor_pos.y, (BYTE)cursor_pos.x);	//將這行設為等待更新
						}

						pcuratb[cursor_pos.x] = cur_attr;
						curstr[cursor_pos.x] = *buf;

						cursor_pos.x++;
					}
				}
			}
		buf++;
	}
	UpdateCursorPos();
	CheckHyperLinks();

	CheckStrTrigger();

//Update Lines
	if (this == view->telnet)
	{
		view->HideCaret();
		for (int i = first_line;i <= last_line;i++)
		{
			if (GetUpdateLine(i))
			{
				UpdateLine(i);
				RemoveUpdateLine(i);
			}
		}
		view->UpdateWindow();
		view->ShowCaret();

		if (is_getting_article)
		{
			//為了相容Firebird BBS
			//如最後一行還是空白則等待BBS server傳送最後一行
			char* last_line_txt = screen[ last_line ];
			while (*last_line_txt == ' ')
				last_line_txt++;

            char* pos = last_line_txt;
            while (*pos != '~') ++pos;
            ++pos;
            int current_line = atoi(pos);

            if ((int)(last_line_txt - screen[ last_line ]) < (int)strlen(screen[ last_line ]))
			{
                int stamp = 1;
                if (current_download_line == 0)
                    current_download_line = current_line;
                else
				{
                    stamp = current_line - current_download_line;
                    current_download_line = current_line;
                }

                CString tmp;
                if (get_article_with_ansi)
				{
                    if(stamp == 2)
					{
                        downloaded_article += GetLineWithAnsi(last_line - 2);
                        downloaded_article.TrimRight(" ");
                        downloaded_article += "\r\n";
                    }
					downloaded_article += GetLineWithAnsi(last_line - 1);
                }
				else
				{
                    if(stamp == 2)
					{
                        downloaded_article += screen[ last_line - 2 ];
                        downloaded_article.TrimRight(" ");
                        downloaded_article += "\r\n";
                    }
					downloaded_article += screen[ last_line - 1 ];
                }
				downloaded_article.TrimRight(" ");
				downloaded_article += "\r\n";

				if (IsEndOfArticleReached())
				{
					CopyArticleComplete();
				}
				else
				{
					const char* key = key_map->FindKey(VK_DOWN, 0);
					SendString(key ? key : "^[[B");
					if (!download_article_dlg)
					{
						download_article_dlg = new CDownloadArticleDlg(this);
						download_article_dlg->DoModal();
					}
				}
			}
		}
	}
}

void CTelnetConn::UpdateCursorPos()
{
	if (this != ((CTermView*)view)->telnet)
		return;
	CPoint pos;
	pos.x = cursor_pos.x * view->chw + view->left_margin;
	int y;
	if (is_ansi_editor)
	{
		y = cursor_pos.y - scroll_pos + 1;
		pos.y = y * view->lineh + view->top_margin - 2;
	}
	else
	{
		y = cursor_pos.y - first_line + 1;
		pos.y = y * view->lineh + view->top_margin - 2;
	}
	view->SetCaretPos(pos);
	if (y < 1 || y > site_settings.lines_per_page)
		view->HideCaret();
	else
	{
		view->ShowCaret();
	}
}

void CTelnetConn::OnClose(int nErrorCode)
{
	ClearAllFlags();
	is_disconnected = true;

	Close();

	TCITEM tcitem;
	tcitem.mask = TCIF_IMAGE;
	tcitem.iImage = 1;

	int idx = view->parent->ConnToIndex(this);

	if (idx < view->parent->tab.GetItemCount())	//如果已經斷線卻未關閉畫面
	{
		view->parent->tab.SetItem(idx, &tcitem);	//改為紅色圖示
		//如果設定自訂重連，而且在時間內被斷線，且間隔時間已到
		if (site_settings.auto_reconnect &&
			time <= site_settings.connect_interval
			&& site_settings.reconnect_interval == 0)
		{
			view->ReConnect(this);	//重新連線
		}
		else
		{
			if (AppConfig.auto_close)	//如果設定為斷線自動關閉
			{
				view->parent->SwitchToConn(this);	//切換到已經斷線的畫面
				view->parent->CloseConn(view->parent->ConnToIndex(this));	//關閉畫面
			}
		}
	}
}


void CTelnetConn::UpdateLine(int line)
{
	CRect rc;
	rc.top = (line - scroll_pos) * view->lineh + view->top_margin;
	LPBYTE attrib = GetLineAttr(line);
	rc.left = view->left_margin + (*(attrib - 2) - 1) * view->chw;
	rc.right = view->left_margin + (*(attrib - 3) + 1) * view->chw;
	rc.bottom = rc.top + view->lineh;
	::InvalidateRect(view->m_hWnd, rc, FALSE);
	rc.top = rc.bottom - 1;
	rc.left = view->left_margin;
	rc.right = view->left_margin + view->chw * site_settings.cols_per_page;
	::InvalidateRect(view->m_hWnd, rc, FALSE);
}

void CTelnetConn::ClearCurrentLine(int param)
{
	if (cursor_pos.x > site_settings.cols_per_page)
		cursor_pos.x = site_settings.cols_per_page;
	if (cursor_pos.x < 0)
		cursor_pos.x = 0;

	CRect rc;
	switch (param)
	{
	default:
	case 0:
		memset((screen[cursor_pos.y] + cursor_pos.x), ' ', site_settings.cols_per_page - cursor_pos.x);
		memset((GetLineAttr(cursor_pos.y) + cursor_pos.x), 7, site_settings.cols_per_page - cursor_pos.x);
		break;
	case 1:
		memset(screen[cursor_pos.y], ' ', cursor_pos.x);
		memset(GetLineAttr(cursor_pos.y), 7, cursor_pos.x);
		break;
	case 2:
		memset(screen[cursor_pos.y], ' ', site_settings.cols_per_page);
		screen[cursor_pos.y][site_settings.cols_per_page] = 0;
		memset(GetLineAttr(cursor_pos.y), 7, site_settings.cols_per_page);
	}
	SetUpdateWholeLine(cursor_pos.y);
}

void CTelnetConn::ClearScreen(int param)
{
	long a;

	for (a = 0;a < site_settings.line_count - site_settings.lines_per_page;a++)	//中間每行的指標都往前移動24行
	{
		//2004/2/1 終於想到更聰明的!!
		//將要丟棄的舊行緩衝區直接和要新配置的新頁緩衝區swap!!! 這樣最簡便 :
		char* tmpline = screen[a];
		screen[a] = screen[a+site_settings.lines_per_page];
		RemoveUpdateLine(a);

		screen [a+site_settings.lines_per_page] = tmpline;
//		InitNewLine(tmpline);
//		SetUpdateWholeLine(a+site_settings.lines_per_page);
	}

	for (a = site_settings.line_count - site_settings.lines_per_page;a < site_settings.line_count;a++)	//清空新的一頁
	{
		InitNewLine(screen[a]);
		SetUpdateWholeLine(a);
	}

//	現在所有畫面已經向上捲動一頁了

	switch (param)
	{
	case 2:
		break;
	case 0:
	default:
		for (a = site_settings.line_count - site_settings.lines_per_page;a < cursor_pos.y;a++)	//將舊的畫面複製一份過來
		{
			if (a < site_settings.lines_per_page)
				break;
			memcpy(screen[a], screen[a-site_settings.lines_per_page], GetLineBufLen());
			SetUpdateWholeLine(a);
		}
		if (cursor_pos.y >= site_settings.lines_per_page)
		{
			memcpy(screen[cursor_pos.y], screen[cursor_pos.y-site_settings.lines_per_page], cursor_pos.x);
			memcpy(GetLineAttr(cursor_pos.y), GetLineAttr(cursor_pos.y - site_settings.lines_per_page), cursor_pos.x);
		}
		break;
	case 1:
		//將舊的畫面複製一份過來
		if (cursor_pos.y >= site_settings.lines_per_page)
		{
			memcpy(screen[cursor_pos.y],
				   screen[cursor_pos.y-site_settings.lines_per_page] + cursor_pos.x, site_settings.cols_per_page - cursor_pos.x);
			memcpy(GetLineAttr(cursor_pos.y),
				   GetLineAttr(cursor_pos.y - site_settings.lines_per_page) + cursor_pos.x, site_settings.cols_per_page - cursor_pos.x);
		}
		SetUpdateWholeLine(cursor_pos.y);
		for (a = cursor_pos.y + 1;a < site_settings.line_count;a++)
		{
			if (a < site_settings.lines_per_page)
				break;
			memcpy(screen[a], screen[a-site_settings.lines_per_page], GetLineBufLen());
			SetUpdateWholeLine(a);
		}
		break;
	}
}

void CTelnetConn::InsertLines(int num)	//Insert blank line at current row (shift screen down)
{
	if (num <= 0)	num = 1;
	if (num > site_settings.lines_per_page)	num = site_settings.lines_per_page;

	int i;
	for (i = first_line + scr_bottom;i > cursor_pos.y;i--)
	{
		//swap(screen[i],screen[i-num]);
		char* tmpline = screen[i];
		screen[i] = screen[i-num];
		screen[i-num] = tmpline;
		SetUpdateWholeLine(i);
		InitNewLine(tmpline);
		SetUpdateWholeLine(i - num);
	}
}



void CTelnetConn::GotoXY(int line, int col)
{
	if (line <= 0)	line = 1;
	if (col <= 0)	col = 1;
	if (line > site_settings.lines_per_page)	line = site_settings.lines_per_page;
	if (col > site_settings.cols_per_page)	col = site_settings.cols_per_page;

	if (line <= site_settings.lines_per_page)
		cursor_pos.y = first_line + line - 1;
	else
		cursor_pos.y = first_line + site_settings.lines_per_page - 1;

	if (line < 1)
		cursor_pos.y = 0;

	if (col <= site_settings.cols_per_page)
		cursor_pos.x = col - 1;
	else
		cursor_pos.x = site_settings.cols_per_page - 1;

	if (col < 0)
		cursor_pos.x = 0;
}

void CTelnetConn::SetScrollRange(int pt, int pb)
{
	if (pt <= 0)	pt = 1;
	if (pt > site_settings.lines_per_page)	pt = site_settings.lines_per_page;
	if (pb <= 0)	pb = site_settings.lines_per_page;
	if (pb > site_settings.lines_per_page)	pb = site_settings.lines_per_page;

	scr_top = pt - 1;
	scr_bottom = pb - 1;
	if (scr_top > scr_bottom)
	{
		scr_top = 0;
		scr_bottom = site_settings.lines_per_page - 1;
	}
}

void CTelnetConn::LineFeed()
{
	int _firstl = first_line + scr_top;
	int _lastl = first_line + scr_bottom;
	if (cursor_pos.y < _lastl)	//if at the middle of screen
		cursor_pos.y++;
	else	//at the buttom of the scroll range
	{
		if (scr_top == 0)
			_firstl = 0;

		LPSTR tmpline = screen[_firstl];

		for (int i = _firstl;i < _lastl;i++)
		{
			screen[i] = screen[i+1];
			SetUpdateWholeLine(i);
		}
		screen[_lastl] = tmpline;
		InitNewLine(tmpline);
		SetUpdateWholeLine(_lastl);
	}
}


void CTelnetConn::ScrollUp()
{
//		scroll up
	int _top = first_line + scr_top;
	int _bottom = first_line + scr_bottom;
	char* tmp = screen[_top];
	for (int i = _top;i < _bottom;i++)
	{
		screen[i] = screen[i+1];
	}
//	screen[_bottom]=new_line(site_settings.cols_per_page,attrib[_bottom]);
	screen[_bottom] = tmp;
//	attrib[_bottom]=tmp;
	InitNewLine(tmp);

	if (((CTermView*)view)->telnet == this)
		view->Invalidate(FALSE);
}

void CTelnetConn::ScrollDown()
{
//		scroll down
	int _top = first_line + scr_top;
	int _bottom = first_line + scr_bottom;
	LPSTR tmpline = screen[_bottom];
	for (int i = _bottom;i > _top;i--)
	{
		screen[i] = screen[i-1];
		SetUpdateWholeLine(i);
	}
	screen[_top] = tmpline;
	InitNewLine(tmpline);
	SetUpdateWholeLine(_top);
}

void CTelnetConn::LineFeed(int param)
{
//		line feed
	if (param <= 0)
		param = 1;
	for (int i = 0;i < param;i++)
		LineFeed();
}


int find_sub_str(char* str, char* sub)
{
	char* pstr = str;
	int len = strlen(sub);
	while (*pstr)
	{
		if (strncmp(pstr, sub, len) == 0)
			return int(pstr -str);
		pstr++;
	}
	return -1;
}

void CTelnetConn::OnConnect(int nErrorCode)
{
	TCITEM item;
	item.mask = TCIF_IMAGE;
	if (nErrorCode)
	{
		CString con_failed;
		con_failed.LoadString(IDS_CON_FAILED);
		strncpy(screen[first_line], con_failed, con_failed.GetLength());

		ClearAllFlags();
		is_disconnected = true;

		int idx = view->parent->ConnToIndex(this);
		item.iImage = 1;
		view->parent->tab.SetItem(idx, &item);
		if (view->telnet == this)
			view->InvalidateRect(FALSE);
	}
	else
	{
		ClearAllFlags();
		is_connected = true;

		int idx = view->parent->ConnToIndex(this);
		item.iImage = 0;
		view->parent->tab.SetItem(idx, &item);
		CString ads_port = address;
		if (port != 23)
		{
			char port_str[16];
			sprintf(port_str, ":%d", port);
			ads_port += port_str;
		}
		CString str("s");
		str += name;
		str += '\t';
#ifdef _COMBO_
		str += LPCTSTR(ads_port) + 9;	// strlen("telnet://") = 9;
#else
		str += ads_port;
#endif
		str += '\t';
		if (!cfg_path.IsEmpty())
			str += cfg_path.Left(cfg_path.GetLength() - name.GetLength());

		view->parent->AddToHistoryMenu(str);
		view->parent->AddToHistory(ads_port);
	}
}

int CTelnetConn::Send(const void *lpBuf, int nBufLen)
{
	idle_time = 0;
	if (!nBufLen)
		return 0;

	if (!is_ansi_editor)
	{
		int r = ::send(telnet, (char*)lpBuf, nBufLen, 0);
		return r;
	}

	buf = (LPBYTE)lpBuf;
	last_byte = (LPBYTE)lpBuf + nBufLen;
	OnText();
	return 0;
}

const char* memstr(const char *src, const char *end, const char *key)
{
	int keylen = strlen(key);
	if (!keylen)
		return NULL;

	end -= keylen;
	for (; src <= end; src++)
	{
		if (memcmp(src, key, keylen) == 0)
			return src;
	}
	return NULL;
}

void CTelnetConn::CheckStrTrigger()
{
	//設定first=n，第n次才開始觸發，則每收到一次msg就first--
	if (site_settings.triggers.count > 0)
	{
//		for(int i=first_line;i<=last_line;i++)
		{
			CTriggerItem* item = site_settings.triggers.pfirst;
			CTriggerItem* pprev = NULL;
			while (item)
			{
//				if( strstr(screen[i],(LPSTR)(LPCTSTR)item->msg) )
				if (memstr((char*)buffer, (char*)last_byte , (LPSTR)(LPCTSTR)item->msg))
				{
					if (item->first > 1)
						item->first--;
					else
					{
						const char* enter;
						if (!key_map || !(enter = key_map->FindKey(VK_RETURN, 0)))
							enter = "\x0d";
						CString respond = UnescapeControlChars(LPCTSTR(item->respond) + 1);
						respond.Replace("\x0d\x0a", enter);
						respond.Replace("\x0d", enter);
						SendMacroString(respond);
						if (item->count > 0)	//使用觸發次數(不計算次數的count=0)
						{
							item->count--;
							if (item->count == 0)	//如果次數到了，就不再觸發，並且移除此觸發項目
							{
								if (pprev)	//如果有前一項
									pprev->pnext = item->pnext;
								else	//如果沒有前一項，表示是第一項
									site_settings.triggers.pfirst = item->pnext;
								if (item == site_settings.triggers.plast)	//如果是刪除最後一項
									site_settings.triggers.plast = pprev;
								delete item;
								site_settings.triggers.count--;
								item = pprev;
							}
						}
					}
				}
				pprev = item;	//紀錄前一個，以用於刪除項目時
				item = item ? item->pnext : NULL;	//next item
			}
		}
	}

}

void CTelnetConn::CreateBuffer()
{
	last_line = site_settings.line_count - 1;

	if (site_settings.line_count < site_settings.lines_per_page)
		site_settings.line_count = site_settings.lines_per_page;

	first_line = site_settings.line_count - site_settings.lines_per_page;
	scr_bottom = site_settings.lines_per_page - 1;

	cursor_pos.x = 0;
	if (is_ansi_editor)
	{
		cursor_pos.y = 0;
		scroll_pos = 0;
		first_line = 0;
	}
	else
	{
		cursor_pos.y = first_line;
		scroll_pos = site_settings.line_count - site_settings.lines_per_page;
	}

	screen = new LPSTR[site_settings.line_count*2];
//原本錯誤發生在這一行....
//	attrib=(LPBYTE*)&screen[site_settings.line_count];
//因為 screen的型態是LPSTR*,不是LPSTR,所以將screen+1 會等於screen+sizeof(LPSTR)=而非LPBYTE(screen)+1

	for (long i = 0;i < site_settings.line_count;i++)
		screen[i] = AllocNewLine();
}

inline void CTelnetConn::ReceiveData()
{
	int len = Recv(buffer, 4096);

	last_byte = buffer + len;
	for (buf = buffer; buf < last_byte ; buf++)
	{
		if (*((BYTE*)ansi_param) == IAC)
		{
			if ((pansi_param - ansi_param) < 63)	//檢查是否已經超出ansi_buffer,64th byte是結尾0
			{
				*pansi_param = *buf;
				pansi_param++;
			}
			if ((long(pansi_param - ansi_param) == 3 && ((BYTE)*(ansi_param + 1)) != SB) || *buf == SE)
			{
				OnIAC();
				*ansi_param = 0;	//清除IAC指令暫存
				pansi_param = ansi_param;
			}
		}
		else if (*buf == IAC)
		{
			if (!*ansi_param)
				//如果不是sub negotiation的結尾 (IAC SB...SB IAC)
			{
				*((BYTE*)pansi_param) = IAC;
				pansi_param++;
			}
			else
			{
				if ((pansi_param - ansi_param) < 63)	//檢查是否已經超出ansi_buffer,64th byte是結尾0
				{
					*pansi_param = *buf;
					pansi_param++;
				}
			}
		}
		else
			OnText();
	}
}

void CTelnetConn::LocalEcho(void* str, int len)
{
	buf = (LPBYTE)str;
	last_byte = (LPBYTE)buf + len;
	OnText();
}

int CTelnetConn::SendString(LPCTSTR str)
{
	const char* enter = NULL;
	const char* CRLF = "\x0d\x0a";
	if (key_map)
		enter = key_map->FindKey(VK_RETURN, 0);

	if (!enter)	enter = "\x0d";

	CString data;
	int l, limit;
	if (is_ansi_editor)
	{
		l = cursor_pos.x;
		if (view->paste_block)	//區塊貼上
		{
			char* eol;
			int x = cursor_pos.x;
			int rl = 0;
            while (eol = const_cast<char*>(strchr(str, 13)))
			{
				rl += Send(str, eol - str);
				cursor_pos.x = x;
				if (cursor_pos.y < last_line)
					cursor_pos.y++;
				else
					EditorLineFeed(NULL, NULL, 0);
				str = eol + 2;
				rl += 2;
			}
			if (*str)
				rl += Send(str, strlen(str));
			return rl;
		}
		if (!AppConfig.site_settings.paste_autowrap || strstr(str, "\x1b["))	//有控制碼
			return Send(str, strlen(str));
		else
			enter = CRLF;
		limit = AppConfig.ed_cols_per_page;
	}
	else	//不是ANSI Editor,是BBS
	{
		limit = AppConfig.site_settings.paste_autowrap_col;
		l = 0;
	}

	if (AppConfig.site_settings.paste_autowrap)	//如果使用自動換行
	{
		while (*str)
		{
			if (*str == 13 && *(str + 1) == 10)
			{
				str += 2;
				data += enter;
				l = 0;
			}
			else
			{
				int w = get_chw(LPSTR(str));
				if (w == 1)
				{
					while (str[w] && str[w] != 0x0d && str[w] != ' ' &&
						   isascii((unsigned char)str[w]))
						w++;
					if (str[w] == ' ')
						w++;
				}
				if ((l + w) > limit)
				{
					data += enter;
					l = 0;
				}

				for (int i = 0; i < w; i++)
					data += str[i];

				str += w;
				l += w;
			}
		}
	}
	else	//如果不用自動換行
	{
		data = str;
		data.Replace("\x0d\x0a", enter);
	}

	l = data.GetLength();
	if (is_ansi_editor)
		return Send(LPCTSTR(data), l);
	const char* pdata = LPCTSTR(data);
	char* _pdata = NULL;
	if (site_settings.text_input_conv)	// Encoding conversion
	{
		switch (site_settings.text_input_conv)
		{
		case BIG52GB:
			_pdata = new char[l];
			view->chi_conv.Big52GB(pdata, _pdata, l);
			pdata = _pdata;
			break;
		case GB2BIG5:
			_pdata = new char[l];
			view->chi_conv.GB2Big5(pdata, _pdata, l);
			pdata = _pdata;
		}
	}

	int sl = 0;
	int r = 0;
	while (l > sl)
	{
		r = Send((pdata + sl), l - sl);
		if (r == SOCKET_ERROR)
		{
			sl = r;
			break;
		}
		sl += r;
	}

	if (_pdata)
		delete []_pdata;

	return sl;
}

BOOL CTelnetConn::Create()
{
	telnet = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	if (bind(telnet, (sockaddr*)&addr, sizeof(addr)))
		return FALSE;

	WSAAsyncSelect(telnet, view->m_hWnd, WM_SOCKET, FD_READ | FD_CLOSE | FD_CONNECT);
	return TRUE;
}

void CTelnetConn::InsertChar(int n)
{
	if (n <= 0)
		n = 1;
	if (cursor_pos.x + n > site_settings.cols_per_page)
		n = site_settings.cols_per_page - cursor_pos.x;
	LPSTR curstr = screen[cursor_pos.y];
	int l = site_settings.cols_per_page - cursor_pos.x - n;
	LPSTR p = curstr + cursor_pos.x + n;
	LPSTR eol = curstr + site_settings.cols_per_page;
	while (eol >= p)
	{
		*eol = *(eol - n);
		eol--;
	}
	memset(curstr + cursor_pos.x, ' ', n);
	memset(GetLineAttr(cursor_pos.y) + cursor_pos.x, 7, n);
	SetUpdateLine(cursor_pos.y, (BYTE)cursor_pos.x);
	SetUpdateLine(cursor_pos.y, (BYTE)site_settings.cols_per_page);
}

void CTelnetConn::DeleteLines(int n)	//delete n lines
{
	if (n <= 0)
		n = 1;
	if (n > site_settings.lines_per_page)
		n = site_settings.lines_per_page;

	int y = cursor_pos.y;
	int yn = y + n;
	if (yn > last_line)
		yn = last_line + 1;

	LPSTR* tmplines = new LPSTR[n];
	while (y < yn)
	{
		tmplines[y-cursor_pos.y] = screen[y];
		y++;
	}
	for (y = cursor_pos.y;y <= last_line;y++)
	{
		if (y + n <= last_line)
		{
			screen[y] = screen[y+n];
			SetUpdateWholeLine(y);
		}
		else
		{
			screen[y] = tmplines[(y-cursor_pos.y)-(last_line+1-yn)];
			InitNewLine(screen[y]);
			SetUpdateWholeLine(y);
		}
	}
	delete []tmplines;
}

void CTelnetConn::GoUp(int p)	//Up
{
	cursor_pos.y -= p;
	if (cursor_pos.y < first_line)
		cursor_pos.y = first_line;
	if (view->telnet == this && is_ansi_editor && cursor_pos.y < scroll_pos)
	{
		view->OnVScroll(SB_LINEUP, 0, NULL);
	}
	UpdateCursorPos();
}

void CTelnetConn::GoDown(int p)	//Down
{
	cursor_pos.y += p;
	if (cursor_pos.y > last_line)
		cursor_pos.y = last_line;
	if (view->telnet == this && is_ansi_editor && cursor_pos.y >= scroll_pos + site_settings.lines_per_page)
	{
		view->OnVScroll(SB_LINEDOWN, 0, NULL);
	}
	UpdateCursorPos();
}
void CTelnetConn::GoRight(int p)	//Right
{
	/*	if(is_ansi_editor && IsEmptyLine(screen[cursor_pos.y]+cursor_pos.x,site_settings.cols_per_page-cursor_pos.x))
		{
			if(cursor_pos.y<last_line)
			{
				cursor_pos.x=0;
				GoDown(1);
			}
			return;
		}
	*/
	cursor_pos.x += p;
	if (cursor_pos.x > site_settings.cols_per_page)
		cursor_pos.x = site_settings.cols_per_page;
	UpdateCursorPos();
}
void CTelnetConn::GoLeft(int p)	//Left
{
	if (is_ansi_editor && cursor_pos.x == 0 && cursor_pos.y > 0)
	{
		GoUp(1);
		End();	//End Key
		return;
	}
	cursor_pos.x -= p;
	if (cursor_pos.x < 0)
	{
		cursor_pos.x = 0;
	}
	UpdateCursorPos();
}
void CTelnetConn::SaveCursorPos()
{
	old_cursor_pos = cursor_pos;
}
void CTelnetConn::RestoreCursorPos()
{
	cursor_pos = old_cursor_pos;
	UpdateCursorPos();
}

void CTelnetConn::DeleteChar(int n)	//Delete a character position (shift line to the left)
{
	Delete(n);
}
void CTelnetConn::NextPages(int n)
{
}

void CTelnetConn::BackTab(int n)
{
	cursor_pos.x -= cursor_pos.x % 8;
}

void CTelnetConn::SetMode(int p)
{
	switch (p)
	{
	case 4:
		insert_mode = 1;
		break;
	}
}
void CTelnetConn::ResetMode(int p)
{
	switch (p)
	{
	case 4:
		insert_mode = 0;
		break;
	}

}
void CTelnetConn::DeviceStatusReport(int p)
{

}

void CTelnetConn::PrevPages(int n)
{

}

inline void CTelnetConn::ProcessAnsiEscapeSequence()
{
//	int l=strlen(ansi_param);
	if (*ansi_param == '[')
	{
		pansi_param--;	//回到最後一個字元

		char *param = ansi_param + 1;
		BYTE type = *pansi_param;
		*pansi_param = ';';

		if (type == 'm')	//ansi color
		{
			LPSTR pparam = param;
			while (*pparam)
			{
				if (*pparam == ';')
				{
					*pparam = 0;
					SetCurrentAttributes(USHORT(atoi(param)));
					param = pparam + 1;
				}
				pparam++;
			}
		}
		else
		{
			int p1, p2;
			while (*param && *param != ';')
				param++;
			if (*param)
			{
				*param = 0;
				p1 = atoi(ansi_param + 1);
				param++;
				p2 = atoi(param);
			}
			else
				p1 = p2 = 0;

			switch (type)
			{
			case 'K':
				ClearCurrentLine(atoi(param));
				break;
//------------ 2 parameters ---------------
			case 'H':
			case 'f':
				GotoXY(p1, p2);
				break;
			case 'r':
				SetScrollRange(p1, p2);
				break;
			case 'J':
				ClearScreen(p1);
				break;
			case 'E':
				LineFeed(p1);
				break;
			case 'L':
				InsertLines(p1);
				break;
			case 'A':
				if (p1 <= 0)
					p1 = 1;
				GoUp(p1);
				break;
			case 'B':
				if (p1 <= 0)
					p1 = 1;
				GoDown(p1);
				break;
			case 'C':
				if (p1 <= 0)
					p1 = 1;
				GoRight(p1);
				break;
			case 'D':
				if (p1 <= 0)
					p1 = 1;
				GoLeft(p1);
				break;
//新加入的ansi支援
			case 's':	//save cursor pos
				SaveCursorPos();
				break;
			case 'u':	//restore cursor pos
				RestoreCursorPos();
				break;
			case '@':	//insert char
				InsertChar(p1);
				break;
			case 'M':	//delete n line
				DeleteLines(p1);
				break;
			case 'P':	//delete char
				DeleteChar(p1);
				break;
				/*
							case 'U':	//next n page
								NextPages(p1);
								break;
							case 'V':	//previous n page
								PrevPages(p1);
								break;
				*/
			case 'Z':	//cursor back tab
				BackTab(p1);
				break;
			case 'h':	//set mode
				SetMode(p1);
				break;
			case 'l':	//reset mode
				ResetMode(p1);
				break;
			case 'n':	//Device Status Report
				DeviceStatusReport(p1);
				break;
			default:
				{
					Sleep(0);
				}
				break;
			}
		}
	}
	else	//如果沒有'['
	{
		switch (*buf)
		{
		case 'D':
			//				scroll up
			ScrollUp();
			break;
		case 'M':
			//				scroll down
			ScrollDown();
			break;
		case 'E':
			break;
		case '7':
			old_cursor_pos = cursor_pos;
			saved_attr = cur_attr;
			break;
		case '8':
			cursor_pos = old_cursor_pos;
			cur_attr = saved_attr;
			UpdateCursorPos();
			break;
		}	//end switch
	}
}

inline void CTelnetConn::SetCurrentAttributes(USHORT clr)
{
	if (clr < 48)
	{
		if (clr >= 40)
		{
			//背景色
			clr -= 40;
			if (!attr_flags&1)
				SetBgColor(cur_attr, (BYTE)clr);
			else
				SetFgColor(cur_attr, (BYTE)clr);
		}
		else if (clr >= 30)
		{
			//前景色
			clr -= 30;
			if (!attr_flags&1)
				SetFgColor(cur_attr, (BYTE)clr);
			else
				SetBgColor(cur_attr, (BYTE)clr);
		}
		else //clr<30
		{
			//其他屬性
			switch (clr)
			{
			case 0:		//清除
				cur_attr = 7;		//0000,0111b=7d
				attr_flags = 0;
				break;
			case 1:		//高亮度
				cur_attr |= 8;		//0000,1000b=8d
				break;
			case 5:		//閃爍
				cur_attr |= 128;		//1000,0000b=128d
				break;
			case 7:
				{
					attr_flags |= 1;	//反相
					BYTE fg = cur_attr & 7;	//儲存前景色
					cur_attr &= 248;		//1111,1000b=248d
					cur_attr |= ((cur_attr & 112) >> 4);	//將背景色存到前景	//0111,0000b=112d
					cur_attr &= 143;		//1000,1111b=143d;
					cur_attr |= (fg << 4);	//將前景色存到背景
				}
			}
		}
	}
}

char* CTelnetConn::ResizeLine(int line, int newl)
{
	LPBYTE attr = GetLineAttr(line);
	if (site_settings.cols_per_page == newl)
		return screen[line];

	DWORD d = *(DWORD*)(attr + (GetLineBufLen() / 2));
	char* str = screen[line];
	char* newstr = new char[GetLineBufLen(newl)];
	LPBYTE newatb = (LPBYTE)newstr + (GetLineBufLen(newl) / 2);
	if (site_settings.cols_per_page > newl)
	{
		memcpy(newstr, str, newl);
		memcpy(newatb, attr, newl);
	}
	else
	{
		memcpy(newstr, str, site_settings.cols_per_page);
		memcpy(newatb, attr, site_settings.cols_per_page);
		memset(newstr + site_settings.cols_per_page, ' ', newl - site_settings.cols_per_page);
		memset(newatb + site_settings.cols_per_page, 7, newl - site_settings.cols_per_page);
	}
	delete []str;
	*(DWORD*)(newatb + newl) = d;
	return newstr;
}


void CTelnetConn::ReSizeBuffer(long new_line_count, int new_cols_per_page, int new_lines_per_page)
{
	if (new_line_count > MAX_LINE_COUNT)
		new_line_count = MAX_LINE_COUNT;

	site_settings.lines_per_page = new_lines_per_page;
	scr_bottom = site_settings.lines_per_page - 1;

	if (new_line_count == site_settings.line_count)	//如果最大行數不變
	{
		if (site_settings.cols_per_page != new_cols_per_page)	//如果有改變每行字數才重新配置
			for (int i = 0;i < site_settings.line_count;i++)
				screen[i] = ResizeLine(i, new_cols_per_page);
	}
	else
	{
		LPSTR* new_screen = new LPSTR[new_line_count];
		if (new_line_count > site_settings.line_count)
		{
			for (int i = 0;i < site_settings.line_count;i++)
				new_screen[i] = ResizeLine(i, new_cols_per_page);

			delete []screen;
			screen = new_screen;

			site_settings.cols_per_page = new_cols_per_page;
			for (;site_settings.line_count < new_line_count;site_settings.line_count++)
				screen[site_settings.line_count] = AllocNewLine();
		}
		else	//if(new_line_count<site_settings.line_count)
		{
			for (int i = 0;i < new_line_count;i++)
				new_screen[i] = ResizeLine(i, new_cols_per_page);

			for (;last_line >= new_line_count; last_line--)
				delete []screen[last_line];

			delete []screen;
			screen = new_screen;
		}
	}
	site_settings.line_count = new_line_count;
	site_settings.cols_per_page = new_cols_per_page;
	first_line = site_settings.line_count - site_settings.lines_per_page;
	last_line = site_settings.line_count - 1;
	if (scroll_pos > first_line)
		scroll_pos = first_line;
	if (is_ansi_editor)
		first_line = 0;

	if (cursor_pos.y >= site_settings.line_count)
	{
		scroll_pos = site_settings.line_count - site_settings.lines_per_page;
		cursor_pos.y = last_line;
	}
	if (view->telnet == this)
	{
		view->SetScrollBar();
		view->InvalidateRect(FALSE);
	}
}


BOOL CTelnetConn::IsEmptyLine(LPSTR line, int len)
{
	LPSTR eol = line + len;
	while (line < eol)
	{
		if (*line != ' ')
			return FALSE;
		line++;
	}
	eol += 5 + site_settings.cols_per_page;
	line = eol - len;
	while (line < eol)
	{
		if (*line != 7)
			return FALSE;
		line++;
	}
	return TRUE;
}

inline void CTelnetConn::EditorLineFeed(LPSTR newline, LPBYTE newlineatb, int l)
{
	cursor_pos.y++;
	if (cursor_pos.y == site_settings.line_count)
	{
		ReSizeBuffer(site_settings.line_count + site_settings.lines_per_page, site_settings.cols_per_page, site_settings.lines_per_page);
		if (newline)
		{
			memcpy(screen[cursor_pos.y], newline, l);
			memcpy(GetLineAttr(cursor_pos.y), newlineatb, l);
		}
		if (view->telnet == this)
			view->OnVScroll(SB_LINEDOWN, 0, NULL);
		else
			scroll_pos++;
	}
	else
	{
		if (insert_mode)
		{
			if (!IsEmptyLine(screen[last_line], site_settings.cols_per_page))		//最後一行有資料
				ReSizeBuffer(site_settings.line_count + site_settings.lines_per_page, site_settings.cols_per_page, site_settings.lines_per_page);	//加大緩衝區

			LPSTR tmp = screen[last_line];
			for (int i = last_line;i > cursor_pos.y;i--)
				screen[i] = screen[i-1];

			screen[cursor_pos.y] = tmp;
			InitNewLine(tmp);
			if (newline)
			{
				memcpy(screen[cursor_pos.y], newline, l);
				memcpy(GetLineAttr(cursor_pos.y), newlineatb, l);
			}
		}

		if (cursor_pos.y == scroll_pos + site_settings.lines_per_page)	//如果在畫面的最後一行
		{
			if (view->telnet == this)
				view->OnVScroll(SB_LINEDOWN, 0, NULL);
			else
				scroll_pos++;
		}
		else
		{
			if (view->telnet == this)
				view->Invalidate(FALSE);
		}
	}
}

inline void CTelnetConn::EditorCarriageRetiurn()
{
	if (!insert_mode)
	{
		cursor_pos.x = 0;
		EditorLineFeed(NULL, NULL, 0);
		return;
	}

	if (cursor_pos.x == 0)
	{
		cursor_pos.x = site_settings.cols_per_page;
		cursor_pos.y--;
		EditorLineFeed(NULL, NULL, 0);
		cursor_pos.y++;
		if (cursor_pos.y == scroll_pos + site_settings.lines_per_page)	//如果在畫面的最後一行
		{
			if (view->telnet == this)
				view->OnVScroll(SB_LINEDOWN, 0, NULL);
			else
				scroll_pos++;
		}
	}
	else if (IsEmptyLine(screen[cursor_pos.y] + cursor_pos.x, site_settings.cols_per_page))
		EditorLineFeed(NULL, NULL, 0);
	else
	{
		int newlinel = site_settings.cols_per_page - cursor_pos.x;
		LPSTR newline = screen[cursor_pos.y] + cursor_pos.x;
		LPBYTE newlineatb = GetLineAttr(cursor_pos.y) + cursor_pos.x;
		EditorLineFeed(newline, newlineatb, newlinel);
		memset(newline, ' ', newlinel);
		memset(newlineatb, 7, newlinel);
	}
}

inline void CTelnetConn::EditorLineBack(LPSTR newline, LPBYTE newlineatb, int l)
{
	if (l)
	{
		int space = site_settings.cols_per_page - cursor_pos.x;
		if (space >= l)
		{
			memcpy(screen[cursor_pos.y] + cursor_pos.x, newline, l);
			memcpy(GetLineAttr(cursor_pos.y) + cursor_pos.x, newlineatb, l);
		}
		else
		{
			l = site_settings.cols_per_page - space;
			memcpy(screen[cursor_pos.y] + cursor_pos.x, newline, space);
			memcpy(GetLineAttr(cursor_pos.y) + cursor_pos.x, newlineatb, space);
			memcpy(newline, newline + space, l);
			memcpy(newlineatb, newlineatb + space, l);
			memset(newline + l, ' ', space);
			memset(newlineatb + l, 7, space);
			return;
		}
	}
	//delete line

	for (int y = cursor_pos.y + 1;y < last_line;y++)
		screen[y] = screen[y+1];

	screen[last_line] = newline;

	memset(newline, ' ', site_settings.cols_per_page);
	*(DWORD*)(newline + site_settings.cols_per_page) = 0;
	memset(newlineatb, 7, site_settings.cols_per_page);
	*(DWORD*)(newlineatb + site_settings.cols_per_page) = 0;

	if (cursor_pos.y < scroll_pos)	//如果在畫面的第一行
	{
		if (view->telnet == this)
			view->OnVScroll(SB_LINEUP, 0, NULL);
		else
			scroll_pos--;
	}
	else
	{
		if (view->telnet == this)
			view->Invalidate(FALSE);
	}
}

void CTelnetConn::Back(int num)
{
	if (cursor_pos.x > 0)
	{
		LPSTR curstr = screen[cursor_pos.y];
		cursor_pos.x -= num;
		if (is_ansi_editor || site_settings.localecho)
			Delete(num);	//Delete
	}
	else if (cursor_pos.y > 0)
	{
		if (is_ansi_editor)
		{
			LPSTR newline = screen[cursor_pos.y];
			LPBYTE newlineatb = GetLineAttr(cursor_pos.y);
			cursor_pos.y--;
			End();	//End
			LPSTR eonl = newline + site_settings.cols_per_page - 1;	
			LPBYTE eonla = newlineatb + site_settings.cols_per_page - 1;
			while (eonl >= newline && *eonl == ' ' && *eonla == 7)
				eonl--, eonla--;
			eonl++;
			EditorLineBack(newline, newlineatb, eonl - newline);
		}
	}
	UpdateCursorPos();
}

inline void CTelnetConn::SetUpdateLine(long line, BYTE curx)
{
	LPBYTE attrib = GetLineAttr(line);
	if (GetUpdateLine(line))	//如果這行已經設為等待更新
	{
		if (curx < *(attrib - 2))	//如果比start更小
			*(attrib - 2) = curx;
		if (curx > *(attrib - 3))	//如果比end更大
			*(attrib - 3) = curx;
	}
	else
	{
		*(attrib - 1) = 1;	//第一次設為等待更新
		*(attrib - 2) = *(attrib - 3) = curx;	//start和end=curx
	}
}

inline void CTelnetConn::RemoveUpdateLine(long line)
{
	*(DWORD*)(GetLineAttr(line) - 4) = 0;
}

inline BOOL CTelnetConn::GetUpdateLine(long line)
{
	return *(GetLineAttr(line) - 1);
}

inline void CTelnetConn::SetUpdateWholeLine(long line)
{
	LPBYTE attrib = GetLineAttr(line);
	*(attrib - 1) = 1;
	*(attrib - 2) = 0;
	*(attrib - 3) = (BYTE)site_settings.cols_per_page - 1;
}

void CTelnetConn::CheckHyperLinks()
{
	long bottom = scroll_pos + site_settings.lines_per_page;
	for (long i = scroll_pos;i < bottom;i++)
	{
		screen[i][site_settings.cols_per_page] = 0;
		if (GetUpdateLine(i))	//如果這行內容有變更，才重新找超連結
		{
			// 如果找到超連結，設定超連結旗標
			// 如果沒找到，清除超連結旗標
			int len;
			SetHyperLink(i, !!AppConfig.hyper_links.FindHyperLink(screen[i], len));
		}
	}
}


void CTelnetConn::Delete(int num)
{
	if (num <= 0)	num = 1;
	LPSTR curstr = screen[cursor_pos.y];
	if (cursor_pos.x > site_settings.cols_per_page)
		cursor_pos.x = site_settings.cols_per_page;
	if (cursor_pos.x < 0)
		cursor_pos.x = 0;

	SetUpdateLine(cursor_pos.y, (BYTE)cursor_pos.x);

	if (cursor_pos.x + num > site_settings.cols_per_page)
		num = site_settings.cols_per_page - cursor_pos.x;

	int len = site_settings.cols_per_page - (cursor_pos.x + num);
	strncpy(curstr + cursor_pos.x , curstr + cursor_pos.x + num , len);
	memmove(GetLineAttr(curstr) + cursor_pos.x,
			GetLineAttr(curstr) + cursor_pos.x + num, len);
	memset(curstr + cursor_pos.x + len, ' ', num);
	memset(GetLineAttr(curstr) + cursor_pos.x + len, 7, num);
	curstr[site_settings.cols_per_page] = 0;
	SetUpdateLine(cursor_pos.y, (BYTE)site_settings.cols_per_page);
}

void CTelnetConn::End()
{
	LPSTR curstr = screen[cursor_pos.y];
	int end = site_settings.cols_per_page - 1;
	LPBYTE attrib = GetLineAttr(cursor_pos.y);
	while (curstr[end] == ' ' && end >= 0 && attrib[end] == 7)
		end--;
	cursor_pos.x = end + 1;
	UpdateCursorPos();
}


void CTelnetConn::SendMacroString(CString str)
{
//	先檢查是否有巨集指令，像是暫停
	const char* _pstr = str;
    const char* pstr;
    for (pstr = _pstr; *pstr; pstr += get_chw(pstr))
	{
		if (*pstr == '#' && *(pstr + 1))
		{
			pstr++;
			switch (*pstr)
			{
                case 'p': {
                    POSITION pos;
					if (pstr > _pstr)
						Send(_pstr, int(pstr) - int(_pstr) - 1);

					pstr++;
					int pause = atoi(pstr);
                    for (pos = delay_send.GetHeadPosition(); pos ;delay_send.GetNext(pos))
					{
						if (pause > delay_send.GetAt(pos).time)
							break;
					}
					while (isdigit(*pstr))
						*pstr++;

					CTelnetConnDelayedSend ds;
					ds.str = pstr;
					ds.time = pause;
					if (pos)
						delay_send.InsertBefore(pos, ds);
					else
						delay_send.AddTail(ds);
					return;
				}
			case '#':
				if (pstr > _pstr)
					Send(_pstr, int(pstr) - int(_pstr));
				SendMacroString(pstr + 1);
				return;
			}
		}
	}
	if (pstr > _pstr)
		Send(_pstr, int(pstr) - int(_pstr));
}

void CTelnetConn::SendNaws()
{
	BYTE naws[] = {IAC, SB, NAWS, 0, 80, 0, 24, IAC, SE};
	naws[3] = HIBYTE(site_settings.cols_per_page);
	naws[4] = LOBYTE(site_settings.cols_per_page);
	naws[5] = HIBYTE(site_settings.lines_per_page);
	naws[6] = LOBYTE(site_settings.lines_per_page);
	Send(naws, sizeof(naws));
}


int CTelnetConn::IsEndOfArticleReached()
{
	char* last_line_txt = screen[last_line];
	char* percent;
	if ((percent = strchr(last_line_txt, '%'))
		&& percent > last_line_txt && isdigit(percent[-1]))
	{
		char* num = percent;
		while (num > last_line_txt && isdigit(num[-1]))
			--num;
		if (num < percent && atoi(num) < 100)
			return 0;
		return 1;
	}
	return 2;
}

CString CTelnetConn::GetLineWithAnsi(long line)
{
	CString data("\x1b[m");

	LPSTR str = screen[line];
	LPSTR strend;
	LPBYTE attr = GetLineAttr(str);
	BYTE tmpatb = *attr;
	if (tmpatb != 7)
		data += AttrToStr(7, tmpatb);

	strend = str + site_settings.cols_per_page - 1;
	BYTE *atbend = GetLineAttr(line) + site_settings.cols_per_page - 1;
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

	return data;
}

CString AttrToStr(BYTE prevatb, BYTE attr)
{
	CString ret = "\x1b[";
	if (attr == 7)
	{
		ret += 'm';
		return ret;
	}

	BYTE fg = attr & 7;
	BYTE bk = GetAttrBkColor(attr);
	BYTE blink = attr & 128;
	BYTE hilight = attr & 8;

	BYTE hilight_changed = 0;
	BYTE blink_changed = 0;
	BYTE fg_changed = 0;
	BYTE bk_changed = 0;

	if (fg != (prevatb&7))	//如果前景色改變
		fg_changed = 1;
	if (bk != GetAttrBkColor(prevatb))	//如果背景色改變
		bk_changed = 1;

	if (hilight != (prevatb&8))	//如果高亮度改變
	{
		hilight_changed = 1;
		if (!hilight)	//如果變成不亮,要重設所有屬性
		{
			blink_changed = fg_changed = bk_changed = 1;
			ret += ';';
		}
	}

	if (blink != (prevatb&128))	//如果閃爍改變
	{
		blink_changed = 1;
		if (!blink)	//如果變成不閃爍,要重設所有屬性
		{
			if (!(hilight_changed && !hilight))	//如果所有屬性還沒重設過才重設
			{
				ret += ';';
				hilight_changed = fg_changed = bk_changed = 1;
			}
		}
	}

	if (hilight_changed && hilight)
		ret += "1;";
	if (blink_changed && blink)
		ret += "5;";
	char num[4] = {0, 0, ';', 0};
	if (fg_changed)
	{
		*num = '3';
		*(num + 1) = '0' + fg;
		ret += num;
	}
	if (bk_changed)
	{
		*num = '4';
		*(num + 1) = '0' + bk;
		ret += num;
	}

	char* pret = LPSTR(LPCTSTR(ret));
	if (pret[ret.GetLength()-1] == ';')
		pret[ret.GetLength()-1] = 'm';
	else
		ret += 'm';
	return ret;
}

void CTelnetConn::CopyArticle(bool with_color, bool in_editor)
{
//	downloaded_article.Empty();
    current_download_line = 0;
	get_article_in_editor = in_editor;
	get_article_with_ansi = with_color;
	for (int y = scroll_pos; y < last_line; ++y)
	{
		if (!with_color)
			downloaded_article += screen[y];
		else
			downloaded_article += GetLineWithAnsi(y);
		downloaded_article.TrimRight(" ");
		downloaded_article += "\r\n";
	}

	if (IsEndOfArticleReached())
		CopyArticleComplete();
	else
	{
		is_getting_article = true;
		const char* key = key_map->FindKey(VK_DOWN, 0);
		SendString(key ? key : "^[[B");
	}
}

void CTelnetConn::CopyArticleComplete(bool cancel)
{
	is_getting_article = false;

	if (!cancel)
	{

		if (get_article_in_editor)
		{
			view->OnAnsiEditor();
			CTelnetConn* editor = view->telnet;
			//FIXME: dirty hack to insert ansi string to the editor
			view->LockWindowUpdate();
			editor->Send((LPCTSTR)downloaded_article, downloaded_article.GetLength());
			editor->scroll_pos = 0;
			editor->cursor_pos.x = editor->cursor_pos.y = 0;
			editor->UpdateCursorPos();
			view->SetScrollBar();
			view->UnlockWindowUpdate();
		}
		else
		{
			// FIXME: unicode???
			CClipboard::SetText(view->m_hWnd, downloaded_article);
		}

		if (download_article_dlg)	// dismiss cancel dialog
			download_article_dlg->OnCancel();
	}
	get_article_in_editor = false;
	get_article_with_ansi = false;
	downloaded_article.Empty();
}
