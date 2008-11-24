#if !defined(AFX_CONNECTPAGE_H__D8AF351E_3E94_49DF_9B83_8B18DB394CBD__INCLUDED_)
#define AFX_CONNECTPAGE_H__D8AF351E_3E94_49DF_9B83_8B18DB394CBD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConnectPage.h : header file
//

#include "AddressDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CConnectPage dialog

class CConnectPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CConnectPage)

// Construction
public:
	CConnectPage();
	~CConnectPage();

// Dialog Data
	//{{AFX_DATA(CConnectPage)
	enum { IDD = IDD_CONNECT };
	CString	address;
	CString	name;
	UINT	port;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CConnectPage)
public:
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CConnectPage)
	afx_msg BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONNECTPAGE_H__D8AF351E_3E94_49DF_9B83_8B18DB394CBD__INCLUDED_)
