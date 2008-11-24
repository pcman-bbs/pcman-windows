// ChiConv.h: interface for the CChiConv class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHICONV_H__6D4B7931_4162_4FE1_8060_F5B2ADE116EB__INCLUDED_)
#define AFX_CHICONV_H__6D4B7931_4162_4FE1_8060_F5B2ADE116EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afx.h>

class CChiConv
{
public:
	void Release();
	void AddRef(){m_Ref++;}
	const unsigned char* GetBig52GBTab();
	const unsigned char* GetGB2Big5Tab();
	void Big52GB(const char *text_in, char *text_out, int len);
	void GB2Big5(const char *text_in, char *text_out, int len);
	CChiConv();
	virtual ~CChiConv();
	void SetTablePath(CString tab_dir);

protected:
	CString m_TabDirPath;
	unsigned char* m_Big52GB_Tab;
	unsigned char* m_GB2Big5_Tab;
	static int m_Ref;
};

#endif // !defined(AFX_CHICONV_H__6D4B7931_4162_4FE1_8060_F5B2ADE116EB__INCLUDED_)
