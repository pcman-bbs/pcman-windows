// BuildMenu.h : main header file for the BUILDMENU application
//

#if !defined(AFX_BUILDMENU_H__9A10E364_EDCD_40CB_B3F1_4708EF5E5F33__INCLUDED_)
#define AFX_BUILDMENU_H__9A10E364_EDCD_40CB_B3F1_4708EF5E5F33__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

/////////////////////////////////////////////////////////////////////////////
// CBuildMenuApp:
// See BuildMenu.cpp for the implementation of this class
//

class CBuildMenuApp : public CWinApp
{
public:
	CBuildMenuApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBuildMenuApp)
public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CBuildMenuApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BUILDMENU_H__9A10E364_EDCD_40CB_B3F1_4708EF5E5F33__INCLUDED_)
