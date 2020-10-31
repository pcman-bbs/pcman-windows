#if !defined(AFX_CUSTOMIZEDLG_H__191EDA05_2EF0_11D7_A5E5_DAA8618A9A2A__INCLUDED_)
#define AFX_CUSTOMIZEDLG_H__191EDA05_2EF0_11D7_A5E5_DAA8618A9A2A__INCLUDED_

#include <list>
#include <map>

#include "..\BuildMenu\BuildMenu.h"
#include "HotkeyEdit.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CustomizeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCustomizeDlg dialog

class CCustomizeDlg : public CDialog
{
// Construction
public:
	struct HotkeyData
	{
		BYTE type;
		WORD state;
		WORD cmd;
		HTREEITEM hitem;

		HotkeyData(){}
		~HotkeyData(){}
	};

	CCustomizeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCustomizeDlg)
	enum { IDD = IDD_CUSTOMIZEKEY };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomizeDlg)
protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	afx_msg LRESULT OnHotkeyDone(WPARAM wparam, LPARAM lparam);

	// Generated message map functions
	//{{AFX_MSG(CCustomizeDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnApply();
	afx_msg void OnDel();
	afx_msg void OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CTreeCtrl menu_tree_;
	CListBox key_list_;
	CWnd text_assigned_to_;
	CHotkeyEdit hkedit_;
	CButton btn_apply_hotkey_;
	CButton btn_delete_hotkey_;

	AcceleratorTable accel_table_;
	std::list<HotkeyData> hotkey_data_storage_;
	std::map<WORD, HotkeyData *> cmd_to_hotkey_data_;

	HotkeyData *NewHotkeyData();
	void UpdateTreeItemDisplay(HotkeyData *data);
	void UpdateHotkeyDisplay(HotkeyData *data);
	HotkeyData *HotkeyDataFromKeys(BYTE fVirt, WORD key);
	void UIWriteAccels();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUSTOMIZEDLG_H__191EDA05_2EF0_11D7_A5E5_DAA8618A9A2A__INCLUDED_)
