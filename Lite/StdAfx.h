// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__EF878A28_6FEC_11D5_A2FD_D45914C10000__INCLUDED_)
#define AFX_STDAFX_H__EF878A28_6FEC_11D5_A2FD_D45914C10000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#define _WIN32_WINNT  0x0400

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include"rfc854.h"

extern CString AppPath;
extern CString ConfigPath;

inline CString LoadString(UINT id)
{
	CString s;
	s.LoadString(id);
	return s;
}

#include "FileUtil.h"
#include <atlbase.h>

#include "..\Resource\resource.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__EF878A28_6FEC_11D5_A2FD_D45914C10000__INCLUDED_)
