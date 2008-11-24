#if !defined(AFX_GENERALPAGE_H__29EFDE10_33B2_11D7_A5E5_D7F93CE5E92D__INCLUDED_)
#define AFX_GENERALPAGE_H__29EFDE10_33B2_11D7_A5E5_D7F93CE5E92D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GeneralPage.h : header file
//
#include "../Resource/resource.h"

/////////////////////////////////////////////////////////////////////////////
// CGeneralPage dialog
class MyPropertySheet;

class CGeneralPage : public CPropertyPage
{
// Construction
public:

	CGeneralPage();
	~CGeneralPage();

// Dialog Data
	//{{AFX_DATA(CGeneralPage)
	//}}AFX_DATA
	enum { IDD = IDD_GENERAL };
	CHotKeyCtrl	m_wndHotkey;
	COLORREF active_tab_textcolor;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGeneralPage)
public:
	virtual void OnOK();
protected:
	//}}AFX_VIRTUAL
	void DoDataExchange(CDataExchange* pDX);

// Implementation
protected:
	void OnActiveTabTextColor();
	// Generated message map functions
	//{{AFX_MSG(CGeneralPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GENERALPAGE_H__29EFDE10_33B2_11D7_A5E5_D7F93CE5E92D__INCLUDED_)
