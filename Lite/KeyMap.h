// KeyMap.h: interface for the CKeyMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KEYMAP_H__AB4BBFDD_49D9_4629_BD90_B0E9D6F55B80__INCLUDED_)
#define AFX_KEYMAP_H__AB4BBFDD_49D9_4629_BD90_B0E9D6F55B80__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>

#include "HotKeyEdit.h"

extern CString AppPath;
extern CString ConfigPath;

const int max_kms = 13;
struct CKeyMapEntry
{
	WORD key;
	BYTE fVirt;
	char str[max_kms];
};

class CKeyMap : public CArray<CKeyMapEntry, CKeyMapEntry&>
{
public:
	CKeyMap();
	inline static CString GetKeyMapDir();
	static BOOL ReName(LPCTSTR oldname, LPCTSTR newname);
	static BOOL DelMap(LPCTSTR kmname);
	static char default_kmname[];
	/*
		const static char* default_left;
		const static char* default_right;
		const static char* default_up;
		const static char* default_down;
	*/
	void Release();
	void Save();
	static CKeyMap* Load(LPCTSTR kmname);
	static CKeyMap* FindMap(LPCTSTR kmname);
	int ref_count;
	char name[12];
	const char* FindKey(WORD key, UINT nFlags);

};

inline const char* CKeyMap::FindKey(WORD key, UINT nFlags)
{
	BYTE fVirt = 0;
	if (!(nFlags &(1 << 8)))	//如果不是extended，檢查是不是NumPad
	{
		if ((key >= VK_PRIOR && key <= VK_DOWN) || key == VK_INSERT || key == VK_DELETE)
			fVirt |= FNUMPAD;
	}

	if (HIBYTE(GetKeyState(VK_SHIFT)))
		fVirt |= FSHIFT;
	if (HIBYTE(GetKeyState(VK_CONTROL)))
		fVirt |= FCONTROL;

	CKeyMapEntry* ent = GetData();
	CKeyMapEntry* end_ent;
	for (end_ent = ent + GetSize(); ent != end_ent ; ent++)
		if (ent->key == key && (ent->fVirt&(FSHIFT | FCONTROL | FNUMPAD)) == fVirt)
			return ent->str;
	return NULL;
}

inline CString CKeyMap::GetKeyMapDir()
{
	return ::AppPath + "keyboard\\";
}

#endif // !defined(AFX_KEYMAP_H__AB4BBFDD_49D9_4629_BD90_B0E9D6F55B80__INCLUDED_)
