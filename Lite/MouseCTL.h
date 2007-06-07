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

BOOL MouseCTL_OnLButtonUp_PreProcess(HWND hWnd, UINT nFlags, CPoint point);
void MouseCTL_OnLButtonUp_PostProcess(HWND hWnd, UINT nFlags, CPoint point);

BOOL MouseCTL_OnMouseWheel(HWND hWnd, UINT nFlags, short zDelta, CPoint pt);

void MouseCTL_OnTimer(HWND hWnd, UINT nIDEvent);

void MouseCTL_OnSetFocus(HWND hWnd);
void MouseCTL_Init(HWND hWnd);
void MouseCTL_Reset(BOOL bRestoreMouse);
void MouseCTL_Reset();

BOOL MouseCTL_IsCursorInSelect();

extern CTermView *g_pView;