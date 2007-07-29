// AppConfig.cpp: implementation of the CAppConfig class.
//
//////////////////////////////////////////////////////////////////////

#include <afxwin.h>
#include "AppConfig.h"
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

void CAppConfig::Load(LPCTSTR config_path)
{
	CFile f;
	if (f.Open(config_path, CFile::modeRead))
	{
		//General settings pure value
		f.Read(this, ((DWORD)&rebar_bands[0] - (DWORD)this));
		f.Read(rebar_bands, sizeof(CReBarBandPos) * (IsCombo ? 5 : 4));
		f.Read(&link_underline, ((DWORD)&site_settings - (DWORD)&link_underline));
		//General settings object data
		site_settings.ReadFile(f);

		bkpath = LoadString(f);
		wavepath = LoadString(f);
		last_bbslist_item = LoadString(f);

		main_toolbar_inf.LoadFromFile(f);
		hyper_links.Load(f);

		//	WWW Settings
		if (IsCombo)
		{
			webbar_inf.LoadFromFile(f);
			f.Read(&ads_open_new, ((DWORD)&webpage_filter - (DWORD)&ads_open_new));
		}
		f.Close();
	}
	else
		Default();

}

void fprint_color(CFile& file, const char* name, COLORREF clr)
{
	fprintf(file, "%s=%d,%d,%d\r\n", name, GetRValue(clr), GetGValue(clr), GetBValue(clr));
}

void fprint_wndstate(CFile& file, const char* name, CWindowState& state)
{
	fprintf(file,
			"%s=%d,%d,%d,%d,%d\r\n",
			name,
			state.rect.left,
			state.rect.top,
			state.rect.right,
			state.rect.bottom,
			state.showcmd);
}

void fprint_toolbarinf(CFile& file, const char* name, CCustomToolBarInfo& inf)
{
	CString value;
	for (int i = 0; i < inf.count; ++i)
	{
		char tmp[10];
		if (! value.IsEmpty())
			value += ',';
		sprintf(tmp, "%d", inf.index[i]);
		value += tmp;
	}
	value = name + ('=' + value);
	value += "\r\n";
	file.Write(LPCTSTR(value), value.GetLength());
}

void fprint_rebar(CFile& file, const char* name, CReBarBandPos& pos)
{
	fprintf(file, "%s=%d,%d,%d\r\n", name, pos.wID, pos.fStyle, pos.cx);
}

void CAppConfig::Save(LPCTSTR config_path)
{
	CFile file;
	if (! file.Open(config_path, CFile::modeWrite | CFile::modeCreate))
		return;

//	General Setting
	fprintf(file, "[General]\r\n");
	fprintf(file, "multiple_instance=%d\r\n", multiple_instance);
	fprintf(file, "scrolltab=%d\r\n", scrolltab);
	fprintf(file, "close_query=%d\r\n", close_query);
	fprintf(file, "dblclk_close=%d\r\n", dblclk_close);
	fprintf(file, "tab_add_number=%d\r\n", tab_add_number);
	fprintf(file, "tab_button=%d\r\n", tab_button);
	fprintf(file, "showtb=%d\r\n", showtb);
	fprintf(file, "showsb=%d\r\n", showsb);
	fprintf(file, "showtab=%d\r\n", showtab);
	fprintf(file, "showads=%d\r\n", showads);
	fprintf(file, "showclose=%d\r\n", showclose);
	fprintf(file, "use_ansi_bar=%d\r\n", use_ansi_bar);
	fprintf(file, "kktab=%d\r\n", kktab);
	fprintf(file, "is_full_scr=%d\r\n", is_full_scr);
	fprintf(file, "use_MouseCTL=%d\r\n", use_MouseCTL);

	fprintf(file, "fullscr_showtb=%d\r\n", fullscr_showtb);
	fprintf(file, "fullscr_showsb=%d\r\n", fullscr_showsb);
	fprintf(file, "fullscr_showtab=%d\r\n", fullscr_showtab);
	fprintf(file, "fullscr_showads=%d\r\n", fullscr_showads);
	fprintf(file, "fullscr_showclose=%d\r\n", fullscr_showclose);
	fprintf(file, "full_use_ansi_bar=%d\r\n", full_use_ansi_bar);

	fprintf(file, "minimize_to_tray=%d\r\n", minimize_to_tray);
	fprintf(file, "pcman_hotkey=%d\r\n", pcman_hotkey);
	fprintf(file, "pcman_hotkey_mod=%d\r\n", pcman_hotkey_mod);

	fprintf(file, "max_history=%d\r\n", max_history);

	file.Write("\r\n", 2);

//	BBS Setttings
	fprintf(file, "[BBS]\r\n");
	fprintf(file, "auto_cancelsel=%d\r\n", auto_cancelsel);
	fprintf(file, "auto_copy=%d\r\n", auto_copy);
	fprintf(file, "enter_reconnect=%d\r\n", enter_reconnect);
	fprintf(file, "nocon_enter_reconnect=%d\r\n", nocon_enter_reconnect);
	fprintf(file, "auto_close=%d\r\n", auto_close);
	fprintf(file, "auto_font=%d\r\n", auto_font);
	fprintf(file, "bbs_close_query=%d\r\n", bbs_close_query);
	fprintf(file, "auto_switch=%d\r\n", auto_switch);
	fprintf(file, "flash_window=%d\r\n", flash_window);
	fprintf(file, "smooth_draw=%d\r\n", smooth_draw);

	fprintf(file, "bktype=%d\r\n", bktype);
	fprintf(file, "bkratio=%d\r\n", bkratio);		//	ratio/10*100%
	fprintf(file, "sound=%d\r\n", sound);		//type of beep
	fprintf(file, "old_textout=%d\r\n", old_textout);
	fprintf(file, "save_session=%d\r\n", save_session);
	fprintf(file, "switch_back_on_close=%d\r\n", switch_back_on_close);
	fprintf(file, "dblclk_select=%d\r\n", 1);
	fprintf(file, "bkpath=%s\r\n", LPCTSTR(bkpath));	//背景圖路徑
	fprintf(file, "wavepath=%s\r\n", LPCTSTR(wavepath));

//	AnsiEditor Settings
	fprintf(file, "ed_cols_per_page=%d\r\n", ed_cols_per_page);
	fprintf(file, "ed_lines_per_page=%d\r\n", ed_lines_per_page);

//	HyperLink Settings
	fprintf(file, "link_underline=%d\r\n", link_underline);
	fprintf(file, "link_autoswitch=%d\r\n", link_autoswitch);

	file.Write("\r\n", 2);

	fprintf(file, "[Font]\r\n");
	fprintf(file, "face=%s\r\n", font_info.lfFaceName);
	fprintf(file, "height=%d\r\n",  font_info.lfHeight);
	fprintf(file, "width=%d\r\n",  font_info.lfWidth);
	fprintf(file, "esc=%d\r\n",  font_info.lfEscapement);
	fprintf(file, "orient=%d\r\n",  font_info.lfOrientation);
	fprintf(file, "weight=%d\r\n",  font_info.lfWeight);
	fprintf(file, "italic=%d\r\n",  font_info.lfItalic);
	fprintf(file, "underline=%d\r\n",  font_info.lfUnderline);
	fprintf(file, "charset=%d\r\n",  font_info.lfCharSet);
	fprintf(file, "oprec=%d\r\n",  font_info.lfOutPrecision);
	fprintf(file, "cprec=%d\r\n",  font_info.lfClipPrecision);
	fprintf(file, "quality=%d\r\n",  font_info.lfQuality);
	fprintf(file, "pitch=%d\r\n",  font_info.lfPitchAndFamily);
	file.Write("\r\n", 2);

	site_settings.WriteFile(file);
	file.Write("\r\n", 2);

	hyper_links.Save(file);
	file.Write("\r\n", 2);

	fprintf(file, "[Color]\r\n");
	fprint_color(file, "black", colormap[0]);
	fprint_color(file, "dark_red", colormap[1]);
	fprint_color(file, "dark_green", colormap[2]);
	fprint_color(file, "brown", colormap[3]);
	fprint_color(file, "dark_blue", colormap[4]);
	fprint_color(file, "dark_magenta", colormap[5]);
	fprint_color(file, "dark_cyan", colormap[6]);
	fprint_color(file, "light_gray", colormap[7]);
	fprint_color(file, "gray", colormap[8]);
	fprint_color(file, "red", colormap[9]);
	fprint_color(file, "green", colormap[10]);
	fprint_color(file, "yellow", colormap[11]);
	fprint_color(file, "blue", colormap[12]);
	fprint_color(file, "magenta", colormap[13]);
	fprint_color(file, "cyan", colormap[14]);
	fprint_color(file, "white", colormap[15]);
	fprint_color(file, "active_tab_textcolor", active_tab_textcolor);
	file.Write("\r\n", 2);

	fprintf(file, "[Window]\r\n");
// Window position
	fprint_wndstate(file, "main", mainwnd_state);
	fprint_wndstate(file, "site_list", sitelist_state);
	fprint_wndstate(file, "freq_str", freqstr_state);
// Customize toolbar buttons
	fprint_toolbarinf(file, "main_toolbar", main_toolbar_inf);

	if (IsCombo)
		fprint_toolbarinf(file, "web_bar", webbar_inf);

// ReBar Position
	fprint_rebar(file, "rebar0", rebar_bands[0]);	// id=1
	fprint_rebar(file, "rebar1", rebar_bands[2]);	// id=3
	fprint_rebar(file, "rebar2", rebar_bands[3]);	// id=4
	fprint_rebar(file, "rebar3", rebar_bands[1]);	// id=2

	if (IsCombo)
	{
		fprint_rebar(file, "rebar4", rebar_bands[4]);	// id=5
		fprintf(file, "rebar5=6,160,336\r\n");	//id=6
	}
	last_bbslist_item.Replace(';', '\\');
	fprintf(file, "last_bbslist_item=%s\r\n", last_bbslist_item);
	file.Write("\r\n", 2);

//	Web Settings
	if (IsCombo)
	{
		fprintf(file, "[Web]\r\n");
		fprintf(file, "ads_open_new=%d\r\n", ads_open_new);
		fprintf(file, "disable_popup=%d\r\n", disable_popup);
		fprintf(file, "showwb=%d\r\n", showwb);
		fprintf(file, "fullscr_showwb=%d\r\n", fullscr_showwb);
		fprintf(file, "showsearchbar=%d\r\n", 1);
		fprintf(file, "fullscr_showsearchbar=%d\r\n", 1);
		fprintf(file, "autosort_favorite=%d\r\n", autosort_favorite);
		fprintf(file, "disable_script_error=%d\r\n", disable_script_error);
		fprintf(file, "use_ie_fav=%d\r\n", use_ie_fav);
		fprintf(file, "autowrap_favorite=%d\r\n", autowrap_favorite);
		file.Write("\r\n", 2);
	}
}

CAppConfig::CAppConfig()
{
}

void CAppConfig::BackupConfig(CString dir1, CString dir2)
{
	MoveFile(dir1, dir2);
	CreateDirectory(dir1, NULL);
	/*
		CreateDirectory(dir2,NULL);
		CFileFind finder;	BOOL found=finder.FindFile(dir1+"*.*");
		while( found )
		{
			found=finder.FindNextFile();
			CopyFile(finder.GetFilePath(),dir2+finder.GetFileName(),FALSE);
		}
	*/
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

CString LoadString(CFile& file)
{
	CString str;
	DWORD len = 0;
	file.Read(&len, sizeof(DWORD));
	file.Read(str.GetBuffer(len), len);
	str.ReleaseBuffer();
	return str;
}

void fprintf(CFile& file, const char* format, ...)
{
	char line[4096];
	va_list va;
	va_start(va, format);
	int len = wvsprintf(line, format, va);
	va_end(va);
	file.Write(line, len);
}

void CAppConfig::SaveHistory(CFile &f)
{
	int i;
	fprintf(f, "[Menu]\r\n");
	for (i = 0; i < history_menu.GetSize(); ++i)
	{
		CString str = history_menu[i];
		if (str[0] == 's')	// bbs
		{
			int p = str.Find('\t');
			if (p <= 0)
				continue;
			int p2 = str.Find('\t', p + 1);
			if (p2 > 0)	// 有進階設定檔
			{
				CString name = str.Mid(1, p - 1);
				CString address = str.Mid(p, (p2 - p));
				CString cfg = str.Mid(p2 + 1);

				str = 's' + name;
				str += address;
				if (0 == strnicmp(cfg, ConfigPath, ConfigPath.GetLength()))
				{
					cfg = cfg.Mid(ConfigPath.GetLength());
					cfg = cfg.Left(cfg.GetLength() - name.GetLength());
					cfg.Replace(';', '\\');
					cfg += name;
					str += '\t';
					str += cfg;
				}
			}
		}
		str += "\r\n";
		f.Write(LPCTSTR(str), str.GetLength());
	}
	fprintf(f, "\r\n[DropDown]\r\n");
	for (i = 0; i < history_dropdown.GetSize(); ++i)
		fprintf(f, "%s\r\n", LPCTSTR(history_dropdown[i]));
	fprintf(f, "\r\n[Typed]\r\n");
	POSITION pos = history.GetHeadPosition();
	for (; pos; history.GetNext(pos))
		fprintf(f, "%s\r\n", LPCTSTR(history.GetAt(pos)));

}
