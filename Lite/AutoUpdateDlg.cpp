// AutoUpdateDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "..\Resource\common.h"
#include "AutoUpdateDlg.h"
#include "pcman.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAutoUpdateDlg dialog


CAutoUpdateDlg::CAutoUpdateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAutoUpdateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAutoUpdateDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	checked = false;
}


void CAutoUpdateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutoUpdateDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAutoUpdateDlg, CDialog)
	//{{AFX_MSG_MAP(CAutoUpdateDlg)
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	ON_BN_CLICKED(IDC_OPEN_DLPAGE, OnOpenDLPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoUpdateDlg message handlers

void CAutoUpdateDlg::OnOK() 
{
	// TODO: Add extra validation here
	if(checked)
		EndDialog(0);
	else
		EndDialog(1);
	//CDialog::OnOK();
}

void CAutoUpdateDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	if(checked)
		EndDialog(0);
	else
		EndDialog(2);
	//CDialog::OnCancel();
}

void CAutoUpdateDlg::OnCheck1() 
{
	// TODO: Add your control notification handler code here
	checked = !checked;
}

void CAutoUpdateDlg::OnOpenDLPage()
{
	CMainFrame* mainfrm = (CMainFrame*)AfxGetApp()->GetMainWnd();
	mainfrm->PostMessage(WM_DOWNLOAD_PAGE);
}