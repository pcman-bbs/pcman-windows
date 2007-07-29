#if !defined(AFX_KEYMAPDLG_H__27C436BC_7E1F_419C_A7FF_BC829562A045__INCLUDED_)
#define AFX_KEYMAPDLG_H__27C436BC_7E1F_419C_A7FF_BC829562A045__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KeyMapDlg.h : header file
//
#include "../Resource/resource.h"
#include "HotkeyEdit.h"
#include "KeyMap.h"

/////////////////////////////////////////////////////////////////////////////
// CKeyMapDlg dialog

class CCustomSheet;
class CKeyMapDlg : public CDialog
{
	DECLARE_DYNCREATE(CKeyMapDlg)

// Construction
public:
	inline BOOL CKeyMapDlg::IsAdd(){return ::GetDlgCtrlID(addbtn) == IDC_SAVEKEY;}
	inline BOOL CKeyMapDlg::IsEdit(){return ::GetDlgCtrlID(editbtn) == IDC_SAVEKEY;}
	void CKeyMapDlg::DoDataExchange(CDataExchange* pDX);

	void LeaveEditMode();
	void EnterEditMode(HWND savebtn);
	void UpdateDisplay();
	CKeyMap* pmap;
	CHotkeyEdit hkedit;
	HWND editbtn;
	HWND addbtn;
	HWND delbtn;
	HWND okb;
	HWND cancelb;

	CListCtrl list;
	CCustomSheet* dlg;
	CKeyMapDlg();
	~CKeyMapDlg();

// Dialog Data
	//{{AFX_DATA(CKeyMapDlg)
	enum { IDD = IDD_KEYMAP };
	CEdit	edit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CKeyMapDlg)
public:
	virtual void OnOK();
protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CKeyMapDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDelKey();
	afx_msg void OnAddKey();
	afx_msg void OnItemchangedKeylist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSavekey();
	afx_msg void OnModifykey();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KEYMAPDLG_H__27C436BC_7E1F_419C_A7FF_BC829562A045__INCLUDED_)
