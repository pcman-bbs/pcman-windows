// DlgLayout.h: interface for the CDlgLayout class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLGLAYOUT_H__5B87ECB3_7057_4C3C_8552_CFB73F4EBA76__INCLUDED_)
#define AFX_DLGLAYOUT_H__5B87ECB3_7057_4C3C_8552_CFB73F4EBA76__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>

//////////////////////////////////////////////////////////////////////
/*
	Class Name:		CDlgLayout

	Purpose:		Provide an really easy way to manage dialog controls.

	Limitation:		Everyone can freely use this class for any purpose.

	Author:			PCMan (Hong Jen Yee) <pcman.tw@gmail.com>

	Homepage:		http://pcman.ptt.cc/

	Description:	This class is trying to provide programmers with a
					simple way to manage dialog controls.
					The original idea is from VCL of Delphi.
					In Delphi, people use anchors to position controls and
					I found there is no similiar component in MFC, so I
					write this class myself.
					Though it has limited functionality, it's very easy
					to use and maintain.

	Features:		Easy to use and maintain.
					Minimal memory usage and, most important,
					can be "used without MFC."

*/
//////////////////////////////////////////////////////////////////////


// Definitions of Anchor Flags
// the "DLA" prefix stands for "Dialog Layout Anchor"

const UINT DLA_LEFT = 1;
const UINT DLA_TOP = 2;
const UINT DLA_RIGHT = 4;
const UINT DLA_BOTTOM = 8;
const UINT DLA_ALL = 15;	//The combination of the other four flags

// This structure is used to define the "Layout Table,"
// which is an array of DLGLAYOUT.
struct DLGLAYOUT
{
DWORD id: 28;
DWORD layout: 4;
};


class CDlgLayout
{
public:
	void OnSize(WPARAM w, LPARAM l);
	void OnSize(UINT nType, int cx, int cy);
	void OnGetMinMaxInfo(MINMAXINFO* mminfo);	// MFC version
	LRESULT OnGetMinMaxInfo(LPARAM lparam);	// non-MFC version called in WndProc
	void Load(HWND _hdlg, DLGLAYOUT *table, int size);	//call this in response to WM_INITDIALOG

	CDlgLayout();
	virtual ~CDlgLayout();

protected:
	void OnSize(int cx, int cy);	// call this in response to WM_SIZE
	HWND hdlg;
	int oldcx, oldcy;
	int mincx, mincy;

	DLGLAYOUT* tab;
	DLGLAYOUT* tab_end;
};

//for non-MFC
inline LRESULT CDlgLayout::OnGetMinMaxInfo(LPARAM lparam)
{
	if (mincx)
	{
		LPMINMAXINFO(lparam)->ptMinTrackSize.x = mincx;
		LPMINMAXINFO(lparam)->ptMinTrackSize.y = mincy;
	}
	return 0;
}

//for non-MFC
inline void CDlgLayout::OnSize(WPARAM w, LPARAM l)
{
	if (w != SIZE_MINIMIZED)
		OnSize(LOWORD(l), HIWORD(l));
}

//for MFC
inline void CDlgLayout::OnGetMinMaxInfo(MINMAXINFO *mminfo)
{
	if (mincx)
	{
		mminfo->ptMinTrackSize.x = mincx;
		mminfo->ptMinTrackSize.y = mincy;
	}
}

//for MFC
inline void CDlgLayout::OnSize(UINT nType, int cx, int cy)
{
	if (nType != SIZE_MINIMIZED)
		OnSize(cx, cy);
}

#endif // !defined(AFX_DLGLAYOUT_H__5B87ECB3_7057_4C3C_8552_CFB73F4EBA76__INCLUDED_)
