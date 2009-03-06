// WebCfgPage.cpp : implementation file
//

#include "../Lite/stdafx.h"
#include "../Lite/pcman.h"
#include "webcfgpage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWebCfgPage property page


CWebCfgPage::CWebCfgPage() : CPropertyPage(CWebCfgPage::IDD)
{
	//{{AFX_DATA_INIT(CWebCfgPage)
	//}}AFX_DATA_INIT
}


void CWebCfgPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWebCfgPage)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWebCfgPage, CPropertyPage)
	//{{AFX_MSG_MAP(CWebCfgPage)
	ON_BN_CLICKED(IDC_IE_FAV, OnIeFav)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWebCfgPage message handlers

BOOL CWebCfgPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CheckDlgButton(IDC_ADS_NEW	, AppConfig.ads_open_new);
	CheckDlgButton(IDC_DISABLE_SCRIPT_ERROR, AppConfig.disable_script_error);
	CheckDlgButton(IDC_DISABLE_POPUP, AppConfig.disable_popup);
	CheckDlgButton(IDC_IE_FAV, AppConfig.use_ie_fav);
	GetDlgItem(IDC_AUTO_SORT)->EnableWindow(AppConfig.use_ie_fav);
	CheckDlgButton(IDC_AUTO_SORT, AppConfig.autosort_favorite);
	CheckDlgButton(IDC_AUTO_WRAP, AppConfig.autowrap_favorite);
	CheckDlgButton(IDC_SEARCHBAR_CLEANUP, AppConfig.searchbar_cleanup);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CWebCfgPage::OnOK()
{
	AppConfig.ads_open_new		= IsDlgButtonChecked(IDC_ADS_NEW);
	AppConfig.disable_script_error		= IsDlgButtonChecked(IDC_DISABLE_SCRIPT_ERROR);
	AppConfig.disable_popup		= IsDlgButtonChecked(IDC_DISABLE_POPUP);
	AppConfig.use_ie_fav		= IsDlgButtonChecked(IDC_IE_FAV);
	AppConfig.autosort_favorite = IsDlgButtonChecked(IDC_AUTO_SORT);
	AppConfig.autowrap_favorite = IsDlgButtonChecked(IDC_AUTO_WRAP);
	AppConfig.searchbar_cleanup = IsDlgButtonChecked(IDC_SEARCHBAR_CLEANUP);
	CPropertyPage::OnOK();
}

void CWebCfgPage::OnIeFav()
{
	GetDlgItem(IDC_AUTO_SORT)->EnableWindow(IsDlgButtonChecked(IDC_IE_FAV));
}
