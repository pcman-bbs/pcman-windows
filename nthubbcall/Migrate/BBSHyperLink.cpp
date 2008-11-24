// BBSHyperLink.cpp: implementation of the CBBSHyperLink class.
//
//////////////////////////////////////////////////////////////////////

#include "AppConfig.h"
#include "BBSHyperLink.h"
#include "StrUtils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
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
	email = -1;
	WORD c = 0;
	file.Read(&c, sizeof(c));
	links.SetSize(c);
	for (int i = 0;i < c;i++)
	{
		links[i].scheme = LoadString(file);
		if (links[i].scheme == '@')
			email = i;
		links[i].program = LoadString(file);
		file.Read(&links[i].color, sizeof(links[i].color));
	}
}

void CBBSHyperLink::Save(CFile &file)
{
	CString section = "[HyperLink]\r\n";
	for (int i = 0, c = links.GetSize(); i < c; ++i)
	{
		section += links[i].scheme;
		section += '=';
		section += links[i].program;
		section += '|';
		char color_str[32];
		COLORREF color = links[i].color;
		sprintf(color_str, "%d,%d,%d\r\n",
				GetRValue(color), GetGValue(color), GetBValue(color));
		section += color_str;
	}
	file.Write(LPCTSTR(section), section.GetLength());
	/*
		WORD c=links.GetSize();
		file.Write(&c,sizeof(c));
		for(int i=0;i<c;i++)
		{
			SaveString(file,links[i].scheme);
			SaveString(file,links[i].program);
			file.Write(&links[i].color,sizeof(links[i].color));
		}
	*/
}

/*
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
*/

void CBBSHyperLink::Default()
{
	links.RemoveAll();
	CBBSHyperLinkData d;
	d.color = 0x0066ff;
	d.scheme = "http";
	links.Add(d);
	d.scheme = "https";
	links.Add(d);
	d.scheme = "mms";
	links.Add(d);
	d.scheme = "rtsp";
	links.Add(d);
	d.scheme = "ed2k";
	links.Add(d);
	d.scheme = "telnet";
	d.color = 0xffff00;
	links.Add(d);
	d.scheme = "ftp";
	d.color = 0x00ff00;
	links.Add(d);

//	E-mail為內建型態，要進階處理
	d.scheme = "@";
	d.color = 0xff0080;
	email = links.Add(d);

}


