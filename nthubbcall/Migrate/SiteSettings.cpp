// SiteSettings.cpp: implementation of the CSiteSettings class.
//
//////////////////////////////////////////////////////////////////////

#include "SiteSettings.h"
#include "AppConfig.h"
#include "StrUtils.h"

#include <stdarg.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSiteSettings::CSiteSettings()
{
	use_global = false;
}

CSiteSettings::~CSiteSettings()
{

}


void CSiteSettings::ReadFile(CFile& file)
{
	file.Read(this, DWORD(&termtype) - DWORD(this));
	termtype = LoadString(file);
	idle_str = LoadString(file);
	esc_convert = LoadString(file);
}

void CSiteSettings::WriteFile(CFile& file)
{
	fprintf(file, "[Site]\r\n");
	fprintf(file, "use_global=%d\r\n\r\n", use_global);

	if (! use_global)
	{
		fprintf(file, "line_count=%d\r\n", line_count);
		fprintf(file, "idle_interval=%d\r\n", idle_interval);
		fprintf(file, "connect_interval=%d\r\n", connect_interval);
		fprintf(file, "reconnect_interval=%d\r\n", reconnect_interval);
		fprintf(file, "paste_autowrap_col=%d\r\n", paste_autowrap_col);
		fprintf(file, "cols_per_page=%d\r\n", cols_per_page);
		fprintf(file, "lines_per_page=%d\r\n", lines_per_page);

		fprintf(file, "prevent_idle=%d\r\n", prevent_idle);
		fprintf(file, "auto_reconnect=%d\r\n", auto_reconnect);

		fprintf(file, "showscroll=%d\r\n", showscroll);
		fprintf(file, "line_wrap=%d\r\n", line_wrap);
		fprintf(file, "paste_autowrap=%d\r\n", paste_autowrap);
		fprintf(file, "auto_dbcs_mouse=%d\r\n", auto_dbcs_mouse);
		fprintf(file, "auto_dbcs_arrow=%d\r\n", auto_dbcs_arrow);
		fprintf(file, "auto_dbcs_del=%d\r\n", auto_dbcs_del);
		fprintf(file, "auto_dbcs_backspace=%d\r\n", auto_dbcs_backspace);
		fprintf(file, "localecho=%d\r\n", localecho);

		fprintf(file, "text_output_conv=%d\r\n", text_output_conv);	// 顯示文字轉碼	0=none, 1=gb2big5, 2=big52gb
		fprintf(file, "text_input_conv=%d\r\n", text_input_conv);		// 輸入文字轉碼	0=none, 1=gb2big5, 2=big52gb

		if (0 == strcmp(KeyMapName, "預設"))
			strcpy(KeyMapName, "Default");

		fprintf(file, "key_map_name=%s\r\n", KeyMapName);
		fprintf(file, "termtype=%s\r\n", LPCTSTR(termtype));
		fprintf(file, "idle_str=%s\r\n", LPCTSTR(idle_str));
		fprintf(file, "esc_convert=%s\r\n", LPCTSTR(EscapeControlChars(esc_convert)));
	}

	if (triggers.count > 0)
	{
		fprintf(file, "[Triggers]\r\n");
		triggers.SaveToFile(file);
	}
}

BOOL CSiteSettings::Load(LPCTSTR fpath)
{
	BOOL str_trigger = 0;
	CFile file;
	if (file.Open(fpath, CFile::modeRead))
	{
		//檢查是否使用預設值
		file.Read(&str_trigger, sizeof(str_trigger));
		if (str_trigger)	//如果使用預設值
		{
			*this = AppConfig.site_settings;
			use_global = true;
		}
		else	//載入個別設定
			ReadFile(file);
		if (!triggers.LoadFromFile(file))
		{
			file.Close();
			return FALSE;
		}
		file.Close();
	}
	else
		*this = AppConfig.site_settings;
	return TRUE;
}

void CSiteSettings::Save(LPCTSTR fpath)
{
	// 不需要使用進階設定檔
	if (use_global && triggers.count == 0)
		return;

	CFile file;
	if (!file.Open(fpath, CFile::modeWrite | CFile::modeCreate))
		return;
	WriteFile(file);
}
