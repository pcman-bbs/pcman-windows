#if !defined(AFX_FORMATDLG_H__3DDF0A61_AC31_11D5_B739_F87567CD0000__INCLUDED_)
#define AFX_FORMATDLG_H__3DDF0A61_AC31_11D5_B739_F87567CD0000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AnsiBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAnsiBar dialog

class CTermView;

class CAnsiBar : public CDialogBar
{
// Construction
public:
	enum
	{
		UC_ALL = 0,
		UC_FG = 1,
		UC_BK = 2,
		UC_BLINK = 3
	};

	void Send(DWORD type);
	BOOL bchild;
	int width;
	void AddAttr(BYTE& atb, UINT type);

	void UpdateColor(UINT type);

	BYTE is_blink;
	BYTE bk;
	BYTE fg;
	BOOL bactive;
	CTermView* view;
	CAnsiBar(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAnsiBar)
	enum { IDD = IDD_FORMAT };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnsiBar)
public:
	//}}AFX_VIRTUAL

// Implementation

	// Generated message map functions
	afx_msg void OnBk();
	afx_msg void OnFg();
	afx_msg void OnBlink();
	afx_msg void OnSend();
	afx_msg void OnSendDropDown();
	//{{AFX_MSG(CAnsiBar)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMove(int x, int y);
	//}}AFX_MSG
protected:
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORMATDLG_H__3DDF0A61_AC31_11D5_B739_F87567CD0000__INCLUDED_)
