#if !defined(AFX_AUTOLOGIN_H__29EFDE0B_33B2_11D7_A5E5_D7F93CE5E92D__INCLUDED_)
#define AFX_AUTOLOGIN_H__29EFDE0B_33B2_11D7_A5E5_D7F93CE5E92D__INCLUDED_

#include "TriggerList.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AutoReplyPage.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CAutoReplyPage dialog


class CAutoReplyPage : public CPropertyPage
{

// Construction
public:
	CTriggerList *triggers;
	void EnableParentBtn(BOOL enable);
	void EnableEdits(BOOL enable);
	void ResetUI();
	void UpdateDisplay();
	CListBox list;
	CEdit msg;
	CEdit respond;
	CEdit first;
	CEdit count;
	CButton add;
	CButton modify;
	CButton del;
	CButton protect;
	CMainFrame* parent;
	CAutoReplyPage();
	~CAutoReplyPage();

// Dialog Data
	//{{AFX_DATA(CAutoReplyPage)
	enum { IDD = IDD_AUTO_REPLY };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAutoReplyPage)
public:
	virtual void OnOK();
	virtual void OnCancel();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
	// Generated message map functions
	//{{AFX_MSG(CAutoReplyPage)
	afx_msg void OnAdd();
	afx_msg void OnDel();
	afx_msg void OnModify();
	afx_msg void OnSelChange();
	virtual BOOL OnInitDialog();
	afx_msg void OnSave();
	afx_msg void OnCancelBtn();
	afx_msg void OnProtect();
	afx_msg void OnHelp();
	afx_msg void OnPassword();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOLOGIN_H__29EFDE0B_33B2_11D7_A5E5_D7F93CE5E92D__INCLUDED_)
