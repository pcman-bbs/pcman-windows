// BBSHyperLink.h: interface for the CBBSHyperLink class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BBSHYPERLINK_H__72C7C203_C3E5_450D_90C9_B1DF68A0DE65__INCLUDED_)
#define AFX_BBSHYPERLINK_H__72C7C203_C3E5_450D_90C9_B1DF68A0DE65__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ConfigFile.h"

struct CBBSHyperLinkData
{
	CString scheme;
	CString program;
	COLORREF color;
};

class CBBSHyperLink : public CConfigFile::ConfigHandler
{
public:
	int GetURLType(const char* url);
	void Default();
	CArray<CBBSHyperLinkData, CBBSHyperLinkData&> links;
	int email;
	void OpenURL(LPCTSTR url);
	void Save(CString& section);
	void Load(char* section);
	CBBSHyperLink();
	const char* FindEMailLink(const char *src, int &len) const;
	const char* FindHyperLink(const char *src, int &len) const;
};

#endif // !defined(AFX_BBSHYPERLINK_H__72C7C203_C3E5_450D_90C9_B1DF68A0DE65__INCLUDED_)
