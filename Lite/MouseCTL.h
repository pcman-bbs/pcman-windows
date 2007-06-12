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

BOOL MouseCTL_SendInput(char cKey_In, BOOL bExtendKey, BOOL bControlKey);
void MouseCTL_OnAction(DWORD dwActionID, DWORD dwActionID_Sub);
void WINAPI MouseCTL_UpdateStatus();
HCURSOR MouseCTL_GetCurrentMouseCursor();

extern CTermView *g_pView;
extern char g_szMouseGesture[30];