#if !defined(_MYSTRING_H_INCLUDED_)
#define _MYSTRING_H_INCLUDED_
#include <string.h>
//str[nr]str[i]說明
//r: 找最後一個
//n: 限制搜尋範圍從開頭到len長
//i: case insensitive
//case sensitive
char* strrstr(const char* str, const char* sub);
char* strnrstr(const char* str, const char* sub, int len);

//case insensitive
char* strrstri(const char* str, const char* sub);
char* strnrstri(const char* str, const char* sub, int len);

char* strstri(const char* str, const char* sub);
char* strnstri(const char* str, const char* sub, int len);

bool IsBig5(LPCTSTR str, LPCTSTR ppos);

inline int get_chw(LPCTSTR str)
{	return (*str > 0 && *str < 127) ? 1 : 2;	}

inline bool IsBig5(LPCTSTR str, int pos)
{
	return IsBig5(str, str + pos);
}

char* strstriptail(LPSTR str);

char* strnextline(char* line);

char* MultiByteToMultiByte(UINT srcCodePage, UINT destCodePage, LPCSTR mbstr, int* outlen = NULL);
#define MultiByteToUTF8(str, outlen) MultiByteToMultiByte(CP_ACP, CP_UTF8, str, outlen)
#define UTF8ToMultiByte(str, outlen) MultiByteToMultiByte(CP_UTF8, CP_ACP, str, outlen)
/*
char* MultiByteToUTF8(LPCSTR mbstr, int* outlen = NULL);
char* UTF8ToMultiByte(LPCSTR utf8str, int* outlen = NULL);
*/

CString EscapeControlChars(LPCTSTR src);
CString UnescapeControlChars(LPCTSTR src);

const char* CharToHex(int ch, char* buffer);

#define SKIP_SPACES_AND_CRLF(str)	while(*str && strchr(" \t\r\n", *str))	++str;
#define SKIP_SPACES(str)	while(*str && (*str == ' ' || *str =='\t') )	++str;

// Base64 manipulating routines:
// If out == NULL, the returned value is the required length of output buffer.
// Otherwise, the returned value is the number of written bytes.
UINT Base64Encode(BYTE* in, UINT inlen, BYTE* out, UINT outlen);
UINT Base64Decode(BYTE* in, UINT inlen, BYTE* out, UINT outlen);

// Convert a string to md5 hash string
// out should be at least 33 bytes (include '\0' at tail) in length.
// char* StrToMD5( const char* in, int len, char* out );

#endif
