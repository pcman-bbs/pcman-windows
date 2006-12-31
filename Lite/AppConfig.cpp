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
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAppConfig AppConfig;

const COLORREF CAppConfig::default_colormap[]={
	0,RGB(128,0,0),RGB(0,128,0),RGB(128,128,0),
	RGB(0,0,128),RGB(128,0,128),RGB(0,128,128),RGB(192,192,192),
	RGB(128,128,128),RGB(255,0,0),RGB(0,255,0),RGB(255,255,0),
	RGB(0,0,255),RGB(255,0,255),RGB(0,255,255),0x00ffffff
};


#ifdef _COMBO_
TBBUTTON CAppConfig::maintb_btns[]={
//	Combo Version --------------------------------
	{0,ID_SITE_LIST,TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,0},
	{1,ID_NEW_WWW,	TBSTATE_ENABLED,TBSTYLE_BUTTON|TBSTYLE_DROPDOWN,	0,1},
	{2,ID_GOBACK,		TBSTATE_ENABLED,TBSTYLE_BUTTON|TBSTYLE_DROPDOWN,	0,2},
	{3,ID_GOFORWARD,	TBSTATE_ENABLED,TBSTYLE_BUTTON|TBSTYLE_DROPDOWN,	0,3},
//-------------------------------------------------
	{4,ID_DISCONNECT,	TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,4},
	{5,ID_RECONNECT,	TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,5},
	{6,ID_CONNECT_CLOSE,TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,6},
	{7,ID_ADDTOFAVORITE,TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,7},
	{0,ID_SEPARATOR,	TBSTATE_ENABLED,TBSTYLE_SEP,	0,0},

	{8,ID_COPY,			TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,8},
	{9,ID_ANSICOPY,		TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,9},
	{10,ID_PASTE,		TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,10},
	{11,ID_AUTO_DBCS,	TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,11},
	{0,ID_SEPARATOR,	TBSTATE_ENABLED,TBSTYLE_SEP,	0,0},

	{12,ID_CONFIG_FONT,	TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,12},
	{13,ID_VIEW_CONFIG,	TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,13},
	{14,ID_VIEW_FULLSCR,TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,14},
	{0,ID_SEPARATOR,	TBSTATE_ENABLED,TBSTYLE_SEP,	0,0},

	{15,ID_SETBK,		TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,15},
	{16,ID_TOOL_SYMBOLS,TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,16},
	{17,ID_TOOL_LOCK,	TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,17},
	{0,ID_SEPARATOR,	TBSTATE_ENABLED,TBSTYLE_SEP,	0,0},

	{18,ID_ABOUT,		TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,18}
};

TBBUTTON CAppConfig::webbar_btns[]={
	{0,ID_WWWHOME,		TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,0},
	{1,ID_WEBPAGE_ADDAD,TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,1},
	{2,ID_BLOCK_POPUP,TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,2},
	{3,ID_ADSHTTP,		TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,3},
	{4,ID_ADSTELNET,	TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,4},
//	{3,ID_ADSHTTP,		TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,3},
};
#else
TBBUTTON CAppConfig::maintb_btns[]={
	{0,ID_SITE_LIST,TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,0},
	{1,ID_CONNECT_NEW,	TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,1},
	{2,ID_DISCONNECT,	TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,2},
	{3,ID_RECONNECT,	TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,3},
	{4,ID_CONNECT_CLOSE,TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,4},
	{5,ID_ADDTOFAVORITE,TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,5},
	{0,ID_SEPARATOR,	TBSTATE_ENABLED,TBSTYLE_SEP,	0,0},

	{6,ID_COPY,			TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,6},
	{7,ID_ANSICOPY,		TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,7},
	{8,ID_PASTE,		TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,8},
	{9,ID_AUTO_DBCS,	TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,9},
	{0,ID_SEPARATOR,	TBSTATE_ENABLED,TBSTYLE_SEP,	0,0},

	{10,ID_CONFIG_FONT,	TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,10},
	{11,ID_VIEW_CONFIG,	TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,11},
	{12,ID_VIEW_FULLSCR,TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,12},
	{0,ID_SEPARATOR,	TBSTATE_ENABLED,TBSTYLE_SEP,	0,0},

	{13,ID_SETBK,		TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,13},
	{14,ID_TOOL_SYMBOLS,TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,14},
	{15,ID_TOOL_LOCK,	TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,15},
	{0,ID_SEPARATOR,	TBSTATE_ENABLED,TBSTYLE_SEP,	0,0},

	{16,ID_ABOUT,		TBSTATE_ENABLED,TBSTYLE_BUTTON,	0,16}
};
#endif

CString GetIEPath()
{
	CString fpath;	long len=_MAX_PATH;
	HKEY hk = NULL;
//	RegOpenKey( HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\IEXPLORE.EXE", &hk );
//	RegQueryValue( hk, NULL, fpath.GetBuffer(_MAX_PATH), &len);
	RegQueryValue( HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\IEXPLORE.EXE", fpath.GetBuffer(_MAX_PATH), &len);
//	RegCloseKey( hk );
	fpath.ReleaseBuffer();
	return fpath;
}

void CAppConfig::Load(LPCTSTR config_path)
{
	GetIEPath();
	CFile f;
	if(f.Open(config_path,CFile::modeRead))
	{
//General settings pure value
		f.Read(this,((DWORD)&site_settings-(DWORD)this));
//		f.Read(this,((DWORD)&termtype-(DWORD)this));
//General settings object data
		site_settings.ReadFile(f);

		bkpath=LoadString(f);
		wavepath=LoadString(f);
		last_bbslist_item=LoadString(f);

		main_toolbar_inf.LoadFromFile(f);
		hyper_links.Load(f);

//		LoadHistory(f);
//	WWW Settings
	#if defined _COMBO_
		webbar_inf.LoadFromFile(f);
		f.Read(&ads_open_new,((DWORD)&webpage_filter-(DWORD)&ads_open_new));
	#endif
		f.Close();
	}
	else
		Default();

#if defined _COMBO_
	LoadWebPageFilter();
#endif

	lock_pcman=false;
}

void CAppConfig::Save(LPCTSTR config_path)
{
	CFile f;
	if(f.Open(config_path,CFile::modeWrite|CFile::modeCreate))
	{
//General settings pure value
		f.Write(this,((DWORD)&site_settings-(DWORD)this));
//General settings object data
//		site_settings.Save(f);
		site_settings.WriteFile(f);
		SaveString(f,bkpath);
		SaveString(f,wavepath);
		SaveString(f,last_bbslist_item);

		main_toolbar_inf.SaveToFile(f);
		hyper_links.Save(f);

//		SaveHistory(f);
//	WWW Settings
	#if defined _COMBO_
		webbar_inf.SaveToFile(f);
		f.Write(&ads_open_new,((DWORD)&webpage_filter-(DWORD)&ads_open_new));
	#endif

		f.Close();
	}

#if defined _COMBO_
	SaveWebPageFilter();
#endif
}

CAppConfig::CAppConfig()
	: main_toolbar_inf(maintb_btns,sizeof(maintb_btns)/sizeof(TBBUTTON))
#ifdef _COMBO_
	,webbar_inf(webbar_btns,sizeof(webbar_btns)/sizeof(TBBUTTON))
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
	dir2+="PCMan_bak\\";
	CreateDirectory(dir2,NULL);
//	for(int i=0;i<(sizeof(personal_files)/sizeof(char*));i++)
//		CopyFile(dir1+personal_files[i],dir2+personal_files[i],TRUE);
	CFileFind finder;	BOOL found=finder.FindFile(dir1+"*.*");
	while( found )
	{
		found=finder.FindNextFile();
		CopyFile(finder.GetFilePath(),dir2+finder.GetFileName(),FALSE);
	}
}

BOOL CAppConfig::QueryPassword(BOOL confirm,LPCTSTR title)
{
	while(passwd.IsEmpty())
	{
		CPasswdDlg dlg;
		dlg.title=title;
		if(dlg.DoModal()==IDCANCEL)
			return FALSE;

		if(confirm)
		{
			CString passwd1=dlg.passwd;
			dlg.title.LoadString(IDS_CONFIM_PASSWD2);
			if(dlg.DoModal()==IDCANCEL)
				return FALSE;
			if(dlg.passwd != passwd1)
			{
				MessageBox(NULL, LoadString(IDS_DIFF_PASSWD), LoadString(IDS_ERR),MB_OK|MB_ICONSTOP);
				dlg.title.Empty();
			}
			else
				passwd=dlg.passwd;
		}
		else
			passwd=dlg.passwd;
	}
	return TRUE;
}

BOOL IsContainAnsiCode(LPCTSTR str)
{
	while(*str)
	{
		if(*str=='\x1b' && *(str+1)=='[' )
		{
			str+=2;
			while( (isdigit(*str) || *str==';') && *str)
				str++;
			if(*str=='m')
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
	DWORD len=0;
	file.Read(&len,sizeof(DWORD));
	file.Read(str.GetBuffer(len),len);
	str.ReleaseBuffer();
	return str;
}

#ifdef	_COMBO_
inline void CAppConfig::LoadWebPageFilter()
{
	CMemIniFile file;
	if(file.Open(ConfigPath + WWW_ADFILTER_FILENAME, CFile::modeRead))
	{
		int c = 0;
		file.Read4(&c);
		webpage_filter.SetSize( c, 4 );
		for(int i=0; i<c; i++)
			webpage_filter[i] = LoadString(file);
		file.Close();
	}
}

inline void CAppConfig::SaveWebPageFilter()
{
	CFile file;
	if(file.Open(ConfigPath + WWW_ADFILTER_FILENAME, CFile::modeWrite|CFile::modeCreate))
	{
		int c = webpage_filter.GetSize();
		file.Write( &c, 4 );
		for(int i=0; i<c; i++)
			SaveString( file, LPCTSTR(webpage_filter[i]) );
		file.Close();
	}
}
#endif

// FIXME: The format of all config files should be converted to plain text in the 
//        future for easier maintainance and compatibility during upgrade.
// NOTE:  The performance of Win32 API GetPrivateProfileInt is not good enough, and
//        it only deals with files smaller than 64K.  We should write our own ini loader.

//DEL void CAppConfig::LoadIni()
//DEL {
//DEL 	CString ini = ConfigPath + "Config.ini";
//DEL 	multiple_instance = GetPrivateProfileInt("General","multiple_instance",0,ini);
//DEL 	scrolltab = GetPrivateProfileInt("General","scrolltab",0,ini);
//DEL 	close_query = GetPrivateProfileInt("General","close_query",0,ini);
//DEL 	dblclk_close = GetPrivateProfileInt("General","dblclk_close",0,ini);
//DEL 	tab_add_number = GetPrivateProfileInt("General","tab_add_number",0,ini);
//DEL 	tab_button = GetPrivateProfileInt("General","tab_button",0,ini);
//DEL 	showtb = GetPrivateProfileInt("General","showtb",0,ini);
//DEL 	showsb = GetPrivateProfileInt("General","showsb",0,ini);
//DEL 	showtab = GetPrivateProfileInt("General","showtab",0,ini);
//DEL 	showads = GetPrivateProfileInt("General","showads",0,ini);
//DEL 	use_ansi_bar = GetPrivateProfileInt("General","use_ansi_bar",0,ini);
//DEL 	kktab = GetPrivateProfileInt("General","kktab",0,ini);
//DEL 	is_full_scr = GetPrivateProfileInt("General","is_full_scr",0,ini);
//DEL 	fullscr_showtb = GetPrivateProfileInt("General","fullscr_showtb",0,ini);
//DEL 	fullscr_showsb = GetPrivateProfileInt("General","fullscr_showsb",0,ini);
//DEL 	fullscr_showtab = GetPrivateProfileInt("General","fullscr_showtab",0,ini);
//DEL 	fullscr_showads = GetPrivateProfileInt("General","fullscr_showads",0,ini);
//DEL 	full_use_ansi_bar = GetPrivateProfileInt("General","full_use_ansi_bar",0,ini);
//DEL 	minimize_to_tray = GetPrivateProfileInt("General","minimize_to_tray",0,ini);
//DEL 	pcman_hotkey = GetPrivateProfileInt("General","pcman_hotkey",0,ini);
//DEL 	pcman_hotkey_mod = GetPrivateProfileInt("General","pcman_hotkey_mod",0,ini);
//DEL 	max_history = GetPrivateProfileInt("General","max_history",0,ini);
//DEL 	max_history_menu = GetPrivateProfileInt("General","max_history_menu",0,ini);
//DEL 	ed_cols_per_page = GetPrivateProfileInt("AnsiEditor","",0,ini);
//DEL 	ed_lines_per_page = GetPrivateProfileInt("AnsiEditor","",0,ini);
//DEL 
//DEL //Main window
//DEL 	GetPrivateProfileStruct("Position","mainwnd_state",&mainwnd_state,sizeof(mainwnd_state),ini);
//DEL 	GetPrivateProfileStruct("Position","sitelist_state",&sitelist_state,sizeof(sitelist_state),ini);
//DEL 	GetPrivateProfileStruct("Position","freqstr_state",&freqstr_state,sizeof(freqstr_state),ini);
//DEL //ReBar Position
//DEL 	GetPrivateProfileStruct("Position","rebar_bands",rebar_bands,sizeof(rebar_bands),ini);
//DEL 
//DEL //	HyperLink Settings
//DEL 	link_underline = GetPrivateProfileInt("HyperLink","link_underline",0,ini);
//DEL 
//DEL //	BBS Setttings
//DEL 	auto_cancelsel = GetPrivateProfileInt("BBS","auto_cancelsel",0,ini);
//DEL 	auto_copy = GetPrivateProfileInt("BBS","auto_copy",0,ini);
//DEL 	enter_reconnect = GetPrivateProfileInt("BBS","enter_reconnect",0,ini);
//DEL 	nocon_enter_reconnect = GetPrivateProfileInt("BBS","nocon_enter_reconnect",0,ini);
//DEL 	auto_close = GetPrivateProfileInt("BBS","auto_close",0,ini);
//DEL 	auto_font = GetPrivateProfileInt("BBS","auto_font",0,ini);
//DEL 	bbs_close_query = GetPrivateProfileInt("BBS","bbs_close_query",0,ini);
//DEL 	auto_switch = GetPrivateProfileInt("BBS","auto_switch",0,ini);
//DEL 	flash_window = GetPrivateProfileInt("BBS","flash_window",0,ini);
//DEL 	smooth_draw = GetPrivateProfileInt("BBS","smooth_draw",0,ini);
//DEL 
//DEL 	GetPrivateProfileStruct("BBS","colormap",colormap,sizeof(colormap),ini);
//DEL 	GetPrivateProfileStruct("BBS","font_info",&font_info,sizeof(font_info),ini);
//DEL 
//DEL 	bktype = GetPrivateProfileInt("BBS","bktype",0,ini);
//DEL 	bkratio = GetPrivateProfileInt("BBS","bkratio",0,ini);
//DEL 	sound = GetPrivateProfileInt("BBS","sound",0,ini);
//DEL 	old_textout = GetPrivateProfileInt("BBS","old_textout",0,ini);
//DEL 	active_tab_textcolor = GetPrivateProfileInt("BBS","active_tab_textcolor",0,ini);
//DEL 	save_session = GetPrivateProfileInt("BBS","save_session",0,ini);
//DEL 
//DEL //	Site Settings
//DEL //	pure data section
//DEL 	long line_count;
//DEL 	DWORD idle_interval;
//DEL 	UINT connect_interval;
//DEL 	UINT reconnect_interval;
//DEL 	UINT paste_autowrap_col;
//DEL 	WORD cols_per_page;
//DEL 	WORD lines_per_page;
//DEL 
//DEL 	BYTE prevent_idle;
//DEL 	BYTE auto_reconnect;
//DEL 
//DEL 	BYTE showscroll;
//DEL 	BYTE line_wrap;
//DEL 	BYTE paste_autowrap;
//DEL 	BYTE auto_dbcs_mouse;
//DEL 	BYTE auto_dbcs_arrow;
//DEL 	BYTE auto_dbcs_del;
//DEL 	BYTE auto_dbcs_backspace;
//DEL 	BYTE localecho;
//DEL 
//DEL 	char KeyMapName[12];
//DEL 
//DEL //	object section of Site Settings
//DEL 	CString termtype;
//DEL 	CString idle_str;
//DEL 	CString esc_convert;
//DEL 
//DEL //	BBS Setttings
//DEL 	CString bkpath;	//­I´º¹Ï¸ô®|
//DEL 	CString passwd;
//DEL 	CString wavepath;
//DEL 	CString last_bbslist_item;
//DEL 
//DEL }
