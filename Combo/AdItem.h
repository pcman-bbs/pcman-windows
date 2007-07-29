#if !defined(AFX_ADITEM_H__C6BFB522_2650_11D6_8461_ACBF81CD0000__INCLUDED_)
#define AFX_ADITEM_H__C6BFB522_2650_11D6_8461_ACBF81CD0000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AdItem.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAdItem dialog

class CAdItem : public CDialog
{
// Construction
public:
	CAdItem(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	CString url;
	CString title;
	//{{AFX_DATA(CAdItem)
	enum { IDD = IDD_ADITEM };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdItem)
protected:
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAdItem)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADITEM_H__C6BFB522_2650_11D6_8461_ACBF81CD0000__INCLUDED_)
