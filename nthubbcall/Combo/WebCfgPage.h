#if !defined(AFX_WEBCFGPAGE_H__F895CF8E_8FAC_431F_924D_B7B54B404BD3__INCLUDED_)
#define AFX_WEBCFGPAGE_H__F895CF8E_8FAC_431F_924D_B7B54B404BD3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// webcfgpage.h : header file
//
#include "../resource/resource.h"
/////////////////////////////////////////////////////////////////////////////
// CWebCfgPage dialog

class CWebCfgPage : public CPropertyPage
{
// Construction
public:
	CWebCfgPage();   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWebCfgPage)
	enum { IDD = IDD_WEB };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWebCfgPage)
public:
	virtual void OnOK();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWebCfgPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnIeFav();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WEBCFGPAGE_H__F895CF8E_8FAC_431F_924D_B7B54B404BD3__INCLUDED_)
