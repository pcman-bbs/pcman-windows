#include "stdafx.h"
#include "StrUtils.h"

char* strrstr(const char* str,const char* sub)
{	return strnrstr(str,sub,strlen(str));	}

char* strrstri(const char* str,const char* sub)
{	return strnrstri(str,sub,strlen(str));	}

char* strnrstr(const char* str,const char* sub,int len)
{
	const char* pend=str+len;
	const char* pstr;
	int sl=strlen(sub);
	for( pstr=pend-sl; pstr>=str; pstr--)
	{
		if(!strncmp(pstr,sub,sl))
			return (char*)pstr;
	}
	return NULL;
}

char* strnrstri(const char* str,const char* sub,int len)
{
	const char* pend=str+len;
	const char* pstr;
	int sl=strlen(sub);
	for( pstr=pend-sl; pstr>=str; pstr--)
	{
		if(!strnicmp(pstr,sub,sl))
			return (char*)pstr;
	}
	return NULL;
}


//case insensitive
char* strstri(const char* str,const char* sub)
{	return strnstri(str,sub,strlen(str));	}

char* strnstri(const char* str,const char* sub,int len)
{
	const char* pend=str+len;
	const char* pstr;
	int sl=strlen(sub);
	for( pstr=pend-sl; pstr>=str; pstr--)
	{
		if(!strnicmp(pstr,sub,sl))
			return (char*)pstr;
	}
	return NULL;
}

bool IsBig5(LPCTSTR str,LPCTSTR ppos)
{
	if(ppos==str)
		return ((get_chw(str)==2));

	int w;
	do
	{
		w=get_chw(str);
		str+=w;
	}
	while(str<ppos);
	if(ppos==str)
		w=get_chw(str);
	return ((w==2)&&(ppos==str));
}

char* strstriptail(LPSTR str)
{
	int len=strlen(str);
	str+=len-1;
	while(*str==' ')
		str--;
	str++;
	*str=0;
	return str;
}


CString EscapeControlChars(LPCTSTR src)
{
	CString str2;
	for( ; *src ; src++)
	{
		if(*src<0x1f)
		{
			str2+='^';
			str2+=char(*src+'@');
		}
		else if(*src==0x7f)
			str2+="\\x7f";
		else
			str2+=*src;
	}
	return str2;
}

CString UnescapeControlChars(LPCTSTR src)
{
	CString str2;
	while(*src)
	{
		if(*src=='^')
		{
			src++;
			if(*src != '^' && *src>='@' && *src< '`' )
				str2+=char(*src-'@');
			else
				str2+='^';
			if(!*src)
				break;
			src++;
		}
		else if(*src=='\\')
		{
			src++;
			if(toupper(*src)=='X')
			{
				src++;
				if( *src && *(src+1))
				{
					BYTE d2=toupper(*src);
					src++;
					BYTE d1=toupper(*src);
					src++;
					d2-= (isdigit(d2)?'0':'A'-10);
					d1-= (isdigit(d1)?'0':'A'-10);
					str2+=char(d2*16+d1);
				}
				else
					str2+=*src;
			}
			else
				str2+='\\';
		}
		else
		{
			str2+=*src;
			if( get_chw(src)==2 )
			{
				src++;
				str2+=*src;
			}
			src++;
		}
	}
	return str2;
}

