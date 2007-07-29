// BBSHyperLink.cpp: implementation of the CBBSHyperLink class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcman.h"
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

void CBBSHyperLink::Load(char* section)
{
	SKIP_SPACES(section);
	if (*section)
	{
		links.RemoveAll();
		char* line = strtok(section, "\r\n");
		for (; line; line = strtok(NULL, "\r\n"))
		{
			// format of each line: schema=program|color
			CBBSHyperLinkData data;
			char* eq = strchr(line, '=');
			if (!eq)
				continue;
			*eq = '\0';
			data.scheme = line;
			line = eq + 1;
			eq = strchr(line, '|');
			if (!eq)
				continue;
			*eq = '\0';
			data.program = line;
			line = eq + 1;
			int r, g, b;
			if (sscanf(line, "%d,%d,%d", &r, &g, &b) < 3)
				continue;
			data.color = RGB(r, g, b);
			int i = links.Add(data);
			if (links[i].scheme == '@')
				email = i;
		}
	}
}

void CBBSHyperLink::Save(CString& section)
{
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
}

//	const char URL_unsafe[]="{}|\\^~[]`%#\"<>";
const char URL_safe[] = "!$&'()*+,-./:;=?@_|~%#";
inline bool IsURLChar(int ch)
{
	return ch > 0 && (isalnum(ch) || strchr(URL_safe, ch));
}

const char URL_scheme_safe[] = "+-.";
inline bool IsURLSchemeChar(int ch)
{
	return ch > 0 && (isalnum(ch) || strchr(URL_scheme_safe, ch));
}

const char* CBBSHyperLink::FindEMailLink(const char *src, int &len) const
{
	const char* plink = NULL;
	while (*src)
	{
		while (*src && (*src == '@' || !IsURLChar(*src)))
			src += get_chw(src);
		plink = src;

		while (IsURLChar(*src) && *src != '@')
			src++;
		if (*src == '@' && plink != src)
		{
			const char* pend = src;	bool has_dot = false;
			while (IsURLChar(*pend))
			{
				if (*pend == '.')
					has_dot = true;
				pend++;
			}
			if (pend > src && has_dot && *(pend - 1) != '.')
			{
				len = int(pend) - int(plink);
				return plink;
			}
		}
	}
	return NULL;
}

const char* CBBSHyperLink::FindHyperLink(const char *src, int &len) const
{
	const char* pemail = FindEMailLink(src, len);
	const char* plink = NULL;
	while (*src)
	{
		while (*src && !IsURLSchemeChar(*src))
			src += get_chw(src);
		plink = src;

		while (IsURLSchemeChar(*src))
			src++;
		if (strncmp(src, "://", 3) == 0)
		{
			const char* pend = src;
			while (IsURLChar(*pend))
				pend++;
			if (pend > src)
			{
				if (pemail && pemail < plink)
					return pemail;

				//檢查是否為已知連結
				for (int i = 0;i < links.GetSize();i++)
				{
					int scheme_len = links[i].scheme.GetLength();
					const char* _plink = src - scheme_len;
					if (_plink >= plink && strnicmp(_plink, links[i].scheme, scheme_len) == 0)
					{
						plink = _plink;
						break;
					}
				}
				len = int(pend) - int(plink);
				return plink;
			}
		}
	}
	return pemail;
}


void CBBSHyperLink::OpenURL(LPCTSTR url)
{
	//查詢是否有設定超連結開啟程式，如果沒有，用預設程式開啟
	int t = GetURLType(url);
	CString mailto;
	LPCTSTR path = url;
	LPCTSTR param = NULL;

	//檢查是不是e-mail
	if (t >= 0)
	{
		if (t == email)	//如果是E-mail
		{
			mailto = "mailto:";
			mailto += url;
			path = mailto;
		}
		else if (!links[t].program.IsEmpty())
		{
			path = links[t].program;
			param = url;
		}
	}
	if (strnicmp(url, "telnet", 6) == 0)
		((CMainFrame*)AfxGetMainWnd())->OnNewConnectionAds(url);
	else
	{
#ifdef _COMBO_
		if (path == url)	//如果是預設程式開啟，而且不是E-mail
			((CMainFrame*)AfxGetApp()->m_pMainWnd)->view.ConnectWeb(url, AppConfig.link_autoswitch);
		else
#endif
			::ShellExecute(AfxGetMainWnd()->m_hWnd, "open", path, param, NULL,
						   AppConfig.link_autoswitch ? SW_SHOWMAXIMIZED : SW_SHOWMINIMIZED);
	}
}

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


int CBBSHyperLink::GetURLType(const char *url)
{
	CBBSHyperLinkData* plinks = links.GetData();
	//檢查是不是e-mail
	const char* psymbol = strstr(url, "://");
	if (!psymbol && strchr(url, '@'))	//如果是E-mail
		return email;

	int len = int(psymbol) - int(url);
	int i;
	for (i = 0;i < links.GetSize(); ++i)
	{
		if (strnicmp(plinks[i].scheme, url, len) == 0)	//如果有偵測到
			break;
	}

	return i < links.GetSize() ? i : -1;
}

