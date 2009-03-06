// AppConfig.h: interface for the CAppConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APPCONFIG_H__14C7B573_F013_4BD6_8CD8_132DD4F5189F__INCLUDED_)
#define AFX_APPCONFIG_H__14C7B573_F013_4BD6_8CD8_132DD4F5189F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BBSHyperLink.h"	// Added by ClassView
#include "SiteSettings.h"	// Added by ClassView

extern CString ConfigPath;
extern BOOL IsCombo;

#define	CONFIG_DIR		"Config\\";
const char CONFIG_FILENAME[] = "Config";
const char TOOLBAR_BMP_FILENAME[] = "Toolbar.bmp";
const char ICON_BMP_FILENAME[] = "Icons.bmp";
const char FUS_FILENAME[] = "FUS";
const char UI_FILENAME[] = "UI";

const char HOMEPAGE_FILENAME[] = "Homepage";
//	const char SITE_SEPARATOR[]="      ";

const char BBS_FAVORITE_FILENAME[] = "BBSFavorites";

const char SESSION_FILENAME[] = "Session";
const char HISTORY_FILENAME[] = "History";

const char WWW_FAVORITE_FILENAME[] = "WebFavorites";
const char WWW_ADFILTER_FILENAME[] = "AdFilters";

BOOL IsContainAnsiCode(LPCTSTR str);
CString LoadString(CFile& file);
void SaveString(CFile& file, LPCTSTR str);

void fprintf(CFile& file, const char* format, ...);

class CReBarBandPos
{
public:
	UINT wID;
	UINT fStyle;
	UINT cx;
	inline CReBarBandPos(){wID = -1;}
};

class CWindowState
{
public:
	int padding_bytes_of_unknown_usage;	// FIXME: what's this?
	int showcmd;
	RECT rect;
};


class CCustomToolBarInfo	//目前支援最多255個按鈕
{
public:
	int count;
	int allcount;
	BYTE* index;
	TBBUTTON* pbtns;

	CCustomToolBarInfo() : index(NULL) {}

	~CCustomToolBarInfo()
	{
		if (index)
			delete []index;
	}

	inline void LoadFromFile(CFile& f)
	{
		f.Read(&count, sizeof(count));
		index = new BYTE[count];
		f.Read(index, count*sizeof(*index));
	}
};

class CAppConfig
{
public:
	inline void LoadHistory(CFile& f);
	void SaveHistory(CFile& f);
	void BackupConfig(CString dir1, CString dir2);

	CAppConfig();

	static const COLORREF default_colormap[];

	inline void Default();
	void Load(LPCTSTR config_path);
	void Save(LPCTSTR config_path);

//	pure data section
//	General Setting
	BYTE multiple_instance;
	BYTE scrolltab;
	BYTE close_query;
	BYTE dblclk_close;
	BYTE tab_add_number;
	BYTE tab_button;
	BYTE showtb;
	BYTE showsb;
	BYTE showtab;
	BYTE showads;
	BYTE showclose;
	BYTE use_ansi_bar;
	BYTE kktab;
	BYTE is_full_scr;
	BYTE use_MouseCTL;

	//By BBcall
	BYTE is_instant_tran;
	BYTE instant_tran_show;
	BYTE tran_show;

	BYTE fullscr_showtb;
	BYTE fullscr_showsb;
	BYTE fullscr_showtab;
	BYTE fullscr_showads;
	BYTE fullscr_showclose;
	BYTE full_use_ansi_bar;

	BYTE minimize_to_tray;
	WORD pcman_hotkey;
	WORD pcman_hotkey_mod;

	int max_history;

//	AnsiEditor Settings
	int	ed_cols_per_page;
	int	ed_lines_per_page;

//Main window
	CWindowState mainwnd_state;
	CWindowState sitelist_state;
	CWindowState freqstr_state;

//ReBar Position
	CReBarBandPos rebar_bands[6];

//	HyperLink Settings
	BOOL link_underline;
	BOOL link_autoswitch;

//	BBS Setttings
	BYTE auto_cancelsel;
	BYTE auto_copy;
	BYTE enter_reconnect;
	BYTE nocon_enter_reconnect;
	BYTE auto_close;
	BYTE auto_font;
	BYTE bbs_close_query;
	BYTE auto_switch;
	BYTE flash_window;
	BYTE smooth_draw;
	COLORREF colormap [16];
	LOGFONT font_info;
	UINT bktype;
	UINT bkratio;		//	ratio/10*100%
	BYTE sound;		//type of beep
	BYTE old_textout;
	COLORREF active_tab_textcolor;
	BYTE save_session;
	BYTE switch_back_on_close;

//	Site Settings
	CSiteSettings site_settings;

//	BBS Setttings
	CString bkpath;	//背景圖路徑
	CString passwd;
	CString wavepath;
	CString last_bbslist_item;

	CCustomToolBarInfo main_toolbar_inf;
//	static TBBUTTON maintb_btns[];

	CStringList history;
//	CFavMenu favorites;

//	WWW Settings
	CCustomToolBarInfo webbar_inf;
	BYTE ads_open_new;
	BYTE disable_popup;
	BYTE searchbar_cleanup;
	BYTE showwb;
	BYTE fullscr_showwb;
	BYTE autosort_favorite;
	BYTE disable_script_error;
	BYTE use_ie_fav;
	BYTE autowrap_favorite;

// Object Section
	CStringArray webpage_filter;

	CBBSHyperLink hyper_links;
	CStringArray history_menu;
	CStringArray history_dropdown;
};

inline void CAppConfig::Default()
{
	multiple_instance = 0;
	scrolltab = 0;
	close_query = 1;
	dblclk_close = 1;
	tab_add_number = 1;
	tab_button = 1;
	showtb = 1;
	showsb = 1;
	showtab = 1;
	showads = 1;
	showclose = 1;
	kktab = 0;
	is_full_scr = 0;
	use_MouseCTL = 0;
	use_ansi_bar = 1;

	//By BBcall
	is_instant_tran = 1;
	instant_tran_show = 1;
	tran_show = 1;

	fullscr_showtb = 0;
	fullscr_showsb = 0;
	fullscr_showtab = 1;
	fullscr_showads = 1;
	fullscr_showclose = 1;
	full_use_ansi_bar = 0;

	minimize_to_tray = 0;
	pcman_hotkey = 0xc0;	//'`'
	pcman_hotkey_mod = MOD_ALT;

	if (IsCombo)
		max_history = 600;
	else
		max_history = 40;

//	AnsiEditor Settings
	ed_cols_per_page = 80;
	ed_lines_per_page = 24;

//Main window
	mainwnd_state.showcmd = SW_SHOWMAXIMIZED;
	SetRect(&mainwnd_state.rect, 50, 50, 50 + 640, 50 + 480);
//Site List
	sitelist_state.showcmd = SW_SHOW;
	SetRect(&sitelist_state.rect, 60, 60, 60 + 520, 60 + 512);

//FreqStr Dlg
	freqstr_state.showcmd = SW_SHOW;
	SetRect(&freqstr_state.rect, 100, 80, 600, 480);

//ReBar position
	//determined by it's constructor

//Main ToolBar Buttons (should init in CAppConfig constructor)
//	main_toolbar_inf.LoadDefault();

//	HyperLink Settings
	link_underline = 1;
	link_autoswitch = 0;

//	BBS Setttings
	auto_cancelsel = 1;
	auto_copy = 0;
	enter_reconnect = 1;
	nocon_enter_reconnect = 1;
	auto_close = 0;
	auto_font = 1;
	bbs_close_query = 1;
	auto_switch = 0;
	flash_window = 1;
	smooth_draw = 0;
	memcpy(colormap, default_colormap, sizeof(colormap));
	memset(&font_info, 0, sizeof(font_info));
	font_info.lfHeight = -16;
	font_info.lfWeight = FW_NORMAL;
	font_info.lfCharSet = CHINESEBIG5_CHARSET;
	font_info.lfOutPrecision = OUT_STROKE_PRECIS;
	font_info.lfClipPrecision = CLIP_STROKE_PRECIS;
	font_info.lfQuality = DRAFT_QUALITY;
	font_info.lfPitchAndFamily = 49;
//	font_info.lfPitchAndFamily=FIXED_PITCH|FF_DONTCARE;
	strcpy(font_info.lfFaceName, "細明體" /*LoadString(IDS_DEFAULT_FONT_FACE)*/);

	bktype = 0;
	bkratio = 4;		//	ratio/10*100%
	sound = 1;
	old_textout = 1;
	active_tab_textcolor = 0xa00000;

	switch_back_on_close = 0;

	site_settings.Default();
//	Site Settings

//	object section
//	last_bbslist_item = LoadString( IDS_BBS_FAVORITE_NAME );

//	hyper_links.Default();
//	WWW Settings

	showwb = 1;
	fullscr_showwb = 1;
//	webbar_inf.LoadDefault();
	ads_open_new = 0;
	disable_popup = 1;
	autosort_favorite = 1;
	disable_script_error = 1;
	use_ie_fav = 1;
	autowrap_favorite = 1;

};


CString LoadString(CFile& file);

inline void CAppConfig::LoadHistory(CFile &f)
{
	int i;
	WORD c = 0;
	DWORD n = 0;

	f.Read(&n, 4);
	if (n > 0)
	{
		history_dropdown.SetSize(n);
		for (i = 0; i < n; ++i)
			history_dropdown[i] = LoadString(f);
	}

	f.Read(&c, sizeof(c));
	if (c > 0)
	{
		history_menu.SetSize(c);
		for (i = 0; i < c; ++i)
			history_menu[i] = LoadString(f);
	}

	f.Read(&n, sizeof(n));
	for (i = 0; i < n ;++i)
		history.AddTail(LoadString(f));
}

extern CAppConfig AppConfig;

#endif // !defined(AFX_APPCONFIG_H__14C7B573_F013_4BD6_8CD8_132DD4F5189F__INCLUDED_)
