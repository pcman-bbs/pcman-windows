// BBSHyperLink.cpp: implementation of the CBBSHyperLink class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcman.h"
#include "BBSHyperLink.h"
#include "StrUtils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBBSHyperLink::CBBSHyperLink()
{
}

void CBBSHyperLink::Load(CFile &file)
{
	email=-1;
	WORD c=0;
	file.Read(&c,sizeof(c));
	links.SetSize(c);
	for(int i=0;i<c;i++)
	{
		links[i].scheme=LoadString(file);
		if(links[i].scheme=='@')
			email=i;
		links[i].program=LoadString(file);
		file.Read(&links[i].color,sizeof(links[i].color));
	}
}

void CBBSHyperLink::Save(CFile &file)
{
	WORD c=links.GetSize();
	file.Write(&c,sizeof(c));
	for(int i=0;i<c;i++)
	{
		SaveString(file,links[i].scheme);
		SaveString(file,links[i].program);
		file.Write(&links[i].color,sizeof(links[i].color));
	}
}

//	const char URL_unsafe[]="{}|\\^~[]`%#\"<>";
const char URL_safe[]="!$&'()*+,-./:;=?@_|~%#";
inline bool IsURLChar(int ch)
{
	return ch>0 && (isalnum(ch) || strchr(URL_safe,ch));
}

const char URL_scheme_safe[]="+-.";
inline bool IsURLSchemeChar(int ch)
{
	return ch>0 && (isalnum(ch) || strchr(URL_scheme_safe,ch));
}

const char* CBBSHyperLink::FindEMailLink(const char *src, int &len) const
{
	const char* plink=NULL;
	while( *src  )
	{
		while( *src && (*src=='@' || !IsURLChar(*src)) )
			src+=get_chw(src);
		plink=src;

		while( IsURLChar(*src) && *src!='@' )
			src++;
		if( *src=='@' && plink!=src)
		{
			const char* pend=src;	bool has_dot=false;
			while( IsURLChar(*pend) )
			{
				if( *pend=='.' )
					has_dot=true;
				pend++;
			}
			if(pend > src && has_dot && *(pend-1) != '.' )
			{
				len=int(pend)-int(plink);
				return plink;
			}
		}
	}
	return NULL;
}

const char* CBBSHyperLink::FindHyperLink(const char *src, int &len) const
{
	const char* pemail=FindEMailLink(src,len);
	const char* plink=NULL;
	while( *src  )
	{
		while( *src && !IsURLSchemeChar(*src))
			src+=get_chw(src);
		plink=src;

		while( IsURLSchemeChar(*src) )
			src++;
		if( strncmp(src,"://",3)==0 )
		{
			const char* pend=src;
			while( IsURLChar(*pend) )
				pend++;
			if(pend > src )
			{
				if(pemail && pemail < plink)
					return pemail;

				//檢查是否為已知連結
				for(int i=0;i<links.GetSize();i++)
				{
					int scheme_len=links[i].scheme.GetLength();
					const char* _plink=src-scheme_len;
					if( _plink >= plink && strnicmp(_plink,links[i].scheme,scheme_len)==0 )
					{
						plink=_plink;
						break;
					}
				}
				len=int(pend)-int(plink);
				return plink;
			}
		}
	}
	return pemail;
}


void CBBSHyperLink::OpenURL(LPCTSTR url)
{
	//查詢是否有設定超連結開啟程式，如果沒有，用預設程式開啟
	int t=GetURLType(url);
	CString mailto;
	LPCTSTR path=url;
	LPCTSTR param=NULL;

	//檢查是不是e-mail
	if( t>=0 )
	{
		if( t==email )	//如果是E-mail
		{
			mailto="mailto:";
			mailto+=url;
			path=mailto;
		}
		else if(!links[t].program.IsEmpty() )
		{
			path=links[t].program;
			param=url;
		}
	}
	if( strnicmp(url,"telnet",6) ==0 )
		((CMainFrame*)AfxGetMainWnd())->OnNewConnectionAds(url);
	else
	{
#ifdef _COMBO_
		if( path == url )	//如果是預設程式開啟，而且不是E-mail
			((CMainFrame*)AfxGetApp()->m_pMainWnd)->view.ConnectWeb(url, AppConfig.link_autoswitch);
		else
#endif
			::ShellExecute(AfxGetMainWnd()->m_hWnd,"open",path,param,NULL, 
						AppConfig.link_autoswitch ? SW_SHOWMAXIMIZED : SW_SHOWMINIMIZED);
	}
}

void CBBSHyperLink::Default()
{
	links.RemoveAll();
	CBBSHyperLinkData d;
	d.color=0x0066ff;
	d.scheme="http";
	links.Add(d);
	d.scheme="https";
	links.Add(d);
	d.scheme="mms";
	links.Add(d);
	d.scheme="rtsp";
	links.Add(d);
	d.scheme="ed2k";
	links.Add(d);
	d.scheme="telnet";
	d.color=0xffff00;
	links.Add(d);
	d.scheme="ftp";
	d.color=0x00ff00;
	links.Add(d);

//	E-mail為內建型態，要進階處理
	d.scheme="@";
	d.color=0xff0080;
	email=links.Add(d);

}


int CBBSHyperLink::GetURLType(const char *url)
{
	CBBSHyperLinkData* plinks=links.GetData();
	//檢查是不是e-mail
	const char* psymbol=strstr(url,"://");
	if( !psymbol && strchr(url,'@'))	//如果是E-mail
		return email;

	int len=int(psymbol)-int(url);
	for(int i=0;i<links.GetSize();i++)
	{
		if( strnicmp(plinks[i].scheme,url,len)==0 )	//如果有偵測到
			break;
	}

	return i<links.GetSize() ? i:-1;
}

