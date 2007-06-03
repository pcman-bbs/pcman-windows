#include "StdAfx.h"

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

#define MT_TIME_GET(MouseState) MouseState&0x00FFFFFF
#define MT_TIME_SET(MouseState, time) MouseState = (MouseState&0xFF000000)|(time&0x00FFFFFF)

static DWORD m_dwWndFocusTimeCounter;
static DWORD m_dwMenuTimeCounter;

static HCURSOR m_page_cursor;
static HCURSOR m_topic_cursor;

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

}

BOOL MouseCTL_OnLButtonUp_PreProcess(HWND hWnd, UINT nFlags, CPoint point)
{
	m_MouseState.L &= ~MT_STATE_PRESS;
	KillTimer(hWnd, MT_EVENT_STILL);
	if (m_MouseState.R & MT_STATE_STILL)
	{
		ReleaseCapture();
		m_MouseState.R = 0;
		return FALSE;
	}else if (m_MouseState.L & MT_STATE_STILL)
	{
		ReleaseCapture();
		m_MouseState.L = 0;
		return FALSE;
	}
	if (m_MouseState.L & MT_STATE_STILL_BY_TIMER)
	{
	}else
		ReleaseCapture();

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

	int nScan;
	int nKey;

	if (m_MouseState.L & MT_STATE_STILL_BY_TIMER)
	{
		m_MouseState.L |= MT_STATE_STILL;//不好，因為下面有一個也是在設  MT_STATE_STILL
		return;
	}

	if (m_MouseState.L & MT_STATE_STILL)
	{
		ReleaseCapture();
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
			SetCapture(hWnd);
			SetCursor(m_topic_cursor);
		}else 
		{
			nKey = VK_RETURN;
			nScan = MapVirtualKey (nKey, 0);
			keybd_event(nKey, nScan, 0, 0 );
			keybd_event(nKey, nScan, KEYEVENTF_KEYUP, 0);			
		}
	}
}

void MouseCTL_OnRButtonDblClk(HWND hWnd, UINT nFlags, CPoint point) 
{

}

void MouseCTL_OnRButtonDown(HWND hWnd, UINT nFlags, CPoint point) 
{
	DWORD dwStill;

	dwStill = m_MouseState.R & MT_STATE_STILL;
	m_MouseState.R = 0;
	MT_TIME_SET(m_MouseState.R, GetTickCount());
	m_MouseState.R |= MT_STATE_PRESS;
	m_MouseState.R |= dwStill;
	SetTimer(hWnd, MT_EVENT_STILL, MT_TIME_STILL, NULL);
}

BOOL MouseCTL_OnRButtonUp(HWND hWnd, UINT nFlags, CPoint point) 
{
	BOOL xRet;

	xRet = FALSE;

	m_MouseState.R &= ~MT_STATE_PRESS;

	KillTimer(hWnd, MT_EVENT_STILL);
	if (m_MouseState.R & MT_STATE_STILL_BY_TIMER)
	{
		m_MouseState.R |= MT_STATE_STILL;//不好，因為下面有一個也是在設  MT_STATE_STILL
		goto __Exit;
	}

	if (m_MouseState.R & MT_STATE_STILL)
	{
		ReleaseCapture();
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

			}else	
			{
				m_MouseState.R |= MT_STATE_STILL;
				SetCapture(hWnd);
				SetCursor(m_page_cursor);
			}
		}else 
		{
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
	int nScan;
	int nKey;

	nKey = VK_LEFT;

	nScan = MapVirtualKey (nKey, 0);
	keybd_event(nKey, nScan, KEYEVENTF_EXTENDEDKEY, 0 );
	keybd_event(nKey, nScan, KEYEVENTF_EXTENDEDKEY|KEYEVENTF_KEYUP, 0);

}

void MouseCTL_OnMButtonDblClk(HWND hWnd, UINT nFlags, CPoint point) 
{

}

BOOL MouseCTL_OnMouseWheel(HWND hWnd, UINT nFlags, short zDelta, CPoint pt)
{
	
	int nScan, nScan0;
	int nKey, nKey0;
	int nGap;
	int nKeyMode;
	BOOL bKeyExt;

	bKeyExt = FALSE;
	nKeyMode = 0;
	m_MouseState.R |= MT_STATE_WHELL_ACT;
	m_MouseState.L |= MT_STATE_WHELL_ACT;

	KillTimer(hWnd, MT_EVENT_STILL);

	nGap = zDelta/WHEEL_DELTA;
	if (nGap == 0)
		return TRUE;

	if (nGap < 0) 
	{
		if (m_MouseState.R & MT_STATE_PRESS ||
			m_MouseState.R & MT_STATE_STILL)
		{
			bKeyExt = TRUE;
			nKey = VK_NEXT;
		}else if (m_MouseState.L & MT_STATE_PRESS ||
			m_MouseState.L & MT_STATE_STILL)
		{
			nKeyMode = 1;
			nKey = 0xBB;
		}else
		{
			bKeyExt = TRUE;
			nKey = VK_DOWN;
		}
	}else 
	{
		if (m_MouseState.R & MT_STATE_PRESS ||
			m_MouseState.R & MT_STATE_STILL)
		{
			bKeyExt = TRUE;
			nKey = VK_PRIOR;
		}else if (m_MouseState.L & MT_STATE_PRESS ||
			m_MouseState.L & MT_STATE_STILL)
		{
			nKey = 0xBD;
		}else
		{
			bKeyExt = TRUE;
			nKey = VK_UP;
		}
	}

	if (nKeyMode == 1)
	{
		nKey0 = VK_SHIFT;
		nScan0 = MapVirtualKey (nKey0, 0);
		keybd_event(nKey0, nScan0, 0, 0 );
	}
	nScan = MapVirtualKey (nKey, 0);
	keybd_event(nKey, nScan, bKeyExt?KEYEVENTF_EXTENDEDKEY:0, 0 );
	keybd_event(nKey, nScan, (bKeyExt?KEYEVENTF_EXTENDEDKEY:0)|KEYEVENTF_KEYUP, 0);

	if (nKeyMode == 1)
	{
		keybd_event(nKey0, nScan0, KEYEVENTF_KEYUP, 0);
	}

	return TRUE;
}

void MouseCTL_OnTimer(HWND hWnd, UINT nIDEvent)
{
	if (nIDEvent != MT_EVENT_STILL)
		return ;

	HCURSOR hCursor;
	DWORD *pdwMouseState;

	KillTimer(hWnd, nIDEvent);
	hCursor = NULL;
	pdwMouseState = NULL;
	
	if (m_MouseState.R & MT_STATE_PRESS)
	{
		hCursor = m_page_cursor;
		pdwMouseState = &m_MouseState.R;
	}else if (m_MouseState.L & MT_STATE_PRESS)
	{
		hCursor = m_topic_cursor;
		pdwMouseState = &m_MouseState.L;
	}
	if (m_MouseState.L & MT_STATE_STILL)
		hCursor = NULL;
	
	if (hCursor)
	{
		ReleaseCapture();
		SetCapture(hWnd);
		SetCursor(hCursor);
		*pdwMouseState |= MT_STATE_STILL_BY_TIMER;
	}
		
}

void MouseCTL_OnSetFocus(HWND hWnd)
{
	m_dwWndFocusTimeCounter = GetTickCount();
}

void MouseCTL_Init(HWND hWnd)
{
	m_page_cursor = LoadCursor(GetModuleHandle(NULL), MAKEINTRESOURCE(IDC_PAGE_CUR));
	m_topic_cursor = LoadCursor(GetModuleHandle(NULL), MAKEINTRESOURCE(IDC_TOPIC_CUR));
}

void MouseCTL_Reset()
{
	memset(&m_MouseState, 0, sizeof(m_MouseState));
	m_dwWndFocusTimeCounter = 0;
	m_dwMenuTimeCounter = 0;

	ReleaseCapture();
	SetCursor(NULL);
}
