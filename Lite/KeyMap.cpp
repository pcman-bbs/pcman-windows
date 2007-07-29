// KeyMap.cpp: implementation of the CKeyMap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcman.h"
#include "KeyMap.h"
#include "TermView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

char CKeyMap::default_kmname[12] = "Default";
/*
const char* CKeyMap::default_left	="\x1bOD\x1bOD";
const char* CKeyMap::default_right	="\x1bOC\x1bOC";
const char* CKeyMap::default_up		="\x1bOA";
const char* CKeyMap::default_down	="\x1bOB";
*/

CKeyMap* CKeyMap::FindMap(LPCTSTR kmname)
{
	for (int i = 0; i < CTermView::all_telnet_conns.GetSize(); i++)
	{
		CConn* pcon = (CConn*)CTermView::all_telnet_conns[i];
		if (!pcon->is_ansi_editor && !pcon->is_web)	//確定是BBS站台才用Keymap
		{
			if (((CTelnetConn*)pcon)->key_map)
				if (strncmp(((CTelnetConn*)pcon)->key_map->name, kmname, 11) == 0)
					return ((CTelnetConn*)pcon)->key_map;
		}
	}
	return NULL;
}

CKeyMap* CKeyMap::Load(LPCTSTR kmname)
{
	CKeyMap* pkm = FindMap(kmname);
	if (pkm)	//如果已經載入
	{
		pkm->ref_count++;
		return pkm;
	}

	pkm = new CKeyMap;
	if (!pkm)
		return NULL;
	pkm->ref_count = 1;
	CFile kmf;
	if (kmf.Open(GetKeyMapDir() + kmname, CFile::modeRead)
		|| kmf.Open(GetKeyMapDir() + default_kmname, CFile::modeRead))
	{
		WORD sz = 0;
		kmf.Read(&sz, sizeof(sz));
		pkm->SetSize(sz);
		kmf.Read(pkm->GetData(), sz* sizeof(CKeyMapEntry));
		kmf.Close();
		pkm->FreeExtra();
	}
	strncpy(pkm->name, kmname, 12);
	return pkm;
}

void CKeyMap::Save()
{
	CFile kmf;
	if (kmf.Open(GetKeyMapDir() + name, CFile::modeWrite | CFile::modeCreate))
	{
		WORD sz = GetSize();
		kmf.Write(&sz, sizeof(sz));
		kmf.Write(GetData(), sz* sizeof(CKeyMapEntry));
		kmf.Close();
	}
}

void CKeyMap::Release()
{
	ref_count--;
	if (!ref_count)
		delete this;
}

BOOL CKeyMap::DelMap(LPCTSTR kmname)
{
	if (CKeyMap::FindMap(kmname))
		return FALSE;
	return DeleteFile(GetKeyMapDir() + kmname);
}

BOOL CKeyMap::ReName(LPCTSTR oldname, LPCTSTR newname)
{
	CKeyMap* pmap = FindMap(oldname);
	BOOL r = MoveFile(GetKeyMapDir() + oldname, GetKeyMapDir() + newname);
	if (r && pmap)
		strncpy(pmap->name, newname, 12);
	return r;
}

CKeyMap::CKeyMap()
{
	// FIXME: File name of keymap should be in English
//	::LoadString( AfxGetResourceHandle(), IDS_DEFAULT_KEYMAP, default_kmname, sizeof(default_kmname) );
}
