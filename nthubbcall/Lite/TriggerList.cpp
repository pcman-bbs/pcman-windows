// TriggerList.cpp: implementation of the CTriggerList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcman.h"
#include "TriggerList.h"
#include "AES.h"
#include "StrUtils.h"

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

static void SetTriggerItem(CTriggerItem& item, char* line, CAES& crypto)
{
	char* eq = strchr(line, '=');
	if (!eq)
		return;
	*eq = '\0';
	char* key = line;
	char* val = eq + 1;
	if (0 == strcmp(key, "first"))
		item.first = atoi(val);
	else if (0 == strcmp(key, "count"))
		item.count = atoi(val);
	else if (0 == strcmp(key, "recv"))
		item.msg = UnescapeControlChars(val);
	else if (0 == strcmp(key, "send"))
	{
		if (*val == '+')	// encrypted string
		{
			++val;
			if (!crypto.IsInitialized())
			{
				if (!AppConfig.QueryPassword(FALSE))
					return;
				crypto.EnterPassword(AppConfig.passwd);
			}
			int base64_len = strlen(val);
			int enc_len = Base64Decode((BYTE*)val, base64_len, NULL, 0);
			char* enc_buf = new char[enc_len];
			enc_len = Base64Decode((BYTE*)val, base64_len, (BYTE*)enc_buf, enc_len);
			char* dec_buf = new char[enc_len + 2];
			memset(dec_buf, 0, enc_len + 2);
			crypto.Decrypt(enc_buf, dec_buf, enc_len);
			delete enc_buf;
			int dec_len = strlen(dec_buf);
			char* ppasswd = dec_buf + dec_len + 1;
			if (memcmp(LPCTSTR(AppConfig.passwd), ppasswd, AppConfig.passwd.GetLength()))
			{
				AppConfig.passwd.Empty();
				delete []dec_buf;
				return;
			}
			item.respond = '+';
			item.respond += dec_buf;
			delete []dec_buf;
		}
		else
		{
			item.respond = val;
		}
	}
}

void CTriggerList::Load(char* section)
{
	SKIP_SPACES(section);
	if (*section)
	{
		CAES crypto;

		RemoveAll();
		char* line = section;
		char* nextline = NULL;
		for (; line; line = nextline)
		{
			nextline = strnextline(line);
			// format of each trigger item:
			// <item>
			// first=int
			// count=int
			// recv=str
			// send=str
			//   If str[0] == '-', it's a normal string.
			//   If str[0] == '+', it's a base64 encoded binary string
			// </item>
			SKIP_SPACES(line);
			if (strcmp(line, "<item>"))
				continue;

			CTriggerItem* item = Add();
			while ((line = nextline))
			{
				nextline = strnextline(line);
				if (0 == strcmp(line, "</item>"))
					break;
				SetTriggerItem(*item, line, crypto);
			}
		}
	}

}

void CTriggerList::Save(CString& section)
{
	CAES crypto;
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
		if (item->respond[0] == '+')	// encrypted
		{
			section += '+';
			if (!crypto.IsInitialized())
				crypto.EnterPassword(AppConfig.passwd);

			int len = item->respond.GetLength();
			int dec_len = len + AppConfig.passwd.GetLength() + 1;	//連密碼一起儲存
			dec_len = (dec_len / 16 + (dec_len % 16 ? 1 : 0)) * 16;
			char* dec_buf = new char[ dec_len ];
			memset(dec_buf, 0, dec_len);
			memcpy(dec_buf, LPCTSTR(item->respond) + 1, len);
			memcpy(dec_buf + len, LPCTSTR(AppConfig.passwd), AppConfig.passwd.GetLength() + 1);
			char* enc_buf = new char[ dec_len ];
			memset(enc_buf, 0, dec_len);
			crypto.Encrypt(dec_buf, enc_buf, dec_len);
			delete []dec_buf;

			int base64_len = Base64Encode((BYTE*)enc_buf, dec_len, NULL, 0);
			char* base64 = new char[ base64_len + 1 ];
			base64_len = Base64Encode((BYTE*)enc_buf, dec_len, (BYTE*)base64, base64_len);
			base64[ base64_len ] = '\0';
			delete []enc_buf;

			section += base64;
			delete base64;
		}
		else
		{
			section += item->respond;
		}
		section += "\r\n</item>\r\n\r\n";
	}
}

