#if !defined(AFX_OTHERPAGE_H__29EFDE11_33B2_11D7_A5E5_D7F93CE5E92D__INCLUDED_)
#define AFX_OTHERPAGE_H__29EFDE11_33B2_11D7_A5E5_D7F93CE5E92D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OtherPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COtherPage dialog
class CConfigDlg;

class COtherPage : public CPropertyPage
{
// Construction
public:
	COtherPage();
	~COtherPage();

// Dialog Data
	//{{AFX_DATA(COtherPage)
	enum { IDD = IDD_OTHER };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COtherPage)
public:
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
	// Generated message map functions
	//{{AFX_MSG(COtherPage)
	afx_msg void OnSetWave();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OTHERPAGE_H__29EFDE11_33B2_11D7_A5E5_D7F93CE5E92D__INCLUDED_)
