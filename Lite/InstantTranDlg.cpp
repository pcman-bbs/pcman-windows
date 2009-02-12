// The class created by BBcall //

#include "stdafx.h"
#include "pcman.h"
#include "InstantTranDlg.h"
#include "SearchPlugin.h"

IMPLEMENT_DYNCREATE(CTranslationDlg, CDialog)

CTranslationDlg::CTranslationDlg() : CDialog(CTranslationDlg::IDD)
{
	//{{AFX_DATA_INIT(CTranslationDlg)
	//}}AFX_DATA_INIT
}

CTranslationDlg::~CTranslationDlg()
{
}

BEGIN_MESSAGE_MAP(CTranslationDlg, CDialog)
	//{{AFX_MSG_MAP(CMainFrame)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CTranslationDlg::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CKeyMapDlg)
	//}}AFX_DATA_MAP
}

void CTranslationDlg::OnOK()
{
	AppConfig.max_translation_length = GetDlgItemInt(IDC_MAX_TRANLEN, FALSE);
	SearchPluginCollection.MaxTranLength = AppConfig.max_translation_length;
	CDialog::OnOK();
}

void CTranslationDlg::OnCancel()
{
	CDialog::OnCancel();
}

BOOL CTranslationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_MAX_TRANLENSPIN))->SetRange32(0, 100);
	SetDlgItemInt(IDC_MAX_TRANLEN, AppConfig.max_translation_length, FALSE);

	return TRUE;
}