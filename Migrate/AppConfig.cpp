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

