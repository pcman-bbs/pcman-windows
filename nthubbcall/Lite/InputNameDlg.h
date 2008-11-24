#if !defined(AFX_DIRNAME_H__905E67E3_98E0_11D5_B2E6_1480A5CD841C__INCLUDED_)
#define AFX_DIRNAME_H__905E67E3_98E0_11D5_B2E6_1480A5CD841C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InputNameDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInputNameDlg dialog

class CInputNameDlg : public CDialog
{
// Construction
public:
	CString name;
	int max;
	CInputNameDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInputNameDlg)
	enum { IDD = IDD_INPUTNAME };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputNameDlg)
protected:
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInputNameDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRNAME_H__905E67E3_98E0_11D5_B2E6_1480A5CD841C__INCLUDED_)
