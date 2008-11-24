// WebConn.h: interface for the CWebConn class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CWebConn_H__6CA20F68_1FDA_11D6_8461_9C9E15C10000__INCLUDED_)
#define AFX_CWebConn_H__6CA20F68_1FDA_11D6_8461_9C9E15C10000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../Lite/Conn.h"
#include "webbrowser.h"
#include <afxtempl.h>

class CWebConn : public CConn
{
public:
	void ClearAllFlags()
	{
		CConn::ClearAllFlags();
		is_web = true;
	}

	CWebBrowser web_browser;

	CWebConn();
	virtual ~CWebConn();

};

#endif // !defined(AFX_CWebConn_H__6CA20F68_1FDA_11D6_8461_9C9E15C10000__INCLUDED_)
