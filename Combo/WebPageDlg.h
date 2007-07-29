#if !defined(AFX_WEBPAGEDLG_H__9245C086_A850_4C2A_A9D1_0D4AF4993277__INCLUDED_)
#define AFX_WEBPAGEDLG_H__9245C086_A850_4C2A_A9D1_0D4AF4993277__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WebPageDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWebPageDlg dialog

#include "../Resource/resource.h"

class CWebPageDlg : public CDialog
{
// Construction
public:
	CWebPageDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWebPageDlg)
	enum { IDD = IDD_WEBPAGE };
	CString	m_Name;
	CString	m_URL;
	CString	m_NamePrompt;
	CString	m_URLPrompt;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWebPageDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWebPageDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WEBPAGEDLG_H__9245C086_A850_4C2A_A9D1_0D4AF4993277__INCLUDED_)
