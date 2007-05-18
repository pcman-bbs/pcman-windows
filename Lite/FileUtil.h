#include <windows.h>

inline BOOL IsFileExist(LPCTSTR path)
{
	return GetFileAttributes(path) != -1;
}