// InputNameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcman.h"
#include "InputNameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputNameDlg dialog


CInputNameDlg::CInputNameDlg(CWnd* pParent /*=NULL*/)
		: CDialog(CInputNameDlg::IDD, pParent)
{
	max = 0;
	//{{AFX_DATA_INIT(CInputNameDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


BEGIN_MESSAGE_MAP(CInputNameDlg, CDialog)
	//{{AFX_MSG_MAP(CInputNameDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputNameDlg message handlers

BOOL CInputNameDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CEdit* pedit = (CEdit*)GetDlgItem(IDC_NAME);
	pedit->SetWindowText(name);
	if (max > 0)
		pedit->SetLimitText(max);
	return TRUE;
}

void CInputNameDlg::OnOK()
{
	GetDlgItem(IDC_NAME)->GetWindowText(name);
	if (!name.IsEmpty())
		CDialog::OnOK();
}
