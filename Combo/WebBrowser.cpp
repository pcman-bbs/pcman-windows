// WebBrowser.cpp : implementation file
//

#include "../Lite/stdafx.h"
#include "../Lite/pcman.h"
#include "../Lite/WinUtils.h"
#include "../Lite/Clipboard.h"

#include "WebBrowser.h"
#include <mshtml.h>
#include <atlbase.h>
#include <mmsystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "../Lite/mainfrm.h"
#include "WebConn.h"

static const IID IID_ITravelLogStg	= { 0x7EBFDD80, 0xAD18, 0x11d3, {0xA4, 0xC5, 0x00, 0xC0, 0x4F, 0x72, 0xD6, 0xB8}};

/////////////////////////////////////////////////////////////////////////////
// CWebBrowser

CMainFrame* CWebBrowser::parent = NULL;
CTermView* CWebBrowser::view = NULL;

//	IMPLEMENT_DYNCREATE(CWebBrowser,CWnd)
extern "C" const IID CGID_IWebBrowser =
		{0xed016940, 0xbd5b, 0x11cf,
		 {0xba, 0x4e, 0x0, 0xc0, 0x4f, 0xd7, 0x08, 0x16}
		} ;

CWebBrowser::CWebBrowser(): CWnd()//, m_CurTravelPos(NULL)
{
	m_TravelLog = NULL;
	HKEY hk = NULL;
	RegOpenKey(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Internet Explorer\\Main"), &hk);
	char yes[] = "yes";	DWORD type = REG_SZ;	DWORD len = 3;
	RegSetValueEx(hk, _T("Disable Script Debugger"), 0, REG_SZ, (LPBYTE)yes, 3);
	RegCloseKey(hk);

	m_ClickTime = 0;
	m_ClickMsg = 0;
//	m_ClickPos = 0;

	//{{AFX_DATA_INIT(CWebBrowser)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


BEGIN_MESSAGE_MAP(CWebBrowser, CWnd)
	//{{AFX_MSG_MAP(CWebBrowser)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_PARENTNOTIFY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_WBTITLECHANGE, OnAsyncTitleChange)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CWebBrowser, CWnd)
//{{AFX_EVENTSINK_MAP(CWebBrowser)
ON_EVENT(CWebBrowser, 1, 263 /* WindowClosing */, OnWindowClosing, VTS_BOOL VTS_PBOOL)
ON_EVENT(CWebBrowser, 1, 113 /* TitleChange */, OnTitleChange, VTS_BSTR)
ON_EVENT(CWebBrowser, 1, 102 /* StatusTextChange */, OnStatusTextChange, VTS_BSTR)
ON_EVENT(CWebBrowser, 1, 108 /* ProgressChange */, OnProgressChange, VTS_I4 VTS_I4)
ON_EVENT(CWebBrowser, 1, 251 /* NewWindow2 */, OnNewWindow2, VTS_PDISPATCH VTS_PBOOL)
ON_EVENT(CWebBrowser, 1, 252 /* NavigateComplete2 */, OnNavigateComplete2, VTS_DISPATCH VTS_PVARIANT)
ON_EVENT(CWebBrowser, 1, 250 /* BeforeNavigate2 */, OnBeforeNavigate2, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
ON_EVENT(CWebBrowser, 1, 259 /* DocumentComplete */, OnDocumentComplete, VTS_DISPATCH VTS_PVARIANT)
//}}AFX_EVENTSINK_MAP
//	ON_EVENT(CWebBrowser, 1, 264 /* WindowSetLeft */, OnWindowSetLeft, VTS_I4)
//	ON_EVENT(CWebBrowser, 1, 267 /* WindowSetHeight */, OnWindowSetHeight, VTS_I4)
//	ON_EVENT(CWebBrowser, 1, 265 /* WindowSetTop */, OnWindowSetTop, VTS_I4)
//	ON_EVENT(CWebBrowser, 1, 266 /* WindowSetWidth */, OnWindowSetWidth, VTS_I4)
//	ON_EVENT(CWebBrowser, 1, 227 /* UpdatePageStatus */, OnUpdatePageStatus, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT)
END_EVENTSINK_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWebBrowser message handlers

const DWORD SET_FEATURE_ON_THREAD = 0x00000001;
const DWORD SET_FEATURE_ON_PROCESS = 0x00000002;
const DWORD SET_FEATURE_IN_REGISTRY = 0x00000004;
const DWORD SET_FEATURE_ON_THREAD_LOCALMACHINE = 0x00000008;
const DWORD SET_FEATURE_ON_THREAD_INTRANET = 0x00000010;
const DWORD SET_FEATURE_ON_THREAD_TRUSTED = 0x00000020;
const DWORD SET_FEATURE_ON_THREAD_INTERNET = 0x00000040;
const DWORD SET_FEATURE_ON_THREAD_RESTRICTED = 0x00000080;

enum INTERNETFEATURELIST
{
	FEATURE_OBJECT_CACHING = 0,
	FEATURE_ZONE_ELEVATION = 1,
	FEATURE_MIME_HANDLING = 2,
	FEATURE_MIME_SNIFFING = 3,
	FEATURE_WINDOW_RESTRICTIONS = 4,
	FEATURE_WEBOC_POPUPMANAGEMENT = 5,
	FEATURE_BEHAVIORS = 6,
	FEATURE_DISABLE_MK_PROTOCOL = 7,
	FEATURE_LOCALMACHINE_LOCKDOWN = 8,
	FEATURE_SECURITYBAND = 9,
	FEATURE_RESTRICT_ACTIVEXINSTALL = 10,
	FEATURE_VALIDATE_NAVIGATE_URL = 11,
	FEATURE_RESTRICT_FILEDOWNLOAD = 12,
	FEATURE_ADDON_MANAGEMENT = 13,
	FEATURE_PROTOCOL_LOCKDOWN = 14,
	FEATURE_HTTP_USERNAME_PASSWORD_DISABLE = 15,
	FEATURE_SAFE_BINDTOOBJECT = 16,
	FEATURE_UNC_SAVEDFILECHECK = 17,
	FEATURE_GET_URL_DOM_FILEPATH_UNENCODED = 18,
	FEATURE_ENTRY_COUNT = 19,
};

typedef HRESULT(WINAPI *PCoInternetSetFeatureEnabled)(INTERNETFEATURELIST, DWORD, BOOL);

int CWebBrowser::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	wb_ctrl.Create(NULL, NULL, WS_VISIBLE | WS_CHILD, CRect(0, 0, 0, 0), this, 1);
	wb_ctrl.ShowWindow(SW_SHOW);
	wb_ctrl.put_Visible(TRUE);

	CComPtr<IDispatch> pwb(wb_ctrl.get_Application());
	if (pwb.p)
	{
		CComQIPtr<IServiceProvider, &IID_IServiceProvider> psp(pwb);
		if (psp.p)
			psp.p->QueryService(SID_STravelLogCursor, IID_ITravelLogStg, (void**) &m_TravelLog);
	}

	HMODULE urlmon = LoadLibrary("urlmon.dll");
	if (urlmon)
	{
		PCoInternetSetFeatureEnabled CoInternetSetFeatureEnabled = NULL;
		CoInternetSetFeatureEnabled = (PCoInternetSetFeatureEnabled)GetProcAddress(urlmon, "CoInternetSetFeatureEnabled");
		if (CoInternetSetFeatureEnabled)
		{
			CoInternetSetFeatureEnabled(FEATURE_WEBOC_POPUPMANAGEMENT, SET_FEATURE_ON_PROCESS, TRUE);
			CoInternetSetFeatureEnabled(FEATURE_SECURITYBAND, SET_FEATURE_ON_PROCESS, TRUE);
		}
		FreeLibrary(urlmon);
	}

	return 0;
}

void CWebBrowser::OnSize(UINT nType, int cx, int cy)
{
	wb_ctrl.MoveWindow(0, 0, cx, cy);
	CWnd::OnSize(nType, cx, cy);
}

void CWebBrowser::OnSetFocus(CWnd* pOldWnd)
{
//	SetFocus();
//	parent->UpdateAddressBar();
}


BOOL CWebBrowser::PreTranslateMessage(MSG *pMsg)
{
	//以下參考自 MFC --- CFormView
	// since 'IsDialogMessage' will eat frame window accelerators,
	//   we call all frame windows' PreTranslateMessage first
	// allow owner & frames to translate before IsDialogMessage does
	if (parent->PreTranslateMessage(pMsg))
		return TRUE;

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		m_ClickTime = timeGetTime();
		m_ClickMsg = WM_LBUTTONDOWN;
	}

	// filter both messages to dialog and from children
	if (pMsg->message == WM_IME_CHAR)
	{
		return FALSE;
	}

	BOOL ret = FALSE;
	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
	{
		IDispatch* app = wb_ctrl.get_Application();
		if (app)
		{
			IOleInPlaceActiveObject* ip = NULL;
			if (SUCCEEDED(app->QueryInterface(IID_IOleInPlaceActiveObject, (void**)&ip)))
			{
				if (S_OK == ip->TranslateAccelerator(pMsg))
					ret = TRUE;

				// Some notes from Hong Jen Yee (PCMan):
				// FIXME: Very dirty hack used to fix the malfunction of "Enter" key in
				// rich-text editor of GMail and Yahoo mail.
				// Warnings: I've no idea why this works, but it do works, so let's use it.
				// Nobody knows will this cause other problems. Use at your own risk.
				// Maybe some pages with heavy use of Javascripts will be affected.
				if (!ret && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
				{
					if (!(GetKeyState(VK_CONTROL) & 0x8000) &&
						!(GetKeyState(VK_MENU) & 0x8000) &&
						!(GetKeyState(VK_SHIFT) & 0x8000))
					{
						pMsg->message = WM_CHAR;	// Damn! Incredible!! This works!!
						if (S_OK == ip->TranslateAccelerator(pMsg))
							ret = TRUE;
					}
				}
				ip->Release();
			}
			app->Release();
		}
	}
	return ret ? TRUE : PreTranslateInput(pMsg);
}


CWnd* CWebBrowser::SetFocus()
{
	LPDISPATCH lpd = wb_ctrl.get_Document();
	if (lpd)
	{
		IHTMLDocument2* pdoc = NULL;
		if (SUCCEEDED(lpd->QueryInterface(IID_IHTMLDocument2, (void**)&pdoc)))
		{
			IHTMLWindow2* pwnd = NULL;
			if (SUCCEEDED(pdoc->get_parentWindow(&pwnd)))
			{
				pwnd->focus();
				pwnd->Release();
			}
			pdoc->Release();
		}
		lpd->Release();
	}
	return this;
}

void CWebBrowser::OnWindowClosing(BOOL IsChildWindow, BOOL FAR* Cancel)
{
	// TODO: Add your control notification handler code here
	parent->PostMessage(WM_REMOVE_WEBCONN, 0, LPARAM(web_conn));
}

//DEL void CWebBrowser::OnWindowSetLeft(long Left)
//DEL {
//DEL 	// TODO: Add your control notification handler code here
//DEL
//DEL }

//DEL void CWebBrowser::OnWindowSetHeight(long Height)
//DEL {
//DEL 	// TODO: Add your control notification handler code here
//DEL
//DEL }

//DEL void CWebBrowser::OnWindowSetTop(long Top)
//DEL {
//DEL 	// TODO: Add your control notification handler code here
//DEL
//DEL }

//DEL void CWebBrowser::OnWindowSetWidth(long Width)
//DEL {
//DEL 	// TODO: Add your control notification handler code here
//DEL
//DEL }

//DEL void CWebBrowser::OnUpdatePageStatus(LPDISPATCH pDisp, VARIANT FAR* nPage, VARIANT FAR* fDone)
//DEL {
//DEL 	// TODO: Add your control notification handler code here
//DEL
//DEL }

void CWebBrowser::OnTitleChange(LPCTSTR Text)
{
	PostMessage(WM_WBTITLECHANGE, 0, (LPARAM)strdup(Text));
}

void CWebBrowser::OnStatusTextChange(LPCTSTR Text)
{
	if (view->con != web_conn)
		return;
	parent->status_bar.SetWindowText(Text);
}

void CWebBrowser::OnProgressChange(long Progress, long ProgressMax)
{
	if (view->con != web_conn)
		return;
	pgs = Progress;
	pgsmax = ProgressMax;
	if (pgs == pgsmax)
		pgs = 0;
	if (view->con == web_conn)
	{
		parent->progress_bar.SetRange32(0, ProgressMax);
		parent->progress_bar.SetPos(Progress);
	}
}

void CWebBrowser::OnNewWindow2(LPDISPATCH FAR* ppDisp, BOOL FAR* Cancel)
{
	if (AppConfig.disable_popup)
	{
		bool bCanOpen = false;
		DWORD ClickTime = timeGetTime();
		switch (m_ClickMsg)
		{
		case WM_LBUTTONDOWN:
			if (600 >= (ClickTime - m_ClickTime))
				bCanOpen = true;
			break;
		case WM_RBUTTONDOWN:
			if (4000 >= (ClickTime - m_ClickTime))
				bCanOpen = true;
		}
		m_ClickMsg = 0;

		/*		if( bCanOpen )
				{
					WORD x = LOWORD(m_ClickPos);
					WORD y = HIWORD(m_ClickPos);

					CComPtr<IHTMLDocument> pdoc = (IHTMLDocument*)wb_ctrl.get_Document();
					if( pdoc.p )
					{
						CComQIPtr<IHTMLDocument2, &IID_IHTMLDocument2> pdoc2( pdoc.p );
						CComPtr<IHTMLElement> pelement;
						if( pdoc2.p )
						{
							pdoc2->elementFromPoint(x, y, &pelement.p);
							if(pelement.p)
							{
								BSTR str = NULL;
								pelement->get_tagName(&str);
								CString a = str;
								if( str && wcscmp( str, L"A" ) )
									bCanOpen = false;
								SysFreeString(str);
							}
						}
					}
				}
		*/
		if (!bCanOpen)
		{
			*Cancel = TRUE;
			return;
		}
	}

	IDispatch* p = *ppDisp;
	CWebConn* nweb_conn = (CWebConn*)view->ConnectWeb("", FALSE);
	*ppDisp = nweb_conn->web_browser.wb_ctrl.get_Application();
}

void CWebBrowser::OnNavigateComplete2(LPDISPATCH pDisp, VARIANT FAR* URL)
{
	/*	switch( wb_ctrl.m_Action)
		{
		case CWebBrowserCtrl::WBA_GoBack:
			m_TravelLog.GetPrev(m_CurTravelPos);
			break;
		case CWebBrowserCtrl::WBA_GoForward:
			m_TravelLog.GetNext(m_CurTravelPos);
			break;
		default:
			POSITION tail = m_TravelLog.GetTailPosition();
			if( m_CurTravelPos )
			{
				m_TravelLog.GetNext(m_CurTravelPos);
				while( m_CurTravelPos && tail != m_CurTravelPos )
				{
					POSITION tmp = m_CurTravelPos;
					m_TravelLog.GetNext(m_CurTravelPos);
					m_TravelLog.RemoveAt(tmp);
				}
			}
			m_CurTravelLog.Title = wb_ctrl.get_LocationName();
			m_TravelLog.AddTail(m_CurTravelLog);
			m_CurTravelPos = m_TravelLog.GetTailPosition();
		}
		wb_ctrl.m_Action = CWebBrowserCtrl::WBA_None;
	*/
	HKEY hk = NULL;
	RegOpenKey(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Internet Explorer\\Main"), &hk);
	char yes[4];	DWORD type = REG_SZ;	DWORD len = 3;
//	RegQueryValueEx(hk, _T("Disable Script Debugger"), 0,&type , (LPBYTE)yes, &len);
	strcpy(yes, "yes");
	RegSetValueEx(hk, _T("Disable Script Debugger"), 0, REG_SZ, (LPBYTE)yes, 3);
	RegCloseKey(hk);

	web_conn->address = GetLocationURL();
	if (view->con == web_conn)
		parent->UpdateAddressBar();
}

void CWebBrowser::OnBeforeNavigate2(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, BOOL FAR* Cancel)
{
	*Cancel = parent->FindAdFilter(NULL, LPCTSTR(URL));
	if (*Cancel)
		parent->PostMessage(WM_REMOVE_WEBCONN, 0, LPARAM(web_conn));
}

void CWebBrowser::OnDocumentComplete(LPDISPATCH pDisp, VARIANT FAR* URL)
{
	CString url = V_BSTR(URL);
	view->parent->AddToHistory(url);
}

char wbclass[] = "WebView";

BOOL CWebBrowser::PreCreateWindow(CREATESTRUCT &cs)
{
	cs.dwExStyle |= WS_EX_CONTROLPARENT;

	WNDCLASS wcl;
	wcl.cbClsExtra = 0;
	wcl.cbWndExtra = DLGWINDOWEXTRA;
	wcl.hbrBackground = HBRUSH(COLOR_WINDOW + 1);
	wcl.hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	wcl.hIcon = NULL;
	wcl.hInstance = AfxGetInstanceHandle();
	wcl.lpfnWndProc = AfxGetAfxWndProc();
	wcl.lpszClassName = wbclass;
	wcl.lpszMenuName = NULL;
	wcl.style = 0;

	RegisterClass(&wcl);

	cs.lpszClass = wbclass;
	return TRUE;
}

LRESULT CWebBrowser::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_ERASEBKGND)
		return TRUE;
	else if (message == WM_COMMAND && LOWORD(wParam) == IDCANCEL)
	{
		wb_ctrl.Stop();
	}
	return ::DefWindowProc/*DefDlgProc*/(m_hWnd, message, wParam, lParam);
}

LRESULT CWebBrowser::OnAsyncTitleChange(WPARAM wp, LPARAM lp)
{
	LPCTSTR Text = (LPCTSTR)lp;

	int i = view->parent->ConnToIndex(web_conn);
	TCITEM item;
	item.mask = TCIF_TEXT;
	if (strcmp(Text, web_conn->name))
	{
		web_conn->name = Text;
		parent->tab.SetItemText(i, web_conn->name);
	}

	free((void*)Text);

	if (web_conn == view->con)
		parent->SetWindowText(web_conn->name + parent->window_title);
	parent->FilterWebConn(web_conn);
	return 0;
}

void CWebBrowser::SendString(CString str)
{
	CString bak;
	CClipboard::GetText(bak);
	CClipboard::SetText(NULL, str);
	wb_ctrl.SetFocus();
	wb_ctrl.ExecWB(OLECMDID_PASTE, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	CClipboard::SetText(NULL, bak);
}

void CWebBrowser::OnParentNotify(UINT message, LPARAM lParam)
{

	switch (message)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		m_ClickTime = timeGetTime();
		m_ClickMsg = message;
		break;
	}
}

void CWebBrowser::Find()
{
	SetFocus();
	LPDISPATCH lpd = wb_ctrl.get_Document();
	if (!lpd)
		return;
	IOleCommandTarget* pcmd = NULL;
	if (SUCCEEDED(lpd->QueryInterface(IID_IOleCommandTarget, (void**)&pcmd)))
	{
		pcmd->Exec(&CGID_IWebBrowser, HTMLID_FIND, 0, NULL, NULL);
		pcmd->Release();
	}
	lpd->Release();
}
