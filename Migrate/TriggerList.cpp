// TriggerList.cpp: implementation of the CTriggerList class.
//
//////////////////////////////////////////////////////////////////////

#include "TriggerList.h"
#include "StrUtils.h"
#include "AppConfig.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTriggerList::CTriggerList()
{
	pfirst=NULL;
	plast=NULL;
	count=0;
}

CTriggerList::~CTriggerList()
{
	RemoveAll();
}

CTriggerItem* CTriggerList::Add()
{
	CTriggerItem* item=new CTriggerItem;
	item->first=0;
	item->count=0;
	item->pnext=NULL;
	if(plast)
	{
		plast->pnext=item;
		plast=item;
	}
	else
	{
		pfirst=plast=item;
	}
	count++;
	return item;
}

void CTriggerList::RemoveAll()
{
	CTriggerItem* item=pfirst;
	while(item)
	{
		CTriggerItem* idel=item;
		item=item->pnext;
		delete idel;
	}
	pfirst=NULL;
	plast=NULL;
	count=0;
}


BOOL CTriggerList::LoadFromFile(CFile &file)
{
	//讀取字串觸發...
	CString tmp;
	RemoveAll();
	int c=0;
	file.Read(&c,sizeof(int));

	while(c)
	{
		tmp=LoadString(file);
		CTriggerItem* ntitem=Add();
		ntitem->msg=UnescapeControlChars(tmp);
		//讀取可能加密的字串
		DWORD l;
		file.Read(&l,sizeof(DWORD));	//	l=strlen(respond);
		char* tmpbuf=new char[l+1];
		file.Read(tmpbuf,l);
		if( *tmpbuf=='*' )	//如果有加密
		{
			l--;
/*
FIXME
			if(!crypto.IsInitialized())
			{
				if(!AppConfig.QueryPassword(FALSE))
					return FALSE;
				crypto.EnterPassword(AppConfig.passwd);
			}
			char* decbuf = new char[l+2];
			memset(decbuf,0,l+2);
			crypto.Decrypt(tmpbuf+1,decbuf,l);
			memcpy(tmpbuf+1,decbuf,l);
			delete []decbuf;

			char* ppasswd = tmpbuf+strlen(tmpbuf)+1;
			if( memcmp(LPCTSTR(AppConfig.passwd),ppasswd,AppConfig.passwd.GetLength()) )
			{
				AppConfig.passwd.Empty();
				delete []tmpbuf;
				return FALSE;
			}
			*ppasswd=0;
*/
		}
		else
			tmpbuf[l]=0;
		//------------讀取加密字串完畢
		ntitem->respond=tmpbuf;
		delete []tmpbuf;

		file.Read(&ntitem->first,sizeof(WORD));
		file.Read(&ntitem->count,sizeof(WORD));
		c--;
	}
	return TRUE;
}

void CTriggerList::SaveToFile(CFile &file)
{
#if 0
FIXME:

	file.Write(&count,sizeof(count));

	for(CTriggerItem* pi=pfirst;pi;pi=pi->pnext)
	{
		//儲存要偵測的字串
		SaveString(file,pi->msg);
		//儲存回應字串，視情況加密
		if(pi->respond[0]=='*')	//需要加密
		{
			if(!crypt.IsInitialized())
				crypt.EnterPassword(AppConfig.passwd);
			DWORD len=pi->respond.GetLength()+1;
			DWORD len2=len+AppConfig.passwd.GetLength();	//連密碼一起儲存
			len2 = (len2/16 + (len2%16?1:0))*16+1;
			file.Write(&len2,sizeof(len2));
			len2--;
			char* buf=new char[len2+2];
			memset(buf,0,len2);
			memcpy(buf,LPCTSTR(pi->respond),len);
			memcpy(buf+len,LPCTSTR(AppConfig.passwd),AppConfig.passwd.GetLength());
			char *buf2 = new char[len2+2];
			memset(buf2,0,len2+2);
			crypt.Encrypt(buf+1,buf2+1,len2);
			*buf2 = *buf;
			file.Write(buf2,len2+1);
			delete []buf2;
			delete []buf;
		}
		else
			SaveString(file,pi->respond);
		//儲存第一次開始觸發的次數
		file.Write(&pi->first,sizeof(WORD));
		//儲存總共要觸發的次數
		file.Write(&pi->count,sizeof(WORD));
	}
#endif
}

void CTriggerList::CopyFrom(CTriggerList &newval)
{
	RemoveAll();
	for(CTriggerItem* pi=newval.pfirst;pi;pi=pi->pnext)
	{
		CTriggerItem* ni=Add();
		ni->msg=pi->msg;
		ni->respond=pi->respond;
		ni->first=pi->first;
		ni->count=pi->count;
	}
}
