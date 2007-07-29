#ifndef _WIN_UTILS_H_
#define _WIN_UTILS_H_
#include <afxwin.h>

BOOL RegWndClass(LPSTR name, WNDPROC proc, HBRUSH bgcolor = NULL, HICON icon = NULL, HCURSOR cursor = NULL, DWORD style = CS_HREDRAW | CS_VREDRAW);

inline BOOL IsWinNT(){ return (GetVersion() < 0x80000000);}

#endif
