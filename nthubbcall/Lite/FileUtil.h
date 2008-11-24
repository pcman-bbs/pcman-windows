#ifndef _FILE_UTIL_H_INCLUDED_
#define _FILE_UTIL_H_INCLUDED_

#include <windows.h>

inline BOOL IsFileExist(LPCTSTR path)
{
	return GetFileAttributes(path) != -1;
}

#endif
