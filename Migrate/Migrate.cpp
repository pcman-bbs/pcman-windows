#include <afxwin.h>

#include <windows.h>
#include "AppConfig.h"
#include "SiteSettings.h"

/*
int WINAPI WinMain( HINSTANCE hinst, HINSTANCE hprev, LPSTR lpcmd, int showcmd )
{

	return 0;
}
*/

inline BOOL IsFileExist(LPCTSTR path)
{
	return GetFileAttributes(path) != -1;
}

class CApp : public CWinApp
{
public:
	BOOL InitInstance();
}theApp;

CString ConfigPath;
BOOL IsCombo = FALSE;

BOOL CApp::InitInstance()
{
	CString AppPath;

	::GetModuleFileName(AfxGetInstanceHandle(),AppPath.GetBuffer(MAX_PATH),_MAX_PATH);
	AppPath.ReleaseBuffer();
	AppPath=AppPath.Left(AppPath.ReverseFind('\\')+1);
	ConfigPath = AppPath + CONFIG_DIR;

	CAppConfig cfg;
	CString OldConfigPath = AppPath + "Config.2004\\";

	// 識別是 Combo or Lite 版本
	if( IsFileExist( ConfigPath + "Webbar.bmp" ) || 
		IsFileExist( ConfigPath + WWW_ADFILTER_FILENAME ) || 
		IsFileExist( ConfigPath + WWW_FAVORITE_FILENAME ) )
		IsCombo = TRUE;

	cfg.BackupConfig( ConfigPath, OldConfigPath );

	cfg.Load( OldConfigPath + CONFIG_FILENAME );

	cfg.Save( ConfigPath + "Config.ini" );

	// Favorites
	CopyFile( OldConfigPath + BBS_FAVORITE_FILENAME, ConfigPath + BBS_FAVORITE_FILENAME, TRUE );
	if( IsCombo )
		CopyFile( OldConfigPath + WWW_FAVORITE_FILENAME, ConfigPath + WWW_FAVORITE_FILENAME, TRUE );

	// Toolbars
	CopyFile( OldConfigPath + TOOLBAR_BMP_FILENAME, ConfigPath + TOOLBAR_BMP_FILENAME, TRUE );
	CopyFile( OldConfigPath + ICON_BMP_FILENAME, ConfigPath + ICON_BMP_FILENAME, TRUE );
	if( IsCombo )
		CopyFile( OldConfigPath + "Webbar.bmp", ConfigPath + "Webbar.bmp", TRUE );

	// History
	CFile f;
	if(  f.Open( OldConfigPath + HISTORY_FILENAME, CFile::modeRead )  )
	{
		cfg.LoadHistory(f);
		f.Close();

		if(  f.Open( ConfigPath + HISTORY_FILENAME, CFile::modeWrite|CFile::modeCreate )  )
		{
			cfg.SaveHistory(f);
			f.Close();
		}
	}

	// Homepage
	if(  f.Open( OldConfigPath + HOMEPAGE_FILENAME, CFile::modeRead )  )
	{
		CFile fo;
		if( fo.Open( ConfigPath + HOMEPAGE_FILENAME, CFile::modeWrite|CFile::modeCreate ) )
		{
			CArchive ar( &f, CArchive::load );
			CString str;
			while( ar.ReadString(str) )
			{
				int p = str.Find( '\t' );
				if( p > 0 )
				{
					p = str.Find( '\t', p + 1 );
					if( p > 0 )
						str = str.Left( p );
				}
				str += "\r\n";
				fo.Write( LPCTSTR(str), str.GetLength() );
			}
			fo.Close();

			ar.Close();
		}
		f.Close();
	}

	// ExitLog / Session
	if(  f.Open( OldConfigPath + "ExitLog", CFile::modeRead )  )
	{
		CFile fo;
		if( fo.Open( ConfigPath + "Session", CFile::modeWrite|CFile::modeCreate ) )
		{
			CString str;
			while( ! (str = LoadString( f )).IsEmpty() )
			{
				if( str[0] == 's' )
				{
					int p = str.Find( '\t' );
					if( p > 0 )
					{
						p = str.Find( '\t', p + 1 );
						if( p > 0 )
						{
							CString cfg = str.Mid( p + 1 );
							str = str.Left( p );
							if( 0 == strnicmp( cfg, ConfigPath, ConfigPath.GetLength() ) )
							{
								cfg = cfg.Mid( ConfigPath.GetLength() );
								p = cfg.ReverseFind( ';' );
								if( p > 0 )
								{
									cfg = cfg.Left( p );
									cfg.Replace( ';', '\\' );
									cfg += '\\';
									str += '\t';
									str += cfg;
								}
							}
						}
					}
				}
				str += "\r\n";
				fo.Write( LPCTSTR(str), str.GetLength() );
			}
			fo.Close();
		}
		f.Close();
	}

	// FUS
	CopyFile( OldConfigPath + "FUS", ConfigPath + "FUS", TRUE );

	// UI, FIXME: old UI file shouldn't be used in new programs
	CopyFile( OldConfigPath + "UI", ConfigPath + "UI", TRUE );

#if 0	// import old UI
	if(  f.Open(OldConfigPath + "UI", CFile::modeRead)  )
	{
		WORD c;
		f.Read( &c, 2 );
		ACCEL* acc = new ACCEL[c];
		f.Read( acc, c * sizeof(ACCEL) );
		f.Close();

		CArray<ACCEL, ACCEL&> accels;
		accels.SetSize( c );
		int i;
		for( i = 0; i < c; ++i )
			accels[i] = acc[i];
		delete acc;
		acc = NULL;

		BYTE* data = NULL;
		DWORD rest = 0;
		if( f.Open( ConfigPath + "UI2", CFile::modeRead ) )
		{
			f.Read( &c, sizeof(c) );
			acc = new ACCEL[c];
			f.Read( acc, c * sizeof(ACCEL) );
			rest = f.GetLength() - (c * sizeof(ACCEL));
			data = new BYTE[rest];
			f.Read( data, rest * sizeof(BYTE) );
			f.Close();
		}

		if( acc && data )
		{
			for( i = 0; i < c; ++i )
			{
				int j;
				for( j = 0; j < accels.GetSize(); ++j )
				{
					// 在舊版 UI 裡面有設定此命令的熱鍵
					if(	accels[j].cmd == acc[i].cmd )
						break;
				}
				if( j >= accels.GetSize() )
				{
					// 舊版 UI 內沒有此命令，表示為 2007 新增的預設熱鍵
					ACCEL new_acc = acc[i];
					for( i = 0; i < accels.GetSize(); ++i )
					{
						// 在舊版 UI 裡面此熱鍵已被使用
						if(	accels[j].fVirt == acc[i].fVirt 
							&& accels[j].key == acc[i].key )
							break;
					}

					// 舊版 UI 中並未使用此熱鍵
					if( i >= accels.GetSize() )
						accels.Add( new_acc );	// 加入此熱鍵
				}
			}
			if( f.Open( ConfigPath + "UI", CFile::modeWrite|CFile::modeCreate ) )
			{
				f.Write( accels.GetData(), accels.GetSize() * sizeof(ACCEL) );
				f.Write( data, rest );
				f.Close();
			}
			delete []acc;
			delete []data;
		}
	}
#endif	// import old UI

	// per-site settings
	CFileFind finder;
	BOOL found = finder.FindFile( OldConfigPath + "*;*" );
	while( found )
	{
		found = finder.FindNextFile();
		CString src = finder.GetFilePath();
		CString dest = finder.GetFileTitle();
		dest.Replace(";", "__");
		dest = ConfigPath + dest;
		dest += ".ini";
		CSiteSettings site;
		if( site.Load( src ) )
			site.Save( dest );
	}

	// AD Filter
	if( IsCombo && f.Open( OldConfigPath + WWW_ADFILTER_FILENAME, CFile::modeRead ) )
	{
		DWORD c;
		f.Read(&c, sizeof(c));
		if( c > 0 )
		{
			CFile fo;
			if(fo.Open(ConfigPath + WWW_ADFILTER_FILENAME, CFile::modeWrite|CFile::modeCreate))
			{
				for( int i = 0; i < c; ++i )
				{
					CString line = LoadString( f );
					fo.Write( LPCTSTR( line ), line.GetLength() );
					fo.Write( "\r\n", 2 );
				}
				fo.Close();
			}
		}
		f.Close();
	}

	return CWinApp::InitInstance();
}
