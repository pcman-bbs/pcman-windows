// TriggerList.h: interface for the CTriggerList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRIGGERLIST_H__D3AB6861_3446_11D7_A5E5_EE1034247E3B__INCLUDED_)
#define AFX_TRIGGERLIST_H__D3AB6861_3446_11D7_A5E5_EE1034247E3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include "ConfigFile.h"

class CTriggerItem
{
public:
	CString msg;
	CString respond;
	WORD first;
	WORD count;
	CTriggerItem* pnext;
};

class CTelnetConnDelayedSend
{
public:
	CString str;
	int time;
};

class CTriggerList : public CConfigFile::ConfigHandler
{
public:
	CTriggerItem* pfirst;
	CTriggerItem* plast;
	int count;
	void RemoveAll();
	CTriggerItem* Add();
	CTriggerList();
	virtual ~CTriggerList();
	void CopyFrom(CTriggerList &newval);

	virtual void Load(char* section);
	virtual void Save(CString& section);
};

#endif // !defined(AFX_TRIGGERLIST_H__D3AB6861_3446_11D7_A5E5_EE1034247E3B__INCLUDED_)
