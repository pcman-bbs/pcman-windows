// SiteSettings.h: interface for the CSiteSettings class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SITESETTINGS_H__A00B22E8_9898_43A8_883B_B61A62969CD9__INCLUDED_)
#define AFX_SITESETTINGS_H__A00B22E8_9898_43A8_883B_B61A62969CD9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include "KeyMap.h"	// Added by ClassView
#include "TriggerList.h"	// Added by ClassView

enum EncodingConv {GB2BIG5 = 1, BIG52GB = 2};

class CSiteSettings
{
public:
	CSiteSettings();
	~CSiteSettings();
public:
	static CString GetFilePath(CString cfg_path);
	void Save(LPCTSTR fpath);
	BOOL Load(LPCTSTR fpath);
//	pure data section
	long line_count;
	DWORD idle_interval;
	UINT connect_interval;
	UINT reconnect_interval;
	UINT paste_autowrap_col;
	WORD cols_per_page;
	WORD lines_per_page;

	BYTE prevent_idle;
	BYTE auto_reconnect;

	BYTE showscroll;
	BYTE line_wrap;
	BYTE paste_autowrap;
	BYTE auto_dbcs_mouse;
	BYTE auto_dbcs_arrow;
	BYTE auto_dbcs_del;
	BYTE auto_dbcs_backspace;
	BYTE localecho;

	BYTE text_output_conv;	// 顯示文字轉碼	0=none, 1=gb2big5, 2=big52gb
	BYTE text_input_conv;		// 輸入文字轉碼	0=none, 1=gb2big5, 2=big52gb

	BYTE use_global;	// Use global settings

//	object section of Site Settings
	CString key_map_name;
	CString termtype;
	CString idle_str;
	CString esc_convert;
	CTriggerList triggers;	//觸發字串

//	inline void CopyFrom(CSiteSettings* newval);
	inline CSiteSettings& operator = (CSiteSettings& newval);

	inline bool operator==(CSiteSettings &ss)
	{
		return !memcmp(this, &ss, DWORD(&key_map_name) - DWORD(this)) &&
			   termtype == ss.termtype &&
			   idle_str == ss.idle_str &&
			   esc_convert == ss.esc_convert &&
			   text_output_conv == ss.text_output_conv &&
			   text_input_conv == ss.text_input_conv &&
			   !strncmp(key_map_name, ss.key_map_name, 10);
	}

	inline bool operator !=(CSiteSettings &ss)
	{	return !operator==(ss);	}

	inline void Default();
};

inline void CSiteSettings::Default()
{
	line_count = 72;
	idle_interval = 180;
	connect_interval = 15;
	reconnect_interval = 0;

	prevent_idle = 1;
	auto_reconnect = 1;

	showscroll = 1;
	auto_dbcs_mouse = 1;
	auto_dbcs_arrow = 1;
	auto_dbcs_del = 1;
	auto_dbcs_backspace = 1;
	localecho = 0;
	line_wrap = 0;
	paste_autowrap = 1;
	paste_autowrap_col = 78;

	cols_per_page = 80;
	lines_per_page = 24;
	key_map_name = CKeyMap::default_kmname;

//	object section

	idle_str = "^[OA^[OB";
	termtype = "VT100";
	esc_convert = "\x15";	//	Ctrl+U

	text_output_conv = 0;	// 顯示文字轉碼	0=none, 1=gb2big5, 2=big52gb
	text_input_conv = 0;		// 輸入文字轉碼	0=none, 1=gb2big5, 2=big52gb

	use_global = 1;
}

//inline void CSiteSettings::CopyFrom(CSiteSettings* newval)
inline CSiteSettings& CSiteSettings::operator = (CSiteSettings & newval)
{
	if (this != &newval)
	{
		memcpy(this, &newval, size_t(DWORD(&key_map_name) - DWORD(this)));
		key_map_name = newval.key_map_name;
		termtype = newval.termtype;
		idle_str = newval.idle_str;
		esc_convert = newval.esc_convert;
	}
	return *this;
}

#endif // !defined(AFX_SITESETTINGS_H__A00B22E8_9898_43A8_883B_B61A62969CD9__INCLUDED_)
