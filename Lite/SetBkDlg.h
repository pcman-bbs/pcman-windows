#if !defined(AFX_SETBKDLG_H__F87BB861_0C6B_11D6_BD42_0040F427D59A__INCLUDED_)
#define AFX_SETBKDLG_H__F87BB861_0C6B_11D6_BD42_0040F427D59A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetBkDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetBkDlg dialog
class CTermView;

class CSetBkDlg : public CDialog
{
// Construction
public:
	CTermView* view;
	CSetBkDlg(CTermView* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetBkDlg)
	enum { IDD = IDD_SETBK };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetBkDlg)
protected:
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetBkDlg)
	virtual void OnOK();
	afx_msg void OnBrowse();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETBKDLG_H__F87BB861_0C6B_11D6_BD42_0040F427D59A__INCLUDED_)
