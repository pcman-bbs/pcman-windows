// CUcs2Conv.cpp: implementation of the CUcs2Conv class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "Ucs2Conv.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUcs2Conv::CUcs2Conv()
{
	m_Big52Ucs2_Tab = NULL;
	m_Ucs22Big5_Tab = NULL;
	m_Ucs2TabSize = 0;
}

CUcs2Conv::~CUcs2Conv()
{
	if (m_Big52Ucs2_Tab)
		delete []m_Big52Ucs2_Tab;
	if (m_Ucs22Big5_Tab)
		delete []m_Ucs22Big5_Tab;
}

void CUcs2Conv::Big52Ucs2(const char* text_in, wchar_t* text_out)
{
	Big52Ucs2(text_in, text_out, strlen(text_in));
}

void CUcs2Conv::Ucs22Big5(const wchar_t* text_in, char* text_out)
{
	for (; *text_in; text_in++, text_out++)
	{
		if (*text_in >= 0x81)
		{
			// Use binary search to locate the char in the table.
			wchar_t ucs2 = 0;
			int upper = m_Ucs2TabSize / 4;
			int lower = 0;
			int index = -1;
			while ((upper - lower) >= 2)
			{
				index = lower + (upper - lower) / 2;
				ucs2 = *(wchar_t*) & m_Ucs22Big5_Tab[ index * 4 ];
				if (*text_in < ucs2)
					upper = index;
				else if (*text_in > ucs2)
					lower = index;
				else
					break;
			}
			if (*text_in == ucs2 && index >= 0)
			{
				text_out[0] = m_Ucs22Big5_Tab[ index * 4 + 2 ];
				text_out[1] = m_Ucs22Big5_Tab[ index * 4 + 3 ];
			}
			else
			{
				::WideCharToMultiByte(950, 0, text_in, 1, text_out, 2, "\xff\xfd", NULL);
			}
			++text_out;
		}
		else
		{
			*text_out = (char) * text_in;
		}
	}
}

size_t CUcs2Conv::Big52Ucs2(const char* text_in, wchar_t* text_out, int len)
{
	const int SEG_LEN = (0x7E-0x40 + 1) + (0xFE - 0xA1 + 1);
	const wchar_t* pstart = text_out;

	for (int i = 0; i < len; ++text_in, ++text_out, ++i)
	{
		unsigned char ch1 = BYTE(*text_in), ch2 = BYTE(*(text_in + 1));
		//If these 2 bytes are within the range of Big5
		if (ch1 >= 0x81 && ch1 <= 0xFE && ch2 >= 0x40 && ch2 <= 0xFE)
		{
			int index = -1;
			if (ch1 >= 0x81 && ch1 <= 0xA0)	// 0x8140 - 0xA0FE
			{
				// ch2 should be within 0x40 ~ 0x7E or 0x A1 ~ 0xFE
				if (ch2 <= 0x7E)
				{
					index = (ch1 - 0x81) * SEG_LEN + (ch2 - 0x40);
					index *= 2;
				}
				else if (ch2 >= 0xA1)
				{
					index = (ch1 - 0x81) * SEG_LEN + (0x7E-0x40 + 1) + (ch2 - 0xA1);
					index *= 2;
				}
			}
			else if (ch1 >= 0xC6 && ch1 <= 0xC8)	// 0xC640 - 0xC8FE
			{
				// ch2 should be within 0x40 ~ 0x7E or 0x A1 ~ 0xFE
				if (ch2 <= 0x7E)
				{
					index = ((ch1 - 0xC6) + (0xA0 - 0x81 + 1)) * SEG_LEN + (ch2 - 0x40);
					index *= 2;
				}
				else if (ch2 >= 0xA1)
				{
					index = ((ch1 - 0xC6) + (0xA0 - 0x81 + 1)) * SEG_LEN + (0x7E-0x40 + 1) + (ch2 - 0xA1);
					index *= 2;
				}
			}
			else if (ch1 >= 0xFA && ch1 <= 0xFE)	// 0xFA40 - 0xFEFE
			{
				// ch2 should be within 0x40 ~ 0x7E or 0x A1 ~ 0xFE
				if (ch2 <= 0x7E)
				{
					index = ((ch1 - 0xFA) + (0xA0 - 0x81 + 1) + (0xC8 - 0xC6 + 1)) * SEG_LEN + (ch2 - 0x40);
					index *= 2;
				}
				else if (ch2 >= 0xA1)
				{
					index = ((ch1 - 0xFA) + (0xA0 - 0x81 + 1) + (0xC8 - 0xC6 + 1)) * SEG_LEN + (0x7E-0x40 + 1) + (ch2 - 0xA1);
					index *= 2;
				}
			}

			if (index >= 0)
			{
				//*text_out = m_Big52Ucs2_Tab[index] | m_Big52Ucs2_Tab[index+1] << 8;
				*text_out = *(wchar_t*) & m_Big52Ucs2_Tab[index];
			}
			else
			{
				::MultiByteToWideChar(950, 0, text_in, 2, text_out, 1);
			}
			text_in++;
			++i;
		}
		//邏輯上可能有點問題，這裡把 Big5 範圍之外的編碼一律視為 ascii碼
		else
		{
			//在大部分情況中，這邊是被切掉低位元組的 DBCS Big5 中文字
			if (ch1 >= 0x81)
			{
				*text_out = 0x3f;
			}
			//這邊是正常的 ascii
			else
			{
				*text_out = *text_in;
			}
		}
	}
	return text_out - pstart;
}

BOOL CUcs2Conv::InitBig52Ucs2Tab()
{
	CFile file;
	long size;
	if (file.Open(m_TabDirPath + "B2U", CFile::modeRead)
		&& (size = file.GetLength()) <= B2U_TAB_SIZE)
	{
		m_Big52Ucs2_Tab = new unsigned char[size];
		file.Read(m_Big52Ucs2_Tab, size);
		return TRUE;
	}
	else
	{
		m_Big52Ucs2_Tab = NULL;
		return FALSE;
	}
}

BOOL CUcs2Conv::InitUcs22Big5Tab()
{
	CFile file;
	if (file.Open(m_TabDirPath + "U2B", CFile::modeRead)
		&& (m_Ucs2TabSize = file.GetLength()) <= U2B_TAB_SIZE)
	{
		m_Ucs22Big5_Tab = new unsigned char[m_Ucs2TabSize];
		file.Read(m_Ucs22Big5_Tab, m_Ucs2TabSize);
		return TRUE;
	}
	else
	{
		m_Ucs2TabSize = 0;
		m_Ucs22Big5_Tab = NULL;
		return FALSE;
	}
}

void CUcs2Conv::SetTablePath(CString tab_dir)
{
	m_TabDirPath = tab_dir;
}

