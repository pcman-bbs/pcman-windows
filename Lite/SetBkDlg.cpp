// SetBkDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcman.h"
#include "SetBkDlg.h"
#include "TermView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetBkDlg dialog


CSetBkDlg::CSetBkDlg(CTermView* pParent /*=NULL*/)
		: CDialog(CSetBkDlg::IDD, pParent)
{
	view = pParent;
	//{{AFX_DATA_INIT(CSetBkDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


BEGIN_MESSAGE_MAP(CSetBkDlg, CDialog)
	//{{AFX_MSG_MAP(CSetBkDlg)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetBkDlg message handlers

void CSetBkDlg::OnOK()
{
	CSliderCtrl* slider = (CSliderCtrl*)GetDlgItem(IDC_RATIO);
	AppConfig.bkratio = slider->GetPos();
	int i;
	for (i = 0;i < 5; ++i)
	{
		if (((CButton*)GetDlgItem(100 + i))->GetCheck())
		{
			AppConfig.bktype = i;
			break;
		}
	}
	GetDlgItem(IDC_PATH)->GetWindowText(AppConfig.bkpath);
	if (AppConfig.bkpath.IsEmpty() && i > 1)
		MessageBox(LoadString(IDS_CHOOSE_BACKGROUND));
	else
		CDialog::OnOK();
}

void CSetBkDlg::OnBrowse()
{
	CFileDialog dlg(TRUE, "bmp", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
					LoadString(IDS_BMP_FILTEER), this);
	if (dlg.DoModal() == IDOK)
		GetDlgItem(IDC_PATH)->SetWindowText(dlg.GetPathName());
}

BOOL CSetBkDlg::OnInitDialog()
{
	CSliderCtrl* slider = (CSliderCtrl*)GetDlgItem(IDC_RATIO);
	slider->SetRange(0, 10);
	slider->SetPos(AppConfig.bkratio);
	((CButton*)GetDlgItem(AppConfig.bktype + 100))->SetCheck(TRUE);	//Radio ªºID=100~104
	GetDlgItem(IDC_PATH)->SetWindowText(AppConfig.bkpath);
	return TRUE;
}


