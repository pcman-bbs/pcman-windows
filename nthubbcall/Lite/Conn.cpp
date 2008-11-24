// Conn.cpp: implementation of the CConn class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcman.h"
#include "Conn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTermView* CConn::view = NULL;

CConn::CConn()
{
	ClearAllFlags();
}

CConn::~CConn()
{

}

void CConn::ClearAllFlags()
{
	is_ansi_editor = false;
	is_cancelled = false;
	is_connected = false;
	is_connecting = false;
	is_disconnected = false;
	is_web = false;
	is_lookup_host = false;
	is_telnet = false;
}
