#include <windows.h>

inline BOOL IsFileExist(LPCTSTR path)
{
	GetFileAttributes(path);
	return GetLastError()!=2;
}