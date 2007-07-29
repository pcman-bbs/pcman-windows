// BBSHyperLink.h: interface for the CBBSHyperLink class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BBSHYPERLINK_H__72C7C203_C3E5_450D_90C9_B1DF68A0DE65__INCLUDED_)
#define AFX_BBSHYPERLINK_H__72C7C203_C3E5_450D_90C9_B1DF68A0DE65__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afx.h>
#include <Afxtempl.h>

struct CBBSHyperLinkData
{
	CString scheme;
	CString program;
	DWORD color;
};

class CBBSHyperLink
{
public:
	void Default();
	CArray<CBBSHyperLinkData, CBBSHyperLinkData&> links;
	int email;
	void Save(CFile& file);
	void Load(CFile& file);
	CBBSHyperLink();
};

#endif // !defined(AFX_BBSHYPERLINK_H__72C7C203_C3E5_450D_90C9_B1DF68A0DE65__INCLUDED_)
