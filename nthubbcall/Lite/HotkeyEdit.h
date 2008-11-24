#if !defined(AFX_HOTKEYEDIT_H__40D83A83_2FCE_11D7_A5E5_FD8012AD8128__INCLUDED_)
#define AFX_HOTKEYEDIT_H__40D83A83_2FCE_11D7_A5E5_FD8012AD8128__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HotkeyEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHotkeyEdit window

#include "Hotkey.h"

/*
Defines for the fVirt field of the Accelerator table structure.
#define FVIRTKEY  TRUE          // Assumed to be == TRUE
#define FNOINVERT 0x02
#define FSHIFT    0x04
#define FCONTROL  0x08
#define FALT      0x10
*/

const UINT WM_HOTKEYDONE = WM_APP + 'H';
const BYTE FBACK = 1 << 7;

class CHotkeyEdit : public CEdit
{
// Construction
public:
	BYTE fVirt;
	WORD key;

	BYTE accepted;
	CHotkeyEdit();
//	static const char* str_no_hotkey;
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHotkeyEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
//	static CString HotkeyToStr(BYTE _fVirt,WORD _key);
	virtual ~CHotkeyEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CHotkeyEdit)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HOTKEYEDIT_H__40D83A83_2FCE_11D7_A5E5_FD8012AD8128__INCLUDED_)
