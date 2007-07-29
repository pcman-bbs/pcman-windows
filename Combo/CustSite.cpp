
//=--------------------------------------------------------------------------=
//  (C) Copyright 1996-1999 Microsoft Corporation. All Rights Reserved.
//=--------------------------------------------------------------------------=


//
// NOTE:
// Some of the code in this file is MFC implementation specific.
// Changes in future versions of MFC implementation may require
// the code to be changed. Please check the readme of this
// sample for more information
//

#include "..\Lite\stdafx.h"
#include "custsite.h"
#include <mshtml.h>

#include "..\Lite\AppConfig.h"

// DEFINE_GUID(_CGID_DocHostCommandHandler,0xf38bc242,0xb950,0x11d1,0x89,0x18,0x00,0xc0,0x4f,0xc2,0xc8,0x36);
static CLSID const _CGID_DocHostCommandHandler
= { 0xf38bc242, 0xb950, 0x11d1, { 0x89, 0x18, 0x00, 0xc0, 0x4f, 0xc2, 0xc8, 0x36 } };

BEGIN_INTERFACE_MAP(CCustomControlSite, COleControlSite)
INTERFACE_PART(CCustomControlSite, IID_IOleCommandTarget, OleCommandTarget)
INTERFACE_PART(CCustomControlSite, IID_IDocHostUIHandler, DocHostUIHandler)
END_INTERFACE_MAP()


ULONG FAR EXPORT  CCustomControlSite::XOleCommandTarget::AddRef()
{
	METHOD_PROLOGUE(CCustomControlSite, OleCommandTarget)
	return pThis->ExternalAddRef();
}


ULONG FAR EXPORT  CCustomControlSite::XOleCommandTarget::Release()
{
	METHOD_PROLOGUE(CCustomControlSite, OleCommandTarget)
	return pThis->ExternalRelease();
}

HRESULT FAR EXPORT  CCustomControlSite::XOleCommandTarget::QueryInterface(REFIID riid, void **ppvObj)
{
	METHOD_PROLOGUE(CCustomControlSite, OleCommandTarget)
	HRESULT hr = (HRESULT)pThis->ExternalQueryInterface(&riid, ppvObj);
	return hr;
}


STDMETHODIMP CCustomControlSite::XOleCommandTarget::Exec(
	/* [unique][in] */ const GUID __RPC_FAR *pguidCmdGroup,
	/* [in] */ DWORD nCmdID,
	/* [in] */ DWORD nCmdexecopt,
	/* [unique][in] */ VARIANT __RPC_FAR *pvaIn,
	/* [unique][out][in] */ VARIANT __RPC_FAR *pvaOut
)
{

	HRESULT hr = S_OK;
	BOOL bActiveX = FALSE;
	if (pguidCmdGroup && IsEqualGUID(*pguidCmdGroup, _CGID_DocHostCommandHandler))
	{
		if ((nCmdID == 40 || nCmdID == 41) && AppConfig.disable_script_error)	//OLECMDID_SHOWSCRIPTERROR
		{
			if (nCmdID == 41)
			{
				IHTMLDocument2*             pDoc = NULL;
				IHTMLWindow2*               pWindow = NULL;
				IHTMLEventObj*              pEventObj = NULL;
				BSTR                        rgwszName = SysAllocString(L"messageText");
				DISPID                      rgDispID;
				VARIANT                     rgvaEventInfo;
				DISPPARAMS                  params;

				params.cArgs = 0;
				params.cNamedArgs = 0;

				// Get the document that is currently being viewed.
				hr = pvaIn->punkVal->QueryInterface(IID_IHTMLDocument2, (void **) & pDoc);
				// Get document.parentWindow.
				hr = pDoc->get_parentWindow(&pWindow);
				pDoc->Release();
				// Get the window.event object.
				hr = pWindow->get_event(&pEventObj);
				// Get the error info from the window.event object.
				// Get the property's dispID.
				hr = pEventObj->GetIDsOfNames(IID_NULL, &rgwszName, 1,
											  LOCALE_SYSTEM_DEFAULT, &rgDispID);
				// Get the value of the property.
				hr = pEventObj->Invoke(rgDispID, IID_NULL, LOCALE_SYSTEM_DEFAULT,
									   DISPATCH_PROPERTYGET, &params, &rgvaEventInfo, NULL, NULL);
				SysFreeString(rgwszName);
				pWindow->Release();
				pEventObj->Release();
				if (wcsstr(rgvaEventInfo.bstrVal, L" ActiveX ") != NULL)
					bActiveX = TRUE;
				SysFreeString(rgvaEventInfo.bstrVal);
			}

			(*pvaOut).vt = VT_BOOL;
			// Continue running scripts on the page.
			(*pvaOut).boolVal = VARIANT_TRUE;

			if (nCmdID == 40)
				;//pMainFrame->SetMessageText("Script Error!");
			else if (bActiveX)
				;//pMainFrame->SetMessageText("ActiveX Denied!");
			else
			{
				(*pvaOut).boolVal = VARIANT_FALSE;
				return OLECMDERR_E_NOTSUPPORTED;
			}
			return hr;
		}
	}
	return OLECMDERR_E_NOTSUPPORTED;
}

STDMETHODIMP CCustomControlSite::XOleCommandTarget::QueryStatus(
	/* [unique][in] */ const GUID __RPC_FAR *pguidCmdGroup,
	/* [in] */ ULONG cCmds,
	/* [out][in][size_is] */ OLECMD __RPC_FAR prgCmds[  ],
	/* [unique][out][in] */ OLECMDTEXT __RPC_FAR *pCmdText
)
{
	METHOD_PROLOGUE(CCustomControlSite, OleCommandTarget)
	return OLECMDERR_E_NOTSUPPORTED;
}


/*

    DOCHOSTUIFLAG_DIALOG = 0x00000001,
    DOCHOSTUIFLAG_DISABLE_HELP_MENU = 0x00000002,
    DOCHOSTUIFLAG_NO3DBORDER = 0x00000004,
    DOCHOSTUIFLAG_SCROLL_NO = 0x00000008,
    DOCHOSTUIFLAG_DISABLE_SCRIPT_INACTIVE = 0x00000010,
    DOCHOSTUIFLAG_OPENNEWWIN = 0x00000020,
    DOCHOSTUIFLAG_DISABLE_OFFSCREEN = 0x00000040,
    DOCHOSTUIFLAG_FLAT_SCROLLBAR = 0x00000080,
    DOCHOSTUIFLAG_DIV_BLOCKDEFAULT = 0x00000100,
    DOCHOSTUIFLAG_ACTIVATE_CLIENTHIT_ONLY = 0x00000200,
    DOCHOSTUIFLAG_OVERRIDEBEHAVIORFACTORY = 0x00000400,
    DOCHOSTUIFLAG_CODEPAGELINKEDFONTS = 0x00000800,
    DOCHOSTUIFLAG_URL_ENCODING_DISABLE_UTF8 = 0x00001000,
    DOCHOSTUIFLAG_URL_ENCODING_ENABLE_UTF8 = 0x00002000,
    DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE = 0x00004000,
    DOCHOSTUIFLAG_ENABLE_INPLACE_NAVIGATION = 0x00010000,
    DOCHOSTUIFLAG_IME_ENABLE_RECONVERSION = 0x00020000,
    DOCHOSTUIFLAG_THEME = 0x00040000,
    DOCHOSTUIFLAG_NOTHEME = 0x00080000,
    DOCHOSTUIFLAG_NOPICS = 0x00100000,
    DOCHOSTUIFLAG_NO3DOUTERBORDER = 0x00200000
*/
#define DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE 16384
#define DOCHOSTUIFLAG_CODEPAGELINKEDFONTS  0x00000800
#define DOCHOSTUIFLAG_THEME  0x00040000
#define DOCHOSTUIFLAG_LOCAL_MACHINE_ACCESS_CHECK  0x00800000

ULONG FAR EXPORT  CCustomControlSite::XDocHostUIHandler::AddRef()
{
	METHOD_PROLOGUE(CCustomControlSite, DocHostUIHandler)
	return pThis->ExternalAddRef();
}


ULONG FAR EXPORT  CCustomControlSite::XDocHostUIHandler::Release()
{
	METHOD_PROLOGUE(CCustomControlSite, DocHostUIHandler)
	return pThis->ExternalRelease();
}

HRESULT FAR EXPORT  CCustomControlSite::XDocHostUIHandler::QueryInterface(REFIID riid, void **ppvObj)
{
	METHOD_PROLOGUE(CCustomControlSite, DocHostUIHandler)
	HRESULT hr = (HRESULT)pThis->ExternalQueryInterface(&riid, ppvObj);
	return hr;
}

// * CImpIDocHostUIHandler::GetHostInfo
// *
// * Purpose: Called at initialization
// *
HRESULT FAR EXPORT  CCustomControlSite::XDocHostUIHandler::GetHostInfo(DOCHOSTUIINFO* pInfo)
{
	try
	{
		METHOD_PROLOGUE(CCustomControlSite, DocHostUIHandler)
		pInfo->dwFlags = DOCHOSTUIFLAG_THEME
						 | DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE
						 | DOCHOSTUIFLAG_FLAT_SCROLLBAR
						 | DOCHOSTUIFLAG_LOCAL_MACHINE_ACCESS_CHECK;
		//|DOCHOSTUIFLAG_CODEPAGELINKEDFONTS;
		pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;
	}
	catch (...)
	{
	}
	return S_OK;
}

// * CImpIDocHostUIHandler::ShowUI
// *
// * Purpose: Called when MSHTML.DLL shows its UI
// *
HRESULT FAR EXPORT  CCustomControlSite::XDocHostUIHandler::ShowUI(
	DWORD dwID,
	IOleInPlaceActiveObject * /*pActiveObject*/,
	IOleCommandTarget * pCommandTarget,
	IOleInPlaceFrame * /*pFrame*/,
	IOleInPlaceUIWindow * /*pDoc*/)
{

	METHOD_PROLOGUE(CCustomControlSite, DocHostUIHandler)
	// We've already got our own UI in place so just return S_OK
	return S_OK;
}

// * CImpIDocHostUIHandler::HideUI
// *
// * Purpose: Called when MSHTML.DLL hides its UI
// *
HRESULT FAR EXPORT  CCustomControlSite::XDocHostUIHandler::HideUI(void)
{
	METHOD_PROLOGUE(CCustomControlSite, DocHostUIHandler)
	return S_OK;
}

// * CImpIDocHostUIHandler::UpdateUI
// *
// * Purpose: Called when MSHTML.DLL updates its UI
// *
HRESULT FAR EXPORT  CCustomControlSite::XDocHostUIHandler::UpdateUI(void)
{
	METHOD_PROLOGUE(CCustomControlSite, DocHostUIHandler)
	// MFC is pretty good about updating it's UI in it's Idle loop so I don't do anything here
	return S_OK;
}

// * CImpIDocHostUIHandler::EnableModeless
// *
// * Purpose: Called from MSHTML.DLL's IOleInPlaceActiveObject::EnableModeless
// *
HRESULT FAR EXPORT  CCustomControlSite::XDocHostUIHandler::EnableModeless(BOOL /*fEnable*/)
{
	METHOD_PROLOGUE(CCustomControlSite, DocHostUIHandler)
	return E_NOTIMPL;
}

// * CImpIDocHostUIHandler::OnDocWindowActivate
// *
// * Purpose: Called from MSHTML.DLL's IOleInPlaceActiveObject::OnDocWindowActivate
// *
HRESULT FAR EXPORT  CCustomControlSite::XDocHostUIHandler::OnDocWindowActivate(BOOL /*fActivate*/)
{
	try
	{
		METHOD_PROLOGUE(CCustomControlSite, DocHostUIHandler)
	}
	catch (...)
	{
	}
	return E_NOTIMPL;
}

// * CImpIDocHostUIHandler::OnFrameWindowActivate
// *
// * Purpose: Called from MSHTML.DLL's IOleInPlaceActiveObject::OnFrameWindowActivate
// *
HRESULT FAR EXPORT  CCustomControlSite::XDocHostUIHandler::OnFrameWindowActivate(BOOL /*fActivate*/)
{
	METHOD_PROLOGUE(CCustomControlSite, DocHostUIHandler)
	return E_NOTIMPL;
}

// * CImpIDocHostUIHandler::ResizeBorder
// *
// * Purpose: Called from MSHTML.DLL's IOleInPlaceActiveObject::ResizeBorder
// *
HRESULT FAR EXPORT  CCustomControlSite::XDocHostUIHandler::ResizeBorder(
	LPCRECT /*prcBorder*/,
	IOleInPlaceUIWindow* /*pUIWindow*/,
	BOOL /*fRameWindow*/)
{
	METHOD_PROLOGUE(CCustomControlSite, DocHostUIHandler)
	return E_NOTIMPL;
}

// * CImpIDocHostUIHandler::ShowContextMenu
// *
// * Purpose: Called when MSHTML.DLL would normally display its context menu
// *
HRESULT FAR EXPORT  CCustomControlSite::XDocHostUIHandler::ShowContextMenu(
	DWORD /*dwID*/,
	POINT* /*pptPosition*/,
	IUnknown* /*pCommandTarget*/,
	IDispatch* /*pDispatchObjectHit*/)
{
	try
	{
		METHOD_PROLOGUE(CCustomControlSite, DocHostUIHandler)
	}
	catch (...)
	{
	}
	return E_NOTIMPL; // We've shown our own context menu. MSHTML.DLL will no longer try to show its own.
}

// * CImpIDocHostUIHandler::TranslateAccelerator
// *
// * Purpose: Called from MSHTML.DLL's TranslateAccelerator routines
// *
HRESULT FAR EXPORT  CCustomControlSite::XDocHostUIHandler::TranslateAccelerator(LPMSG lpMsg,
		/* [in] */ const GUID __RPC_FAR *pguidCmdGroup,
		/* [in] */ DWORD nCmdID)
{
	METHOD_PROLOGUE(CCustomControlSite, DocHostUIHandler)
	return S_FALSE;
}

// * CImpIDocHostUIHandler::GetOptionKeyPath
// *
// * Purpose: Called by MSHTML.DLL to find where the host wishes to store
// *	its options in the registry
// *
HRESULT FAR EXPORT  CCustomControlSite::XDocHostUIHandler::GetOptionKeyPath(BSTR* pbstrKey, DWORD)
{

	METHOD_PROLOGUE(CCustomControlSite, DocHostUIHandler)
	return E_NOTIMPL;
}

STDMETHODIMP CCustomControlSite::XDocHostUIHandler::GetDropTarget(
	/* [in] */ IDropTarget __RPC_FAR *pDropTarget,
	/* [out] */ IDropTarget __RPC_FAR *__RPC_FAR *ppDropTarget)
{
	METHOD_PROLOGUE(CCustomControlSite, DocHostUIHandler)
	/*	try{
		if(theApp.m_pDropTarget)
		{
			IDropTarget* pDrop = (IDropTarget*)theApp.m_pDropTarget;
			pDrop->AddRef();
			*ppDropTarget = pDrop;
		}
		}catch(...)
		{
		}
	*/	return S_OK;
}

STDMETHODIMP CCustomControlSite::XDocHostUIHandler::GetExternal(
	/* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch)
{
	// return the IDispatch we have for extending the object Model
	/*	try{
		IDispatch* pDisp = (IDispatch*)theApp.m_pDispOM;
		pDisp->AddRef();
		*ppDispatch = pDisp;
		}catch(...)
		{
		}
	*/
	return S_OK;
}

STDMETHODIMP CCustomControlSite::XDocHostUIHandler::TranslateUrl(
	/* [in] */ DWORD dwTranslate,
	/* [in] */ OLECHAR __RPC_FAR *pchURLIn,
	/* [out] */ OLECHAR __RPC_FAR *__RPC_FAR *ppchURLOut)
{
	METHOD_PROLOGUE(CCustomControlSite, DocHostUIHandler)
	return E_NOTIMPL;
}

STDMETHODIMP CCustomControlSite::XDocHostUIHandler::FilterDataObject(
	/* [in] */ IDataObject __RPC_FAR *pDO,
	/* [out] */ IDataObject __RPC_FAR *__RPC_FAR *ppDORet)
{
	METHOD_PROLOGUE(CCustomControlSite, DocHostUIHandler)
	return E_NOTIMPL;
}



