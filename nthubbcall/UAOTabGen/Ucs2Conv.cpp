// CUcs2Conv.cpp: implementation of the CUcs2Conv class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <io.h>	// For _filelength, which is Microsoft specific
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
	//InitBig52Ucs2Tab();
	//InitUcs22Big5Tab();
}

CUcs2Conv::~CUcs2Conv()
{

}

void CUcs2Conv::Big52Ucs2(const char* text_in, wchar_t* text_out)
{
	const int SEG_LEN = (0x7E-0x40) + (0xFE - 0xA1) + 2;//
	const int _XX = 0xA1 - 0x7E- 1;//

	for (; *text_in; text_in++, text_out++)
	{
		unsigned char ch1, ch2;
		if ((ch1 = BYTE(*text_in)) >= 0x81 && ch1 <= 0xFE && (ch2 = BYTE(*(text_in + 1))) >= 0x40)
		{

			int segment = (ch1 - 0x81) * SEG_LEN + 1;
			int offset = (ch2 < 0xa1 ? ch2 - 0x40 : ch2 - 0x40 - _XX);
			int index = 2 * (segment + offset) - 2 ;

			*text_out = m_Big52Ucs2_Tab[index] << 8 | m_Big52Ucs2_Tab[index+1];
			text_in++;
		}

		//邏輯上可能有點問題，這裡把 Big5 範圍之外的編碼一律視為 ascii碼
		else
		{
			*text_out = *text_in;
		}
	}
}

void CUcs2Conv::Ucs22Big5(const wchar_t* text_in, char* text_out)
{

	for (; *text_in; text_in++, text_out++)
	{
		if (*text_in >= 0x81)
		{
			int index = 2 * (*text_in - 0x81);

			*text_out = m_Ucs22Big5_Tab[index];
			text_out++;
			*text_out = m_Ucs22Big5_Tab[index+1];
		}

		else
		{
			*text_out = (char) * text_in;
		}
	}
}

size_t CUcs2Conv::Big52Ucs2(const char* text_in, wchar_t* text_out, int len)
{
	const int SEG_LEN = (0x7E-0x40) + (0xFE - 0xA1) + 2;//
	const int _XX = 0xA1 - 0x7E- 1;//
	wchar_t* pstart = text_out;

	for (int i = 0; i < len; ++text_in, ++text_out, ++i)
	{
		unsigned char ch1, ch2;
		if ((ch1 = BYTE(*text_in)) >= 0x81 && (ch2 = BYTE(*(text_in + 1))) >= 0x40)
		{
			int index = 2 * (
							((ch1 - 0x81) * SEG_LEN + 1) +
							(ch2 < 0xa1 ? ch2 - 0x40 : ch2 - 0x40 - _XX)
						) - 2 ;

			*text_out = (m_Big52Ucs2_Tab[index] << 8) | m_Big52Ucs2_Tab[index+1];
			++text_in;
			++i;
		}

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
	FILE* file = fopen("_B2U", "rb");
	if (file)
	{
		fread(m_Big52Ucs2_Tab, 1, _filelength(_fileno(file)), file);
		fclose(file);
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CUcs2Conv::InitUcs22Big5Tab()
{
	FILE* file = fopen("_U2B", "rb");
	if (file)
	{
		fread(m_Ucs22Big5_Tab, 1, _filelength(_fileno(file)), file);
		fclose(file);
		return TRUE;
	}
	else
		return FALSE;
}

