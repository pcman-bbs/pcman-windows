// AES.cpp: implementation of the CAES class.
//
//////////////////////////////////////////////////////////////////////

#include "AES.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAES::CAES() : m_bInit(false)
{

}

CAES::~CAES()
{

}


void CAES::EnterPassword(CString passwd)
{
	int len = (passwd.GetLength() / 16 + (passwd.GetLength() % 16 ? 1 : 0)) * 16;
	char pad[32];
	memset(pad, 0, len);
	strcpy(pad, passwd);
	MakeKey(pad, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 16);
	m_bInit = true;
}
