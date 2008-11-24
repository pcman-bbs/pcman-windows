#if !defined(AFX_COLORCONFIGDLG_H__8D190400_AABF_11D5_B739_F87567CD0000__INCLUDED_)
#define AFX_COLORCONFIGDLG_H__8D190400_AABF_11D5_B739_F87567CD0000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorConfigDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorConfigDlg dialog

class CColorConfigDlg : public CDialog
{
// Construction
public:
	void OnSelColor(UINT id);
	COLORREF colormap[16];

	CColorConfigDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CColorConfigDlg)
	enum { IDD = IDD_COLOR };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorConfigDlg)
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CColorConfigDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnDefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORCONFIGDLG_H__8D190400_AABF_11D5_B739_F87567CD0000__INCLUDED_)
