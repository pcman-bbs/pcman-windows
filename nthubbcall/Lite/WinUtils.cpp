#include "WinUtils.h"

BOOL RegWndClass(LPSTR name, WNDPROC proc, HBRUSH bgcolor, HICON icon, HCURSOR cursor, DWORD style)
{
	WNDCLASS wcl;
	if (::GetClassInfo(AfxGetInstanceHandle(), name, &wcl))
		return TRUE;

	wcl.cbClsExtra = 0;
	wcl.cbWndExtra = 0;
	wcl.hbrBackground = bgcolor;
	wcl.hCursor = cursor;
	wcl.hIcon = icon;
	wcl.hInstance = AfxGetInstanceHandle();
	wcl.lpfnWndProc = proc;
	wcl.lpszClassName = name;
	wcl.lpszMenuName = NULL;
	wcl.style = style;

	return RegisterClass(&wcl);
}

