#if !defined(AFX_CUSTOMIZEDLG_H__191EDA05_2EF0_11D7_A5E5_DAA8618A9A2A__INCLUDED_)
#define AFX_CUSTOMIZEDLG_H__191EDA05_2EF0_11D7_A5E5_DAA8618A9A2A__INCLUDED_

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
		WORD id;
		BYTE fVirt;
		WORD key;
		WORD state;
		HTREEITEM hitem;
		HotkeyData* pnext;
		HotkeyData* pprev;

		HotkeyData(){pnext = NULL;}
		~HotkeyData(){}
	};

	CCustomizeDlg(CWnd* pParent = NULL);   // standard constructor

	void DeleteHotkeyForID(HotkeyData* hkdel);
	HotkeyData* HotkeyDataFromKeys(BYTE fVirt, WORD key);
	HotkeyData* HotkeyDataFromID(HotkeyData* start, WORD id);
	char* UIRead(CTreeCtrl& tree, char* buf, HTREEITEM parent);
	void UICleanup();
	void UIWriteAccels();
	void UIWrite(CFile& ui, HTREEITEM parent);
	void UpdateItemDisplay(HTREEITEM item);
	static LRESULT CALLBACK HotkeyEdit2Proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	CWnd assign;
	WNDPROC old_hkedit2_proc;
	ACCEL* accels;
	WORD accel_count;
	WORD menuitem_count;
	CEdit hkedit2;
	CHotkeyEdit hkedit;
	CTreeCtrl tree;
	CListBox list;
	HotkeyData* pfirst;
	HotkeyData* plast;
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
	void DeleteHotkeyData(HotkeyData* hkdel);
	HotkeyData* NewHotkeyData();
	afx_msg LRESULT OnHotkeyDone(WPARAM wparam, LPARAM lparam);

	// Generated message map functions
	//{{AFX_MSG(CCustomizeDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnApply();
	afx_msg void OnDel();
	afx_msg void OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnCancel();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUSTOMIZEDLG_H__191EDA05_2EF0_11D7_A5E5_DAA8618A9A2A__INCLUDED_)
