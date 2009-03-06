// AppConfig.cpp: implementation of the CAppConfig class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcman.h"
#include "AppConfig.h"
#include "MainFrm.h"
#include "PasswdDlg.h"
#include "StrUtils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAppConfig AppConfig;

const COLORREF CAppConfig::default_colormap[] =
{
	0, RGB(128, 0, 0), RGB(0, 128, 0), RGB(128, 128, 0),
	RGB(0, 0, 128), RGB(128, 0, 128), RGB(0, 128, 128), RGB(192, 192, 192),
	RGB(128, 128, 128), RGB(255, 0, 0), RGB(0, 255, 0), RGB(255, 255, 0),
	RGB(0, 0, 255), RGB(255, 0, 255), RGB(0, 255, 255), 0x00ffffff
};


#ifdef _COMBO_
TBBUTTON CAppConfig::maintb_btns[] =
{
//	Combo Version --------------------------------
	{0, ID_SITE_LIST, TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{1, ID_NEW_WWW,	TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_DROPDOWN,	0, 1},
	{2, ID_GOBACK,		TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_DROPDOWN,	0, 2},
	{3, ID_GOFORWARD,	TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_DROPDOWN,	0, 3},
//-------------------------------------------------
	{4, ID_DISCONNECT,	TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 4},
	{5, ID_RECONNECT,	TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 5},
	{6, ID_CONNECT_CLOSE, TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 6},
	{7, ID_ADDTOFAVORITE, TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 7},
	{0, ID_SEPARATOR,	TBSTATE_ENABLED, TBSTYLE_SEP,	0, 0},

	{8, ID_COPY,			TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 8},
	{9, ID_ANSICOPY,		TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 9},
	{10, ID_PASTE,		TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 10},
	{11, ID_AUTO_DBCS,	TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 11},
	{0, ID_SEPARATOR,	TBSTATE_ENABLED, TBSTYLE_SEP,	0, 0},

	{12, ID_FONT_BTN,	TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 12},
	{13, ID_VIEW_CONFIG,	TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 13},
	{14, ID_VIEW_FULLSCR, TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 14},
	{0, ID_SEPARATOR,	TBSTATE_ENABLED, TBSTYLE_SEP,	0, 0},

	{15, ID_SETBK,		TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 15},
	{16, ID_TOOL_SYMBOLS, TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 16},
	{17, ID_TOOL_LOCK,	TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 17},
	{0, ID_SEPARATOR,	TBSTATE_ENABLED, TBSTYLE_SEP,	0, 0},

	{18, ID_ABOUT,		TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 18},

	{19, ID_NCIKU,      TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 19},
	{20, ID_WIKIPEDIA,      TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 20},
};

TBBUTTON CAppConfig::webbar_btns[] =
{
	{0, ID_WWWHOME,		TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{1, ID_WEBPAGE_ADDAD, TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 1},
	{2, ID_BLOCK_POPUP, TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 2},
	{3, ID_ADSHTTP,		TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 3},
	{4, ID_ADSTELNET,	TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 4},
//	{3,ID_ADSHTTP,		TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,3},
};
#else
TBBUTTON CAppConfig::maintb_btns[] =
{
	{0, ID_SITE_LIST, TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{1, ID_CONNECT_NEW,	TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 1},
	{2, ID_DISCONNECT,	TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 2},
	{3, ID_RECONNECT,	TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 3},
	{4, ID_CONNECT_CLOSE, TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 4},
	{5, ID_ADDTOFAVORITE, TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 5},
	{0, ID_SEPARATOR,	TBSTATE_ENABLED, TBSTYLE_SEP,	0, 0},

	{6, ID_COPY,			TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 6},
	{7, ID_ANSICOPY,		TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 7},
	{8, ID_PASTE,		TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 8},
	{9, ID_AUTO_DBCS,	TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 9},
	{0, ID_SEPARATOR,	TBSTATE_ENABLED, TBSTYLE_SEP,	0, 0},

	{10, ID_FONT_BTN,	TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 10},
	{11, ID_VIEW_CONFIG,	TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 11},
	{12, ID_VIEW_FULLSCR, TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 12},
	{0, ID_SEPARATOR,	TBSTATE_ENABLED, TBSTYLE_SEP,	0, 0},

	{13, ID_SETBK,		TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 13},
	{14, ID_TOOL_SYMBOLS, TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 14},
	{15, ID_TOOL_LOCK,	TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 15},
	{0, ID_SEPARATOR,	TBSTATE_ENABLED, TBSTYLE_SEP,	0, 0},

	{16, ID_ABOUT,		TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 16},
	{17, ID_NCIKU,      TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 17},
	{18, ID_WIKIPEDIA,      TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 18},
	
};
#endif

CString GetIEPath()
{
	CString fpath;	long len = _MAX_PATH;
	HKEY hk = NULL;
//	RegOpenKey( HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\IEXPLORE.EXE", &hk );
//	RegQueryValue( hk, NULL, fpath.GetBuffer(_MAX_PATH), &len);
	RegQueryValue(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\IEXPLORE.EXE", fpath.GetBuffer(_MAX_PATH), &len);
//	RegCloseKey( hk );
	fpath.ReleaseBuffer();
	return fpath;
}

void CAppConfig::Load(LPCTSTR config_path)
{
	GetIEPath();
	CFile f;

	Default();  // set default value before loading ini
	SetFilePath(config_path);
	if (! CConfigFile::Load())  // the ini file cannot be loaded
		Save(ConfigPath + CONFIG_FILENAME); // save default value as ini

#if defined _COMBO_
	LoadWebPageFilter();
#endif

	lock_pcman = false;
}

void CAppConfig::Save(LPCTSTR config_path)
{
	SetFilePath(config_path);
	CConfigFile::Save();

#if defined _COMBO_
	SaveWebPageFilter();
#endif
}

CAppConfig::CAppConfig()
		: main_toolbar_inf(maintb_btns, sizeof(maintb_btns) / sizeof(TBBUTTON))
#ifdef _COMBO_
		, webbar_inf(webbar_btns, sizeof(webbar_btns) / sizeof(TBBUTTON))
#endif
{
}

/*
const char* CAppConfig::personal_files[]={
	CONFIG_FILENAME,
	TOOLBAR_BMP_FILENAME,
	ICON_BMP_FILENAME,
	FUS_FILENAME,
	UI_FILENAME,
	HOMEPAGE_FILENAME,
	BBS_FAVORITE_FILENAME
#if defined(_COMBO_)
	,WWW_FAVORITE_FILENAME
#endif
};

*/

void CAppConfig::BackupConfig(CString dir1, CString dir2)
{
	CreateDirectory(dir2, NULL);

	CFileFind finder;
	BOOL found = finder.FindFile(dir1 + "*.*");
	while (found)
	{
		found = finder.FindNextFile();
		CopyFile(finder.GetFilePath(), dir2 + finder.GetFileName(), FALSE);
	}
	finder.Close();
}

BOOL CAppConfig::QueryPassword(BOOL confirm, LPCTSTR title)
{
	while (passwd.IsEmpty())
	{
		CPasswdDlg dlg;
		dlg.title = title;
		if (dlg.DoModal() == IDCANCEL)
			return FALSE;

		if (confirm)
		{
			CString passwd1 = dlg.passwd;
			dlg.title.LoadString(IDS_CONFIM_PASSWD2);
			if (dlg.DoModal() == IDCANCEL)
				return FALSE;
			if (dlg.passwd != passwd1)
			{
				MessageBox(NULL, LoadString(IDS_DIFF_PASSWD), LoadString(IDS_ERR), MB_OK | MB_ICONSTOP);
				dlg.title.Empty();
			}
			else
				passwd = dlg.passwd;
		}
		else
			passwd = dlg.passwd;
	}
	return TRUE;
}

BOOL IsContainAnsiCode(LPCTSTR str)
{
	while (*str)
	{
		if (*str == '\x1b' && *(str + 1) == '[')
		{
			str += 2;
			while ((isdigit(*str) || *str == ';') && *str)
				str++;
			if (*str == 'm')
				return TRUE;
			str--;
		}
		str++;
	}
	return FALSE;
}

CString LoadString(CMemIniFile& file)
{
	CString str;
	DWORD len = 0;
	file.Read(&len, sizeof(DWORD));
	file.Read(str.GetBuffer(len), len);
	str.ReleaseBuffer();
	return str;
}

#ifdef	_COMBO_
inline void CAppConfig::LoadWebPageFilter()
{
	CFile file;
	if (file.Open(ConfigPath + WWW_ADFILTER_FILENAME, CFile::modeRead))
	{
		DWORD len = file.GetLength();
		char* buf = new char[ len + 1];
		file.Read(buf, len);
		file.Close();
		buf[len] = '\0';

		char *line;
		char *nextline = NULL;
		for (line = buf; line; line = nextline)
		{
			nextline = strnextline(line);
			if (*line)
				webpage_filter.Add(line);
		}
		delete []buf;
		webpage_filter.FreeExtra();
	}
}

inline void CAppConfig::SaveWebPageFilter()
{
	CFile file;
	if (file.Open(ConfigPath + WWW_ADFILTER_FILENAME, CFile::modeWrite | CFile::modeCreate))
	{
		int c = webpage_filter.GetSize();
		for (int i = 0; i < c; ++i)
		{
			CString& line = webpage_filter[i];
			file.Write(LPCTSTR(line), line.GetLength());
			file.Write("\r\n", 2);
		}
		file.Close();
	}
}
#endif

bool CAppConfig::OnDataExchange(bool load)
{
	CString lfFaceName;
	if (!load)
		lfFaceName = font_info.lfFaceName;

//	General Setting
	BEGIN_CFG_SECTION(General)
	CFG_BYTE(multiple_instance)
	CFG_BYTE(scrolltab)
	CFG_BYTE(close_query)
	CFG_BYTE(dblclk_close)
	CFG_BYTE(tab_add_number)
	CFG_BYTE(tab_button)
	CFG_BYTE(showtb)
	CFG_BYTE(showsb)
	CFG_BYTE(showtab)
	CFG_BYTE(showads)
	CFG_BYTE(showclose)
	CFG_BYTE(use_ansi_bar)
	CFG_BYTE(kktab)
	CFG_BYTE(is_full_scr)
	CFG_BYTE(use_MouseCTL)

	CFG_BYTE(fullscr_showtb)
	CFG_BYTE(fullscr_showsb)
	CFG_BYTE(fullscr_showtab)
	CFG_BYTE(fullscr_showads)
	CFG_BYTE(fullscr_showclose)
	CFG_BYTE(full_use_ansi_bar)

	CFG_BYTE(minimize_to_tray)
	CFG_SHORT(pcman_hotkey)
	CFG_SHORT(pcman_hotkey_mod)

	CFG_INT(max_history)
	CFG_BYTE(saved_charset)
	CFG_BYTE(autoupdate_disable)
	CFG_INT(max_translation_length)

	END_CFG_SECTION()

//	BBS Setttings
	BEGIN_CFG_SECTION(BBS)

	CFG_BYTE(auto_cancelsel)
	CFG_BYTE(auto_copy)
	CFG_BYTE(enter_reconnect)
	CFG_BYTE(nocon_enter_reconnect)
	CFG_BYTE(auto_close)
	CFG_BYTE(auto_font)
	CFG_BYTE(bbs_close_query)
	CFG_BYTE(auto_switch)
	CFG_BYTE(flash_window)
	CFG_BYTE(smooth_draw)

	//	LOGFONT font_info;
	CFG_INT(bktype)
	CFG_INT(bkratio)		//	ratio/10*100%
	CFG_BYTE(sound)		//type of beep
	CFG_BYTE(old_textout)
	CFG_BYTE(save_session)
	CFG_BYTE(switch_back_on_close)
	CFG_BYTE(dblclk_select)
	CFG_STR(bkpath)	//背景圖路徑
	CFG_STR(wavepath)

//	AnsiEditor Settings
	CFG_INT(ed_cols_per_page)
	CFG_INT(ed_lines_per_page)

//	HyperLink Settings
	CFG_INT(link_underline)
	CFG_INT(link_autoswitch)

	END_CFG_SECTION()

	BEGIN_CFG_SECTION(Font)
	_CFG_STR("face", lfFaceName)
	_CFG_INT("height", font_info.lfHeight)
	_CFG_INT("width", font_info.lfWidth)
	_CFG_INT("esc", font_info.lfEscapement)
	_CFG_INT("orient", font_info.lfOrientation)
	_CFG_INT("weight", font_info.lfWeight)
	_CFG_BYTE("italic", font_info.lfItalic)
	_CFG_BYTE("underline", font_info.lfUnderline)
//	   _CFG_BYTE( font_info.lfStrikeOut )
	_CFG_BYTE("charset", font_info.lfCharSet)
	_CFG_BYTE("oprec", font_info.lfOutPrecision)
	_CFG_BYTE("cprec", font_info.lfClipPrecision)
	_CFG_BYTE("quality", font_info.lfQuality)
	_CFG_BYTE("pitch", font_info.lfPitchAndFamily)
	END_CFG_SECTION()

	BEGIN_CFG_SECTION(Site)
	_CFG_LONG("line_count", site_settings.line_count)
	_CFG_LONG("idle_interval", site_settings.idle_interval)
	_CFG_INT("connect_interval", site_settings.connect_interval)
	_CFG_INT("reconnect_interval", site_settings.reconnect_interval)
	_CFG_INT("paste_autowrap_col", site_settings.paste_autowrap_col)
	_CFG_SHORT("cols_per_page", site_settings.cols_per_page)
	_CFG_SHORT("lines_per_page", site_settings.lines_per_page)

	_CFG_BYTE("prevent_idle", site_settings.prevent_idle)
	_CFG_BYTE("auto_reconnect", site_settings.auto_reconnect)

	_CFG_BYTE("showscroll", site_settings.showscroll)
	_CFG_BYTE("line_wrap", site_settings.line_wrap)
	_CFG_BYTE("paste_autowrap", site_settings.paste_autowrap)
	_CFG_BYTE("auto_dbcs_mouse", site_settings.auto_dbcs_mouse)
	_CFG_BYTE("auto_dbcs_arrow", site_settings.auto_dbcs_arrow)
	_CFG_BYTE("auto_dbcs_del", site_settings.auto_dbcs_del)
	_CFG_BYTE("auto_dbcs_backspace", site_settings.auto_dbcs_backspace)
	_CFG_BYTE("localecho", site_settings.localecho)

	_CFG_BYTE("text_output_conv", site_settings.text_output_conv)	// 顯示文字轉碼	0=none, 1=gb2big5, 2=big52gb
	_CFG_BYTE("text_input_conv", site_settings.text_input_conv)		// 輸入文字轉碼	0=none, 1=gb2big5, 2=big52gb

	_CFG_STR("key_map_name", site_settings.key_map_name)
	_CFG_STR("termtype", site_settings.termtype)
	_CFG_STR("idle_str", site_settings.idle_str)
	_CFG_ESTR("esc_convert", site_settings.esc_convert)
	END_CFG_SECTION()

	BEGIN_CFG_SECTION(Color)
	_CFG_COLOR("black", colormap[0])
	_CFG_COLOR("dark_red", colormap[1])
	_CFG_COLOR("dark_green", colormap[2])
	_CFG_COLOR("brown", colormap[3])
	_CFG_COLOR("dark_blue", colormap[4])
	_CFG_COLOR("dark_magenta", colormap[5])
	_CFG_COLOR("dark_cyan", colormap[6])
	_CFG_COLOR("light_gray", colormap[7])
	_CFG_COLOR("gray", colormap[8])
	_CFG_COLOR("red", colormap[9])
	_CFG_COLOR("green", colormap[10])
	_CFG_COLOR("yellow", colormap[11])
	_CFG_COLOR("blue", colormap[12])
	_CFG_COLOR("magenta", colormap[13])
	_CFG_COLOR("cyan", colormap[14])
	_CFG_COLOR("white", colormap[15])

	CFG_COLOR(active_tab_textcolor)
	END_CFG_SECTION()

	BEGIN_CFG_SECTION(Window)
// Window position
	CFG_CUSTOM("main", mainwnd_state)
	CFG_CUSTOM("site_list", sitelist_state)
	CFG_CUSTOM("freq_str", freqstr_state)

// Customize toolbar buttons
	CFG_CUSTOM("main_toolbar", main_toolbar_inf)
#if defined _COMBO_
	CFG_CUSTOM("web_bar", webbar_inf)
#endif

// ReBar Position
	CFG_CUSTOM("rebar0", rebar_bands[0])
	CFG_CUSTOM("rebar1", rebar_bands[1])
	CFG_CUSTOM("rebar2", rebar_bands[2])
	CFG_CUSTOM("rebar3", rebar_bands[3])
#if defined _COMBO_
	CFG_CUSTOM("rebar4", rebar_bands[4])
	CFG_CUSTOM("rebar5", rebar_bands[5])
#endif
	CFG_BYTE(use_riched20)
	CFG_STR(last_bbslist_item)
	END_CFG_SECTION()

//	Web Settings
#if defined (_COMBO_)
	BEGIN_CFG_SECTION(Web)
	CFG_BYTE(ads_open_new)
	CFG_BYTE(disable_popup)
	CFG_BYTE(searchbar_cleanup)
	CFG_BYTE(showwb)
	CFG_BYTE(fullscr_showwb)
	CFG_BYTE(showsearchbar)
	CFG_BYTE(fullscr_showsearchbar)
	CFG_BYTE(autosort_favorite)
	CFG_BYTE(disable_script_error)
	CFG_BYTE(use_ie_fav)
	CFG_BYTE(autowrap_favorite)
	CFG_SHORT(search_engine)
	END_CFG_SECTION()
// Object Section
//	CStringArray webpage_filter;
#endif

	BEGIN_CFG_FILE(table)
	CFG_SECTION(General)
	CFG_SECTION(BBS)
	CFG_SECTION(Color)
	CFG_SECTION(Font)
	CFG_SECTION(Site)
	CFG_CUSTOM_SECTION("HyperLink", hyper_links)
#if defined (_COMBO_)
	CFG_SECTION(Web)
#endif
	CFG_SECTION(Window)
	END_CFG_FILE()

	bool ret = DoDataExchange(load, table);

	if (load)
		strcpy(font_info.lfFaceName, lfFaceName);
	return ret;
}

void CHistoryLoader::CTypedHistoryHandler::Load(char* section)
{
	if (*section)
	{
		char* line = section;
		char* nextline = NULL;
		for (; line; line = nextline)
		{
			nextline = strnextline(line);
			if (*line)
				AppConfig.history.AddTail(line);
		}
	}
}

void CHistoryLoader::CTypedHistoryHandler::Save(CString& section)
{
	POSITION pos;
	for (pos = AppConfig.history.GetHeadPosition(); pos; AppConfig.history.GetNext(pos))
	{
		section += AppConfig.history.GetAt(pos);
		section += "\r\n";
	}
}

void CHistoryLoader::CMenuHistoryHandler::Load(char* section)
{
	if (*section)
	{
		char* line = section;
		char* nextline = NULL;
		for (; line; line = nextline)
		{
			nextline = strnextline(line);
			if (*line)
				AppConfig.favorites.history.Add(line);
		}
	}
}

void CHistoryLoader::CMenuHistoryHandler::Save(CString& section)
{
	int c = AppConfig.favorites.history.GetSize();
	for (int i = 0; i < c; ++i)
	{
		section += AppConfig.favorites.history.ElementAt(i);
		section += "\r\n";
	}
}

void CHistoryLoader::CDropDownHistoryHandler::Load(char* section)
{
	if (*section)
	{
		char* line = section;
		char* nextline = NULL;
		for (; line; line = nextline)
		{
			nextline = strnextline(line);
			if (*line)
				mainfrm->address_bar.AddString(line);
		}
	}
}

void CHistoryLoader::CDropDownHistoryHandler::Save(CString& section)
{
	int c = mainfrm->address_bar.GetCount();
	for (int i = 0 ; i < int(c); i++)
	{
		CString str;
		mainfrm->address_bar.GetLBText(i, str);
		section += str;
		section += "\r\n";
	}
}

bool CHistoryLoader::OnDataExchange(bool load)
{
	BEGIN_CFG_FILE(table)
	CFG_CUSTOM_SECTION("Menu", menu)
	CFG_CUSTOM_SECTION("DropDown", drop)
	CFG_CUSTOM_SECTION("Typed", typed)
	END_CFG_FILE()
	bool ret = DoDataExchange(load, table);
	return ret;
}
