#include <windows.h>
#include <stdio.h>
#include "Debug_Kit.h"

#ifdef USE_DEBUG_CONSOLE

static HANDLE m_hStdHandle = INVALID_HANDLE_VALUE;

BOOL DebugConsole_Init(int nMode)
{
	if (AllocConsole())
	{
		m_hStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		return TRUE;
	}
	return FALSE;
}



DWORD DebugConsole_printfA(const char*format, ...)
{
	va_list args;
	DWORD len;
	char buff[8192];
	va_start(args, format);

	len = _vsnprintf(buff, sizeof(buff), format, args);

	if (m_hStdHandle != INVALID_HANDLE_VALUE)
	{
		WriteFile(m_hStdHandle, buff, len, &len, NULL);
	}

	return len;
}

DWORD DebugConsole_printfW(const wchar_t*format, ...)
{
	va_list args;
	DWORD len;
	wchar_t buff[8192];
	char *pforateA;
	DWORD dw1;

	va_start(args, format);

	len = _vsnwprintf(buff, sizeof(buff) / sizeof(wchar_t), format, args);

	dw1 = WideCharToMultiByte(0, 0, buff, -1, 0, 0, 0, 0);
	pforateA = (char*)LocalAlloc(LPTR, dw1 + 1);

	WideCharToMultiByte(0, 0, buff, -1, pforateA, dw1 + 1, 0, 0);


	if (m_hStdHandle != INVALID_HANDLE_VALUE)
	{
		WriteFile(m_hStdHandle, pforateA, len, &len, NULL);
	}

	LocalFree(pforateA);

	return len;
}

#endif