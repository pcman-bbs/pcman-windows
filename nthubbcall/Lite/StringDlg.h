#if !defined(AFX_STRINGDLG_H__1979F9C1_330B_11D7_A5E5_90ACC236CC2D__INCLUDED_)
#define AFX_STRINGDLG_H__1979F9C1_330B_11D7_A5E5_90ACC236CC2D__INCLUDED_

#include "../DlgLayout/DlgLayout.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StringDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStringDlg dialog

#include "../DlgLayout/DlgLayout.h"
#include "HotkeyEdit.h"	// Added by ClassView
#include "CtrlEdit.h"	// Added by ClassView

class CStringDlgListCtrl : public CListCtrl
{
public:
	afx_msg void CStringDlgListCtrl::OnKeyDown(UINT ch, UINT n, UINT f);
	DECLARE_MESSAGE_MAP()
};

class CStringDlg : public CDialog
{
// Construction
public:

class MyStringArray : public CStringArray
	{
	public:
		CDWordArray inf;
	};

	CCtrlEdit edit;
	CHotkeyEdit hkedit;
	CDlgLayout dlgl;
	void UpdatePage();
	void UpdateDisplay();
	void EnableOKCancel(BOOL enable);
	void OnSave();
	void OnCancelBtn();
	void ResetUI();
	void SaveList();
	void OnCancel();
	afx_msg void OnOK();
	CString str;
	long inf;

	MyStringArray* stra;
//	BOOL hotkey_changed;
	BOOL data_changed;

	CButton add;
	CButton modify;
	CButton del;
	CButton ansi;
	CButton control;
	CTabCtrl tab;

	CStringDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATAMAP(CStringDlg)
	enum { IDD = IDD_STRINGS };
	CStringDlgListCtrl	list;
	//}}AFX_DATAMAP


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStringDlg)
public:
	//}}AFX_VIRTUAL

// Implementation
protected:
	void LoadList();
	void OnClose();

	// Generated message map functions
	//{{AFX_MSG(CStringDlg)
	afx_msg void OnAdd();
	afx_msg void OnModify();
	afx_msg void OnDel();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnListItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnListGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNewPage();
	afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDelPage();
	afx_msg void OnRenamePage();
	afx_msg void OnDeltaPosPageSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaPosItemSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblClkList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STRINGDLG_H__1979F9C1_330B_11D7_A5E5_90ACC236CC2D__INCLUDED_)
