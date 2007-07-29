#if !defined(AFX_ADLIST_H__C6BFB523_2650_11D6_8461_ACBF81CD0000__INCLUDED_)
#define AFX_ADLIST_H__C6BFB523_2650_11D6_8461_ACBF81CD0000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AdList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAdList dialog

class CAdList : public CDialog
{
// Construction
public:
	CListBox list;
	CAdList(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAdList)
	enum { IDD = IDD_ADLIST };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdList)
protected:
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAdList)
	virtual void OnOK();
	afx_msg void OnAdd();
	afx_msg BOOL OnEdit();
	afx_msg void OnDel();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADLIST_H__C6BFB523_2650_11D6_8461_ACBF81CD0000__INCLUDED_)
