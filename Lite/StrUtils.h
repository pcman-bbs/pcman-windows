#if !defined(_MYSTRING_H_INCLUDED_)
#define _MYSTRING_H_INCLUDED_
#include <string.h>
//str[nr]str[i]說明
//r: 找最後一個
//n: 限制搜尋範圍從開頭到len長
//i: case insensitive
//case sensitive
char* strrstr(const char* str,const char* sub);
char* strnrstr(const char* str,const char* sub,int len);

//case insensitive
char* strrstri(const char* str,const char* sub);
char* strnrstri(const char* str,const char* sub,int len);

char* strstri(const char* str,const char* sub);
char* strnstri(const char* str,const char* sub,int len);

bool IsBig5(LPCTSTR str,LPCTSTR ppos);

inline int get_chw(LPCTSTR str)
{	return (*str>0 && *str<127)?1:2;	}

inline bool IsBig5(LPCTSTR str,int pos)
{
	return IsBig5(str,str+pos);
}

char* strstriptail(LPSTR str);

CString EscapeControlChars(LPCTSTR src);
CString UnescapeControlChars(LPCTSTR src);

#endif