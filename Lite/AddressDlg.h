#if !defined(AFX_ADDRESSDLG_H__AB540E02_41BB_11D5_A2FD_F46114C10000__INCLUDED_)
#define AFX_ADDRESSDLG_H__AB540E02_41BB_11D5_A2FD_F46114C10000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddressDlg.h : header file
//

#include <afxcoll.h>
#include "../Resource/resource.h"

/////////////////////////////////////////////////////////////////////////////
// CAddressDlg dialog

class CAddressDlg : public CDialog
{
// Construction
public:
	~CAddressDlg();
	void OnOK();
	BOOL OnInitDialog();
	CAddressDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAddressDlg)
	enum { IDD = IDD_QUICKCONNECT };
	UINT	port;
	CString	name;
	//}}AFX_DATA

	CString address;
// Overrides
	void DoDataExchange(CDataExchange* pDX);
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddressDlg)
protected:
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddressDlg)
	// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDRESSDLG_H__AB540E02_41BB_11D5_A2FD_F46114C10000__INCLUDED_)
