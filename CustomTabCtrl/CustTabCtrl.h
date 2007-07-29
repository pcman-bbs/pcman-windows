// CustomTabCtrl.h : main header file for the CUSTOMTABCTRL application
//

#if !defined(AFX_CUSTOMTABCTRL_H__B2886D41_35A1_4D53_ADC8_5CA90A02DF0F__INCLUDED_)
#define AFX_CUSTOMTABCTRL_H__B2886D41_35A1_4D53_ADC8_5CA90A02DF0F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CCustomTabCtrlApp:
// See CustomTabCtrl.cpp for the implementation of this class
//

class CCustomTabCtrlApp : public CWinApp
{
public:
	CCustomTabCtrlApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomTabCtrlApp)
public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

public:
	//{{AFX_MSG(CCustomTabCtrlApp)
	afx_msg void OnAppAbout();
	// NOTE - the ClassWizard will add and remove member functions here.
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUSTOMTABCTRL_H__B2886D41_35A1_4D53_ADC8_5CA90A02DF0F__INCLUDED_)
