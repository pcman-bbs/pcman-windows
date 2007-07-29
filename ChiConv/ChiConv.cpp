// ChiConv.cpp: implementation of the CChiConv class.
//
//////////////////////////////////////////////////////////////////////

#include "ChiConv.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChiConv::CChiConv()
{
	m_Big52GB_Tab = NULL;
	m_GB2Big5_Tab = NULL;
}

CChiConv::~CChiConv()
{
	if (m_Big52GB_Tab)
		delete []m_Big52GB_Tab;
	if (m_GB2Big5_Tab)
		delete []m_GB2Big5_Tab;
}

void CChiConv::Big52GB(const char *text_in, char *text_out, int len)
{
	const unsigned char* tab = GetBig52GBTab();
	if (!tab)
		return;
	for (int i = 0; i < len; i++)
	{
		unsigned char ch1, ch2;
		if ((ch1 = BYTE(text_in[i])) >= 0xA1 && ch1 <= 0xF9 && (ch2 = BYTE(text_in[i+1])) >= 0x40)
		{
			// This is a valid Big5 character
			int index = 4 * ((ch1 - 0xA1) * (0xFF - 0x40 + 1) + (ch2 - 0x40));
			text_out[i] = tab[index+2];
			text_out[i+1] = tab[index+3];
			i++;
		}
		else
			text_out[i] = text_in[i];
	}
}

void CChiConv::GB2Big5(const char *text_in, char *text_out, int len)
{
	const unsigned char* tab = GetGB2Big5Tab();
	if (!tab)
		return;
	for (int i = 0; i < len; i++)
	{
		unsigned char ch1, ch2;
		if ((ch1 = BYTE(text_in[i])) >= 0xA1 && ch1 <= 0xF0 && (ch2 = BYTE(text_in[i+1])) >= 0xA0)
		{
			// This is a valid Big5 character
			int index = 4 * ((ch1 - 0xA1) * (0xFF - 0xA0 + 1) + (ch2 - 0xA0));
			text_out[i] = tab[index+2];
			text_out[i+1] = tab[index+3];
			i++;
		}
		else
			text_out[i] = text_in[i];
	}
}

const unsigned char* CChiConv::GetBig52GBTab()
{
	if (!m_Big52GB_Tab)
	{
		CFile tab;
		if (tab.Open(m_TabDirPath + "Big52GB", CFile::modeRead))
		{
			size_t len = tab.GetLength();
			m_Big52GB_Tab = (unsigned char*)new char[len];
			tab.Read(m_Big52GB_Tab, len);
			tab.Close();
		}
	}
	return m_Big52GB_Tab;
}

const unsigned char* CChiConv::GetGB2Big5Tab()
{
	if (!m_GB2Big5_Tab)
	{
		CFile tab;
		if (tab.Open(m_TabDirPath + "GB2Big5", CFile::modeRead))
		{
			size_t len = tab.GetLength();
			m_GB2Big5_Tab = (unsigned char*)new char[len];
			tab.Read(m_GB2Big5_Tab, len);
			tab.Close();
		}
	}
	return m_GB2Big5_Tab;
}

void CChiConv::SetTablePath(CString tab_dir)
{
	m_TabDirPath = tab_dir;
}

int CChiConv::m_Ref = 0;

void CChiConv::Release()
{
	m_Ref--;
	if (0 == m_Ref)
	{
		if (m_Big52GB_Tab)
		{
			delete []m_Big52GB_Tab;
			m_Big52GB_Tab = NULL;
		}
		if (m_GB2Big5_Tab)
		{
			delete []m_GB2Big5_Tab;
			m_GB2Big5_Tab = NULL;
		}
	}
}
