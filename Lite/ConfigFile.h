// Config.h: interface for the CConfigFile class.
//
/////////////////////////////////////////////////////////////////////////////
// Name:        config.h
// Purpose:     Application configuration class dealing with plain text
//              configuration files automatically.
// Author:      PCMan (Hong Jen Yee)   http://pcman.sayya.org/
// E-mail:      pcman.tw@gmail.com
// Created:     2004.7.22
// Modified:    2007.2.19
// Copyright:   (C) 2004 - 2007 PCMan
// Licence:     GPL : http://www.gnu.org/licenses/gpl.html
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIG_H__72516076_9E02_4FED_AE77_4B1DD28B13A1__INCLUDED_)
#define AFX_CONFIG_H__72516076_9E02_4FED_AE77_4B1DD28B13A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//	Copyright (C) 2004 - 2007 PCMan
//	Author: PCMan (Hong Jen Yee) 2004/07/22	07:51 AM
//	I finally came up with a really smart way to maintain ini file.
//	Every time I add a variable to CAppConfig, all I need to do is
//	adding the variable in my "Config Table," and all the data will
//	be load and save automatically.  This is not the most efficient way.
//	In my first version I use some more efficient method, but at last I change
//	my code to what it is now.  Because I think in a program not time-critical,
//	easy-maintaining is much more important.

#include <afx.h>

class CConfigFile
{
public:

	enum ValueType      // type of recognized config value
	{
		VT_END = 0, // last entry of entry array
		VT_BOOL = 1,
		VT_INT,
		VT_LONG,
		VT_SHORT,
		VT_BYTE,
		VT_STR, // CString
		VT_PSTR,    // LPSTR
		VT_ESTR,    // Escaped CString
		VT_COLOR,   // COLORREF
		VT_CUSTOM,  // user defined callback handler with an additinal argument
		VT_LAST = VT_CUSTOM,
		VT_SECT,    // point to array of ConfigEntry
		VT_CUSTOM_SECT // user defined callback handler with an additinal argument
	};

	struct ConfigEntry   // config key/value entry
	{
		bool finished;
		LPCTSTR name;
		ValueType type;
		void* data;
		// void func( bool load, char* line/CString* ret, void* user_data )
		//void (*func)(bool, void*, void*);
	};

	struct ConfigHandler
	{
		ConfigHandler(){}
		~ConfigHandler(){}
		virtual void Load(char* value) = 0;
		virtual void Save(CString& value) = 0;
	};

	typedef ConfigEntry* ConfigTable;

	CString GetFilePath() { return file_path; }
	void SetFilePath(CString path) { file_path = path; }
	bool Save()
	{
		return OnDataExchange(false);	// call virtual function defined in derived class.
	}
	bool Load()
	{
		return OnDataExchange(true);	// call virtual function defined in derived class.
	}
	CConfigFile();
	virtual bool OnDataExchange(bool load) = 0;
	bool DoDataExchange(bool load, ConfigTable table);
	virtual ~CConfigFile();

private:
	bool DoSave(ConfigTable table);
	bool DoLoad(ConfigTable table);

	ConfigEntry* cur_sect;
	CString file_path;
};

#define		BEGIN_CFG_SECTION( name )  ConfigEntry name[] = {
#define		END_CFG_SECTION()		{0}};

#define		BEGIN_CFG_FILE(name)		BEGIN_CFG_SECTION(name)
#define		END_CFG_FILE()		END_CFG_SECTION()

#define     DEFINE_CFG_ENTRY(name, type, variable) \
	{0, name, CConfigFile::VT_##type, &variable},

#define	_CFG_BOOL( name, variable)	DEFINE_CFG_ENTRY( name, BOOL, variable )
#define	CFG_BOOL( variable)	DEFINE_CFG_ENTRY( #variable, BOOL, variable )

#define	_CFG_INT( name, variable)	DEFINE_CFG_ENTRY( name, INT, variable )
#define	CFG_INT( variable)	DEFINE_CFG_ENTRY( #variable, INT, variable )

#define	_CFG_LONG( name, variable)	DEFINE_CFG_ENTRY( name, LONG, variable )
#define	CFG_LONG( variable)	DEFINE_CFG_ENTRY( #variable, LONG, variable )

#define	_CFG_SHORT( name, variable)	DEFINE_CFG_ENTRY( name, SHORT, variable )
#define	CFG_SHORT( variable)	DEFINE_CFG_ENTRY( #variable, SHORT, variable )

#define	_CFG_BYTE( name, variable)	DEFINE_CFG_ENTRY( name, BYTE, variable )
#define	CFG_BYTE( variable)	DEFINE_CFG_ENTRY( #variable, BYTE, variable )

#define	_CFG_STR( name, variable)	DEFINE_CFG_ENTRY( name, STR, variable )
#define	CFG_STR( variable)	DEFINE_CFG_ENTRY( #variable, STR, variable )

#define	_CFG_ESTR( name, variable)	DEFINE_CFG_ENTRY( name, ESTR, variable )
#define	CFG_ESTR( variable)	DEFINE_CFG_ENTRY( #variable, ESTR, variable )

#define	_CFG_COLOR( name, variable)	DEFINE_CFG_ENTRY( name, COLOR, variable )
#define	CFG_COLOR( variable)	DEFINE_CFG_ENTRY( #variable, COLOR, variable )

#define	CFG_CUSTOM( name, handler)	{0, name, CConfigFile::VT_CUSTOM, (void*)(CConfigFile*)&handler},
//#define	CFG_CUSTOM( name, func, arg )	{0, name, CConfigFile::VT_CUSTOM, arg, func },

#define	CFG_SECTION( variable )	DEFINE_CFG_ENTRY( #variable, SECT, variable )
//#define	CFG_CUSTOM_SECTION( name, func, arg )	{0, name, CConfigFile::VT_CUSTOM_SECT, arg, func },
#define	CFG_CUSTOM_SECTION( name, handler)	{0, name, CConfigFile::VT_CUSTOM_SECT, (void*)(CConfigFile*)&handler},

#endif // !defined(AFX_CONFIG_H__72516076_9E02_4FED_AE77_4B1DD28B13A1__INCLUDED_)
