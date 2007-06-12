// stdafx.cpp : source file that includes just the standard includes
//	PCMan4.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#ifdef USE_DEBUG_CONSOLE

static HANDLE m_hStdHandle = INVALID_HANDLE_VALUE;

BOOL DebugConsole_Init(int nMode)
{
	if( AllocConsole() ) 
	{
		m_hStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		return TRUE;
    }
	return FALSE;
}



DWORD DebugConsole_printf(const char*format, ... )
{
    va_list args;
    DWORD len;
    char buff[8192];
    va_start( args, format );
	
    len = _vsnprintf( buff, sizeof(buff), format, args );
	
    if(m_hStdHandle != INVALID_HANDLE_VALUE) 
	{
        WriteFile( m_hStdHandle, buff, len, &len, NULL );
    }
	
    return len;
}

#endif