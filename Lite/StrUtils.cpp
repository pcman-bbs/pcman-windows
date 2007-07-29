#include "stdafx.h"
#include "StrUtils.h"

//case sensitive
char* strrstr(const char* str, const char* sub)
{	return strnrstr(str, sub, strlen(str));	}

char* strnrstr(const char* str, const char* sub, int len)
{
	const char* pend = str + len;
	const char* pstr;
	int sl = strlen(sub);
	for (pstr = pend - sl; pstr >= str; pstr--)
	{
		if (!strncmp(pstr, sub, sl))
			return (char*)pstr;
	}
	return NULL;
}

//case insensitive
char* strrstri(const char* str, const char* sub)
{	return strnrstri(str, sub, strlen(str));	}

char* strnrstri(const char* str, const char* sub, int len)
{
	const char* pend = str + len;
	const char* pstr;
	int sl = strlen(sub);
	for (pstr = pend - sl; pstr >= str; pstr--)
	{
		if (!strnicmp(pstr, sub, sl))
			return (char*)pstr;
	}
	return NULL;
}

char* strstri(const char* str, const char* sub)
{	return strnstri(str, sub, strlen(str));	}

char* strnstri(const char* str, const char* sub, int len)
{
	const char* pend = str + len;
	const char* pstr;
	int sl = strlen(sub);
	for (pstr = str; pstr <= pend - sl; pstr++)
	{
		if (!strnicmp(pstr, sub, sl))
			return (char*)pstr;
	}
	return NULL;
}

bool IsBig5(LPCTSTR str, LPCTSTR ppos)
{
	if (ppos == str)
		return ((get_chw(str) == 2));

	int w;
	do
	{
		w = get_chw(str);
		str += w;
	}
	while (str < ppos);
	if (ppos == str)
		w = get_chw(str);
	return ((w == 2) && (ppos == str));
}

char* strstriptail(LPSTR str)
{
	int len = strlen(str);
	str += len - 1;
	while (*str == ' ')
		str--;
	str++;
	*str = 0;
	return str;
}


CString EscapeControlChars(LPCTSTR src)
{
	CString str2;
	for (; *src ; src++)
	{
		if (*src < 0x1f)
		{
			str2 += '^';
			str2 += char(*src + '@');
		}
		else if (*src == 0x7f)
			str2 += "\\x7f";
		else
			str2 += *src;
	}
	return str2;
}

CString UnescapeControlChars(LPCTSTR src)
{
	CString str2;
	while (*src)
	{
		if (*src == '^')
		{
			src++;
			if (*src != '^' && *src >= '@' && *src < '`')
				str2 += char(*src - '@');
			else
				str2 += '^';
			if (!*src)
				break;
			src++;
		}
		else if (*src == '\\')
		{
			src++;
			if (toupper(*src) == 'X')
			{
				src++;
				if (*src && *(src + 1))
				{
					BYTE d2 = toupper(*src);
					src++;
					BYTE d1 = toupper(*src);
					src++;
					d2 -= (isdigit(d2) ? '0' : 'A' -10);
					d1 -= (isdigit(d1) ? '0' : 'A' -10);
					str2 += char(d2 * 16 + d1);
				}
				else
					str2 += *src;
			}
			else
				str2 += '\\';
		}
		else
		{
			str2 += *src;
			if (get_chw(src) == 2)
			{
				src++;
				str2 += *src;
			}
			src++;
		}
	}
	return str2;
}

char* MultiByteToMultiByte(UINT srcCodePage, UINT destCodePage, LPCSTR srcStr, int* outLen)
{
	wchar_t *uniStr;
	char* destStr;
	int uniLen;
	int destLen;

	// convert an multi-byte to unicode(utf-16)
	uniLen = MultiByteToWideChar(srcCodePage, 0, srcStr, -1, NULL, NULL);
	uniStr = new WCHAR[uniLen];
	uniLen = MultiByteToWideChar(srcCodePage, 0, srcStr, -1, uniStr, uniLen);

	// convert back to utf-8
	destLen = WideCharToMultiByte(destCodePage, 0, uniStr, uniLen, NULL, NULL, NULL, NULL);
	destStr = new char[destLen];
	WideCharToMultiByte(destCodePage, 0, uniStr, uniLen, destStr, destLen, NULL, NULL);

	delete[] uniStr;
	if (outLen)
		*outLen = destLen;

	return destStr;
}


#if 0
char* MultiByteToUTF8(LPCSTR mbstr, int* outlen)
{
	wchar_t *uniStr;
	char* utf8Str;
	int uniLen;
	int utf8Len;

	// convert an multi-byte to unicode(utf-16)
	uniLen = MultiByteToWideChar(CP_ACP, 0, mbstr, -1, NULL, NULL);
	uniStr = new WCHAR[uniLen];
	uniLen = MultiByteToWideChar(CP_ACP, 0, mbstr, -1, uniStr, uniLen);

	// convert back to utf-8
	utf8Len = WideCharToMultiByte(CP_UTF8, 0, uniStr, uniLen, NULL, NULL, NULL, NULL);
	utf8Str = new char[utf8Len];
	WideCharToMultiByte(CP_UTF8, 0, uniStr, uniLen, utf8Str, utf8Len, NULL, NULL);

	delete[] uniStr;
	if (outlen)
		*outlen = utf8Len;

	return utf8Str;
}

char* UTF8ToMultiByte(LPCSTR utf8str, int* outlen)
{
	wchar_t *uniStr;
	char* mbStr;
	int uniLen;
	int mbLen;

	// convert an multi-byte to unicode(utf-16)
	uniLen = MultiByteToWideChar(CP_UTF8, 0, utf8str, -1, NULL, NULL);
	uniStr = new WCHAR[uniLen];
	uniLen = MultiByteToWideChar(CP_UTF8, 0, utf8str, -1, uniStr, uniLen);

	// convert back to utf-8
	mbLen = WideCharToMultiByte(CP_ACP, 0, uniStr, uniLen, NULL, NULL, NULL, NULL);
	mbStr = new char[mbLen];
	WideCharToMultiByte(CP_ACP, 0, uniStr, uniLen, mbStr, mbLen, NULL, NULL);
	delete[] uniStr;

	if (outlen)
		*outlen = mbLen;

	return mbStr;
}
#endif

const char* CharToHex(int ch, char* buffer)
{
	// sprintf is too time-consuming
	// sprintf( buffer, "%02X", ch );

	// NOTE: There is no check here due to performance consideration.
	//       The value of "ch" should not greater than 255.

	int d1 = (ch >> 4); // digit1 = ch / 16 = ch >> 4
	int d2 = (ch & 15); // digit2 = ch % 16 = ch & 15

	buffer[0] = '%';
	buffer[1] = d1 >= 10 ? (d1 - 10 + 'A') : (d1 + '0');
	buffer[2] = d2 >= 10 ? (d2 - 10 + 'A') : (d2 + '0');
	buffer[3] = '\0';

	return buffer;
}

// the content of the source string will be modified!!
char* strnextline(char* line)
{
	char *eol = line, *next_line = NULL;
	while (*eol && *eol != '\r' && *eol != '\n')
		++eol;
	if (*eol)      // end of line, EOF not yet reached
	{
		next_line = eol;
		// skip all CR/LF
		while (*next_line == '\r' || *next_line == '\n')
			++next_line;
		*eol = '\0';
	}
	else // end of config file reached
		next_line = NULL;
	return next_line;
}

static char _int_to_base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
							   "abcdefghijklmnopqrstuvwxyz"
							   "0123456789+/=";

static char _base64_to_int[] =
{
	62, 64, 64, 64, 63,	/* +,-./ */
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61,	/* 0 - 9 */
	64, 64, 64, 64, 64, 64, 64,  /* :;<=>?@ */
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, /* A - Z */
	64, 64, 64, 64, 64, 64,  /* [\]^_` */
	26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51 /* a - z */
};

static int int_to_base64(int val)
{
	if (val < 0 || val > 64)
		return '=';
	return _int_to_base64[val];
}

static int base64_to_int(int val)
{
	if (val > 'z' || val < '+')
		return 64;
	return _base64_to_int[ val - '+' ];
}

// If out == NULL, the returned value is the required length of output buffer.
// Otherwise, the returned value is the number of written bytes.
UINT Base64Encode(BYTE* in, UINT inlen, BYTE* out, UINT outlen)
{
// FIXME: The performancfe of this API should be improved.
	if (out == NULL)
		return (inlen / 3 + inlen % 3) * 4;
	outlen -= outlen % 4;
	int pad = inlen % 3;
	BYTE* _out = out;
	BYTE* oend = out + outlen;
	BYTE* iend = in + inlen - pad;
	BYTE i1, i2, i3, b1, b2, b3, b4;
	for (; in < iend && out < oend; in += 3, out += 4)
	{
		i1 = in[0];
		i2 = in[1];
		i3 = in[2];

		b1 = i1 >> 2;
		b2 = ((i1 & 3) << 4) | (i2 >> 4);
		b3 = ((i2 & 15) << 2) | (i3 >> 6);
		b4 = i3 & 63;

		out[0] = int_to_base64(b1);
		out[1] = int_to_base64(b2);
		out[2] = int_to_base64(b3);
		out[3] = int_to_base64(b4);
	}

	if (pad > 0 && (out + 3) < oend)
	{
		i1 = in[0];
		out[0] = int_to_base64(i1 >> 2);
		if (pad == 1)
		{
			b2 = ((i1 & 3) << 4);
			out[1] = int_to_base64(b2);
			out[2] = out[3] = '=';
		}
		else	// pad == 2
		{
			i2 = in[1];
			b2 = ((i1 & 3) << 4) | (i2 >> 4);
			b3 = ((i2 & 15) << 2);
			out[1] = int_to_base64(b2);
			out[2] = int_to_base64(b3);
			out[3] = '=';
		}
		out += 4;
	}
	return out - _out;
}

// If out == NULL, the returned value is the required length of output buffer.
// Otherwise, the returned value is the number of written bytes.
UINT Base64Decode(BYTE* in, UINT inlen, BYTE* out, UINT outlen)
{
// FIXME: The performancfe of this API should be improved.
	if (out == NULL)
		return inlen / 4 * 3;
	inlen -= inlen % 4; // inlen should be devided by 4, or this is an invalid base64 string
	outlen -= outlen % 3; // inlen should be devided by 3
	BYTE* iend = in + inlen;
	BYTE* oend = out + outlen;
	BYTE* _out = out;
	int b1, b2, b3, b4;
	int pad = 0;
	while (in[inlen - 1 - pad] == '=')
		++pad;
	for (; in < iend && out < oend; in += 4, out += 3)
	{
		b1 = base64_to_int(in[0]);
		b2 = base64_to_int(in[1]);
		b3 = base64_to_int(in[2]);
		b4 = base64_to_int(in[3]);
		out[0] = (b1 << 2) | (b2 >> 4);
		out[1] = (b2 << 4) | (b3 >> 2);
		out[2] = (b3 << 6) | b4;
	}
	for (; pad > 0; --pad)	// removing padding bytes
		--out;
	return out - _out;
}

/*
// Convert a string to md5 hash string
// out should be at least 33 bytes (include '\0' at tail) in length.
char* StrToMD5( const char* in, int len, char* out )
{
	char* _out = out;
	md5_byte_t md5[16];
	md5_state_t md5_state;
	md5_init( &md5_state );
	md5_append( &md5_state, ( md5_byte_t * ) in, len );
	md5_finish( &md5_state, md5 );

	for( int i = 0; i < 16; ++i, out+= 2 )
	{
		// convert to hex string presentation (See CharToHex)
		int d1 = (md5[i] >> 4); // digit1 = md5[i] / 16 = md5[i] >> 4
		int d2 = (md5[i] & 15); // digit2 = md5[i] % 16 = md5[i] & 15
		out[0] = d1 >= 10 ? (d1 - 10 + 'A') : (d1 + '0');
		out[1] = d2 >= 10 ? (d2 - 10 + 'A') : (d2 + '0');
	}
	*out = '\0';
	return _out;
}
*/
