#pragma once

void MouseCTL_OnRButtonDblClk(HWND hWnd, UINT nFlags, CPoint point);
void MouseCTL_OnRButtonDown(HWND hWnd, UINT nFlags, CPoint point);
BOOL MouseCTL_OnRButtonUp(HWND hWnd, UINT nFlags, CPoint point);
void MouseCTL_OnMButtonDown(HWND hWnd, UINT nFlags, CPoint point);
void MouseCTL_OnMButtonUp(HWND hWnd, UINT nFlags, CPoint point);
void MouseCTL_OnMButtonDblClk(HWND hWnd, UINT nFlags, CPoint point);
void MouseCTL_OnLButtonDblClk(HWND hWnd, UINT nFlags, CPoint point);
void MouseCTL_OnLButtonDown(HWND hWnd, UINT nFlags, CPoint point);
void MouseCTL_OnLButtonUp(HWND hWnd, UINT nFlags, CPoint point);
void MouseCTL_OnMouseMove(HWND hWnd, UINT nFlags, CPoint point);

BOOL MouseCTL_OnLButtonUp_PreProcess(HWND hWnd, UINT nFlags, CPoint point);
void MouseCTL_OnLButtonUp_PostProcess(HWND hWnd, UINT nFlags, CPoint point);

BOOL MouseCTL_OnMouseWheel(HWND hWnd, UINT nFlags, short zDelta, CPoint pt);

void MouseCTL_OnTimer(HWND hWnd, UINT nIDEvent);

void MouseCTL_OnSetFocus(HWND hWnd);
void MouseCTL_Init(HWND hWnd);
void MouseCTL_Reset(BOOL bRestoreMouse);
void MouseCTL_Reset();

BOOL MouseCTL_IsCursorInSelect();

void MouseCTL_OnAction(DWORD dwActionID, DWORD dwActionID_Sub);
void WINAPI MouseCTL_UpdateStatus(DWORD nID);
HCURSOR MouseCTL_GetCurrentMouseCursor();

extern CTermView *g_pView;
char *MouseCTL_GetStatusInfo();


struct EXTEND_KEY
{
	char *szName;
	char VirKey;
};

const EXTEND_KEY Extend_Key[] =
{
	{"HOME", VK_HOME},
	{"END", VK_END},
	{"NEXT", VK_NEXT},//page down
	{"PRIOR", VK_PRIOR},//page up
	{"LEFT", VK_LEFT},
	{"RIGHT", VK_RIGHT},
	{"UP", VK_UP},
	{"DOWN", VK_DOWN},
	{"INSERT", VK_INSERT},
	{"DELETE", VK_DELETE},
};

#define KEYSTR_ENTER "ENTER"

