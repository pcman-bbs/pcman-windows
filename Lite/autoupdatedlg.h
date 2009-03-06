#if !defined(AFX_AUTOUPDATEDLG_H__AD5692AC_8AF2_41AA_8CB7_188BDC7969F8__INCLUDED_)
#define AFX_AUTOUPDATEDLG_H__AD5692AC_8AF2_41AA_8CB7_188BDC7969F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AutoUpdateDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAutoUpdateDlg dialog

class CAutoUpdateDlg : public CDialog
{
// Construction
public:
	CAutoUpdateDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAutoUpdateDlg)
	enum { IDD = IDD_AUTOUPDATE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoUpdateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAutoUpdateDlg)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnCheck1();
	afx_msg void OnOpenDLPage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	bool checked;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOUPDATEDLG_H__AD5692AC_8AF2_41AA_8CB7_188BDC7969F8__INCLUDED_)
