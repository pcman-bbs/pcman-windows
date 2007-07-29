#if !defined(AFX_SITEPAGE_H__86A83021_4D63_11D8_9561_D32F581E10CF__INCLUDED_)
#define AFX_SITEPAGE_H__86A83021_4D63_11D8_9561_D32F581E10CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SitePage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSitePage dialog
class CSiteSettings;

class CSitePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CSitePage)
// Construction
public:
	bool show_use_global;
//	int use_default;
	CSiteSettings* psettings;
//	Flags
//	WORD lcount_changed:1;
//	WORD showscroll_changed:1;
//	WORD cols_per_page_changed:1;
//	WORD lines_per_page_changed:1;

	CSitePage();
	~CSitePage();
	void DoDataExchange(CDataExchange* pDX);

// Dialog Data
	//{{AFX_DATA(CSitePage)
	enum { IDD = IDD_SITE };
	CComboBox ctermtype;
	CComboBox ckeymap;
	CComboBox cesc_convert;
	CComboBox cticonv;
	CComboBox ctoconv;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSitePage)
public:
	virtual void OnOK();
protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateDisplay();
	void EnableControls(bool enable);
	// Generated message map functions
	//{{AFX_MSG(CSitePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnIdlehelp();
	afx_msg void OnAddMap();
	afx_msg void OnEditMap();
	afx_msg void OnRenameMap();
	afx_msg void OnDelMap();
	afx_msg void OnSelchangeKeyMap();
	afx_msg void OnUseGlobalSettings();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SITEPAGE_H__86A83021_4D63_11D8_9561_D32F581E10CF__INCLUDED_)
