#include "StdAfx.h"
#include "MainFrm.h"
#include "MouseCTL.h"
#include "MouseGesture.h"
#include "StrUtils.h"

CTermView *g_pView = NULL;
CMouseGesture m_gesture;
char g_szMouseGesture[30];

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


enum GESTURE
{
	MG_BR_U = 1,
	MG_BR_D,
	MG_BR_L,
	MG_BR_R,
	MG_BR_U_D,
	MG_BR_D_U,
	MG_BR_R_L,
	MG_BR_L_R,
	MG_BR_U_D_U,
	MG_BR_R_L_R,
	MG_BR_LDRDL,
};

enum MC_CURSOR
{
	CURSOR_NORMAL = 0,
	CURSOR_TOPIC,
	CURSOR_PAGE,
	CURSOR_RELOAD = 0xFFFFFFFF,
};

void MouseCTL_SetMouseCursor(HWND hWnd, MC_CURSOR Cursor);

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
	}else if (m_MouseState.L & MT_STATE_STILL)
	{
		MouseCTL_SetMouseCursor(hWnd, CURSOR_NORMAL);
		m_MouseState.L = 0;
		return FALSE;
	}
	if (m_MouseState.L & MT_STATE_STILL_BY_TIMER)
	{
	}else
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
	if ((dwCounter - m_dwMenuTimeCounter < 200) || (dwCounter - m_dwWndFocusTimeCounter < 200) )
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
	}else if (m_MouseState.L & MT_STATE_WHELL_ACT)
	{

	}else
	{
		DWORD dw1, dw2;

		dw1 = GetTickCount()&0x00FFFFFF;
		dw2 = MT_TIME_GET(m_MouseState.L);
		if (dw1 - dw2 > MT_TIME_STILL)
		{
			m_MouseState.L |= MT_STATE_STILL;
			MouseCTL_SetMouseCursor(hWnd, CURSOR_TOPIC);
		}else 
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
}

BOOL MouseCTL_OnRButtonUp(HWND hWnd, UINT nFlags, CPoint point) 
{
	BOOL xRet;

	xRet = FALSE;

	UINT dwGestureID;

	MOUSEHOOKSTRUCT MHS = {0};

	MHS.hwnd = hWnd;
	MHS.pt = point;

	dwGestureID = m_gesture.MouseMessage(WM_RBUTTONUP, (LPARAM)&MHS);

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
		goto __Exit;
	}

	if (m_MouseState.R & MT_STATE_STILL)
	{
		MouseCTL_SetMouseCursor(hWnd, CURSOR_NORMAL);
		m_MouseState.R = 0;
	}else if (m_MouseState.R & MT_STATE_WHELL_ACT)
	{

	}else
	{
		DWORD dw1, dw2;

		dw1 = GetTickCount()&0x00FFFFFF;
		dw2 = MT_TIME_GET(m_MouseState.R);
		if (dw1 - dw2 > MT_TIME_STILL)
		{
			if (m_MouseState.L & MT_STATE_STILL)
			{

			}else if (m_gesture.m_LastDirection == NULL)
			{
				m_MouseState.R |= MT_STATE_STILL;
				MouseCTL_SetMouseCursor(hWnd, CURSOR_PAGE);
			}
		}else if (dwGestureID == INVALID_GETURE_ID)
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

	m_MouseState.R |= MT_STATE_WHELL_ACT;
	m_MouseState.L |= MT_STATE_WHELL_ACT;

	KillTimer(hWnd, MT_EVENT_STILL);

	nGap = zDelta/WHEEL_DELTA;
	if (nGap == 0)
		return TRUE;

	bExtendKey = TRUE;

	if (nGap < 0) 
	{
		if (m_MouseState.R & MT_STATE_PRESS ||
			m_MouseState.R & MT_STATE_STILL)
		{
			MouseCTL_OnAction(MA_R_BT_WHELL_DOWN, 0);
		}else if (m_MouseState.L & MT_STATE_PRESS ||
			m_MouseState.L & MT_STATE_STILL)
		{
			MouseCTL_OnAction(MA_L_BT_WHELL_DOWN, 0);
		}else
		{
			MouseCTL_OnAction(MA_WHELL_DOWN, 0);
		}
	}else 
	{
		if (m_MouseState.R & MT_STATE_PRESS ||
			m_MouseState.R & MT_STATE_STILL)
		{
			MouseCTL_OnAction(MA_R_BT_WHELL_UP, 0);
		}else if (m_MouseState.L & MT_STATE_PRESS ||
			m_MouseState.L & MT_STATE_STILL)
		{
			MouseCTL_OnAction(MA_L_BT_WHELL_UP, 0);
		}else
		{
			MouseCTL_OnAction(MA_WHELL_UP, 0);
		}
	}

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
	}else
	{
		s_dwMoveTimes++;

		if (g_szMouseGesture[0] && s_dwMoveTimes > 10)
		{
			g_szMouseGesture[0] = 0;
			MouseCTL_UpdateStatus();
			s_dwMoveTimes = 0;
		}
	}

	if (nFlags & MK_RBUTTON)
	{
		MOUSEHOOKSTRUCT MHS = {0};
		
		MHS.hwnd = hWnd;
		MHS.pt = point;
		
		m_gesture.MouseMessage(WM_MOUSEMOVE, (LPARAM)&MHS);
	}
}

void MouseCTL_OnTimer(HWND hWnd, UINT nIDEvent)
{
	switch(nIDEvent)
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
			}else if (m_MouseState.L & MT_STATE_PRESS)
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

void WINAPI MouseCTL_UpdateStatus()
{
	if (g_pView && g_pView->parent)
		g_pView->parent->UpdateStatus();

}

void MouseCTL_Init(HWND hWnd)
{
	m_page_cursor = LoadCursor(GetModuleHandle(NULL), MAKEINTRESOURCE(IDC_PAGE_CUR));
	m_topic_cursor = LoadCursor(GetModuleHandle(NULL), MAKEINTRESOURCE(IDC_TOPIC_CUR));

	SetTimer(hWnd, MT_EVENT_RELOAD_CURSOR, 1000, NULL);

	m_gesture.m_szDirctionNow = g_szMouseGesture;
	m_gesture.m_dwDirction_BufLen = sizeof(g_szMouseGesture)/sizeof(TCHAR);
	m_gesture.fnUpdateMessage = MouseCTL_UpdateStatus;
	m_gesture.Attach(hWnd, 20);

	m_gesture.AddGesture(MG_BR_U, "rU");
	m_gesture.AddGesture(MG_BR_D, "rD");
	m_gesture.AddGesture(MG_BR_L, "rL");
	m_gesture.AddGesture(MG_BR_R, "rR");
	m_gesture.AddGesture(MG_BR_U_D, "rUD");
	m_gesture.AddGesture(MG_BR_D_U, "rDU");
	m_gesture.AddGesture(MG_BR_R_L, "rRL");
	m_gesture.AddGesture(MG_BR_L_R, "rLR");
	m_gesture.AddGesture(MG_BR_U_D_U, "rUDU");
	m_gesture.AddGesture(MG_BR_R_L_R, "rRLR");
	m_gesture.AddGesture(MG_BR_LDRDL, "rLDRDL");//S型

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

	switch(Cursor)
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
*/
BOOL MouseCTL_SendInput(char cKey_In, BOOL bExtendKey, BOOL bControlKey)
{
	BOOL xRet;
	char buf[20];
	int nBNow;
	CString StrUnCtl;
	int nScan;

	xRet = FALSE;
	buf[0] = 0;
	nBNow = 0;

	if (bControlKey == FALSE && bExtendKey == TRUE)
	{
		nScan = MapVirtualKey (cKey_In, 0);
		keybd_event(cKey_In, nScan, KEYEVENTF_EXTENDEDKEY, 0 );
		keybd_event(cKey_In, nScan, KEYEVENTF_EXTENDEDKEY|KEYEVENTF_KEYUP, 0);
	}
	else
	{
		if (bControlKey)
			buf[nBNow++] = '^';

		buf[nBNow++] = cKey_In;
		buf[nBNow++] = 0;

		if (bControlKey)
			StrUnCtl = UnescapeControlChars(buf);
		else
			StrUnCtl = buf;

		if (g_pView && g_pView->telnet)
			g_pView->telnet->SendString(StrUnCtl);
	}

	xRet = TRUE;

	return xRet;
}

void MouseCTL_OnAction(DWORD dwActionID, DWORD dwActionID_Sub)
{
	DBG_PRINT("MouseCTL_OnAction(%d, %d)\r\n", dwActionID, dwActionID_Sub);

	switch(dwActionID)
	{
	case MA_L_BT_CLICK:
		MouseCTL_SendInput(VK_RETURN, TRUE, FALSE);
		break;
	case MA_R_BT_CLICK:
		break;
	case MA_M_BT_CLICK:
		MouseCTL_SendInput(VK_LEFT, TRUE, FALSE);
		break;
	case MA_WHELL_UP:
		MouseCTL_SendInput(VK_UP, TRUE, FALSE);
		break;
	case MA_WHELL_DOWN:
		MouseCTL_SendInput(VK_DOWN, TRUE, FALSE);
		break;
	case MA_R_BT_WHELL_UP:
		MouseCTL_SendInput(VK_PRIOR, TRUE, FALSE);
		break;
	case MA_R_BT_WHELL_DOWN:
		MouseCTL_SendInput(VK_NEXT, TRUE, FALSE);
		break;
	case MA_L_BT_WHELL_UP:
		MouseCTL_SendInput('-', FALSE, FALSE);
		break;
	case MA_L_BT_WHELL_DOWN:
		MouseCTL_SendInput('+', FALSE, FALSE);
		break;
	case MA_MOUSE_GESTURE:
		{
			switch(dwActionID_Sub)
			{
			case MG_BR_U:
				MouseCTL_SendInput(VK_HOME, TRUE, FALSE);
				break;
			case MG_BR_D:
				MouseCTL_SendInput(VK_END, TRUE, FALSE);
				break;
			case MG_BR_R:
				MouseCTL_SendInput(VK_RETURN, TRUE, FALSE);
				break;
			case MG_BR_L:
				MouseCTL_SendInput(VK_LEFT, TRUE, FALSE);
				break;
			case MG_BR_L_R:
			case MG_BR_R_L:
				break;
			case MG_BR_D_U:
			case MG_BR_U_D:
				break;
			case MG_BR_U_D_U:
				MouseCTL_SendInput('=', FALSE, FALSE);
				break;
			case MG_BR_R_L_R:
				MouseCTL_SendInput('Q', FALSE, TRUE);
				break;
			case MG_BR_LDRDL:
				MouseCTL_SendInput('S', FALSE, FALSE);
				break;
			case UNKNOWN_GETURE_ID:
				break;
			}
		}
		break;
	}
}