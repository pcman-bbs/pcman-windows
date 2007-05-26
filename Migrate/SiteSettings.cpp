// SiteSettings.cpp: implementation of the CSiteSettings class.
//
//////////////////////////////////////////////////////////////////////

#include "SiteSettings.h"
#include "AppConfig.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
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


void CSiteSettings::ReadFile(CFile& file)
{
	file.Read(this,DWORD(&termtype)-DWORD(this));
	termtype=LoadString(file);
	idle_str=LoadString(file);
	esc_convert=LoadString(file);
}

void CSiteSettings::WriteFile(CFile& file)
{
/*
	file.Write(this,DWORD(&termtype)-DWORD(this));
	SaveString(file,termtype);
	SaveString(file,idle_str);
	SaveString(file,esc_convert);
*/
}

BOOL CSiteSettings::Load(LPCTSTR fpath)
{
	BOOL str_trigger=0;
	CFile file;
	if(file.Open(fpath,CFile::modeRead))
	{
		//檢查是否使用預設值
		file.Read(&str_trigger,sizeof(str_trigger));
		if(str_trigger)	//如果使用預設值
			*this = AppConfig.site_settings;
		else	//載入個別設定
			ReadFile(file);
		if( !triggers.LoadFromFile(file) )
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
#if 0
	CFile file;
	BOOL use_default = operator ==( AppConfig.site_settings );

	//如果設定值和預設值相同而且沒有使用字串觸發，就把設定檔刪除
	if(use_default && triggers.count==0 )
	{
		DeleteFile(fpath);
		return;
	}

	if(file.Open(fpath,CFile::modeWrite|CFile::modeCreate))
	{
		file.Write(&use_default,sizeof(use_default));
		if(!use_default)
			WriteFile(file);
		triggers.SaveToFile(file);
		file.Close();
	}
#endif
}
