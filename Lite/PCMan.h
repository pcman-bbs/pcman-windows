// PCMan4.h : main header file for the PCMAN4 application
//

#if !defined(AFX_PCMAN4_H__9231A864_8865_11D5_B2E5_140094CD0000__INCLUDED_)
#define AFX_PCMAN4_H__9231A864_8865_11D5_B2E5_140094CD0000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "../Resource/resource.h"       // main symbols
#include "ListDlg.h"	// Added by ClassView

/////////////////////////////////////////////////////////////////////////////
// CApp:
// See PCMan.cpp for the implementation of this class
//

class CApp : public CWinApp
{
public:
	CApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CApp)
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

public:
	//{{AFX_MSG(CApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PCMAN4_H__9231A864_8865_11D5_B2E5_140094CD0000__INCLUDED_)
