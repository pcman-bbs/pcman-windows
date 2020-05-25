#if !defined(AFX_LISTDLG_H__DC2094A1_89E0_11D5_B2E5_140094CD0000__INCLUDED_)
#define AFX_LISTDLG_H__DC2094A1_89E0_11D5_B2E5_140094CD0000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CListDlg dialog
#include "AppConfig.h"
#include "TermView.h"
#include "../Resource/resource.h"
#include "../DlgLayout/DlgLayout.h"	// Added by ClassView
#include "SiteListCtrl.h"	// Added by ClassView
#include "SitePage.h"	// Added by ClassView

LPBYTE enc_str(LPCTSTR _src, LPCTSTR key, long& rlen);
CString dec_str(LPBYTE src, LPCTSTR key, long len);
const char SITESLIST_UPDATE_URL[] = "http://pcman.ptt.cc/datas/BBSList";

class CListDlg : public CDialog
{
// Construction
public:
	void UpdateCmdUI();
	HTREEITEM copyitem;
	CString m_InitPath;
	void LoadSite(HTREEITEM parent, LPCTSTR fpath);
	void SaveSite(CFile& file, HTREEITEM parent);
	CSiteListCtrl sites;
	HTREEITEM FindSite(HTREEITEM item, LPCTSTR str);
	CDlgLayout dlgl;

	void LoadSites();

	CMainFrame* parent;
	CTermView* view;

	CListDlg(CTermView* pview);   // standard constructor
	~CListDlg();

// Dialog Data
	//{{AFX_DATA(CListDlg)
	enum { IDD = IDD_LIST };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListDlg)
protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	void AddSite(CArchive* ar, HTREEITEM parent, char* str);
	// Generated message map functions
	//{{AFX_MSG(CListDlg)
	afx_msg void OnCancel();
	afx_msg void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedSites(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkSites(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSitesAddfavorite();
	afx_msg void OnSitesAddhome();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnAddDir();
	afx_msg void OnEditSite();
	afx_msg void OnDelSite();
	afx_msg void OnAddSite();
	afx_msg void SaveSites();
	afx_msg void OnFind();
	afx_msg void OnConnect();
	afx_msg void OnCopy();
	afx_msg void OnCut();
	afx_msg void OnPaste(UINT id);
	afx_msg void OnKeyDownSites(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickSites(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateSitesList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTDLG_H__DC2094A1_89E0_11D5_B2E5_140094CD0000__INCLUDED_)
