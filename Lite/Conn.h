// Conn.h: interface for the CConn class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCON_H__370AE7A1_4928_11D7_992C_0040F427D59A__INCLUDED_)
#define AFX_TCON_H__370AE7A1_4928_11D7_992C_0040F427D59A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTermView;

enum
{
	CONNECTED	= 1,
	CONNECTING	= 2,
	DISCONNECTED = 4,
	LOOKFORHOST	= 8,
	CANCEL		= 16,
	ANSI_EDITOR	= 32,
	WWW			= 128
};

class CConn
{
public:
	void ClearAllFlags();
	static CTermView* view;

BYTE is_telnet : 1;
BYTE is_web : 1;
BYTE is_ansi_editor : 1;
BYTE is_connected : 1;
BYTE is_connecting : 1;
BYTE is_disconnected : 1;
BYTE is_lookup_host : 1;
BYTE is_cancelled : 1;

	CString name;	//顯示在tab的名稱
	CString address;

	CConn();
	virtual ~CConn() = 0;
};

#endif // !defined(AFX_TCON_H__370AE7A1_4928_11D7_992C_0040F427D59A__INCLUDED_)
