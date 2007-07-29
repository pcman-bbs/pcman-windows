// WebConn.cpp: implementation of the CWebConn class.
//
//////////////////////////////////////////////////////////////////////

#include "../Lite/stdafx.h"
#include "../Lite/pcman.h"
#include "../Lite/MainFrm.h"
#include "WebConn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWebConn::CWebConn()
{
	web_browser.web_conn = this;
	is_web = true;
}

CWebConn::~CWebConn()
{
	web_browser.wb_ctrl.Stop();
	LockWindowUpdate(web_browser.m_hWnd);
	web_browser.wb_ctrl.DestroyWindow();
	LockWindowUpdate(web_browser.m_hWnd);
	web_browser.DestroyWindow();
}
