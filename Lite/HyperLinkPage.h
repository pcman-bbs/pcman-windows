#if !defined(AFX_HYPERLINKPAGE_H__A3982263_E25D_4BF5_9B22_72B61115E9C5__INCLUDED_)
#define AFX_HYPERLINKPAGE_H__A3982263_E25D_4BF5_9B22_72B61115E9C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HyperLinkPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHyperLinkPage dialog

class CHyperLinkPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CHyperLinkPage)

// Construction
public:
	void UpdateUI();
	void LeaveEditMode();
	void EnableParentBtn(BOOL enable);
	CHyperLinkPage();
	~CHyperLinkPage();

// Dialog Data
	//{{AFX_DATA(CHyperLinkPage)
	enum { IDD = IDD_HYPERLINK };
	CListCtrl	list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CHyperLinkPage)
public:
	virtual void OnOK();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	COLORREF tmpclr;
	// Generated message map functions
	//{{AFX_MSG(CHyperLinkPage)
	afx_msg void OnAdd();
	afx_msg void OnEdit();
	afx_msg void OnDel();
	afx_msg void OnEditSave();
	afx_msg void OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBrowse();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnColor();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HYPERLINKPAGE_H__A3982263_E25D_4BF5_9B22_72B61115E9C5__INCLUDED_)
