// WindowState.h: interface for the CWindowState class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINDOWSTATE_H__4A8B54BE_6159_427E_B8B6_BB13F737E828__INCLUDED_)
#define AFX_WINDOWSTATE_H__4A8B54BE_6159_427E_B8B6_BB13F737E828__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ConfigFile.h"

class CWindowState : public CConfigFile::ConfigHandler
{
public:
	int showcmd;
	RECT rect;
	void Restore(HWND hwnd);
	void Save(HWND hwnd);

	CWindowState();
	virtual ~CWindowState();

	void Load(char* value)
	{
		sscanf(value, "%d,%d,%d,%d,%d", &rect.left,
			   &rect.top, &rect.right, &rect.bottom, &showcmd);
	}
	void Save(CString& value)
	{
		value.Format("%d,%d,%d,%d,%d",
					 rect.left,
					 rect.top,
					 rect.right,
					 rect.bottom,
					 showcmd);
	}
};

#endif // !defined(AFX_WINDOWSTATE_H__4A8B54BE_6159_427E_B8B6_BB13F737E828__INCLUDED_)
