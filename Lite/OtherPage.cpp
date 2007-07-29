// OtherPage.cpp : implementation file
//

#include "stdafx.h"
#include "pcman.h"
#include "OtherPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COtherPage property page

COtherPage::COtherPage() : CPropertyPage(COtherPage::IDD)
{
	//{{AFX_DATA_INIT(COtherPage)
	//}}AFX_DATA_INIT
}

COtherPage::~COtherPage()
{
}


BEGIN_MESSAGE_MAP(COtherPage, CPropertyPage)
	//{{AFX_MSG_MAP(COtherPage)
	ON_BN_CLICKED(IDC_SETWAVE, OnSetWave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COtherPage message handlers

void COtherPage::OnOK()
{
	AppConfig.auto_font		= IsDlgButtonChecked(IDC_AUTOFONT);
	AppConfig.auto_cancelsel = IsDlgButtonChecked(IDC_AUTOCANCELSEL);
	AppConfig.auto_close	= IsDlgButtonChecked(IDC_AUTOCLOSE);
	AppConfig.auto_copy		= IsDlgButtonChecked(IDC_AUTOCOPY);
	AppConfig.enter_reconnect = IsDlgButtonChecked(IDC_ENTER_RECONNECT);
	AppConfig.nocon_enter_reconnect = IsDlgButtonChecked(IDC_NOCON_ENTER_RECONNECT);
	AppConfig.dblclk_select = IsDlgButtonChecked(IDC_DBLCLKSELECT);

	AppConfig.ed_cols_per_page	= GetDlgItemInt(IDC_ONEPAGE_COL	, NULL, FALSE);
	AppConfig.ed_lines_per_page	= GetDlgItemInt(IDC_ONEPAGE_LINE	, NULL, FALSE);

	AppConfig.auto_switch	= IsDlgButtonChecked(IDC_AUTOSWITCH);
	AppConfig.flash_window	= IsDlgButtonChecked(IDC_FLASHWINDOW);

	if (IsDlgButtonChecked(IDC_MUTE))
		AppConfig.sound = 0;
	else if (IsDlgButtonChecked(IDC_PCSPEAKER))
		AppConfig.sound = 1;
	else
		AppConfig.sound = 2;

	GetDlgItemText(IDC_WAVEPATH, AppConfig.wavepath);
	AppConfig.old_textout = IsDlgButtonChecked(IDC_OLD_TEXTOUT);

	CPropertyPage::OnOK();
}

void COtherPage::OnSetWave()
{
	CFileDialog dlg(TRUE, ".wav", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, LoadString(IDS_WAV_FILTER));
	if (dlg.DoModal() == IDOK)
		GetDlgItem(IDC_WAVEPATH)->SetWindowText(dlg.GetPathName());
}

BOOL COtherPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPINONEPAGE_COL))->SetRange32(40, 200);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPINONEPAGE_LINE))->SetRange32(24, 32767);
	CheckDlgButton(IDC_AUTOFONT		, AppConfig.auto_font);
	CheckDlgButton(IDC_AUTOCANCELSEL, AppConfig.auto_cancelsel);
	CheckDlgButton(IDC_AUTOCLOSE	, AppConfig.auto_close);
	CheckDlgButton(IDC_AUTOCOPY		, AppConfig.auto_copy);
	CheckDlgButton(IDC_ENTER_RECONNECT, AppConfig.enter_reconnect);
	CheckDlgButton(IDC_NOCON_ENTER_RECONNECT, AppConfig.nocon_enter_reconnect);
	CheckDlgButton(IDC_DBLCLKSELECT, AppConfig.dblclk_select);

	SetDlgItemInt(IDC_ONEPAGE_COL	, AppConfig.ed_cols_per_page	, FALSE);
	SetDlgItemInt(IDC_ONEPAGE_LINE	, AppConfig.ed_lines_per_page	, FALSE);

	CheckDlgButton(IDC_FLASHWINDOW	, AppConfig.auto_font);
	CheckDlgButton(IDC_AUTOSWITCH	, AppConfig.auto_switch);
	CheckDlgButton(IDC_FLASHWINDOW	, AppConfig.flash_window);

	CheckDlgButton(IDC_MUTE + AppConfig.sound	, 1);
	SetDlgItemText(IDC_WAVEPATH, AppConfig.wavepath);

	CheckDlgButton(IDC_OLD_TEXTOUT	, AppConfig.old_textout);
	return TRUE;
}
