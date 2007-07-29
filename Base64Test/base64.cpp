#include <afxwin.h>
#include "..\Lite\StrUtils.h"
#include <stdio.h>

int main()
{
	char* in = "This is a simple base64 encoding test!\n這是個簡單的 Base64 編碼測試!";
	int inlen = strlen(in);
	int outlen = Base64Encode((BYTE*)in, inlen, NULL, 0);
	char* out = new char[outlen + 1];
	memset(out, 'X', outlen);
	outlen = Base64Encode((BYTE*)in, inlen, (BYTE*)out, outlen);
	out[outlen] = 0;
	printf("encoded: %s\n", out);

	in = out;
	inlen = strlen(in);
	outlen = Base64Decode((BYTE*)in, inlen, NULL, 0);
	out = new char[outlen + 1];
	memset(out, 'X', outlen);
	outlen = Base64Decode((BYTE*)in, inlen, (BYTE*)out, outlen);
	out[outlen] = 0;
	printf("decoded: %s\n", out);
	delete in;
	delete out;
	getchar();
	return 0;
}
