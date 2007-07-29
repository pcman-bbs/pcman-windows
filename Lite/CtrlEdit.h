#if !defined(AFX_CTRLEDIT_H__B0BA58B2_0EF1_41AC_A7F2_EE7C8722F3D5__INCLUDED_)
#define AFX_CTRLEDIT_H__B0BA58B2_0EF1_41AC_A7F2_EE7C8722F3D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CtrlEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCtrlEdit window

class CCtrlEdit : public CEdit
{
// Construction
public:
	CCtrlEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCtrlEdit)
protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCtrlEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCtrlEdit)
	afx_msg void OnCancelMode();
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//}}AFX_MSG
	LRESULT OnPaste(WPARAM w, LPARAM l);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLEDIT_H__B0BA58B2_0EF1_41AC_A7F2_EE7C8722F3D5__INCLUDED_)
