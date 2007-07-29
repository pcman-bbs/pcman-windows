// CWebBrowserCtrl.cpp  : 以 Microsoft Visual C++ 建立之 ActiveX 控制項包裝函式類別的定義


#include "../Lite/stdafx.h"
#include "WebBrowserCtrl.h"
#include ".\webbrowserctrl.h"

/////////////////////////////////////////////////////////////////////////////
// CWebBrowserCtrl

//IMPLEMENT_DYNCREATE(CWebBrowserCtrl, CWnd)

// CWebBrowserCtrl 屬性

// CWebBrowserCtrl 作業

void CWebBrowserCtrl::GoBack(int c)
{
	LPDISPATCH lpd = get_Application();
	IWebBrowser2* pwb = NULL;
	if (SUCCEEDED(lpd->QueryInterface(IID_IWebBrowser2, (void**)&pwb)))
	{
		for (; c > 0; c--)
			pwb->GoBack();
		pwb->Release();
	}
	lpd->Release();
}

void CWebBrowserCtrl::GoForward(int c)
{
	LPDISPATCH lpd = get_Application();
	IWebBrowser2* pwb = NULL;
	if (SUCCEEDED(lpd->QueryInterface(IID_IWebBrowser2, (void**)&pwb)))
	{
		for (; c > 0; c--)
			pwb->GoForward();
		pwb->Release();
	}
	lpd->Release();
}
