#if !defined(AFX_SEARCHBAR_H__C17B2076_BFD0_424A_BDEC_498A6468D7AE__INCLUDED_)
#define AFX_SEARCHBAR_H__C17B2076_BFD0_424A_BDEC_498A6468D7AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SearchBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSearchBar window

class CSearchBar : public CToolBar
{
// Construction
public:
	CSearchBar();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSearchBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetEditFocus();
	bool GetSearchTerm(CString& term);   // return true if term is encoded in UTF-8.
	void UpdateBtn();
	void SetFont(CFont* font);
	CComboBox search_bar;
	BOOL Create(CWnd* pParent);
	virtual ~CSearchBar();

	// Generated message map functions
protected:
	void OnMenuDropDown(NMHDR *pNMHDR, LRESULT *pResult);
	HWND hedit;
	HIMAGELIST img_list;
	static LRESULT CALLBACK EditProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//{{AFX_MSG(CSearchBar)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG

	WNDPROC old_search_bar_proc;
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEARCHBAR_H__C17B2076_BFD0_424A_BDEC_498A6468D7AE__INCLUDED_)
