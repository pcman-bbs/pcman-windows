#pragma once

//#define USE_DEBUG_CONSOLE

#ifdef USE_DEBUG_CONSOLE
BOOL DebugConsole_Init(int nMode);
DWORD DebugConsole_printfA(const char*format, ...);
DWORD DebugConsole_printfW(const wchar_t*format, ...);

#define DBG_INIT DebugConsole_Init
#define DBG_PRINT DebugConsole_printfA
#else
#define DBG_INIT
#define DBG_PRINT
#endif
