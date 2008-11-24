// TriggerList.cpp: implementation of the CTriggerList class.
//
//////////////////////////////////////////////////////////////////////

#include "TriggerList.h"
#include "StrUtils.h"
#include "AppConfig.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTriggerList::CTriggerList()
{
	pfirst = NULL;
	plast = NULL;
	count = 0;
}

CTriggerList::~CTriggerList()
{
	RemoveAll();
}

CTriggerItem* CTriggerList::Add()
{
	CTriggerItem* item = new CTriggerItem;
	item->first = 0;
	item->count = 0;
	item->pnext = NULL;
	if (plast)
	{
		plast->pnext = item;
		plast = item;
	}
	else
	{
		pfirst = plast = item;
	}
	count++;
	return item;
}

void CTriggerList::RemoveAll()
{
	CTriggerItem* item = pfirst;
	while (item)
	{
		CTriggerItem* idel = item;
		item = item->pnext;
		delete idel;
	}
	pfirst = NULL;
	plast = NULL;
	count = 0;
}


BOOL CTriggerList::LoadFromFile(CFile &file)
{
	//讀取字串觸發...
	CString tmp;
	RemoveAll();
	int c = 0;
	file.Read(&c, sizeof(int));

	while (c)
	{
		CTriggerItem* ntitem = Add();
		ntitem->msg = LoadString(file);
		//讀取可能加密的字串
		DWORD l;
		file.Read(&l, sizeof(DWORD));	//	l=strlen(respond);
		char* tmpbuf = new char[l+1];
		file.Read(tmpbuf, l);
		if (*tmpbuf == '*')	//如果有加密
		{
			l--;
			int len = Base64Encode((BYTE*)tmpbuf + 1, l, NULL, 0);
			char* base64 = ntitem->respond.GetBuffer(len + 2);
			base64[0] = '+';
			len = Base64Encode((BYTE*)tmpbuf + 1, l, (BYTE*)base64 + 1, len);
			base64[len + 1] = '\0';
			ntitem->respond.ReleaseBuffer();
		}
		else
		{
			tmpbuf[0] = '-';
			ntitem->respond = tmpbuf;
		}
		delete []tmpbuf;

		file.Read(&ntitem->first, sizeof(WORD));
		file.Read(&ntitem->count, sizeof(WORD));
		c--;
	}
	return TRUE;
}

void CTriggerList::SaveToFile(CFile &file)
{
	CString section;
	for (CTriggerItem* item = pfirst; item; item = item->pnext)
	{
		char buf[16];
		section += "<item>\r\n";
		section += "first=";
		sprintf(buf, "%d\r\n", item->first);
		section += buf;

		section += "count=";
		sprintf(buf, "%d\r\n", item->count);
		section += buf;

		section += "recv=";
		section += item->msg;
		section += "\r\n";

		section += "send=";
		if ('+' == *LPCTSTR(item->respond))
			section += item->respond;
		else
			section += EscapeControlChars(item->respond);

		section += "\r\n</item>\r\n\r\n";
	}

	file.Write(LPCTSTR(section), section.GetLength());
}

void CTriggerList::CopyFrom(CTriggerList &newval)
{
	RemoveAll();
	for (CTriggerItem* pi = newval.pfirst;pi;pi = pi->pnext)
	{
		CTriggerItem* ni = Add();
		ni->msg = pi->msg;
		ni->respond = pi->respond;
		ni->first = pi->first;
		ni->count = pi->count;
	}
}
