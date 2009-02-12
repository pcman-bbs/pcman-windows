// GeneralPage.cpp : implementation file
//

#include "stdafx.h"
#include "pcman.h"
#include "GeneralPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGeneralPage property page

CGeneralPage::CGeneralPage() : CPropertyPage(CGeneralPage::IDD)
{
	//{{AFX_DATA_INIT(CGeneralPage)
	//}}AFX_DATA_INIT
}

CGeneralPage::~CGeneralPage()
{
}

void CGeneralPage::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_HOTKEY, m_wndHotkey);
}

BEGIN_MESSAGE_MAP(CGeneralPage, CPropertyPage)
	//{{AFX_MSG_MAP(CGeneralPage)
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_ACTIVE_TAB_TEXTCOLOR, OnActiveTabTextColor)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGeneralPage message handlers

BOOL CGeneralPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	CheckDlgButton(IDC_MULTI_PCMAN, AppConfig.multiple_instance);
	CheckDlgButton(IDC_CLOSEQUERY, AppConfig.close_query);
	CheckDlgButton(IDC_BBS_CLOSEQUERY, AppConfig.bbs_close_query);
	CheckDlgButton(IDC_DBLCLK_CLOSE, AppConfig.dblclk_close);
	CheckDlgButton(IDC_TABADDNUMBER, AppConfig.tab_add_number);
	CheckDlgButton(IDC_SCROLLTAB, AppConfig.scrolltab);
	CheckDlgButton(IDC_TAB_BUTTON, AppConfig.tab_button);
	CheckDlgButton(IDC_MINIMIZE_TO_TRAY, AppConfig.minimize_to_tray);
	CheckDlgButton(IDC_SWITCH_BACK, AppConfig.switch_back_on_close);
	CheckDlgButton(IDC_ENABLE_AUTOUPDATE, !AppConfig.autoupdate_disable);

	CheckDlgButton(IDC_FULL_SHOWTB, AppConfig.fullscr_showtb);
	CheckDlgButton(IDC_FULL_SHOWADS, AppConfig.fullscr_showads);
	CheckDlgButton(IDC_FULL_SHOWTAB, AppConfig.fullscr_showtab);
	CheckDlgButton(IDC_FULL_SHOWSB, AppConfig.fullscr_showsb);
	CheckDlgButton(IDC_FULL_SHOWCLOSE, AppConfig.fullscr_showclose);
	CheckDlgButton(IDC_FULL_USE_ANSIBAR, AppConfig.full_use_ansi_bar);

#ifdef	_COMBO_
	::ShowWindow(::GetDlgItem(m_hWnd, IDC_FULL_SHOWWB), SW_SHOW);
	CheckDlgButton(IDC_FULL_SHOWWB, AppConfig.fullscr_showwb);
	::ShowWindow(::GetDlgItem(m_hWnd, IDC_FULL_SHOWSEARCH), SW_SHOW);
	CheckDlgButton(IDC_FULL_SHOWSEARCH, AppConfig.fullscr_showsearchbar);
#endif

	WORD mod = ((AppConfig.pcman_hotkey_mod & MOD_ALT) ? HOTKEYF_ALT : 0) |
			   ((AppConfig.pcman_hotkey_mod & MOD_CONTROL) ? HOTKEYF_CONTROL : 0) |
			   ((AppConfig.pcman_hotkey_mod & MOD_SHIFT) ? HOTKEYF_SHIFT : 0);
	m_wndHotkey.SetHotKey(AppConfig.pcman_hotkey, mod);

	active_tab_textcolor = AppConfig.active_tab_textcolor;

	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_MAX_HISTORYSPIN))->SetRange32(0, 32767);
	SetDlgItemInt(IDC_MAX_HISTORY, AppConfig.max_history, FALSE);
	/*	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_MAX_HISTORYMENUSPIN))->SetRange32(0,50);
		SetDlgItemInt( IDC_MAX_HISTORYMENU, AppConfig.max_history_menu, FALSE);
	*/	return TRUE;
}

void CGeneralPage::OnOK()
{
	AppConfig.multiple_instance = IsDlgButtonChecked(IDC_MULTI_PCMAN);
	AppConfig.close_query = IsDlgButtonChecked(IDC_CLOSEQUERY);
	AppConfig.bbs_close_query = IsDlgButtonChecked(IDC_BBS_CLOSEQUERY);
	AppConfig.dblclk_close = IsDlgButtonChecked(IDC_DBLCLK_CLOSE);
	AppConfig.tab_add_number = IsDlgButtonChecked(IDC_TABADDNUMBER);
	AppConfig.scrolltab = IsDlgButtonChecked(IDC_SCROLLTAB);
	AppConfig.tab_button = IsDlgButtonChecked(IDC_TAB_BUTTON);
	AppConfig.minimize_to_tray = IsDlgButtonChecked(IDC_MINIMIZE_TO_TRAY);
	AppConfig.switch_back_on_close = IsDlgButtonChecked(IDC_SWITCH_BACK);

	AppConfig.fullscr_showtb = IsDlgButtonChecked(IDC_FULL_SHOWTB);
	AppConfig.fullscr_showads = IsDlgButtonChecked(IDC_FULL_SHOWADS);
	AppConfig.fullscr_showtab = IsDlgButtonChecked(IDC_FULL_SHOWTAB);
	AppConfig.fullscr_showsb = IsDlgButtonChecked(IDC_FULL_SHOWSB);
	AppConfig.fullscr_showclose = IsDlgButtonChecked(IDC_FULL_SHOWCLOSE);
	AppConfig.full_use_ansi_bar = IsDlgButtonChecked(IDC_FULL_USE_ANSIBAR);
	AppConfig.autoupdate_disable = !IsDlgButtonChecked(IDC_ENABLE_AUTOUPDATE);

#ifdef	_COMBO_
	AppConfig.fullscr_showwb = IsDlgButtonChecked(IDC_FULL_SHOWWB);
	AppConfig.fullscr_showsearchbar = IsDlgButtonChecked(IDC_FULL_SHOWSEARCH);
#endif

	WORD mod, vk;
	m_wndHotkey.GetHotKey(vk, mod);
	if (vk != AppConfig.pcman_hotkey || mod != AppConfig.pcman_hotkey_mod)
	{
		AppConfig.pcman_hotkey_mod = ((mod & HOTKEYF_ALT) ? MOD_ALT : 0) |
									 ((mod & HOTKEYF_CONTROL) ? MOD_CONTROL : 0) |
									 ((mod & HOTKEYF_SHIFT) ? MOD_SHIFT : 0);

		UnregisterHotKey(AfxGetMainWnd()->m_hWnd, 1);
		RegisterHotKey(AfxGetMainWnd()->m_hWnd, 1, AppConfig.pcman_hotkey_mod, AppConfig.pcman_hotkey);
		AppConfig.pcman_hotkey = vk;
	}

	AppConfig.active_tab_textcolor = active_tab_textcolor;
	((CMainFrame*)AfxGetApp()->m_pMainWnd)->tab.Invalidate();

	AppConfig.max_history = GetDlgItemInt(IDC_MAX_HISTORY, FALSE);
//	AppConfig.max_history_menu = GetDlgItemInt( IDC_MAX_HISTORYMENU, FALSE);

	CPropertyPage::OnOK();
}

void CGeneralPage::OnActiveTabTextColor()
{
	CColorDialog dlg(active_tab_textcolor);
	if (dlg.DoModal() == IDOK)
	{
		active_tab_textcolor = dlg.GetColor();
		GetDlgItem(IDC_ACTIVE_TAB_TEXTCOLOR)->Invalidate();
	}
}


void CGeneralPage::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (nIDCtl == IDC_ACTIVE_TAB_TEXTCOLOR)
	{
		CRect& rc = (CRect)lpDrawItemStruct->rcItem;
		SetBkColor(lpDrawItemStruct->hDC, active_tab_textcolor);
		ExtTextOut(lpDrawItemStruct->hDC, rc.left, rc.top, ETO_OPAQUE, rc, NULL, 0, NULL);
	}
}
