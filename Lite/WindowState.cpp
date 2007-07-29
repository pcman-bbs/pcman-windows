// WindowState.cpp: implementation of the CWindowState class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcman.h"
#include "WindowState.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWindowState::CWindowState()
{

}

CWindowState::~CWindowState()
{

}

void CWindowState::Save(HWND hwnd)
{
	WINDOWPLACEMENT wpm;
	wpm.length = sizeof(wpm);
	GetWindowPlacement(hwnd, &wpm);
	CopyRect(&rect, &wpm.rcNormalPosition);
	showcmd = wpm.showCmd;
}

void CWindowState::Restore(HWND hwnd)
{
	MoveWindow(hwnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE);
	ShowWindow(hwnd, showcmd);
}
