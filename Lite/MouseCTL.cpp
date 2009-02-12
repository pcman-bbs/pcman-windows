/********************************************************************
	created:	2007/06/29
	created:	29:6:2007   0:26
	filename: 	MouseCTL.cpp
	file base:	MouseCTL
	file ext:	cpp
	author:		Omar Lin

	purpose:	Mouse Control
*********************************************************************/

#include "StdAfx.h"
#include "MainFrm.h"
#include "MouseCTL.h"
#include "MouseGesture.h"
#include "StrUtils.h"

CTermView *g_pView = NULL;
CMouseGesture m_gesture;
char g_szMouseGesture[60];

struct MOUSE_STATE
{
	DWORD R;
	DWORD L;
	DWORD M;
};

static MOUSE_STATE m_MouseState;

#define MT_STATE_PRESS				0x01000000
#define MT_STATE_STILL				0x02000000
#define MT_STATE_WHELL_ACT			0x04000000
#define MT_STATE_STILL_BY_TIMER		0x08000000

#define MT_TIME_STILL	1000
#define MT_EVENT_STILL	5566
#define MT_EVENT_RELOAD_CURSOR	5566+1

#define MT_TIME_GET(MouseState) MouseState&0x00FFFFFF
#define MT_TIME_SET(MouseState, time) MouseState = (MouseState&0xFF000000)|(time&0x00FFFFFF)

enum MOUSE_CMD_TYPE
{
	CMD_TYPE_KEY_INPUT = 0,
};

enum MOUSE_ACTION
{
	MA_L_BT_CLICK = 1,
	MA_R_BT_CLICK,
	MA_M_BT_CLICK,
	MA_WHELL_UP,
	MA_WHELL_DOWN,
	MA_R_BT_WHELL_UP,
	MA_R_BT_WHELL_DOWN,
	MA_L_BT_WHELL_UP,
	MA_L_BT_WHELL_DOWN,
	MA_MOUSE_GESTURE,
};

struct  MOUSE_ACTION_CMD_INFO
{
	DWORD	nMouseAction;
	char	*szCmd;
	char	*szCmdInfo;
	DWORD	nCmdType;
};

MOUSE_ACTION_CMD_INFO MouseActionArray_Default[] =
{
	{MA_L_BT_CLICK, "\"ENTER\"", "Enter", CMD_TYPE_KEY_INPUT},
	{MA_M_BT_CLICK, "\"LEFT\"", "Left", CMD_TYPE_KEY_INPUT},
	{MA_WHELL_UP, "\"UP\"", "Up", CMD_TYPE_KEY_INPUT},
	{MA_WHELL_DOWN, "\"DOWN\"", "Down", CMD_TYPE_KEY_INPUT},
	{MA_R_BT_WHELL_UP, "\"PRIOR\"", "PageUp", CMD_TYPE_KEY_INPUT},
	{MA_R_BT_WHELL_DOWN, "\"NEXT\"", "PageDown", CMD_TYPE_KEY_INPUT},
	{MA_L_BT_WHELL_UP, "-", "-", CMD_TYPE_KEY_INPUT},
	{MA_L_BT_WHELL_DOWN, "+", "+", CMD_TYPE_KEY_INPUT},
	{0, 0, 0, 0},
};

MOUSE_ACTION_CMD_INFO *g_pMouseActionArray_Now = NULL;

BOOL MouseCTL_MouseAction_Setup_Cmd(MOUSE_ACTION_CMD_INFO *pMA_Array);
BOOL MouseCTL_MouseAction_RemoveAll_Cmd();
MOUSE_ACTION_CMD_INFO *MouseCTL_MouseAction_GeCmdInfo(DWORD nMouseAction_In);



struct  GESTURE_CMD_INFO
{
	char	*szGesture;
	char	*szCmd;
	char	*szCmdInfo;
	DWORD	nCmdType;
};

GESTURE_CMD_INFO GestureArray_Default[] =
{
	{"rU", "\"HOME\"", "Home", CMD_TYPE_KEY_INPUT},
	{"rD", "\"END\"", "End", CMD_TYPE_KEY_INPUT},
	{"rR", "\"ENTER\"", "Enter", CMD_TYPE_KEY_INPUT},
	{"rL", "\"LEFT\"", "Left", CMD_TYPE_KEY_INPUT},
	{"rUDU", "=", "=", CMD_TYPE_KEY_INPUT},
	{"rRLR", "^Q", "Ctrl-q", CMD_TYPE_KEY_INPUT},
	{"rLDRDL", "S", "S", CMD_TYPE_KEY_INPUT},
	{"rUD", "\"LEFT\"\"ENTER\"\"END\"", "Reload(Left Enter End)", CMD_TYPE_KEY_INPUT},
	{0, 0, 0, 0},
};

GESTURE_CMD_INFO *g_pGestureArray_Now = NULL;;
#define GESTURE_FIRST_ID	1

BOOL MouseCTL_Gesture_Setup_Cmd(GESTURE_CMD_INFO *pGestureArray);
BOOL MouseCTL_Gesture_RemoveAll_Cmd();
GESTURE_CMD_INFO *MouseCTL_Gesture_GeCmdInfo(DWORD nID_In);



enum MC_CURSOR
{
	CURSOR_NORMAL = 0,
	CURSOR_TOPIC,
	CURSOR_PAGE,
	CURSOR_RELOAD = 0xFFFFFFFF,
};


void MouseCTL_SetMouseCursor(HWND hWnd, MC_CURSOR Cursor);
const char *MouseCTL_KeyStringToKeyInput(const char *szKeyString, char *cKey_In, BOOL *bExtendKey, BOOL *bControlKey);

static DWORD m_dwWndFocusTimeCounter;
static DWORD m_dwMenuTimeCounter;

static HCURSOR m_page_cursor;
static HCURSOR m_topic_cursor;
static HWND m_hWnd_ForTimer = NULL;

static HCURSOR m_CursorNow = NULL;

void MouseCTL_OnLButtonDblClk(HWND hWnd, UINT nFlags, CPoint point)
{

}

void MouseCTL_OnLButtonDown(HWND hWnd, UINT nFlags, CPoint point)
{
	DWORD dwStill;

	dwStill = m_MouseState.L & MT_STATE_STILL;
	m_MouseState.L = 0;
	MT_TIME_SET(m_MouseState.L, GetTickCount());
	m_MouseState.L |= MT_STATE_PRESS;
	m_MouseState.L |= dwStill;
	SetTimer(hWnd, MT_EVENT_STILL, MT_TIME_STILL, NULL);
	m_hWnd_ForTimer = hWnd;
}

BOOL MouseCTL_OnLButtonUp_PreProcess(HWND hWnd, UINT nFlags, CPoint point)
{
	m_MouseState.L &= ~MT_STATE_PRESS;
	KillTimer(hWnd, MT_EVENT_STILL);
	if (m_MouseState.R & MT_STATE_STILL)
	{
		MouseCTL_SetMouseCursor(hWnd, CURSOR_NORMAL);
		m_MouseState.R = 0;
		return FALSE;
	}
	else if (m_MouseState.L & MT_STATE_STILL)
	{
		MouseCTL_SetMouseCursor(hWnd, CURSOR_NORMAL);
		m_MouseState.L = 0;
		return FALSE;
	}
	if (m_MouseState.L & MT_STATE_STILL_BY_TIMER)
	{
	}
	else
	{
		MouseCTL_SetMouseCursor(hWnd, CURSOR_NORMAL);
	}

	return TRUE;
}

void MouseCTL_OnLButtonUp_PostProcess(HWND hWnd, UINT nFlags, CPoint point)
{
	m_MouseState.L &= ~MT_STATE_WHELL_ACT;
}

void MouseCTL_OnLButtonUp(HWND hWnd, UINT nFlags, CPoint point)
{
	DWORD dwCounter;

	dwCounter = GetTickCount();
	if ((dwCounter - m_dwMenuTimeCounter < 200) || (dwCounter - m_dwWndFocusTimeCounter < 200))
		return ;

	if (m_MouseState.L & MT_STATE_STILL_BY_TIMER)
	{
		m_MouseState.L |= MT_STATE_STILL;//不好，因為下面有一個也是在設  MT_STATE_STILL
		return;
	}

	if (m_MouseState.L & MT_STATE_STILL)
	{
		MouseCTL_SetMouseCursor(hWnd, CURSOR_NORMAL);
		m_MouseState.L = 0;
	}
	else if (m_MouseState.L & MT_STATE_WHELL_ACT)
	{

	}
	else
	{
		DWORD dw1, dw2;

		dw1 = GetTickCount() & 0x00FFFFFF;
		dw2 = MT_TIME_GET(m_MouseState.L);
		if (dw1 - dw2 > MT_TIME_STILL)
		{
			m_MouseState.L |= MT_STATE_STILL;
			MouseCTL_SetMouseCursor(hWnd, CURSOR_TOPIC);
		}
		else
		{
			MouseCTL_OnAction(MA_L_BT_CLICK, 0);
		}
	}
}

void MouseCTL_OnRButtonDblClk(HWND hWnd, UINT nFlags, CPoint point)
{

}

void MouseCTL_OnRButtonDown(HWND hWnd, UINT nFlags, CPoint point)
{
	DWORD dwStill;

	MOUSEHOOKSTRUCT MHS = {0};

	MHS.hwnd = hWnd;
	MHS.pt = point;

	m_gesture.MouseMessage(WM_RBUTTONDOWN, (LPARAM)&MHS);


	dwStill = m_MouseState.R & MT_STATE_STILL;
	m_MouseState.R = 0;
	MT_TIME_SET(m_MouseState.R, GetTickCount());
	m_MouseState.R |= MT_STATE_PRESS;
	m_MouseState.R |= dwStill;
	SetTimer(hWnd, MT_EVENT_STILL, MT_TIME_STILL, NULL);
	m_hWnd_ForTimer = hWnd;
	SetCapture(hWnd);
}

BOOL MouseCTL_OnRButtonUp(HWND hWnd, UINT nFlags, CPoint point)
{
	BOOL xRet;

	xRet = FALSE;

	UINT dwGestureID;

	MOUSEHOOKSTRUCT MHS = {0};

	MHS.hwnd = hWnd;
	MHS.pt = point;

	dwGestureID = m_gesture.MouseMessage(WM_RBUTTONUP, (LPARAM) & MHS);

	if (MouseCTL_GetCurrentMouseCursor() == NULL)
		ReleaseCapture();

	m_MouseState.R &= ~MT_STATE_PRESS;

	KillTimer(hWnd, MT_EVENT_STILL);
	if (m_MouseState.R & MT_STATE_STILL_BY_TIMER)
	{
		m_MouseState.R |= MT_STATE_STILL;//不好，因為下面有一個也是在設  MT_STATE_STILL
		goto __Exit;
	}

	if (dwGestureID != INVALID_GETURE_ID)
	{
		MouseCTL_OnAction(MA_MOUSE_GESTURE, dwGestureID);
		if (g_szMouseGesture[0])
		{
			g_szMouseGesture[0] = 0;
			MouseCTL_UpdateStatus(UNKNOWN_GETURE_ID);
		}
		goto __Exit;
	}

	if (m_MouseState.R & MT_STATE_STILL)
	{
		MouseCTL_SetMouseCursor(hWnd, CURSOR_NORMAL);
		m_MouseState.R = 0;
	}
	else if (m_MouseState.R & MT_STATE_WHELL_ACT)
	{

	}
	else
	{
		DWORD dw1, dw2;

		dw1 = GetTickCount() & 0x00FFFFFF;
		dw2 = MT_TIME_GET(m_MouseState.R);
		if (dw1 - dw2 > MT_TIME_STILL)
		{
			if (m_MouseState.L & MT_STATE_STILL)
			{

			}
			else if (m_gesture.m_LastDirection == NULL)
			{
				m_MouseState.R |= MT_STATE_STILL;
				MouseCTL_SetMouseCursor(hWnd, CURSOR_PAGE);
			}
		}
		else if (dwGestureID == INVALID_GETURE_ID)
		{
			MouseCTL_Reset();

			POINT pt;

			pt = point;
			ClientToScreen(hWnd, &pt);
			SendMessage(hWnd, WM_CONTEXTMENU, (DWORD)NULL, (DWORD)MAKELONG(pt.x, pt.y));
			m_dwMenuTimeCounter = GetTickCount();

			xRet = TRUE;
		}
	}
	m_MouseState.R &= ~MT_STATE_WHELL_ACT;

__Exit:
	return xRet;
}

void MouseCTL_OnMButtonDown(HWND hWnd, UINT nFlags, CPoint point)
{

}

void MouseCTL_OnMButtonUp(HWND hWnd, UINT nFlags, CPoint point)
{
	MouseCTL_OnAction(MA_M_BT_CLICK, 0);
}

void MouseCTL_OnMButtonDblClk(HWND hWnd, UINT nFlags, CPoint point)
{

}

BOOL MouseCTL_OnMouseWheel(HWND hWnd, UINT nFlags, short zDelta, CPoint pt)
{
	int nGap;
	BOOL bExtendKey;
	BOOL bGestureReset;

	m_MouseState.R |= MT_STATE_WHELL_ACT;
	m_MouseState.L |= MT_STATE_WHELL_ACT;

	KillTimer(hWnd, MT_EVENT_STILL);

	nGap = zDelta / WHEEL_DELTA;
	if (nGap == 0)
		return TRUE;

	bExtendKey = TRUE;
	bGestureReset = FALSE;

	if (nGap < 0)
	{
		if (m_MouseState.R & MT_STATE_PRESS ||
			m_MouseState.R & MT_STATE_STILL)
		{
			bGestureReset = TRUE;
			MouseCTL_OnAction(MA_R_BT_WHELL_DOWN, 0);
		}
		else if (m_MouseState.L & MT_STATE_PRESS ||
				 m_MouseState.L & MT_STATE_STILL)
		{
			MouseCTL_OnAction(MA_L_BT_WHELL_DOWN, 0);
		}
		else
		{
			MouseCTL_OnAction(MA_WHELL_DOWN, 0);
		}
	}
	else
	{
		if (m_MouseState.R & MT_STATE_PRESS ||
			m_MouseState.R & MT_STATE_STILL)
		{
			bGestureReset = TRUE;
			MouseCTL_OnAction(MA_R_BT_WHELL_UP, 0);
		}
		else if (m_MouseState.L & MT_STATE_PRESS ||
				 m_MouseState.L & MT_STATE_STILL)
		{
			MouseCTL_OnAction(MA_L_BT_WHELL_UP, 0);
		}
		else
		{
			MouseCTL_OnAction(MA_WHELL_UP, 0);
		}
	}

	if (bGestureReset)
		m_gesture.KillGesture();

	return TRUE;
}

void MouseCTL_OnMouseMove(HWND hWnd, UINT nFlags, CPoint point)
{
	static DWORD s_dwMoveTimes = 0;

	if (nFlags & MK_LBUTTON ||
		nFlags & MK_MBUTTON ||
		nFlags & MK_RBUTTON)
	{
		s_dwMoveTimes = 0;
	}
	else
	{
		s_dwMoveTimes++;

		if (g_szMouseGesture[0] && s_dwMoveTimes > 10)
		{
			g_szMouseGesture[0] = 0;
			MouseCTL_UpdateStatus(UNKNOWN_GETURE_ID);
			s_dwMoveTimes = 0;
		}
	}

	if (nFlags & MK_RBUTTON &&
		!(m_MouseState.R & MT_STATE_WHELL_ACT))
	{
		MOUSEHOOKSTRUCT MHS = {0};

		MHS.hwnd = hWnd;
		MHS.pt = point;

		m_gesture.MouseMessage(WM_MOUSEMOVE, (LPARAM)&MHS);
	}
}

void MouseCTL_OnTimer(HWND hWnd, UINT nIDEvent)
{
	switch (nIDEvent)
	{
	case MT_EVENT_STILL:
		{
			MC_CURSOR Cursor;
			DWORD *pdwMouseState;

			KillTimer(hWnd, nIDEvent);
			Cursor = CURSOR_NORMAL;
			pdwMouseState = NULL;

			if (MouseCTL_IsCursorInSelect())
			{
				MouseCTL_Reset(FALSE);
				return ;
			}

			if (m_MouseState.R & MT_STATE_PRESS)
			{
				Cursor = CURSOR_PAGE;
				pdwMouseState = &m_MouseState.R;
			}
			else if (m_MouseState.L & MT_STATE_PRESS)
			{
				Cursor = CURSOR_TOPIC;
				pdwMouseState = &m_MouseState.L;
			}
			if (m_MouseState.L & MT_STATE_STILL ||
				m_gesture.m_LastDirection != NULL)
				Cursor = CURSOR_NORMAL;

			if (Cursor != CURSOR_NORMAL)
			{
				MouseCTL_SetMouseCursor(hWnd, Cursor);
				*pdwMouseState |= MT_STATE_STILL_BY_TIMER;
			}
		}
		break;
	case MT_EVENT_RELOAD_CURSOR:
		{
			MouseCTL_SetMouseCursor(hWnd, CURSOR_RELOAD);
		}
		break;
	default:
		break;
	}
}

void MouseCTL_OnSetFocus(HWND hWnd)
{
	m_dwWndFocusTimeCounter = GetTickCount();
}

DWORD m_LastGestureID = UNKNOWN_GETURE_ID;

char *MouseCTL_GetStatusInfo()
{
	static char s_StatusInfo[0x100] = {0};
	DWORD dw1;

	if (g_szMouseGesture[0])
	{

		GESTURE_CMD_INFO *pGesture;

		pGesture = MouseCTL_Gesture_GeCmdInfo(m_LastGestureID);

		dw1 = strlen(g_szMouseGesture);
		while (dw1 > 0 && g_szMouseGesture[dw1-1] == ',')
			dw1--;
		if (dw1 > 0)
			g_szMouseGesture[dw1] = 0;

		wsprintf(s_StatusInfo, "%s  (%s)", g_szMouseGesture, 
			(pGesture ? pGesture->szCmdInfo : LoadString(IDS_UNKNOWN_GESTURE)));

		if (dw1 > 0)
			g_szMouseGesture[dw1] = ',';
	}
	else
		s_StatusInfo[0] = 0;

	return s_StatusInfo;
}

void WINAPI MouseCTL_UpdateStatus(DWORD nID)
{
	m_LastGestureID = nID;

	if (g_pView && g_pView->parent)
		g_pView->parent->UpdateStatus();

}

void MouseCTL_Init(HWND hWnd)
{
	m_page_cursor = LoadCursor(GetModuleHandle(NULL), MAKEINTRESOURCE(IDC_PAGE_CUR));
	m_topic_cursor = LoadCursor(GetModuleHandle(NULL), MAKEINTRESOURCE(IDC_TOPIC_CUR));

	SetTimer(hWnd, MT_EVENT_RELOAD_CURSOR, 1000, NULL);

	m_gesture.m_szDirctionNow = g_szMouseGesture;
	m_gesture.m_dwDirction_BufLen = sizeof(g_szMouseGesture) / sizeof(TCHAR);
	m_gesture.fnUpdateMessage = MouseCTL_UpdateStatus;
	m_gesture.Attach(hWnd, 20);//設定手勢的靈敏度，值愈大的話，愈不靈敏，方向的改變要大一點，但是滑鼠的動作就要變大

	MouseCTL_MouseAction_Setup_Cmd(MouseActionArray_Default);
	MouseCTL_Gesture_Setup_Cmd(GestureArray_Default);
}

BOOL MouseCTL_IsCursorInSelect()
{
	CTelnetConn* telnet;

	telnet = NULL;
	if (g_pView)
		telnet = g_pView->telnet;

	if (telnet == NULL)
		return FALSE;

	if (telnet->sel_start.x != telnet->sel_end.x ||
		telnet->sel_start.y != telnet->sel_end.y)
		return TRUE;

	return FALSE;
}

static HCURSOR m_hCursor_Last;

HCURSOR MouseCTL_GetCurrentMouseCursor()
{
	return m_hCursor_Last;
}

void MouseCTL_SetMouseCursor(HWND hWnd, MC_CURSOR Cursor)
{
	HCURSOR hCursor;

	hCursor = NULL;

	switch (Cursor)
	{
	case CURSOR_NORMAL:
		if (m_hCursor_Last)
		{
			DBG_PRINT("Cursor_Normal\r\n");
			hCursor = NULL;
			ReleaseCapture();
			SetCursor(hCursor);
		}
		break;
	case CURSOR_TOPIC:
		hCursor = m_topic_cursor;
		if (hCursor != m_hCursor_Last)
		{
			DBG_PRINT("Cursor_Topic\r\n");
			SetCapture(hWnd);
			SetCursor(hCursor);
		}
		break;
	case CURSOR_PAGE:
		hCursor = m_page_cursor;
		if (hCursor != m_hCursor_Last)
		{
			DBG_PRINT("Cursor_page\r\n");
			SetCapture(hWnd);
			SetCursor(hCursor);
		}
		break;
	case CURSOR_RELOAD:
		hCursor = m_hCursor_Last;
		if (hCursor)
		{
			DBG_PRINT("Cursor_Reload\r\n");
			SetCapture(hWnd);
			SetCursor(hCursor);
		}
		break;
	}
	m_hCursor_Last = hCursor;
}

void MouseCTL_Reset(BOOL bRestoreMouse)
{
	memset(&m_MouseState, 0, sizeof(m_MouseState));
	m_dwWndFocusTimeCounter = 0;
	m_dwMenuTimeCounter = 0;

	if (m_hWnd_ForTimer)
		KillTimer(m_hWnd_ForTimer, MT_EVENT_STILL);
	m_hWnd_ForTimer = NULL;

	if (bRestoreMouse)
	{
		MouseCTL_SetMouseCursor(NULL, CURSOR_NORMAL);
	}
}

void MouseCTL_Reset()
{
	MouseCTL_Reset(TRUE);
}

/*
為什麼不用key_map->FindKey(cKey_In, 0);
因為在使用VK_NEXT時，找到的轉換表是錯的，還要再填Flag的值才行

Extend key 有PageDown PageUp Home End Up Down Left Right Insert Delete 還有右邊的Ctrl鍵
此時的Extend Key的virtual-key 會跟現行的按鍵會有重覆，所以一偵測到Extend Key就virtual-key就是不一樣
*/
BOOL MouseCTL_SendInput(char cKey_In, BOOL bExtendKey, BOOL bControlKey)
{
	BOOL xRet;
	char buf[20];
	int nBNow;
	CString StrUnCtl;
	const char *KeyTable;
	CTelnetConn* telnet;

	xRet = FALSE;

	telnet = NULL;

	if (g_pView)
		telnet = g_pView->telnet;

	if (telnet == NULL)
		goto __Exit;

	if (bExtendKey)
	{
		KeyTable = telnet->key_map->FindKey(cKey_In, bExtendKey ? 0xFFFFFFFF : 0);
		if (KeyTable == NULL)
			goto __Exit;
		StrUnCtl = KeyTable;
	}
	else
	{
		buf[0] = 0;
		nBNow = 0;

		if (bControlKey)
			buf[nBNow++] = '^';

		buf[nBNow++] = cKey_In;
		buf[nBNow++] = 0;

		if (bControlKey)
			StrUnCtl = UnescapeControlChars(buf);
		else
			StrUnCtl = buf;
	}

	telnet->SendString(StrUnCtl);

	xRet = TRUE;

__Exit:
	return xRet;
}

/************************************************************************
EXTEND_KEY中的字串為關鍵字，所以寫字串需要加上" "  如"HOME"
若是想用"號的話  就用 \"
若是在兩個"號中的字串，不是我們的關鍵字，則這個區塊跳過不看
若是只發現一個"號，則這個字元就當做是輸入錯誤而跳過，位移一個字元，看下一個字開始讀
************************************************************************/
const char *MouseCTL_KeyStringToKeyInput(const char *szKeyString_In, char *cKey_In, BOOL *bExtendKey, BOOL *bControlKey)
{
	const char *szKeyString;
	const char *pszNext, *pszBlockEnd, *pBuf1;
	char szStringBlock[0x100];
	int i;

	*cKey_In = 0;
	*bExtendKey = FALSE;
	*bControlKey = FALSE;

	szKeyString = szKeyString_In;

	if (szKeyString == NULL || szKeyString[0] == 0)
		return NULL;

	//當有失敗發生時，就往下一個字串移動
	pszNext = szKeyString + 1;

	if (szKeyString[0] == '\"')
	{
		szKeyString++;

		//尋找相鄰的'\"'  但是 ""\\\""要忽略
		pszBlockEnd = (char*)szKeyString;
		i = 0;
		while (pszBlockEnd[0] && pszBlockEnd[0] != '\"')
		{
			//若是""\\\""要忽略
			if (pszBlockEnd[0] == '\\' && pszBlockEnd[1] == '\"')
				pszBlockEnd ++;

			szStringBlock[i] = pszBlockEnd[0];
			i++;
			pszBlockEnd++;
		}
		szStringBlock[i] = 0;

		//鄰近沒有'\"'的話，那這次的輸入不算，當成這個'\"'是不小心輸入錯誤，下個字串+1 在重新來比對
		if (pszBlockEnd[0] == 0)
			goto __Exit;

		pszBlockEnd ++;//最後一個字元為'\"'
		pszNext = pszBlockEnd;

		//判斷是否為ExtendKey
		for (i = 0; i < sizeof(Extend_Key) / sizeof(Extend_Key[0]); i++)
		{
			pBuf1 = Extend_Key[i].szName;
			if (stricmp(szStringBlock, pBuf1) == 0)
			{
				*cKey_In = Extend_Key[i].VirKey;
				*bExtendKey = TRUE;
				break;
			}
		}

		if (*cKey_In)
			goto __Exit;


		//判斷是否為Enter
		if (stricmp(szStringBlock, KEYSTR_ENTER) == 0)
		{
			*cKey_In = VK_RETURN;
		}

		//若沒有符合的命令，但是卻有  ""來包字串，所以這個字串跳過  不看，看下一個區塊
		goto __Exit;
	}


	//判斷是否為Ctrl複合鍵
	if (szKeyString[0] == '^' && szKeyString[1] && isupper(szKeyString[1]))
	{
		*cKey_In = szKeyString[1];
		*bControlKey = TRUE;
		pszNext = szKeyString + 2;
	}

	if (*cKey_In)
		goto __Exit;

	//若是""\\\""要忽略
	if (szKeyString[0] == '\\' && szKeyString[1] == '\"')
	{
		*cKey_In = szKeyString[1];
		pszNext = szKeyString + 2;
	}

	if (*cKey_In)
		goto __Exit;

	*cKey_In = szKeyString[0];
	pszNext = szKeyString + 1;

__Exit:
	return pszNext;
}

BOOL MouseCTL_RunCommand(const char *szCmdString, DWORD nCmdType)
{
	BOOL xRet;

	xRet = FALSE;

	if (nCmdType == CMD_TYPE_KEY_INPUT)
	{
		const char *pNexKey;
		char cKey_In;
		BOOL bExtendKey;
		BOOL bControlKey;

		pNexKey = szCmdString;
		while (pNexKey && pNexKey[0])
		{
			pNexKey = MouseCTL_KeyStringToKeyInput(pNexKey, &cKey_In, &bExtendKey, &bControlKey);
			if (cKey_In == 0)
				continue;

			MouseCTL_SendInput(cKey_In, bExtendKey, bControlKey);
			xRet = TRUE;
		}
	}

	return xRet;
}

void MouseCTL_OnAction(DWORD dwActionID, DWORD dwActionID_Sub)
{
	DBG_PRINT("MouseCTL_OnAction(%d, %d)\r\n", dwActionID, dwActionID_Sub);

	switch (dwActionID)
	{
	case MA_R_BT_CLICK:
		break;
	case MA_L_BT_CLICK:
	case MA_M_BT_CLICK:
	case MA_WHELL_UP:
	case MA_WHELL_DOWN:
	case MA_R_BT_WHELL_UP:
	case MA_R_BT_WHELL_DOWN:
	case MA_L_BT_WHELL_UP:
	case MA_L_BT_WHELL_DOWN:
		{
			MOUSE_ACTION_CMD_INFO *pMA;

			pMA = MouseCTL_MouseAction_GeCmdInfo(dwActionID);
			if (pMA == NULL)
				break;

			MouseCTL_RunCommand(pMA->szCmd, pMA->nCmdType);
		}
		break;
	case MA_MOUSE_GESTURE:
		{
			if (dwActionID_Sub == UNKNOWN_GETURE_ID)
				break;

			GESTURE_CMD_INFO *pGesture;

			pGesture = MouseCTL_Gesture_GeCmdInfo(dwActionID_Sub);
			if (pGesture == NULL)
				break;

			MouseCTL_RunCommand(pGesture->szCmd, pGesture->nCmdType);

		}
		break;
	}
}

BOOL MouseCTL_Gesture_Setup_Cmd(GESTURE_CMD_INFO *pGestureArray)
{
	BOOL xRet;
	int nID_Now;
	GESTURE_CMD_INFO *pGesture;

	xRet = FALSE;

	m_gesture.RemoveGesture_All();
	g_pGestureArray_Now = pGestureArray;

	if (pGestureArray == NULL)
		goto __Exit;

	pGesture = &g_pGestureArray_Now[0];
	nID_Now = GESTURE_FIRST_ID;
	while (pGesture && pGesture->szGesture && pGesture->szCmd)
	{
		m_gesture.AddGesture(nID_Now, pGesture->szGesture);
		nID_Now ++;
		pGesture++;

		xRet = TRUE;
	}

__Exit:
	return xRet;
}

BOOL MouseCTL_Gesture_RemoveAll_Cmd()
{
	MouseCTL_Gesture_Setup_Cmd(NULL);
	return TRUE;
}

GESTURE_CMD_INFO *MouseCTL_Gesture_GeCmdInfo(DWORD nID_In)
{
	GESTURE_CMD_INFO *pGesture;
	int nID_Now;
	BOOL bOK;

	bOK = FALSE;
	if (nID_In < GESTURE_FIRST_ID || g_pGestureArray_Now == NULL)
		return NULL;

	pGesture = &g_pGestureArray_Now[0];
	nID_Now = GESTURE_FIRST_ID;
	while (pGesture && pGesture->szGesture && pGesture->szCmd)
	{
		if (nID_In == nID_Now)
		{
			bOK = TRUE;
			break;
		}
		pGesture++;
		nID_Now ++;
	}

	if (bOK == FALSE)
		return NULL;

	return pGesture;
}

BOOL MouseCTL_MouseAction_Setup_Cmd(MOUSE_ACTION_CMD_INFO *pMA_Array)
{
	BOOL xRet;

	xRet = FALSE;

	g_pMouseActionArray_Now = pMA_Array;

	if (pMA_Array == NULL)
		goto __Exit;

	xRet = TRUE;

__Exit:
	return xRet;
}

BOOL MouseCTL_MouseAction_RemoveAll_Cmd()
{
	MouseCTL_MouseAction_Setup_Cmd(NULL);
	return TRUE;
}

MOUSE_ACTION_CMD_INFO *MouseCTL_MouseAction_GeCmdInfo(DWORD nMouseAction_In)
{
	MOUSE_ACTION_CMD_INFO *pMA;
	BOOL bOK;

	bOK = FALSE;

	if (g_pMouseActionArray_Now == NULL)
		return NULL;

	pMA = &g_pMouseActionArray_Now[0];
	while (pMA && pMA->nMouseAction && pMA->szCmd)
	{
		if (nMouseAction_In == pMA->nMouseAction)
		{
			bOK = TRUE;
			break;
		}
		pMA++;
	}

	if (bOK == FALSE)
		return NULL;

	return pMA;
}

