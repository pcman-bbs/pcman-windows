// PasswdDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcman.h"
#include "PasswdDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPasswdDlg dialog


CPasswdDlg::CPasswdDlg(CWnd* pParent /*=NULL*/)
		: CDialog(CPasswdDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPasswdDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


BEGIN_MESSAGE_MAP(CPasswdDlg, CDialog)
	//{{AFX_MSG_MAP(CPasswdDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPasswdDlg message handlers

void CPasswdDlg::OnOK()
{
	GetDlgItem(IDC_PASSWD)->GetWindowText(passwd);
	if (!passwd.IsEmpty())
		CDialog::OnOK();
}

BOOL CPasswdDlg::OnInitDialog()
{
	passwd.Empty();
	CDialog::OnInitDialog();
	((CEdit*)GetDlgItem(IDC_PASSWD))->LimitText(16);
	if (!title.IsEmpty())
		SetWindowText(title);
	return TRUE;
}

