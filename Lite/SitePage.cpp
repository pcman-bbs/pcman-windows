// SitePage.cpp : implementation file
//

#include "stdafx.h"
#include "pcman.h"
#include "SitePage.h"
#include "KeyMapDlg.h"
#include "AutoReplyPage.h"
#include "TelnetConn.h"
#include "InputNameDlg.h"
#include "StrUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSitePage property page

IMPLEMENT_DYNCREATE(CSitePage, CPropertyPage)

CSitePage::CSitePage() : CPropertyPage(CSitePage::IDD)
{
	show_use_global = true;
	//{{AFX_DATA_INIT(CSitePage)
	//}}AFX_DATA_INIT
}

CSitePage::~CSitePage()
{
}

void CSitePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSitePage)
	DDX_Control(pDX, IDC_TERMTYPE, ctermtype);
	DDX_Control(pDX, IDC_KEYMAP, ckeymap);
	DDX_Control(pDX, IDC_ESC_CONVERT, cesc_convert);
	DDX_Control(pDX, IDC_TOCONV, ctoconv);
	DDX_Control(pDX, IDC_TICONV, cticonv);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSitePage, CPropertyPage)
	//{{AFX_MSG_MAP(CSitePage)
	ON_BN_CLICKED(IDC_IDLEHELP, OnIdlehelp)
	ON_BN_CLICKED(IDC_ADDMAP, OnAddMap)
	ON_BN_CLICKED(IDC_EDITMAP, OnEditMap)
	ON_BN_CLICKED(IDC_RENAMEMAP2, OnRenameMap)
	ON_BN_CLICKED(IDC_DELMAP, OnDelMap)
	ON_BN_CLICKED(IDC_USE_GLOBAL_SETTINGS, OnUseGlobalSettings)
	ON_CBN_SELCHANGE(IDC_KEYMAP, OnSelchangeKeyMap)
	//}}AFX_MSG_MAP
	//	ON_BN_CLICKED(IDC_USEDEFAULT, OnUseDefault)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSitePage message handlers

BOOL CSitePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CheckDlgButton(IDC_USE_GLOBAL_SETTINGS	, psettings->use_global);

	WIN32_FIND_DATA fd;
	HANDLE hf = (HANDLE)FindFirstFile(AppPath + "keyboard\\*.*", &fd);
	if (hf != INVALID_HANDLE_VALUE)
	{
		for (BOOL found = FindNextFile(hf, &fd); found ; found = FindNextFile(hf, &fd))
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && *fd.cFileName != '.')
				ckeymap.AddString(fd.cFileName);
		FindClose(hf);
	}

	cesc_convert.AddString("^U");
	cesc_convert.AddString("^C^H^H");
	cesc_convert.AddString("^[^[");
	cesc_convert.AddString("^[");
	cesc_convert.SetWindowText(EscapeControlChars(psettings->esc_convert));

	ctoconv.AddString(LoadString(IDS_DONT_USE));
	ctoconv.AddString(LoadString(IDS_GB2BIG5));
	ctoconv.AddString(LoadString(IDS_BIG52GB));
	ctoconv.SetCurSel(psettings->text_output_conv);

	cticonv.AddString(LoadString(IDS_DONT_USE));
	cticonv.AddString(LoadString(IDS_GB2BIG5));
	cticonv.AddString(LoadString(IDS_BIG52GB));
	cticonv.SetCurSel(psettings->text_input_conv);

	if (!show_use_global)
		::DestroyWindow(::GetDlgItem(m_hWnd, IDC_USE_GLOBAL_SETTINGS));

	UpdateDisplay();

	EnableControls(! psettings->use_global);

	return TRUE;
}

void CSitePage::OnOK()
{
	psettings->use_global			= IsDlgButtonChecked(IDC_USE_GLOBAL_SETTINGS);

	if (psettings->use_global)
	{
		*psettings = AppConfig.site_settings;
		psettings->use_global = 1;
		return;
	}

	psettings->showscroll			= IsDlgButtonChecked(IDC_SHOWSCROLL);
	psettings->localecho			= IsDlgButtonChecked(IDC_LOCALECHO);
	psettings->line_wrap			= IsDlgButtonChecked(IDC_LINEWRAP);
	psettings->auto_reconnect		= IsDlgButtonChecked(IDC_AUTORECONNECT);
	psettings->paste_autowrap		= IsDlgButtonChecked(IDC_PASTE_AUTOWRAP);
	psettings->prevent_idle			= IsDlgButtonChecked(IDC_PREVENT_IDLE);
	psettings->auto_dbcs_mouse		= IsDlgButtonChecked(IDC_AUTOBIG5_MOUSE);
	psettings->auto_dbcs_backspace	= IsDlgButtonChecked(IDC_AUTOBIG5_BACKSPACE);
	psettings->auto_dbcs_arrow		= IsDlgButtonChecked(IDC_AUTOBIG5_ARROW);
	psettings->auto_dbcs_del		= IsDlgButtonChecked(IDC_AUTOBIG5_DEL);
//	use_default						=IsDlgButtonChecked(IDC_USEDEFAULT			);

	psettings->paste_autowrap_col = GetDlgItemInt(IDC_PASTE_AUTOWRAP_COL, NULL, FALSE);

	long line_count						= GetDlgItemInt(IDC_LCOUNT				, NULL, FALSE);
	WORD cols_per_page				= GetDlgItemInt(IDC_ONEPAGE_COL			, NULL, FALSE);
	WORD lines_per_page				= GetDlgItemInt(IDC_ONEPAGE_LINE			, NULL, FALSE);
	psettings->connect_interval		= GetDlgItemInt(IDC_CONNECTINTERVAL		, NULL, FALSE);
	psettings->reconnect_interval	= GetDlgItemInt(IDC_RECONNECTINTERVAL	, NULL, FALSE);
	psettings->idle_interval		= GetDlgItemInt(IDC_IDLEINTERVAL			, NULL, FALSE);
	psettings->paste_autowrap_col	= GetDlgItemInt(IDC_PASTE_AUTOWRAP_COL	, NULL, FALSE);

	if (line_count < CTelnetConn::MIN_LINE_COUNT)line_count = CTelnetConn::MIN_LINE_COUNT;
	if (line_count > CTelnetConn::MAX_LINE_COUNT)line_count = CTelnetConn::MAX_LINE_COUNT;

	psettings->line_count = line_count;

	if (cols_per_page < CTelnetConn::MIN_COLS_PER_PAGE)	cols_per_page = CTelnetConn::MIN_COLS_PER_PAGE;
	if (cols_per_page > CTelnetConn::MAX_COLS_PER_PAGE)cols_per_page = CTelnetConn::MAX_COLS_PER_PAGE;

	psettings->cols_per_page = cols_per_page;

	if (lines_per_page < CTelnetConn::MIN_LINE_PER_PAGE)	lines_per_page = CTelnetConn::MIN_LINE_PER_PAGE;
	if (lines_per_page > CTelnetConn::MAX_LINES_PER_PAGE)lines_per_page = CTelnetConn::MAX_LINES_PER_PAGE;

	psettings->lines_per_page = lines_per_page;

	ctermtype.GetWindowText(psettings->termtype);
	ckeymap.GetWindowText(psettings->key_map_name);
	GetDlgItemText(IDC_STR, psettings->idle_str);
	GetDlgItemText(IDC_ESC_CONVERT, psettings->esc_convert);
	psettings->esc_convert = UnescapeControlChars(psettings->esc_convert);

	psettings->text_output_conv = ctoconv.GetCurSel();
	psettings->text_input_conv = cticonv.GetCurSel();

	CPropertyPage::OnOK();
}

void CSitePage::OnIdlehelp()
{
	MessageBox(LoadString(IDS_IDLE_HELP));
}

void CSitePage::OnUseGlobalSettings()
{
	if (! show_use_global)
		return;

	psettings->use_global = !psettings->use_global;
	if (psettings->use_global)
	{
		*psettings = AppConfig.site_settings;
		psettings->use_global = 1;
		UpdateDisplay();
	}
	EnableControls(! psettings->use_global);
}

void CSitePage::OnAddMap()
{
	CInputNameDlg dlg;
	dlg.max = 11;
	while (dlg.name.IsEmpty())
		if (dlg.DoModal() != IDOK)
			return;
	CKeyMapDlg kmdlg;
	//產生新的KeyMap
	//and also load default
	kmdlg.pmap = CKeyMap::Load(dlg.name);

#if 0
	//載入預設值
	CKeyMap* defkm = CKeyMap::Load(CKeyMap::default_kmname);
	//複製預設值到新的鍵盤對映
	kmdlg.pmap->Copy(/*(CArray<CKeyMapEntry,CKeyMapEntry&>&)*/*defkm); //144版後會有bug
	defkm->Release();
#endif

	if (kmdlg.DoModal() == IDOK)
	{
		ckeymap.AddString(dlg.name);
		ckeymap.SelectString(0, dlg.name);
		OnSelchangeKeyMap();
	}
	kmdlg.pmap->Release();
}

void CSitePage::OnEditMap()
{
	CKeyMapDlg kmdlg;
	CString kmname;
	ckeymap.GetWindowText(kmname);
	kmdlg.pmap = CKeyMap::Load(kmname);
	kmdlg.DoModal();
	kmdlg.pmap->Release();
}

void CSitePage::OnRenameMap()
{
	CInputNameDlg dlg;
	dlg.max = 11;
	CString oldn;
	ckeymap.GetWindowText(oldn);
	dlg.name = oldn;
	int sel = ckeymap.GetCurSel();
	if (dlg.DoModal() == IDOK)
	{
		if (CKeyMap::ReName(oldn, dlg.name))
		{
			ckeymap.DeleteString(sel);
			sel = ckeymap.AddString(dlg.name);
			ckeymap.SetCurSel(sel);
		}
	}
}

void CSitePage::OnDelMap()
{
	CString kmn;
	ckeymap.GetWindowText(kmn);
	int sel = ckeymap.GetCurSel();
	if (MessageBox(LoadString(IDS_DEL_CONFIRM), LoadString(IDS_CONFIRM), MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL)
		return;
	if (CKeyMap::DelMap(kmn))
	{
		ckeymap.DeleteString(sel);
		ckeymap.SelectString(0, CKeyMap::default_kmname);
		OnSelchangeKeyMap();
	}
	else
		MessageBox(LoadString(IDS_UNABLE_TO_DEL_KEYMAP), LoadString(IDS_ERR), MB_ICONSTOP | MB_OK);
}

void CSitePage::OnSelchangeKeyMap()
{
	CString kmn;
	ckeymap.GetWindowText(kmn);
	BOOL e;
	if (kmn == CKeyMap::default_kmname)
		e = FALSE;
	else
		e = TRUE;
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_DELMAP), e);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_RENAMEMAP), e);
}


void CSitePage::EnableControls(bool enable)
{
	if (! show_use_global)
		return;

	HWND child;
	for (child = ::GetTopWindow(m_hWnd); child; child = ::GetNextWindow(child, GW_HWNDNEXT))
		::EnableWindow(child, enable);
	if (enable)
		OnSelchangeKeyMap();
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_USE_GLOBAL_SETTINGS), TRUE);
}

void CSitePage::UpdateDisplay()
{
	CheckDlgButton(IDC_SHOWSCROLL			, psettings->showscroll);
	CheckDlgButton(IDC_LOCALECHO			, psettings->localecho);
	CheckDlgButton(IDC_LINEWRAP				, psettings->line_wrap);
	CheckDlgButton(IDC_AUTORECONNECT		, psettings->auto_reconnect);
	CheckDlgButton(IDC_PASTE_AUTOWRAP		, psettings->paste_autowrap);
	CheckDlgButton(IDC_PREVENT_IDLE			, psettings->prevent_idle);
	CheckDlgButton(IDC_AUTOBIG5_MOUSE		, psettings->auto_dbcs_mouse);
	CheckDlgButton(IDC_AUTOBIG5_BACKSPACE	, psettings->auto_dbcs_backspace);
	CheckDlgButton(IDC_AUTOBIG5_ARROW		, psettings->auto_dbcs_arrow);
	CheckDlgButton(IDC_AUTOBIG5_DEL			, psettings->auto_dbcs_del);

	SetDlgItemInt(IDC_LCOUNT			, psettings->line_count, FALSE);
	SetDlgItemInt(IDC_CONNECTINTERVAL	, psettings->connect_interval, FALSE);
	SetDlgItemInt(IDC_RECONNECTINTERVAL	, psettings->reconnect_interval, FALSE);
	SetDlgItemInt(IDC_IDLEINTERVAL		, psettings->idle_interval, FALSE);
	SetDlgItemInt(IDC_PASTE_AUTOWRAP_COL, psettings->paste_autowrap_col, FALSE);
	SetDlgItemInt(IDC_ONEPAGE_COL		, psettings->cols_per_page, FALSE);
	SetDlgItemInt(IDC_ONEPAGE_LINE		, psettings->lines_per_page, FALSE);

	CSpinButtonCtrl spin;
	spin.m_hWnd = ::GetDlgItem(m_hWnd, IDC_SPINLCOUNT);
	spin.SetRange32(24, 32767);
	spin.m_hWnd = ::GetDlgItem(m_hWnd, IDC_SPINAUTOWRAPCOL);
	spin.SetRange32(40, 160);
	spin.m_hWnd = ::GetDlgItem(m_hWnd, IDC_SPINONEPAGE_COL);
	spin.SetRange32(40, 160);
	spin.m_hWnd = ::GetDlgItem(m_hWnd, IDC_SPINONEPAGE_LINE);
	spin.SetRange32(24, 64);
	spin.m_hWnd = NULL;

	SetDlgItemText(IDC_STR , psettings->idle_str);

	ctermtype.AddString("ANSI");
	ctermtype.AddString("VT100");
	ctermtype.AddString("VT220");
	ctermtype.SetWindowText(psettings->termtype);

	ckeymap.SelectString(0, psettings->key_map_name);

	OnSelchangeKeyMap();
}
