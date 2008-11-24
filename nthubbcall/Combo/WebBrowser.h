#if !defined(AFX_WEBBROWSER_H__26BE6502_23AB_11D6_8461_AC4F70CD0000__INCLUDED_)
#define AFX_WEBBROWSER_H__26BE6502_23AB_11D6_8461_AC4F70CD0000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WebBrowser.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWebBrowser html view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#include <afxhtml.h>
#include <tlogstg.h>
#include "webbrowserctrl.h"	// Added by ClassView

class CMainFrame;
class CTermView;
class CWebConn;

#define HTMLID_FIND 1
#define HTMLID_VIEWSOURCE 2
#define HTMLID_OPTIONS 3

extern "C" const IID CGID_IWebBrowser;

class CWebBrowser : public CWnd
{
public:
	static CMainFrame* parent;
	static CTermView* view;
	CWebConn* web_conn;
	long pgs;
	long pgsmax;

	CWebBrowser();
	~CWebBrowser()
	{
		if (m_TravelLog)
			m_TravelLog->Release();
	}

	BOOL PreTranslateMessage(MSG *pMsg);
//	DECLARE_DYNCREATE(CWebBrowser)

// html Data
public:
	//{{AFX_DATA(CWebBrowser)
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	void Find();
	ITravelLogStg* m_TravelLog;

	/*	struct TravelLog_Ent
		{
			CString Title;
			CString URL;
			CString TargetFrameName;
		};
		TravelLog_Ent m_CurTravelLog;
		POSITION m_CurTravelPos;
		CList<TravelLog_Ent,TravelLog_Ent&> m_TravelLog;
	*/
	void SendString(CString str);
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	BOOL PreCreateWindow(CREATESTRUCT &cs);
	CWebBrowserCtrl wb_ctrl;
	CWnd* SetFocus();
	CString GetLocationName();
	CString GetLocationURL();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWebBrowser)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnWindowClosing(BOOL IsChildWindow, BOOL FAR* Cancel);
	afx_msg void OnTitleChange(LPCTSTR Text);
	afx_msg void OnStatusTextChange(LPCTSTR Text);
	afx_msg void OnProgressChange(long Progress, long ProgressMax);
	afx_msg void OnNewWindow2(LPDISPATCH FAR* ppDisp, BOOL FAR* Cancel);
	afx_msg void OnNavigateComplete2(LPDISPATCH pDisp, VARIANT FAR* lpszURL);
	afx_msg void OnBeforeNavigate2(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, BOOL FAR* Cancel);
	afx_msg void OnDocumentComplete(LPDISPATCH pDisp, VARIANT FAR* URL);
	DECLARE_EVENTSINK_MAP()

protected:
	//}}AFX_VIRTUAL

// Implementation

	// Generated message map functions
	//{{AFX_MSG(CWebBrowser)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	DWORD	m_ClickTime;
	WPARAM	m_ClickMsg;
//	LPARAM	m_ClickPos;

	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	LRESULT OnAsyncTitleChange(WPARAM wp, LPARAM lp);
};


inline CString CWebBrowser::GetLocationURL()
{
	/*	BSTR bstr=NULL;
		LPDISPATCH m_pBrowserApp=wb_ctrl.GetApplication();
		if(m_pBrowserApp)
			m_pBrowserApp->get_LocationURL(&bstr);
		CString retVal(bstr);
		return retVal;
	*/
	return wb_ctrl.get_LocationURL();
}

inline CString CWebBrowser::GetLocationName()
{
	/*	BSTR bstr=NULL;
		if(m_pBrowserApp)
			m_pBrowserApp->get_LocationName(&bstr);
		CString retVal(bstr);
		return retVal;
	*/
	return wb_ctrl.get_LocationName();
}



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WEBBROWSER_H__26BE6502_23AB_11D6_8461_AC4F70CD0000__INCLUDED_)
