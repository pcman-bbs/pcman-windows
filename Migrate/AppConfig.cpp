// AppConfig.cpp: implementation of the CAppConfig class.
//
//////////////////////////////////////////////////////////////////////

#include <afxwin.h>
#include "AppConfig.h"
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

void CAppConfig::Load(LPCTSTR config_path)
{
	CFile f;
	if( f.Open(config_path,CFile::modeRead) )
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
}

void CAppConfig::Save(LPCTSTR config_path)
{
	CFile file;
	if( ! file.Open( config_path, CFile::modeWrite|CFile::modeCreate ) )
		return;

//	General Setting
	fprintf( file, "[General]\r\n" );
	fprintf( file, "multiple_instance=%d\r\n", multiple_instance );
	fprintf( file, "scrolltab=%d\r\n", scrolltab );
	fprintf( file, "close_query=%d\r\n", close_query );
	fprintf( file, "dblclk_close=%d\r\n", dblclk_close );
	fprintf( file, "tab_add_number=%d\r\n", tab_add_number );
	fprintf( file, "tab_button=%d\r\n", tab_button );
	fprintf( file, "showtb=%d\r\n", showtb );
	fprintf( file, "showsb=%d\r\n", showsb );
	fprintf( file, "showtab=%d\r\n", showtab );
	fprintf( file, "showads=%d\r\n", showads );
	fprintf( file, "showclose=%d\r\n", showclose );
	fprintf( file, "use_ansi_bar=%d\r\n", use_ansi_bar );
	fprintf( file, "kktab=%d\r\n", kktab );
	fprintf( file, "is_full_scr=%d\r\n", is_full_scr );

	fprintf( file, "fullscr_showtb=%d\r\n", fullscr_showtb );
	fprintf( file, "fullscr_showsb=%d\r\n", fullscr_showsb );
	fprintf( file, "fullscr_showtab=%d\r\n", fullscr_showtab );
	fprintf( file, "fullscr_showads=%d\r\n", fullscr_showads );
	fprintf( file, "fullscr_showclose=%d\r\n", fullscr_showclose );
	fprintf( file, "full_use_ansi_bar=%d\r\n", full_use_ansi_bar );

	fprintf( file, "minimize_to_tray=%d\r\n", minimize_to_tray );
	fprintf( file, "pcman_hotkey=%d\r\n", pcman_hotkey );
	fprintf( file, "pcman_hotkey_mod=%d\r\n", pcman_hotkey_mod );

	fprintf( file, "max_history=%d\r\n", max_history );

    file.Write("\r\n", 2);

//	BBS Setttings
	fprintf( file, "[BBS]\r\n" );
	fprintf( file, "auto_cancelsel=%d\r\n", auto_cancelsel );
	fprintf( file, "auto_copy=%d\r\n", auto_copy );
	fprintf( file, "enter_reconnect=%d\r\n", enter_reconnect );
	fprintf( file, "nocon_enter_reconnect=%d\r\n", nocon_enter_reconnect );
	fprintf( file, "auto_close=%d\r\n", auto_close );
	fprintf( file, "auto_font=%d\r\n", auto_font );
	fprintf( file, "bbs_close_query=%d\r\n", bbs_close_query );
	fprintf( file, "auto_switch=%d\r\n", auto_switch );
	fprintf( file, "flash_window=%d\r\n", flash_window );
	fprintf( file, "smooth_draw=%d\r\n", smooth_draw );

	fprintf( file, "bktype=%d\r\n", bktype );
	fprintf( file, "bkratio=%d\r\n", bkratio );		//	ratio/10*100%
	fprintf( file, "sound=%d\r\n", sound );		//type of beep
	fprintf( file, "old_textout=%d\r\n", old_textout );
	fprintf( file, "save_session=%d\r\n", save_session );
	fprintf( file, "switch_back_on_close=%d\r\n", switch_back_on_close );
	fprintf( file, "dblclk_select=%d\r\n", 1 );
	fprintf( file, "bkpath=%s\r\n", LPCTSTR(bkpath) );	//­I´º¹Ï¸ô®|
	fprintf( file, "wavepath=%s\r\n", LPCTSTR(wavepath) );

//	AnsiEditor Settings
	fprintf( file, "ed_cols_per_page=%d\r\n", ed_cols_per_page );
	fprintf( file, "ed_lines_per_page=%d\r\n", ed_lines_per_page );

//	HyperLink Settings
	fprintf( file, "link_underline=%d\r\n", link_underline );
	fprintf( file, "link_autoswitch=%d\r\n", link_autoswitch );

    file.Write( "\r\n", 2 );

	fprintf( file, "[Font]\r\n" );
	fprintf( file, "face=%s\r\n", font_info.lfFaceName ); 
	fprintf( file, "height=%d\r\n",  font_info.lfHeight );
	fprintf( file, "width=%d\r\n",  font_info.lfWidth ); 
	fprintf( file, "esc=%d\r\n",  font_info.lfEscapement ); 
	fprintf( file, "orient=%d\r\n",  font_info.lfOrientation ); 
	fprintf( file, "weight=%d\r\n",  font_info.lfWeight ); 
	fprintf( file, "italic=%d\r\n",  font_info.lfItalic ); 
	fprintf( file, "underline=%d\r\n",  font_info.lfUnderline ); 
	//	   fprintf( file, "font_info.lfStrikeOut ); 
	fprintf( file, "charset=%d\r\n",  font_info.lfCharSet ); 
	fprintf( file, "oprec=%d\r\n",  font_info.lfOutPrecision ); 
	fprintf( file, "cprec=%d\r\n",  font_info.lfClipPrecision ); 
	fprintf( file, "quality=%d\r\n",  font_info.lfQuality ); 
	fprintf( file, "pitch=%d\r\n",  font_info.lfPitchAndFamily ); 
    file.Write("\r\n", 2);

	site_settings.WriteFile( file );
/*
    fprintf( file, "[Color]\r\n" );
	_CFG_COLOR( "black", colormap[0] )
	_CFG_COLOR( "dark_red", colormap[1] )
	_CFG_COLOR( "dark_green", colormap[2] )
	_CFG_COLOR( "brown", colormap[3] )
	_CFG_COLOR( "dark_blue", colormap[4] )
	_CFG_COLOR( "dark_magenta", colormap[5] )
	_CFG_COLOR( "dark_cyan", colormap[6] )
	_CFG_COLOR( "light_gray", colormap[7] )
	_CFG_COLOR( "gray", colormap[8] )
	_CFG_COLOR( "red", colormap[9] )
	_CFG_COLOR( "green", colormap[10] )
	_CFG_COLOR( "yellow", colormap[11] )
	_CFG_COLOR( "blue", colormap[12] )
	_CFG_COLOR( "magenta", colormap[13] )
	_CFG_COLOR( "cyan", colormap[14] )
	_CFG_COLOR( "white", colormap[15] )

		CFG_COLOR( active_tab_textcolor )
	END_CFG_SECTION()
*/
/*
	fprintf( file, "[Window]\r\n" );
// Window position
    CFG_CUSTOM( "main", mainwnd_state )
    CFG_CUSTOM( "site_list", sitelist_state )
    CFG_CUSTOM( "freq_str", freqstr_state )

// Customize toolbar buttons
	CFG_CUSTOM( "main_toolbar", main_toolbar_inf )
#if defined _COMBO_
		CFG_CUSTOM( "web_bar", webbar_inf )
#endif

// ReBar Position
        CFG_CUSTOM( "rebar0", rebar_bands[0] )
        CFG_CUSTOM( "rebar1", rebar_bands[1] )
        CFG_CUSTOM( "rebar2", rebar_bands[2] )
        CFG_CUSTOM( "rebar3", rebar_bands[3] )
#if defined _COMBO_
        CFG_CUSTOM( "rebar4", rebar_bands[4] )
        CFG_CUSTOM( "rebar5", rebar_bands[5] )
#endif
	    fprintf( file, "last_bbslist_item )
	END_CFG_SECTION()

//	Web Settings
#if defined (_COMBO_)
	fprintf( file, "[Web)
	    fprintf( file, "ads_open_new )
	    fprintf( file, "disable_popup )
	    fprintf( file, "showwb )
	    fprintf( file, "fullscr_showwb )
	    fprintf( file, "showsearchbar )
	    fprintf( file, "fullscr_showsearchbar )
	    fprintf( file, "autosort_favorite )
	    fprintf( file, "disable_script_error )
	    fprintf( file, "use_ie_fav )
	    fprintf( file, "autowrap_favorite )
	    fprintf( file, " search_engine )
	END_CFG_SECTION()
// Object Section
//	CStringArray webpage_filter;
#endif

*/

#if 0
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

#endif
}

CAppConfig::CAppConfig()
{
}

void CAppConfig::BackupConfig(CString dir1, CString dir2)
{
	CreateDirectory(dir2,NULL);
	CFileFind finder;	BOOL found=finder.FindFile(dir1+"*.*");
	while( found )
	{
		found=finder.FindNextFile();
		CopyFile(finder.GetFilePath(),dir2+finder.GetFileName(),FALSE);
	}
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

CString LoadString(CFile& file)
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
	CFile file;
	if(file.Open(ConfigPath + WWW_ADFILTER_FILENAME, CFile::modeRead))
	{
		int c = 0;
		file.Read(&c, 4);
		webpage_filter.SetSize( c, 4 );
		for(int i=0; i<c; i++)
			webpage_filter[i] = LoadString(file);
		file.Close();
	}
}

inline void CAppConfig::SaveWebPageFilter()
{
/*
	CFile file;
	if(file.Open(ConfigPath + WWW_ADFILTER_FILENAME, CFile::modeWrite|CFile::modeCreate))
	{
		int c = webpage_filter.GetSize();
		for( int i = 0; i < c; ++i )
		{
			CString& line = webpage_filter[i];
			file.Write( LPCTSTR( line ), line.GetLength() );
			file.Write( "\r\n", 2 );
		}
		file.Close();
	}
*/
}
#endif


void fprintf( CFile& file, const char* format, ... )
{
	char line[4096];
	va_list va;
	va_start( va, format );
	int len = wvsprintf( line, format, va );
	va_end( va );
	file.Write( line, len );
}

