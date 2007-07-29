// SiteSettings.cpp: implementation of the CSiteSettings class.
//
//////////////////////////////////////////////////////////////////////

#include "SiteSettings.h"
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

CSiteSettings::CSiteSettings()
{

}

CSiteSettings::~CSiteSettings()
{

}

class CSiteSettingsLoader : public CConfigFile
{
public:
	CSiteSettingsLoader(CSiteSettings& settings, CString path)
			: site_settings(settings)
	{
		SetFilePath(CSiteSettings::GetFilePath(path));
	}

	bool OnDataExchange(bool load)
	{
		if (load)
		{
			site_settings = AppConfig.site_settings;
		}
		else	// save
		{
			if (site_settings.use_global)
			{
				BEGIN_CFG_SECTION(Site)
				_CFG_BYTE("use_global", site_settings.use_global)
				END_CFG_SECTION()

				BEGIN_CFG_FILE(table)
				CFG_SECTION(Site)
				CFG_CUSTOM_SECTION("Triggers", site_settings.triggers)
				END_CFG_FILE()
				return DoDataExchange(load, table);
			}
		}

		BEGIN_CFG_SECTION(Site)
		_CFG_BYTE("use_global", site_settings.use_global)
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
		// CTriggerList triggers;	//觸發字串
		END_CFG_SECTION()

		BEGIN_CFG_FILE(table)
		CFG_SECTION(Site)
		CFG_CUSTOM_SECTION("Triggers", site_settings.triggers)
		END_CFG_FILE()

		bool ret = DoDataExchange(load, table);
		return ret;
	}
protected:
	CSiteSettings& site_settings;
};

BOOL CSiteSettings::Load(LPCTSTR fpath)
{
	*this = AppConfig.site_settings;
	CSiteSettingsLoader loader(*this, fpath);
	loader.Load();
	return TRUE;
}

void CSiteSettings::Save(LPCTSTR fpath)
{
	// When we use global settings and don't have triggers,
	// the config file is no more needed, and should be deleted.
	if (use_global && triggers.count == 0)
	{
		DeleteFile(GetFilePath(fpath));
		return;
	}
	CFile file;
	CSiteSettingsLoader loader(*this, fpath);
	loader.Save();
}

CString CSiteSettings::GetFilePath(CString cfg_path)
{
	CString fpath;

	fpath = ConfigPath;
	//fpath += md5;
	cfg_path.Replace("\\", "__");
	fpath += cfg_path;
	fpath += ".ini";

	return fpath;
}
