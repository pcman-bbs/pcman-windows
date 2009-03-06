// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include <shlobj.h>

#include "PCMan.h"

#include "MainFrm.h"
#include "WinUtils.h"
#include "CustomizeDlg.h"
#include "PasswdDlg.h"
#include "MemIniFile.h"
#include "../Resource/resource.h"
#include "FavMenu.h"
#include "BrowseDirDlg.h"
#include "StringDlg.h"
#include "TelnetConn.h"
#include "AddressDlg.h"

#include "SitePage.h"
#include "AutoReplyPage.h"
#include "GeneralPage.h"
#include "OtherPage.h"
#include "HyperLinkPage.h"
#include "ConnectPage.h"

#include "ColorConfigDlg.h"
#include "SetBkDlg.h"

#include "Clipboard.h"
#include "StrUtils.h"
#include "MouseCTL.h"
#include "SearchPlugin.h"
#include "InstantTranDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if defined(_COMBO_)
#include "SearchPlugin.h"
#include "../Combo/WebBrowser.h"
#include "../Combo/WebConn.h"
#include "Conn.h"
#include "../Combo/WebCfgPage.h"
#include "../Combo/WebPageDlg.h"

LPSTR CMainFrame::mainfrm_class_name = "PCManCB";
const char *CMainFrame::window_title = " - PCMan Combo";// (Build: " __DATE__ ")";
#else
LPSTR CMainFrame::mainfrm_class_name = "PCMan";
const char *CMainFrame::window_title = " - PCMan";// (Build: " __DATE__ ")";
#endif

extern CFont fnt;
CUcs2Conv g_ucs2conv;

/////////////////////////////////////////////////////////////////////////////
UINT BackgroundAutoUpdate(LPVOID pParam)
{
	CAutoUpdater updater;
	updater.CheckForUpdate();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_COMMAND_RANGE(ID_SET_CHARSET_DEFAULT, ID_SET_CHARSET_UTF8, OnSetCharset)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_WM_INITMENUPOPUP()
	ON_WM_CLOSE()
	ON_WM_ACTIVATE()
	ON_COMMAND(ID_TAB, OnShowTabBar)
	ON_COMMAND(ID_TOOLBAR, OnShowToolbar)
	ON_COMMAND(ID_CLOSEBTN, OnCloseBtn)
	ON_COMMAND(ID_STATUSBAR, OnShowStatusbar)
	ON_WM_MOVE()
	ON_COMMAND(ID_PGD, OnPgd)
	ON_COMMAND(ID_PGU, OnPgu)
	ON_COMMAND(ID_ADSFOCUS, OnAddressFocus)
	ON_COMMAND(ID_SEND_ANSICODE, OnShowAnsiBar)
	ON_COMMAND(ID_SHOW_SCROLL, OnShowScroll)
	ON_COMMAND(ID_SETDEFAULT, OnSetDefaultProgram)
	ON_COMMAND(ID_CONFIG_AUTOSWITCH, OnAutoSwitch)
	ON_UPDATE_COMMAND_UI(ID_ANSI_INS, OnUpdateEditorInsertMode)
	ON_UPDATE_COMMAND_UI(ID_CONFIG_AUTOSWITCH, OnUpdateAutoswitch)
	ON_UPDATE_COMMAND_UI(ID_KKTAB, OnUpdateKKmanStyleTab)
	ON_UPDATE_COMMAND_UI(ID_SHOW_SCROLL, OnUpdateShowScroll)
	ON_UPDATE_COMMAND_UI(ID_SMOOTH_DRAW, OnUpdateSmoothDraw)
	ON_UPDATE_COMMAND_UI(ID_STATUSBAR, OnUpdateShowStatusbar)
	ON_UPDATE_COMMAND_UI(ID_TAB, OnUpdateShowTabBar)
	ON_UPDATE_COMMAND_UI(ID_TOOLBAR, OnUpdateShowToolbar)
	ON_UPDATE_COMMAND_UI(ID_AUTODBCS_ARROW, OnUpdateAutoDBCSArrow)
	ON_UPDATE_COMMAND_UI(ID_AUTODBCS_BACKSPACE, OnUpdateAutoDBCSBackspace)
	ON_COMMAND(ID_AUTODBCS_BACKSPACE, OnAutoDBCSBackspace)
	ON_COMMAND(ID_AUTODBCS_ARROW, OnAutoDBCSArrow)
	ON_COMMAND(ID_AUTODBCS_DEL, OnAutoDBCSDel)
	ON_UPDATE_COMMAND_UI(ID_AUTODBCS_DEL, OnUpdateAutoDBCSDel)
	ON_COMMAND(ID_AUTODBCS_MOUSE, OnAutoDBCSMouse)
	ON_UPDATE_COMMAND_UI(ID_AUTODBCS_MOUSE, OnUpdateAutoDBCSMouse)
	ON_UPDATE_COMMAND_UI(ID_ADS, OnUpdateShowAddressBar)
	ON_UPDATE_COMMAND_UI(ID_CLOSEBTN, OnUpdateCloseBtn)
	ON_CBN_SELENDOK(IDC_ADS_COMBO, OnAddressBarComboOK)
	ON_BN_CLICKED(IDC_ANSIBAR_BK, OnAnsiBarBk)	//used by ansi bar
	ON_BN_CLICKED(IDC_ANSIBAR_FG, OnAnsiBarFg)	//used by ansi bar
	ON_BN_CLICKED(IDC_ANSIBAR_BLINK, OnAnsiBarBlink)	//used by ansi bar
	ON_BN_CLICKED(IDC_ANSIBAR_SEND, OnAnsiBarSend)	//used by ansi bar
	ON_BN_CLICKED(IDC_ANSIBAR_SENDDROP, OnAnsiBarSendDropDown)	//used by ansi bar
	ON_UPDATE_COMMAND_UI(IDC_ANSIBAR_BLINK, OnUpdateAnsiBarBlink)	//used by ansi bar
	ON_COMMAND(ID_CUSTOMIZE_MAINTB, OnCustomizeMainToolbar)
	ON_COMMAND(ID_GET_LOCALIP, OnGetLocalIP)
	ON_WM_MENUCHAR()
	ON_UPDATE_COMMAND_UI(ID_EDIT_OPENURL, OnUpdateEditOpenURL)
	ON_COMMAND(ID_BACKUP_CONFIG, OnBackupConfig)
	ON_WM_MEASUREITEM()
	ON_WM_DRAWITEM()
	ON_COMMAND(ID_TOOL_LOCK, OnToolLock)
	ON_UPDATE_COMMAND_UI(ID_SELECTALL, OnUpdateSelectAll)
	ON_COMMAND(ID_CONNECT_CLOSE, OnConnectClose)
	ON_COMMAND(ID_CONNECT_CLOSE_ALL_OTHERS, OnConnectCloseAllOthers)
	ON_UPDATE_COMMAND_UI(ID_DISCONNECT, OnUpdateDisconnect)
	ON_UPDATE_COMMAND_UI(ID_RECONNECT, OnUpdateReconnect)
	ON_COMMAND(ID_EXITLOG, OnSaveSession)
	ON_WM_TIMER()
	ON_COMMAND(ID_FREQ_STR, OnShowFrequentlyUsedStr)
	ON_COMMAND(ID_EDIT_FIND, OnEditFind)
	ON_COMMAND(ID_VIEW_FULLSCR, OnViewFullscr)
	ON_COMMAND(ID_TOOL_SYMBOLS, OnToolSymbols)
	ON_COMMAND(ID_VIEW_CONFIG, OnViewConfig)
	ON_COMMAND(ID_CLEAR_HISTORY, OnClearHistory)
	ON_COMMAND(ID_COLORCONFIG, OnColorConfig)
	ON_COMMAND(ID_CUSTOMIZE, OnCustomize)
	ON_COMMAND(ID_AUTO_DBCS, OnAutoDBCSDetect)
	ON_COMMAND(ID_ADDTOFAVORITE, OnAddToFavorite)
	ON_COMMAND(ID_ADDTOHOME, OnAddToHome)
	ON_COMMAND(ID_SITE_LIST, OnSitesList)
	ON_COMMAND(ID_CONNECT_NEW, OnNewConn)
	ON_COMMAND(ID_UP, OnPrevConn)
	ON_COMMAND(ID_DOWN, OnNextConn)
	ON_COMMAND(ID_LASTCON, OnLastCon)
	ON_COMMAND(ID_COPY, OnCopy)
	ON_COMMAND(ID_COPYPASTE, OnCopyPaste)
	ON_COMMAND(ID_COPYARTICLE, OnCopyArticle)
	ON_COMMAND(ID_COPYARTICLE_WITH_ANSI, OnCopyArticleWithAnsi)
	ON_COMMAND(ID_DOWNLOAD_ARTICLE, OnDownloadArticle)
	ON_COMMAND(ID_PLAY_ANSIMOVIE, OnPlayMovie)
	ON_COMMAND(ID_FONT_BTN, OnFont)
	ON_COMMAND(ID_BBS_FONT, OnBBSFont)
	ON_COMMAND(ID_PASTE, OnPaste)
	ON_COMMAND(ID_PASTETINYURL, OnPasteTinyUrl)
	ON_COMMAND(ID_SELECTALL, OnSelAll)
	ON_COMMAND(ID_FILE_EXIT, OnExit)
	ON_COMMAND(IDM_HELP, OnHelp)
	ON_UPDATE_COMMAND_UI(ID_MOUSE_CTL, OnUpdateBBSMouseCTL)
	ON_COMMAND(ID_MOUSE_CTL, OnBBSMouseCTL)
	ON_COMMAND(ID_SWITCH_BACK, OnSwitchBack)
	ON_UPDATE_COMMAND_UI(ID_SEND_ANSICODE, OnUpdateShowAnsiBar)
	ON_UPDATE_COMMAND_UI(ID_CURCON_SETTINGS, OnUpdateIsBBSSite)
	ON_UPDATE_COMMAND_UI(ID_CONNECT_CLOSE, OnUpdateIsConn)
	ON_UPDATE_COMMAND_UI(ID_COPY, OnUpdateIsSel)
	ON_UPDATE_COMMAND_UI(ID_COPYPASTE, OnUpdateIsSel)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPYURL, OnUpdateIsBBS)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FIND, OnUpdateIsConn)
	ON_UPDATE_COMMAND_UI(ID_COPYARTICLE, OnUpdateIsBBSSite)
	ON_UPDATE_COMMAND_UI(ID_COPYARTICLE_WITH_ANSI, OnUpdateIsBBSSite)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_ARTICLE, OnUpdateIsBBSSite)
	ON_UPDATE_COMMAND_UI(ID_PLAY_ANSIMOVIE, OnUpdateIsBBSSite)
	ON_UPDATE_COMMAND_UI(ID_PASTE, OnUpdatePaste)
	ON_UPDATE_COMMAND_UI(ID_PASTEFILE, OnUpdatePaste)
	ON_UPDATE_COMMAND_UI(ID_PASTETINYURL, OnUpdatePaste)
	ON_UPDATE_COMMAND_UI(ID_ANSICOPY, OnUpdateIsSel)
	ON_UPDATE_COMMAND_UI(ID_ADDTOHOME, OnUpdateIsConn)
	ON_UPDATE_COMMAND_UI(IDC_ANSIBAR_SEND, OnUpdateIsConnnected)	//used by ansi bar
	ON_UPDATE_COMMAND_UI(IDC_ANSIBAR_SENDDROP, OnUpdateIsConnnected)	//used by ansi bar
	ON_UPDATE_COMMAND_UI(ID_SELALL_BUF, OnUpdateIsBBS)
	ON_UPDATE_COMMAND_UI(ID_ADDTOFAVORITE, OnUpdateIsSite)
	ON_COMMAND(ID_KKTAB, OnKKmanStyleTab)
	ON_COMMAND(ID_ADS, OnShowAddressBar)

	ON_COMMAND(ID_INSTANT_TRAN, OnInstantTranslation)
	ON_UPDATE_COMMAND_UI(ID_SET_CHARSET_DEFAULT, OnUpdateSetCharset)
	ON_UPDATE_COMMAND_UI(ID_EXITLOG, OnUpdateSaveSession)
	ON_NOTIFY(TCN_SELCHANGE, IDC_MAINTAB, OnSelchangeTab)
	ON_NOTIFY(NM_RCLICK, IDC_MAINTAB, OnRClickTab)
	ON_WM_ACTIVATEAPP()
	ON_WM_VSCROLL()
	ON_REGISTERED_MESSAGE(WM_COMMIT_UPDATE, OnCommitUpdate)
	ON_REGISTERED_MESSAGE(WM_DOWNLOAD_UPDATE_COMPLETE, OnDownLoadUpdateComplete)
	ON_COMMAND(ID_CHECK_UPDATE, OnCheckUpdate)
	ON_COMMAND(ID_NCIKU, OnNciku)
	ON_COMMAND(ID_WIKIPEDIA, OnWikipedia)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_COPYDATA, OnNewConnection)
	ON_MESSAGE(WM_HOTKEY, OnHotKey)
	ON_MESSAGE(WM_NOTIFYICON, OnNotifyIcon)
	ON_MESSAGE(WM_QUERY_APPCONFIG, OnQueryAppConfig)
	ON_MESSAGE(WM_DOWNLOAD_PAGE, OnDownloadPage)
	ON_COMMAND_RANGE(ID_FIRST_HOTSTR, ID_LAST_HOTSTR, OnFrequentlyUsedStr)	//熱鍵送出字串
	ON_COMMAND_RANGE(ID_FIRST_BBS_FAVORITE, ID_LAST_WEB_FAVORITE, OnFavorite)	//我的最愛
	ON_COMMAND_RANGE(ID_SWITCHCON1, ID_SWITCHCON10, OnHotkeySwitch)	//視窗切換

	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNeedText)	// For tooltips

	#if defined(_COMBO_)
	ON_COMMAND(ID_WWWHOME, OnWebHome)
	ON_COMMAND(ID_ADSHTTP, OnAdsHttp)
	ON_COMMAND(ID_ADSTELNET, OnAdsTelnet)
	ON_COMMAND(ID_NEW_WWW, OnNewWebConn)
	ON_COMMAND(ID_NEW_HOME, OnNewHome)
	ON_COMMAND(ID_AD, OnEditAdFilter)
	ON_COMMAND(ID_WEBPAGE_ADDAD, OnWebPageAddToAdFilter)
	ON_COMMAND(ID_IESETUP, OnIESetup)
	ON_COMMAND(ID_WEBPAGE_PRINT, OnWebPagePrint)
	ON_COMMAND(ID_WEBPAGE_PRINTSETTINGS, OnWebPagePrintSettings)
	ON_COMMAND(ID_WEBPAGE_OPEN, OnWebPageOpen)
	ON_COMMAND(ID_NEW_CURPAGE, OnNewCurPage)
	ON_COMMAND(ID_NEW_IE_CURPAGE, OnNewCurPageInIE)
	ON_COMMAND(ID_WEBPAGE_SAVEAS, OnWebPageSaveAs)
	ON_COMMAND(ID_WEBPAGE_VIEWSRC, OnWebPageViewSrc)
	ON_COMMAND(ID_BLOCK_POPUP, OnBlockPopup)
	ON_COMMAND(ID_SEARCHBAR_CLEANUP,OnSearchbarCleanup)
	ON_COMMAND(ID_ADS_OPENNEW, OnAdsOpenNew)
	ON_COMMAND(ID_TOOL_IMPORT_IEFAV, OnImportIEFavorites)
	ON_COMMAND(ID_TOOL_EXPORT_IEFAV, OnExportIEFavorites)
	ON_COMMAND(ID_WEB_SEARCH, OnWebSearch)
	ON_COMMAND_RANGE(ID_FONT_SMALLEST, ID_FONT_LARGEST, OnBrowserFontSize)
	ON_NOTIFY(TBN_DROPDOWN , IDC_TOOLBAR, OnToolbarMenuDropDown)
	ON_COMMAND(ID_GOBACK, OnGoBack)
	ON_COMMAND(ID_GOFORWARD, OnGoForward)
	ON_UPDATE_COMMAND_UI(ID_NEW_IE_CURPAGE, OnUpdateIsWebPage)
	ON_UPDATE_COMMAND_UI(ID_WEBPAGE_SAVEAS, OnUpdateIsWebPage)
	ON_UPDATE_COMMAND_UI(ID_WEBPAGE_PRINT, OnUpdateIsWebPage)
	ON_UPDATE_COMMAND_UI(ID_WEBPAGE_PRINTSETTINGS, OnUpdateIsWebPage)
	ON_UPDATE_COMMAND_UI(ID_WEBPAGE_ADDAD, OnUpdateIsWebPage)
	ON_UPDATE_COMMAND_UI(ID_WEBPAGE_VIEWSRC, OnUpdateIsWebPage)
	ON_UPDATE_COMMAND_UI(ID_ADS_OPENNEW, OnUpdateAddressBarOpenNew)
	ON_UPDATE_COMMAND_UI(ID_BLOCK_POPUP, OnUpdateBlockPopup)
	ON_UPDATE_COMMAND_UI(ID_SEARCHBAR_CLEANUP, OnUpdateSearchbarCleanup)
	ON_UPDATE_COMMAND_UI(ID_WEBBAR, OnUpdateShowWebBar)
	ON_UPDATE_COMMAND_UI(ID_SEARCHBAR, OnUpdateShowSearchBar)
	ON_COMMAND(ID_WEBBAR, OnShowWebBar)
	ON_COMMAND(ID_CUSTOMIZE_WB, OnCustomizeWebBar)
	ON_MESSAGE(WM_REMOVE_WEBCONN, OnRemoveWebConn)
	ON_COMMAND(ID_SEARCHBAR_FOCUS, OnSearchBarFocus)
	ON_COMMAND(ID_SEARCHBAR, OnShowSearchBar)
	#endif

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

//ACCEL* CMainFrame::accels;


CMainFrame::CMainFrame()
{
#if defined(_COMBO_)
	CWebBrowser::parent = this;
#endif
	accels = NULL;

	main_menu = NULL;
	edit_menu = NULL;
	auto_dbcs_menu = NULL;
	bbs_fav_menu = NULL;
	ansi_menu = NULL;
	history_menu = NULL;

	prev_conn = NULL;

	/* FIXME */
	g_ucs2conv.SetTablePath(AppPath);
	g_ucs2conv.InitBig52Ucs2Tab();
	g_ucs2conv.InitUcs22Big5Tab();
	g_pView = &view;
	
	setCharset = false;

#if defined(_COMBO_)
	focus = NULL;
#endif
}

CMainFrame::~CMainFrame()
{
	DestroyAcceleratorTable(m_hAccelTable);
	if (accels)
		delete []accels;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
//	用來建造control的temp Rect
	CRect tmprc(0, 0, 0, 0);

//	Load PCMan Icon
	icon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(icon, TRUE);	SetIcon(icon, FALSE);

	CBitmap imglist_bmp;
//	Load Image List for Site List and Tab...
	imglist_bmp.Attach((HBITMAP)LoadImage(AfxGetInstanceHandle(), ConfigPath + ICON_BMP_FILENAME, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));
	img_icons.Create(16, 16, ILC_COLOR32 | ILC_MASK, 9, 0);
	ImageList_AddMasked(img_icons.m_hImageList, (HBITMAP)imglist_bmp.m_hObject, RGB(255, 0, 255));

//	Create font for UI
	LOGFONT lf;	::GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	lf.lfHeight = -12;	bar_font.CreateFontIndirect(&lf);

//	Create Main Toolbar
	SIZE sizebtn;
	SIZE sizeimg;
	BITMAP bmp;
	toolbar.CreateEx(this,  TBSTYLE_FLAT, CCS_ADJUSTABLE | CBRS_ALIGN_TOP | CBRS_TOOLTIPS |
					 WS_CHILD | WS_VISIBLE, tmprc, IDC_TOOLBAR);
	
	CBitmap toolbar_bkgnd;

	toolbar_bkgnd.Attach((HBITMAP)LoadImage(AfxGetInstanceHandle(), ConfigPath + TOOLBAR_BMP_FILENAME,
											IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_LOADTRANSPARENT));
	toolbar_bkgnd.GetBitmap(&bmp);

	//img_toolbar.Create(bmp.bmHeight, bmp.bmHeight, ILC_COLOR32|ILC_MASK, 9,0);
	//使用 bmBitsPixel 來當作 ILC_COLOR* 的 flags 值
#ifdef _COMBO_
	img_toolbar.Create(bmp.bmHeight, bmp.bmHeight, bmp.bmBitsPixel | ILC_MASK, 19, 0);
#else
	img_toolbar.Create(bmp.bmHeight, bmp.bmHeight, bmp.bmBitsPixel | ILC_MASK, 17, 0);
#endif
	ImageList_AddMasked(img_toolbar.m_hImageList, (HBITMAP)toolbar_bkgnd.m_hObject, RGB(190, 190, 190));

#ifdef _COMBO_
	toolbar.GetToolBarCtrl().SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);
	//bmp.bmWidth/=19;
#else
	//bmp.bmWidth/=17;
#endif
	bmp.bmWidth = bmp.bmHeight;
	sizeimg.cx = bmp.bmWidth;
	sizeimg.cy = bmp.bmHeight;
	sizebtn.cx = bmp.bmWidth + 7;
	sizebtn.cy = bmp.bmHeight + 6;
	toolbar.SetSizes(sizebtn, sizeimg);
	//toolbar.SetBitmap((HBITMAP)toolbar_bkgnd.m_hObject);
	toolbar.LoadToolBar(&AppConfig.main_toolbar_inf);
	toolbar.GetToolBarCtrl().SetImageList(&img_toolbar);
//	Create Tab
	tab.parent = this;
	tab.Create(
		(AppConfig.tab_button ? (TCS_FLATBUTTONS | TCS_BUTTONS)
		 :	(AppConfig.kktab ? TCS_BOTTOM : 0))
		| WS_VISIBLE | TCS_HOTTRACK | TCS_FOCUSNEVER | TCS_TOOLTIPS,
		tmprc, this, IDC_MAINTAB);
//	非常詭異，意外發現，如果一開始建立OwnerDraw，Win XP下HotTrack就會失效
//	但是如果先建立一般的HotTrack Tab，再改成OwnerDraw，就可以保留HotTrack :)
#if defined(_COMBO_)
	tab.ModifyStyle(0, TCS_FIXEDWIDTH | TCS_OWNERDRAWFIXED);
#else
	tab.ModifyStyle(0, TCS_OWNERDRAWFIXED);
#endif
//	tab.ModifyStyle(0,TCS_BOTTOM);
//--------------------------------------------------------------------------
	tab.SetFont(&bar_font);
	tab.SetImageList(&img_icons);
	tab_popup_menu =::LoadMenu(AfxGetInstanceHandle(), LPSTR(IDR_POPUP));
	tab_popup_menu =::GetSubMenu(tab_popup_menu, 0);
#ifdef	_COMBO_
	webtab_popup_menu =::LoadMenu(AfxGetInstanceHandle(), LPSTR(IDR_POPUP2));
	webtab_popup_menu =::GetSubMenu(webtab_popup_menu, 0);
#endif

//	Create Address Bar
//----------位址列-------------
	address_bar.Create(CBS_AUTOHSCROLL | CBS_DROPDOWN, CRect(0, 0, 0, 320), this, IDC_ADS_COMBO);
	address_bar.MoveWindow(0, 0, 200, 24);
	address_bar.SetFont(&bar_font);
	HWND hedit =::GetTopWindow(address_bar.m_hWnd);
	old_address_bar_proc = (WNDPROC)::GetWindowLong(hedit, GWL_WNDPROC);
	::SetWindowLong(hedit, GWL_WNDPROC, (LONG)AddressBarWndProc);
	::SetWindowLong(hedit, GWL_STYLE,::GetWindowLong(hedit, GWL_STYLE)&~ES_NOHIDESEL);
	auto_complete.AttachEdit(hedit, &AppConfig.history, address_bar.m_hWnd);

#ifdef	_COMBO_
// Search Bar
//----------搜尋列------------
	search_bar.Create(this);
	search_bar.SetFont(&bar_font);
#endif

//	Create Close Button
	close_btn.CreateEx(this, TBSTYLE_TRANSPARENT | TBSTYLE_FLAT, CBRS_ALIGN_TOP | WS_CHILD | WS_VISIBLE |
					   CBRS_TOOLTIPS, tmprc, IDC_CLOSEBTNBAR);
	close_btn.LoadToolBar(IDR_CLOSEBTN);

#if defined(_COMBO_)
//----------Web工具列-------------
	web_bar.CreateEx(this, TBSTYLE_TRANSPARENT | TBSTYLE_FLAT, CBRS_TOOLTIPS | CBRS_ALIGN_TOP | CCS_ADJUSTABLE | WS_CHILD |
					 WS_VISIBLE, tmprc, IDC_WEBBAR);
	web_bar_bkgnd.Attach((HBITMAP)LoadImage(AfxGetInstanceHandle(), ConfigPath + WEB_ICON_BMP_FILENAME,
											IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));
	web_bar_bkgnd.GetBitmap(&bmp);
	img_webbar.Create(bmp.bmHeight, bmp.bmHeight, bmp.bmBitsPixel | ILC_MASK, 5, 0);
	ImageList_AddMasked(img_webbar.m_hImageList, (HBITMAP)web_bar_bkgnd.m_hObject, RGB(192, 192, 192));
	web_bar.SetSizes(CSize(23, 22), CSize(16, 16));
	//web_bar.SetBitmap((HBITMAP)web_bar_bkgnd.m_hObject);
	web_bar.LoadToolBar(&AppConfig.webbar_inf);
	web_bar.GetToolBarCtrl().SetImageList(&img_webbar);
//位址列的 combobox 資料在 LoadHistory裡面載入
#endif

//-----------------------------

//	Create the Main BBS View
	view.Create(NULL, NULL, WS_VISIBLE, tmprc, this, 1);
	view.parent = this;

//	Create Status Bar	(Static control)
	status_bar.Create(NULL, WS_CLIPCHILDREN | WS_CHILD | SS_SUNKEN | SS_CENTERIMAGE, tmprc, this, IDC_STATUS);
	status_bar.SetFont(&bar_font);

#if defined(_COMBO_)
//	Create Progress Control (網頁)
	progress_bar.Create(WS_CLIPSIBLINGS | WS_CHILD | PBS_SMOOTH, tmprc, &status_bar, IDC_PROGRESS);
#endif

//	Create Ansi Bar
	ansi_bar.Create(this, IDD_ANSIBAR, 0, 0);
	ansi_bar.view = &view;

//	Create ReBar
	rebar.Create(this, RBS_BANDBORDERS | RBS_DBLCLKTOGGLE | CBRS_TOOLTIPS);
	CReBarCtrl& rbc = rebar.GetReBarCtrl();
	REBARBANDINFO rbi;	rbi.cbSize = sizeof(rbi);
	rbi.fMask = RBBIM_ID;
	//加入主工具列	id=1
	rebar.AddBar(&toolbar);	rbi.wID = 1; rbc.SetBandInfo(0, &rbi);

	//加入ANSI彩色工具列	id=2
	rebar.AddBar(&ansi_bar);
	rbi.cx = 548;
	rbi.fMask = RBBIM_ID | RBBIM_SIZE;
	rbi.wID = 2;	rbc.SetBandInfo(1, &rbi);
	rbi.fMask = RBBIM_ID;

	//加入關閉按鈕	id=3
	rebar.AddBar(&close_btn);	rbi.wID = 3;	rbc.SetBandInfo(2, &rbi);

#ifdef	_COMBO_
	//加入位址列	id=4
	rebar.AddBar(&address_bar);
#else
	rebar.AddBar(&address_bar, LoadString(IDS_ADS_BAR_TITLE));
#endif
	rbi.fStyle = RBBS_BREAK | RBBS_GRIPPERALWAYS | RBBS_FIXEDBMP;
	rbi.fMask = RBBIM_ID | RBBIM_STYLE;
	rbi.wID = 4;	rbc.SetBandInfo(3, &rbi);
	rbi.fStyle = RBBS_GRIPPERALWAYS | RBBS_FIXEDBMP;
	rbi.fMask = RBBIM_ID;
#if defined(_COMBO_)
	//加入Web工具列	id=5
	rebar.AddBar(&web_bar);	rbi.wID = 5;	rbc.SetBandInfo(4, &rbi);
	//加入搜尋列 id=6
	rebar.AddBar(&search_bar, LoadString(IDS_SEARCH_BAR_TITLE));
	rbi.wID = 6; rbc.SetBandInfo(5, &rbi);
#endif

	//ReBar Band visibility
	RecalcLayout();
	//ReBar Position
	CReBarBandPos::Restore(rebar.GetReBarCtrl(), AppConfig.rebar_bands);

//	Load History
	LoadHistory();

//	Load FUS
	LoadFrequentlyUsedStr();

	if (AppConfig.is_full_scr)
	{
		AppConfig.is_full_scr = 0;
		OnViewFullscr();
	}

//	Call Default function to RecalcLayout
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.style = WS_OVERLAPPEDWINDOW;
	RegWndClass(mainfrm_class_name, AfxGetAfxWndProc(), NULL, NULL, AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	cs.lpszClass = mainfrm_class_name;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
	view.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if (view.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);
	if (nType == SIZE_MINIMIZED)
	{
		if (AppConfig.minimize_to_tray && !AppConfig.lock_pcman)
			MinimizeToTray();
	}
	/*	else if(nType == SIZE_RESTORED)
		{
	 		if( !IsWindowVisible() )
	 			ShowWindow(SW_SHOW);
		}
	*/
}

//DEL void CMainFrame::OnSysCommand( UINT nID, LPARAM lParam )
//DEL {
//DEL 	CFrameWnd::OnSysCommand( nID, lParam );
//DEL 	switch( nID )
//DEL 	{
//DEL 	case SC_MINIMIZE:
//DEL 		break;
//DEL 	case SC_RESTORE:
//DEL 		if( !IsWindowVisible() )
//DEL 			ShowWindow(SW_SHOW);
//DEL 		break;
//DEL 	}
//DEL }

void CMainFrame::OnClose()
{
#ifdef _COMBO_
	if (view.con && AppConfig.close_query)
#else
	if (view.telnet && AppConfig.close_query)
#endif

		if (MessageBox(LoadString(IDS_EXIT_CONFIRM), LoadString(IDS_CONFIRM), MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL)
			return;
    
	OnCommitUpdate(NULL, NULL);
}


LRESULT CMainFrame::OnDownLoadUpdateComplete(WPARAM wparam, LPARAM lparam)
{
	CDownloadUpdateDlg *dlg = (CDownloadUpdateDlg *) wparam;
	dlg->OnCancel();
	return 0;
}

LRESULT CMainFrame::OnCommitUpdate(WPARAM wparam, LPARAM lparam)
{
	WSACancelBlockingCall();

	SaveHistory();

	auto_complete.DetachEdit();

	//視窗大小位置
	AppConfig.mainwnd_state.Save(m_hWnd);

	//ReBar Position
	CReBarBandPos::Save(rebar.GetReBarCtrl(), AppConfig.rebar_bands);
	AppConfig.main_toolbar_inf.GetState(toolbar);

	UnregisterHotKey(m_hWnd, 1);

	AppConfig.Save(ConfigPath + CONFIG_FILENAME);
	ShowWindow(SW_HIDE);

//	清除所有 new 出來的分頁物件
	int c = tab.GetItemCount();
	if (AppConfig.save_session)
	{
		CFile logf;
		if (logf.Open(ConfigPath + SESSION_FILENAME,
					  CFile::modeWrite | CFile::modeCreate))
		{
			for (int i = 0; i < c;  i++)
			{
				CConn* pcon = tab.GetCon(i);
				if (pcon->is_ansi_editor)
					continue;
				CString line = (pcon->is_web ? 'w' : 's') + pcon->name;
				line += '\t';
				if (0 == strnicmp("telnet://", pcon->address, 9))	// BBS
					line += LPCTSTR(pcon->address) + 9;	// skip "telnet://"
				else	// Web
					line += pcon->address;
#ifdef	_COMBO_
				// FIXME: should this be pcon->is_telnet? what about ansi editor?
				if (!pcon->is_web)
				{
#endif
					if (static_cast<CTelnetConn*>(pcon)->port != 23)
					{
						char port_str[16];
						sprintf(port_str, ":%d", static_cast<CTelnetConn*>(pcon)->port);
						line += port_str;
					}
					CTelnetConn* telnet = static_cast<CTelnetConn*>(pcon);
					if (! telnet->cfg_path.IsEmpty())
					{
						line += '\t';
						line += telnet->cfg_path.Left(telnet->cfg_path.GetLength() - telnet->name.GetLength());
					}
#ifdef	_COMBO_
				}		// end if( !pcon->is_web )
#endif
				line += "\r\n";
				logf.Write(LPCTSTR(line), line.GetLength());
				delete pcon;
			}
			logf.Close();
		}
		else
		{
			for (int i = 0; i < c;  i++)
				delete tab.GetCon(i);
		}
	}
	else	//不紀錄，只刪除分頁
	{
		for (int i = 0; i < c;  i++)
			delete tab.GetCon(i);
	}

	CFrameWnd::OnClose();
	return 0;
}

void CMainFrame::UpdateStatus()
{
	static CString con_status = LoadString(IDS_CON_STATUS);
	static CString ansied_status = LoadString(IDS_ANSIED_STATUS);
	CTelnetConn* telnet = view.telnet;
	CString text;
	if (telnet)
	{
		if (! telnet->is_ansi_editor)
		{
			int sec = telnet->time % 60;
			int min = (telnet->time - sec) / 60;
			int hr = telnet->time / 3600;
			min = min % 60;
//			text.Format("\t連線時間：%d 小時 %d 分 %d 秒\t\t位址：%s",hr,min,sec,(LPCTSTR)telnet->address);
			text.Format(con_status, hr, min, sec, (LPCTSTR)telnet->address);

			char *szMouseGesture;

			szMouseGesture = MouseCTL_GetStatusInfo();

			if (szMouseGesture && szMouseGesture[0])
			{
				text += "\t\t" + LoadString(IDS_MOUSE_GESTURE) + ": ";
				text += szMouseGesture;
			}
		}
		else
		{
			CString mode;
			mode.LoadString(telnet->insert_mode ? IDS_INSERT : IDS_REPLACE);
			CString file;
			if (telnet->address.IsEmpty())
				file.LoadString(IDS_NOT_SAVED);
			else
				file = telnet->address;
//			text.Format( "ANSI彩色編輯\t編輯模式：%s\t檔案：%s", LPCTSTR(mode), LPCTSTR(file) );
			text.Format(ansied_status, LPCTSTR(mode), LPCTSTR(file));
		}
		status_bar.SetWindowText(text);
	}
	else
		status_bar.SetWindowText("");
}

void CMainFrame::OnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	int sel = tab.GetCurSel();
	if (sel == -1)
		return;

	SwitchToConn(sel);
}

LRESULT CMainFrame::OnNewConnection(WPARAM wparam, LPARAM lparam)
{
	COPYDATASTRUCT* pd = (COPYDATASTRUCT*)lparam;
	OnNewConnectionAds((LPSTR)pd->lpData);
	return 0;
}

void CMainFrame::OnRClickTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	TCHITTESTINFO hti;
	GetCursorPos(&hti.pt);
	POINT pt = hti.pt;
	tab.ScreenToClient(&hti.pt);
	int sel = tab.HitTest(&hti);
	if (sel != -1)
	{
		SwitchToConn(sel);
		CConn* seltelnet = tab.GetCon(sel);
		HMENU popup;
#ifdef	_COMBO_
		if (seltelnet->is_web)
			popup = webtab_popup_menu;
		else
#endif
			if (seltelnet->is_ansi_editor)
				popup = ansi_menu;
			else
				popup = tab_popup_menu;

		int pos = AppConfig.kktab ? GetMenuItemCount(popup) : 0;
		InsertMenu(popup, pos, MF_SEPARATOR | MF_BYPOSITION, 0, NULL);
		CString close_this_page;
		CString close_all_other_pages;
		close_this_page.LoadString(IDS_CLOSE_THIS_PAGE);
		close_all_other_pages.LoadString(IDS_CLOSE_ALL_OTHER_PAGES);
		if (0 == pos)
		{
			InsertMenu(popup, 0, MF_STRING | MF_BYPOSITION, ID_CONNECT_CLOSE_ALL_OTHERS, close_all_other_pages);
			InsertMenu(popup, 0, MF_STRING | MF_BYPOSITION, ID_CONNECT_CLOSE, close_this_page);
			pos+=2;
		}
		else
			InsertMenu(popup, pos + 1, MF_STRING | MF_BYPOSITION, ID_CONNECT_CLOSE, close_this_page);

		::TrackPopupMenu(popup, TPM_RIGHTBUTTON | TPM_LEFTALIGN, pt.x, pt.y, 0, m_hWnd, NULL);
		DeleteMenu(popup, pos, MF_BYPOSITION);
		DeleteMenu(popup, ID_CONNECT_CLOSE_ALL_OTHERS, MF_BYCOMMAND);
		DeleteMenu(popup, ID_CONNECT_CLOSE, MF_BYCOMMAND);

	}
	*pResult = 0;
}

void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	if (nState != WA_INACTIVE)
	{
		if (view.doflash)
		{
			FlashWindow(FALSE);
			view.doflash = 0;
		}
#if !defined(_COMBO_)
#else
		::SetFocus(focus ? focus : view.m_hWnd);
#endif
	}

#if !defined(_COMBO_)
#else
	//	else if(::GetFocus())
	focus =::GetFocus();
#endif
}

void CMainFrame::OnShowTabBar()
{
	BYTE& showtab = AppConfig.is_full_scr ? AppConfig.fullscr_showtab : AppConfig.showtab;
	showtab = !showtab;
	RecalcLayout(FALSE);
}

void CMainFrame::OnShowToolbar()
{
	BYTE& showtb = AppConfig.is_full_scr ? AppConfig.fullscr_showtb : AppConfig.showtb;
	showtb = !showtb;
	RecalcLayout();
}

#ifdef	_COMBO_
void CMainFrame::OnShowWebBar()
{
	BYTE& showwb = AppConfig.is_full_scr ? AppConfig.fullscr_showwb : AppConfig.showwb;
	showwb = !showwb;
	RecalcLayout();
}
#endif

void CMainFrame::OnCloseBtn()
{
	BYTE& showclose = AppConfig.is_full_scr ? AppConfig.fullscr_showclose : AppConfig.showclose;
	showclose = !showclose;
	RecalcLayout();
}

void CMainFrame::OnShowStatusbar()
{
	BYTE& showsb = AppConfig.is_full_scr ? AppConfig.fullscr_showsb : AppConfig.showsb;
	showsb = !showsb;
	status_bar.ShowWindow(showsb ? SW_SHOW : SW_HIDE);
	RecalcLayout();
}

void CMainFrame::OnMove(int x, int y)
{
	CFrameWnd::OnMove(x, y);
	if (AppConfig.bktype == 1)
		view.Invalidate(FALSE);
}

void CMainFrame::OnNewConnectionAds(LPCTSTR cmdline)
{
	if (cmdline[0] == '\0')
		return;

	AddToTypedHistory(cmdline);

	if (0 == strnicmp(cmdline, "telnet:", 7))
	{
		cmdline += 7;
		while (*cmdline == '/')
			cmdline++;
	}
	else if (0 == strnicmp(cmdline, "bbs:", 4))
	{
		cmdline += 4;
		while (*cmdline == '/')
			cmdline++;
	}
	else if (IsFileExist(cmdline))
	{
		int l = strlen(cmdline);
		if (l > 4)
		{
			if (0 == strnicmp(cmdline + (l - 4), ".ans", 4))
			{
				if (view.OpenAnsFile(cmdline))
					return;
			}
			else
			{
#if defined(_COMBO_)
				view.ConnectWeb(cmdline, TRUE);
#endif
				return;
			}
		}
	}
#if defined(_COMBO_)
	else
	{
		view.ConnectWeb(cmdline, TRUE);
		return;
	}
#endif

	CString param = cmdline;
	if (param[param.GetLength()-1] == '/')
		param = param.Left(param.GetLength() - 1);

	int pos = param.Find(':');

	CString address;
	unsigned short port;
	if (pos == -1)
	{
		address = param;
		port = 23;
	}
	else
	{
		address = param.Left(pos);
		CString port_str = param.Mid(pos + 1);
		port = (unsigned short)atoi(port_str);
		if (port == 23)
			param = address;
	}
	view.Connect(address, param, port);
	if(!setCharset)
	{
		switch(AppConfig.saved_charset)
		{
			case 0:
				break;
			
			case 1:
				this->OnSetCharset(ID_SET_CHARSET_DEFAULT);
				break;
			case 2:
				this->OnSetCharset(ID_SET_CHARSET_CP950);
				break;
			case 3:
				this->OnSetCharset(ID_SET_CHARSET_CP936);
				break;
			case 4:
				this->OnSetCharset(ID_SET_CHARSET_CP932);
				break;
			case 5:
				this->OnSetCharset(ID_SET_CHARSET_UTF8);
				break;
		}	
	}
	BringWindowToTop();
}


void CMainFrame::OnPgd()
{
	view.PostMessage(WM_VSCROLL, SB_PAGEDOWN, NULL);
}

void CMainFrame::OnPgu()
{
	if (!view.telnet)
		return;
	view.PostMessage(WM_VSCROLL, SB_PAGEUP, NULL);
}

void CMainFrame::OnShowAddressBar()
{
	BYTE& showads = AppConfig.is_full_scr ? AppConfig.fullscr_showads : AppConfig.showads;
	showads = !showads;
	RecalcLayout(FALSE);
}

void CMainFrame::OnInstantTranslation()
{
	CTranslationDlg tranDlg;
	tranDlg.DoModal();
}

void CMainFrame::OnKKmanStyleTab()
{
	AppConfig.kktab = !AppConfig.kktab;
	if (!AppConfig.showtab)
		return;

	if (AppConfig.kktab)
		tab.ModifyStyle(0, TCS_BOTTOM);
	else
		tab.ModifyStyle(TCS_BOTTOM, 0);

	CRect view_rect;
	view.GetWindowRect(view_rect);
	ScreenToClient(view_rect);
	if (AppConfig.kktab)
	{
		view_rect.top -= TABH;
		view.SetWindowPos(&wndTop, 0, view_rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		tab.SetWindowPos(&wndTop, 0, view_rect.bottom - TABH, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}
	else
	{
		view.SetWindowPos(&wndTop, 0, view_rect.top + TABH, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		tab.SetWindowPos(&wndTop, 0, view_rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		view_rect.top += TABH;
	}
}

void CMainFrame::UpdateAddressBar()
{
#if defined(_COMBO_)
	if (view.con)
	{
		SetWindowText(view.con->name + window_title);
		CString address = view.con->address;
		if (view.telnet && !view.telnet->is_ansi_editor)
		{
			if (view.telnet->port != 23)
			{
				char port_str[16];
				sprintf(port_str, ":%d", view.telnet->port);
				address += port_str;
			}
		}
		address_bar.SetWindowText(address);
	}
	else
	{
		address_bar.SetWindowText(NULL);
		SetWindowText(window_title + 3);
	}
#else
	if (view.telnet)
	{
		CString address = view.telnet->address;
		if (!view.telnet->is_ansi_editor)
		{
			if (view.telnet->port != 23)
			{
				char port_str[16];
				sprintf(port_str, ":%d", view.telnet->port);
				address += port_str;
			}
		}
		address_bar.SetWindowText(address);
	}
	else
		address_bar.SetWindowText("");
#endif

}

void CMainFrame::OnAddressFocus()
{
	address_bar.SetFocus();
}

#if defined(_COMBO_)
void CMainFrame::OnShowSearchBar()
{
	BYTE& showsearchbar = AppConfig.is_full_scr ? AppConfig.fullscr_showsearchbar : AppConfig.showsearchbar;
	showsearchbar = !showsearchbar;
	RecalcLayout(FALSE);
}

void CMainFrame::OnSearchBarFocus()
{
	search_bar.SetEditFocus();
}
#endif

/* Menu 和 Command Item的結構
檔案內部儲存方式:
WORD MAIN_ITEM_COUNT
CMDITEM ITEMS[TOTAL_COUNT]
DWORD ACCELCOUNT
ACCEL ACC_ITEMS[ACCELCOUNT]

struct CMDITEM
{
	BYTE TYPE,	CT_MENU,CT_HAS_SUB,CT_CMD,如果TYPE=0則為Separator，後面幾項都沒有
	WORD ID_OR_SUBCOUNT		如果有CT_HAS_SUB,為SUBCOUNT,如果沒有則為ID
	WORD state
	WORD LEN	TEXT的長度,含0
	CHAR TEXT[]		長度不定,0結尾
}
*/

char* UIAddMenu(char* buf, HMENU menu)
{
	BYTE type = *buf;
	//	*buf=TYPE;
	buf++;
	WORD count = *(WORD*)buf;	//取得sub item count
	buf += 6 + *(WORD*)(buf + 4);	//到第一個sub item

	while (count)
	{
		LPSTR text;
		WORD len;
		if (!*buf)	//Separator
		{
			AppendMenu(menu, MF_SEPARATOR, 0, NULL);
			buf++;
		}
		else
		{
			text = buf + 7;
			len = *(WORD*)(buf + 5);
			if (*buf & CT_HAS_SUB)
			{
				if (*buf & CT_MENU)	//如果是一般選單項目才加入選單
				{
					HMENU sub = CreatePopupMenu();
					AppendMenu(menu, MF_STRING | MF_POPUP | LOBYTE(*(WORD*)(buf + 3)), (UINT)sub, text);
					buf = UIAddMenu(buf, sub);
				}
				else	//不是一般選單項目
					buf = UIAddMenu(buf, NULL);
			}
			else
			{
				//buf=ID
				if (*buf & CT_MENU)	//如果是一般選單項目才加入選單,不是選單，就直接略過
					AppendMenu(menu, MF_STRING | *(WORD*)(buf + 3), *(WORD*)(buf + 1), text);
				buf += 7 + len;	//Next item
			}
		}
		count--;
	}
	return buf;
}

void CMainFrame::OnShowAnsiBar()
{
	BYTE& use_ansi_bar = AppConfig.is_full_scr ? AppConfig.full_use_ansi_bar : AppConfig.use_ansi_bar;
	use_ansi_bar = !use_ansi_bar;
	RecalcLayout(FALSE);
}


void CMainFrame::OnShowScroll()
{
	if (view.telnet)
	{
		view.telnet->site_settings.showscroll = !view.telnet->site_settings.showscroll;
		AppConfig.site_settings.showscroll = view.telnet->site_settings.showscroll;
		view.ShowScrollBar(SB_VERT, view.telnet->site_settings.showscroll);
	}
	else
	{
		AppConfig.site_settings.showscroll = !AppConfig.site_settings.showscroll;
		view.ShowScrollBar(SB_VERT, AppConfig.site_settings.showscroll);
	}
}

void CMainFrame::OnSetDefaultProgram()
{
	CString tmp;
	GetModuleFileName(NULL, tmp.GetBuffer(MAX_PATH + 1), MAX_PATH + 1);
	tmp.ReleaseBuffer();
	CString cmd = tmp + " %1";
	HKEY hk = NULL;

	if (MessageBox(LoadString(IDS_SET_DEFALT_TELNET), LoadString(IDS_CONFIRM), MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		RegCreateKey(HKEY_CLASSES_ROOT, "telnet\\shell\\PCMan\\command", &hk);
		RegSetValue(hk, NULL, REG_SZ, (LPCTSTR)cmd, cmd.GetLength());
		RegCloseKey(hk);
		RegSetValue(HKEY_CLASSES_ROOT, "telnet\\shell", REG_SZ, "PCMan", 5);
	}
	else
	{
		RegSetValue(HKEY_CLASSES_ROOT, "telnet\\shell", REG_SZ, "open", 4);
	}

#ifdef	_COMBO_
	if (MessageBox(LoadString(IDS_SET_DEFAULT_BROWSER), LoadString(IDS_CONFIRM), MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		RegCreateKey(HKEY_CLASSES_ROOT, "http\\shell\\PCMan\\command", &hk);
		RegSetValue(hk, NULL, REG_SZ, (LPCTSTR)cmd, cmd.GetLength());
		RegCloseKey(hk);
		RegSetValue(HKEY_CLASSES_ROOT, "http\\shell", REG_SZ, "PCMan", 5);

		RegCreateKey(HKEY_CLASSES_ROOT, "https\\shell\\PCMan\\command", &hk);
		RegSetValue(hk, NULL, REG_SZ, (LPCTSTR)cmd, cmd.GetLength());
		RegCloseKey(hk);
		RegSetValue(HKEY_CLASSES_ROOT, "https\\shell", REG_SZ, "PCMan", 5);

		RegSetValue(HKEY_CLASSES_ROOT, ".htm", REG_SZ, "htmlfile", 8);
		RegSetValue(HKEY_CLASSES_ROOT, ".html", REG_SZ, "htmlfile", 8);
		RegSetValue(HKEY_CLASSES_ROOT, ".mht", REG_SZ, "mhtmlfile", 8);
		RegCreateKey(HKEY_CLASSES_ROOT, "htmlfile\\shell\\PCMan\\command", &hk);
		RegSetValue(hk, NULL, REG_SZ, (LPCTSTR)cmd, cmd.GetLength());
		RegCloseKey(hk);
		RegCreateKey(HKEY_CLASSES_ROOT, "mhtmlfile\\shell\\PCMan\\command", &hk);
		RegSetValue(hk, NULL, REG_SZ, (LPCTSTR)cmd, cmd.GetLength());
		RegCloseKey(hk);
		RegSetValue(HKEY_CLASSES_ROOT, "htmlfile\\shell", REG_SZ, "PCMan", 5);
		RegSetValue(HKEY_CLASSES_ROOT, "mhtmlfile\\shell", REG_SZ, "PCMan", 5);

		RegCreateKey(HKEY_CLASSES_ROOT, "InternetShortcut\\shell\\PCMan\\command", &hk);
		RegSetValue(hk, NULL, REG_SZ, (LPCTSTR)cmd, cmd.GetLength());
		RegCloseKey(hk);
		RegSetValue(HKEY_CLASSES_ROOT, "InternetShortcut\\shell", REG_SZ, "PCMan", 5);
	}
	else
	{
		RegSetValue(HKEY_CLASSES_ROOT, "http\\shell", REG_SZ, "open", 4);
		RegDeleteKey(HKEY_CLASSES_ROOT, "http\\shell\\PCMan");
		RegDeleteKey(HKEY_CLASSES_ROOT, "https\\shell\\PCMan");
		RegDeleteKey(HKEY_CLASSES_ROOT, "htmlfile\\shell\\PCMan");
		RegDeleteKey(HKEY_CLASSES_ROOT, "mhtmlfile\\shell\\PCMan");
		RegDeleteKey(HKEY_CLASSES_ROOT, "InternetShortcut\\shell\\PCMan");
	}
#endif // #ifdef	_COMBO_

	if (MessageBox(LoadString(IDS_SET_TO_DEFAULT_ANS), LoadString(IDS_CONFIRM), MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		RegCreateKey(HKEY_CLASSES_ROOT, ".ans", &hk);
		RegSetValue(hk, NULL, REG_SZ, "ansfile", 7);
		RegCloseKey(hk);
		RegCreateKey(HKEY_CLASSES_ROOT, "ansfile\\DefaultIcon", &hk);
		RegSetValue(hk, NULL, REG_SZ, (LPCTSTR)(tmp + ",1"), tmp.GetLength() + 2);
		RegCloseKey(hk);
		RegCreateKey(HKEY_CLASSES_ROOT, "ansfile\\Shell\\Open\\Command", &hk);
		RegSetValue(hk, NULL, REG_SZ, (LPCTSTR)(tmp + " %1"), tmp.GetLength() + 3);
		RegCloseKey(hk);
	}
	else
	{
		RegDeleteKey(HKEY_CLASSES_ROOT, ".ans");
		RegDeleteKey(HKEY_CLASSES_ROOT, "ansfile");
	}

	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST | SHCNF_FLUSHNOWAIT, NULL, NULL);
}

void CMainFrame::OnAutoSwitch()
{
	view.auto_switch = !view.auto_switch;
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	return (CFrameWnd::PreTranslateMessage(pMsg) ||
			(hhotstr_acc ?
			 ::TranslateAccelerator(m_hWnd, hhotstr_acc, pMsg) : 0));
}

void CMainFrame::OnUpdateEditorInsertMode(CCmdUI* pCmdUI)
{
	if (view.telnet)
	{
		BOOL bansied = view.telnet->is_ansi_editor;
		pCmdUI->Enable(bansied);
		pCmdUI->SetCheck(bansied && view.telnet->insert_mode);
	}
	else
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnUpdateAutoswitch(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(view.auto_switch);
}

void CMainFrame::OnUpdateKKmanStyleTab(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(AppConfig.kktab);
}

void CMainFrame::OnUpdateShowScroll(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(view.telnet ? (view.telnet->site_settings.showscroll) : AppConfig.site_settings.showscroll);
}

void CMainFrame::OnUpdateSmoothDraw(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(AppConfig.smooth_draw);
}

void CMainFrame::OnUpdateShowStatusbar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(AppConfig.is_full_scr ? AppConfig.fullscr_showsb : AppConfig.showsb);
}

void CMainFrame::OnUpdateShowTabBar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(AppConfig.is_full_scr ? AppConfig.fullscr_showtab : AppConfig.showtab);
}

#ifdef	_COMBO_
void CMainFrame::OnUpdateShowWebBar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(AppConfig.is_full_scr ? AppConfig.fullscr_showwb : AppConfig.showwb);
}
void CMainFrame::OnUpdateShowSearchBar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(AppConfig.is_full_scr ? AppConfig.fullscr_showsearchbar : AppConfig.showsearchbar);
}
#endif

void CMainFrame::OnUpdateShowToolbar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(AppConfig.is_full_scr ? AppConfig.fullscr_showtb : AppConfig.showtb);
}

void CMainFrame::OnUpdateAutoDBCSArrow(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(view.telnet ? view.telnet->site_settings.auto_dbcs_arrow : AppConfig.site_settings.auto_dbcs_arrow);
}

void CMainFrame::OnUpdateAutoDBCSBackspace(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(view.telnet ? view.telnet->site_settings.auto_dbcs_backspace : AppConfig.site_settings.auto_dbcs_backspace);
}

void CMainFrame::OnAutoDBCSBackspace()
{
	if (view.telnet)
		view.telnet->site_settings.auto_dbcs_backspace = !view.telnet->site_settings.auto_dbcs_backspace;
	else
		AppConfig.site_settings.auto_dbcs_backspace = !AppConfig.site_settings.auto_dbcs_backspace;
}

void CMainFrame::OnAutoDBCSArrow()
{
	if (view.telnet)
		view.telnet->site_settings.auto_dbcs_arrow = !view.telnet->site_settings.auto_dbcs_arrow;
	else
		AppConfig.site_settings.auto_dbcs_arrow = !AppConfig.site_settings.auto_dbcs_arrow;
}

void CMainFrame::OnAutoDBCSDel()
{
	if (view.telnet)
		view.telnet->site_settings.auto_dbcs_del = !view.telnet->site_settings.auto_dbcs_del;
	else
		AppConfig.site_settings.auto_dbcs_del = !AppConfig.site_settings.auto_dbcs_del;
}

void CMainFrame::OnUpdateAutoDBCSDel(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(view.telnet ? view.telnet->site_settings.auto_dbcs_del : AppConfig.site_settings.auto_dbcs_del);
}

void CMainFrame::OnAutoDBCSMouse()
{
	if (view.telnet)
		view.telnet->site_settings.auto_dbcs_mouse = !view.telnet->site_settings.auto_dbcs_mouse;
	else
		AppConfig.site_settings.auto_dbcs_mouse = !AppConfig.site_settings.auto_dbcs_mouse;
}

void CMainFrame::OnUpdateAutoDBCSMouse(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(view.telnet ? view.telnet->site_settings.auto_dbcs_mouse : AppConfig.site_settings.auto_dbcs_mouse);
}

void CMainFrame::OnUpdateShowAddressBar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(AppConfig.is_full_scr ? AppConfig.fullscr_showads : AppConfig.showads);
}

void CMainFrame::OnUpdateCloseBtn(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(AppConfig.is_full_scr ? AppConfig.fullscr_showclose : AppConfig.showclose);
}

//---------------------------------Web Browser support-----------------------------

#if defined(_COMBO_)

BOOL CMainFrame::FilterWebConn(CWebConn *web_conn)
{
	BOOL Close = FindAdFilter(web_conn->web_browser.GetLocationName(),
							  web_conn->web_browser.GetLocationURL());
	if (Close)
		PostMessage(WM_REMOVE_WEBCONN, 0, LPARAM(web_conn));
	return Close;
}

BOOL CMainFrame::FindAdFilter(LPCTSTR title, LPCTSTR address)
{
	CString _title;	CString address_bar;
	for (int i = 0; i < AppConfig.webpage_filter.GetSize(); i++)
	{
		int p = AppConfig.webpage_filter[i].Find('\t');
		_title = AppConfig.webpage_filter[i].Left(p);
		address_bar = AppConfig.webpage_filter[i].Mid(p + 1);
		if (address && !address_bar.IsEmpty() && strstr(address, address_bar))
			return TRUE;

		if (title && !_title.IsEmpty() && strstr(title, _title))
			return TRUE;
	}
	return FALSE;
}

void CMainFrame::OnEditAdFilter()
{
	CAdList dlg;
	dlg.DoModal();
}

void CMainFrame::OnAdsHttp()
{
	address_bar.SetFocus();
	address_bar.SetWindowText("http://");
	address_bar.SetEditSel(7, 7);
}

void CMainFrame::OnAdsTelnet()
{
	address_bar.SetFocus();
	address_bar.SetWindowText("telnet://");
	address_bar.SetEditSel(9, -1);
}

void CMainFrame::OnNewWebConn()
{
	view.ConnectWeb("", TRUE);
	OnAdsHttp();
}

void CMainFrame::OnNewHome()
{
	view.ConnectWeb("", TRUE);
	((CWebConn*)view.con)->web_browser.wb_ctrl.GoHome();
}

void CMainFrame::OnAdsOpenNew()
{
	AppConfig.ads_open_new = !AppConfig.ads_open_new;
}

void CMainFrame::OnUpdateAddressBarOpenNew(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(AppConfig.ads_open_new);
}

void CMainFrame::OnUpdateBlockPopup(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(AppConfig.disable_popup);
}

void CMainFrame::OnUpdateSearchbarCleanup(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(AppConfig.searchbar_cleanup);
}

LRESULT CMainFrame::OnRemoveWebConn(WPARAM wparam, LPARAM lparam)
{
	CWebConn* web_conn = reinterpret_cast<CWebConn*>(lparam);
	int i = ConnToIndex(web_conn);
	if (-1 != i)
		CloseConn(i);
	return 0;
}

void CMainFrame::OnIESetup()
{
	typedef BOOL (WINAPI *IEOPTION)(HWND);
	HMODULE hmod = LoadLibrary("inetcpl.cpl");
	if (hmod)
	{
		IEOPTION SetIE = (IEOPTION)GetProcAddress(hmod, "LaunchInternetControlPanel");
		SetIE(m_hWnd);
		FreeLibrary(hmod);
	}
}

void CMainFrame::OnWebPagePrint()
{
	if (!view.telnet || view.con)
	{
		((CWebConn*)view.con)->web_browser.SetFocus();
		((CWebConn*)view.con)->web_browser.wb_ctrl.ExecWB(OLECMDID_PRINT, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}
}

void CMainFrame::OnWebPagePrintSettings()
{
	if (!view.telnet || view.con)
	{
		((CWebConn*)view.con)->web_browser.SetFocus();
		((CWebConn*)view.con)->web_browser.wb_ctrl.ExecWB(OLECMDID_PAGESETUP, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}
}


void CMainFrame::OnWebPageOpen()
{
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, LoadString(IDS_WEBPAGE_FILTER), this);
	if (dlg.DoModal() == IDOK)
		view.ConnectWeb(dlg.GetPathName(), TRUE);
}

void CMainFrame::OnWebPageViewSrc()
{
	if (!view.con || view.telnet)
		return;
	LPDISPATCH lpd = ((CWebConn*)view.con)->web_browser.wb_ctrl.get_Document();
	if (!lpd)
		return;
	IOleCommandTarget* pcmd = NULL;
	if (SUCCEEDED(lpd->QueryInterface(IID_IOleCommandTarget, (void**)&pcmd)))
	{
		pcmd->Exec(&CGID_IWebBrowser, HTMLID_VIEWSOURCE, 0, NULL, NULL);
		pcmd->Release();
	}
	lpd->Release();
}

void CMainFrame::OnNewCurPage()
{
	view.ConnectWeb((view.con && !view.telnet) ? view.con->address : "about:blank", TRUE);
}

void CMainFrame::OnNewCurPageInIE()
{
	if (view.con && view.con->is_web)
		::ShellExecute(m_hWnd, "open", GetIEPath(), view.con->address, NULL, SW_SHOW);
}

void CMainFrame::OnWebPageSaveAs()
{
	if (!view.telnet || view.con)
	{
		((CWebConn*)view.con)->web_browser.SetFocus();
		((CWebConn*)view.con)->web_browser.wb_ctrl.ExecWB(OLECMDID_SAVEAS, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}
}


void CMainFrame::OnBlockPopup()
{
	AppConfig.disable_popup = !AppConfig.disable_popup;
}

void CMainFrame::OnSearchbarCleanup()
{
	AppConfig.searchbar_cleanup = !AppConfig.searchbar_cleanup;
}


void CMainFrame::OnToolbarMenuDropDown(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTOOLBAR nmtb = (LPNMTOOLBAR)pNMHDR;
	RECT rc;
	toolbar.SendMessage(TB_GETRECT, nmtb->iItem, (LPARAM)&rc);
	::MapWindowPoints(toolbar.m_hWnd, NULL, LPPOINT(&rc), 2);
	if (nmtb->iItem == ID_NEW_WWW)
	{
		HMENU pop =::GetSubMenu(::GetSubMenu(main_menu, 0), 0);
		TrackPopupMenu(pop, TPM_LEFTALIGN | TPM_TOPALIGN, rc.left, rc.bottom, 0, m_hWnd, NULL);
	}
	else
	{
		if (!view.con || !view.con->is_web)
			return;
		CMenu pop;
		CWebConn* web_conn = (CWebConn*)view.con;
		pop.CreatePopupMenu();
		if (!web_conn->web_browser.m_TravelLog)
			return;

		CComPtr<IEnumTravelLogEntry> pTLEnum;
		web_conn->web_browser.m_TravelLog->EnumEntries(nmtb->iItem == ID_GOBACK
				? TLEF_RELATIVE_BACK : TLEF_RELATIVE_FORE, &pTLEnum.p);
		UINT id = 1;
		if (pTLEnum.p)
		{
			CComPtr<ITravelLogEntry> pTLEntry;
			while (S_FALSE != pTLEnum->Next(1, &pTLEntry.p, NULL))
			{
				LPOLESTR title;
				pTLEntry.p->GetTitle(&title);
				CString str = title;
				pop.AppendMenu(MF_STRING, id, str);
				CoTaskMemFree(title);
				id++;
			}
		}
		id = pop.TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN | TPM_TOPALIGN, rc.left, rc.bottom, &view, NULL);
		if (id > 0)
			if (nmtb->iItem == ID_GOBACK)
				web_conn->web_browser.wb_ctrl.GoBack(id);
			else
				web_conn->web_browser.wb_ctrl.GoForward(id);
	}
}

void CMainFrame::OnGoBack()
{
	if (view.con && view.con->is_web)
		((CWebConn*)view.con)->web_browser.wb_ctrl.GoBack(1);
}

void CMainFrame::OnGoForward()
{
	if (view.con && view.con->is_web)
		((CWebConn*)view.con)->web_browser.wb_ctrl.GoForward(1);
}


void CMainFrame::OnWebPageAddToAdFilter()
{
	if (view.telnet || !view.con)
		return;
	CWebConn* web_conn = (CWebConn*)view.con;
	CAdItem item;
	item.title = web_conn->web_browser.GetLocationName();
	item.url = web_conn->web_browser.GetLocationURL();
	if (item.DoModal() == IDOK)
	{
		AppConfig.webpage_filter.Add(item.title + '\t' + item.url);
		FilterWebConn(web_conn);
	}
}

void CMainFrame::OnWebHome()
{
	if (view.con)
	{
		if (!view.telnet)
		{
			((CWebConn*)view.con)->web_browser.wb_ctrl.GoHome();
			return;
		}
	}
	view.ConnectWeb("", TRUE)->web_browser.wb_ctrl.GoHome();
}

#endif

//-------------------以下兩版本通用---------------------

void CMainFrame::RecalcLayout(BOOL bNotify)
{
	BYTE showtb, showads, showtab, showsb, use_ansi_bar, showclose;
#ifdef	_COMBO_
	BYTE showwb, showsearchbar;
#endif

	if (AppConfig.is_full_scr)
	{
		showtb = AppConfig.fullscr_showtb;
		showads = AppConfig.fullscr_showads;
		showtab = AppConfig.fullscr_showtab;
		showsb = AppConfig.fullscr_showsb;
		showclose = AppConfig.fullscr_showclose;
		use_ansi_bar = AppConfig.full_use_ansi_bar;
#ifdef	_COMBO_
		showwb = AppConfig.fullscr_showwb;
		showsearchbar = AppConfig.fullscr_showsearchbar;
#endif
	}
	else
	{
		showtb = AppConfig.showtb;
		showads = AppConfig.showads;
		showtab = AppConfig.showtab;
		showsb = AppConfig.showsb;
		showclose = AppConfig.showclose;
		use_ansi_bar = AppConfig.use_ansi_bar;
#ifdef	_COMBO_
		showwb = AppConfig.showwb;
		showsearchbar = AppConfig.showsearchbar;
#endif
	}

	CReBarCtrl& rbc = rebar.GetReBarCtrl();

	CRect rc;
	GetClientRect(rc);

	// Main Toolbar RecalcLayout
	CSize sz = toolbar.CalcFixedLayout(FALSE, TRUE);
	REBARBANDINFO rbi;	rbi.cbSize = sizeof(rbi);
	rbi.fMask = RBBIM_CHILDSIZE | RBBIM_IDEALSIZE;
	rbc.GetBandInfo(rbc.IDToIndex(1), &rbi);
	rbi.cxMinChild = sz.cx;	rbi.cxIdeal = sz.cx;
	rbc.SetBandInfo(rbc.IDToIndex(1), &rbi);

	// Close Button RecalcLayout
	sz = close_btn.CalcFixedLayout(FALSE, TRUE);
	rbc.GetBandInfo(rbc.IDToIndex(3), &rbi);
	rbi.cxMinChild = sz.cx;	rbi.cxIdeal = sz.cx;
	rbc.SetBandInfo(rbc.IDToIndex(3), &rbi);

#ifdef _COMBO_	// Web Bar RecalcLayout
	sz = web_bar.CalcFixedLayout(FALSE, TRUE);
	rbc.GetBandInfo(rbc.IDToIndex(5), &rbi);
	rbi.cxMinChild = sz.cx;	rbi.cxIdeal = sz.cx;
	rbc.SetBandInfo(rbc.IDToIndex(5), &rbi);

	// Set minimal size of search bar
	sz = search_bar.CalcFixedLayout(FALSE, TRUE);
	rbc.GetBandInfo(rbc.IDToIndex(6), &rbi);
	rbi.cxMinChild = sz.cx + 60;	rbi.cxIdeal = sz.cx;
	rbc.SetBandInfo(rbc.IDToIndex(6), &rbi);
#endif

	rbc.ShowBand(rbc.IDToIndex(1), showtb);
	rbc.ShowBand(rbc.IDToIndex(2), use_ansi_bar || (view.telnet && view.telnet->is_ansi_editor));
	rbc.ShowBand(rbc.IDToIndex(3), showclose);
	rbc.ShowBand(rbc.IDToIndex(4), showads);
#ifdef	_COMBO_
	rbc.ShowBand(rbc.IDToIndex(5), showwb);
	rbc.ShowBand(rbc.IDToIndex(6), showsearchbar);
#endif

	int top = 0;
	int height = rc.bottom;
	int barh = rbc.GetBarHeight();
	if (barh > 0)
		barh += 4;
	rebar.MoveWindow(0, 0, rc.right, barh);
	barh = rbc.GetBarHeight();
	if (barh > 0)
		barh += 4;

	top = barh;
	height -= barh;

	if (showsb)
	{
		status_bar.ShowWindow(SW_SHOW);
		status_bar.MoveWindow(0, rc.bottom - SBH, rc.right, SBH);
		height -= SBH;
	}
	else
		status_bar.ShowWindow(SW_HIDE);

#if defined(_COMBO_)
	int pbw = rc.right / 6;
	progress_bar.MoveWindow(rc.right - pbw, 0, pbw, SBH);
#endif

	if (AppConfig.kktab)	//如果使用和KKman相同的連線標籤
	{
		int _top = rc.bottom - TABH - (showsb ? SBH : 0);
		tab.MoveWindow(0, _top, rc.right, TABH);
	}
	else
	{
		tab.MoveWindow(0, top, rc.right, TABH);
		if (showtab)
			top += TABH;
	}

	if (showtab)
	{
		tab.ShowWindow(SW_SHOW);
		height -= TABH;
	}
	else
		tab.ShowWindow(SW_HIDE);

	view.MoveWindow(0, top, rc.right, height, TRUE);
}

void CMainFrame::OnAddressBarComboOK()
{
	CString address;
	int i = address_bar.GetCurSel();
	if (i == CB_ERR)
		address_bar.GetWindowText(address);
	else
		address_bar.GetLBText(i, address);

	int p = address.ReverseFind('\t');
	if (p != -1)
		address = address.Left(p);

#if defined(_COMBO_)
	// FIXME: We doesn't check for *.ans file here!!
	if (!AppConfig.ads_open_new && strncmp("telnet://", address, 9) && view.con && !view.telnet)
	{
		COleVariant v;
		COleVariant url = address;
		((CWebConn*)view.con)->web_browser.wb_ctrl.Navigate2(&url, &v, &v, &v, &v);
		((CWebConn*)view.con)->web_browser.SetFocus();
		return;
	}
#endif

	OnNewConnectionAds(address);
}

void CMainFrame::OnAddressComboCancel()
{
#if defined(_COMBO_)
	if (view.con && view.con->is_web)
		((CWebConn*)view.con)->web_browser.SetFocus();
	else
#endif
		view.SetFocus();
	UpdateAddressBar();
}

LRESULT CALLBACK CMainFrame::AddressBarWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	CMainFrame* mainfrm = (CMainFrame*)AfxGetMainWnd();
	if (msg == WM_KEYDOWN)
	{
		switch (wparam)
		{
		case VK_RETURN:
			if (!mainfrm->address_bar.GetDroppedState())
			{
				mainfrm->PostMessage(WM_COMMAND,
									 MAKEWPARAM(IDC_ADS_COMBO, CBN_SELENDOK),
									 LPARAM(mainfrm->address_bar.GetSafeHwnd()));
			}
			return 0;
			break;
		case VK_ESCAPE:
			mainfrm->OnAddressComboCancel();
			return 0;
		case VK_DOWN:
			if (mainfrm->auto_complete.IsWindowVisible())
				return 0;
			if (!mainfrm->address_bar.GetDroppedState())
				mainfrm->address_bar.ShowDropDown();
			break;
		case VK_UP:
			if (!mainfrm->address_bar.GetDroppedState())
				return 0;
		}
	}
	else if (msg == WM_GETDLGCODE)
		return DLGC_WANTALLKEYS;

	return CallWindowProc(mainfrm->old_address_bar_proc, hwnd, msg, wparam, lparam);
}

#if defined(_COMBO_)
void CMainFrame::OnSearchBarCancel()
{
	if (view.con && view.con->is_web)
		((CWebConn*)view.con)->web_browser.SetFocus();
	else
		view.SetFocus();
}

void CMainFrame::OnWebSearch()
{
	if (AppConfig.search_engine < 0 || AppConfig.search_engine >= SearchPluginCollection.GetCount())
	{
		// MessageBox("Error!");
	}
	else
	{
		CString term;
		bool utf8 = search_bar.GetSearchTerm(term);
		CString searchurl = SearchPluginCollection.UrlForSearch(AppConfig.search_engine,
							term, utf8);

		// FIXME: whether open search result in current window or new window
		//        should be optional in the future!
		/*
			if(view.con && !view.telnet)
			{
				COleVariant v;
				COleVariant url=searchurl;
				((CWebConn*)view.con)->web_browser.wb_ctrl.Navigate2(&url,&v,&v,&v,&v);
				((CWebConn*)view.con)->web_browser.SetFocus();
				return;
			}
		*/
		OnNewConnectionAds(searchurl);	// 內部會呼叫 view.AddToTypedHistory(address);
	}

}

#endif

void CMainFrame::OnAnsiBarBk()
{
	ansi_bar.OnBk();
}
void CMainFrame::OnAnsiBarFg()
{
	ansi_bar.OnFg();
}
void CMainFrame::OnAnsiBarBlink()
{
	ansi_bar.OnBlink();
}
void CMainFrame::OnAnsiBarSend()
{
	ansi_bar.OnSend();
}
void CMainFrame::OnAnsiBarSendDropDown()
{
	ansi_bar.OnSendDropDown();
}

void CMainFrame::OnUpdateAnsiBarBlink(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(ansi_bar.is_blink);
}

void CMainFrame::OnCustomizeMainToolbar()
{
	toolbar.GetToolBarCtrl().Customize();
}

#ifdef	_COMBO_

void CMainFrame::OnCustomizeWebBar()
{
	web_bar.GetToolBarCtrl().Customize();
}

#endif

void CMainFrame::OnGetLocalIP()
{
	char hostname[128];
	gethostname(hostname, 128);
	hostent* host = gethostbyname(hostname);

	if (!host)
		return;

	char** paddr = host->h_addr_list;

	if (!*paddr)
		return;

	while (*(paddr + 1))
		paddr++;

	char *ip = inet_ntoa(*(in_addr*) * paddr);
	MessageBox(ip, LoadString(IDS_LOCAL_IP), MB_OK | MB_ICONINFORMATION);
}

LRESULT CMainFrame::OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu)
{
	LRESULT l = CFrameWnd::OnMenuChar(nChar, nFlags, pMenu);
	if (HIWORD(l) == 0 && AppConfig.favorites.Lookup(pMenu->m_hMenu) >= 0)
		l = AppConfig.favorites.OnMenuChar(nChar, nFlags, pMenu);
	return l;
}

LRESULT CMainFrame::OnHotKey(WPARAM wp, LPARAM lp)
{
	RestoreWindow();
	return 0;
}

inline void CMainFrame::MinimizeToTray()
{
	NOTIFYICONDATA nd;
	nd.cbSize = sizeof(nd);
	nd.hIcon = icon;
	nd.hWnd = m_hWnd;
	GetWindowText(nd.szTip, sizeof(nd.szTip));
	nd.uCallbackMessage = WM_NOTIFYICON;
	nd.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nd.uID = 1;
	Shell_NotifyIcon(NIM_ADD, &nd);
	ShowWindow(SW_HIDE);
}

void CMainFrame::RestoreWindow()
{
	if (!IsWindowVisible())
	{
		NOTIFYICONDATA nd;
		nd.cbSize = sizeof(nd);
		nd.hWnd = m_hWnd;
		nd.uFlags = 0;
		nd.uID = 1;
		Shell_NotifyIcon(NIM_DELETE, &nd);
		ShowWindow(SW_SHOW);
	}
	if (IsIconic())	// Restore minimized window 從 Delphi VCL學來的!
		DefWindowProc(WM_SYSCOMMAND, SC_RESTORE, 0);

	BringWindowToTop();
	SetForegroundWindow();
}

LRESULT CMainFrame::OnNotifyIcon(WPARAM wp, LPARAM lp)
{
	if (lp == WM_LBUTTONUP || lp == WM_RBUTTONUP)
		RestoreWindow();
	return 0;
}

void CMainFrame::OnUpdateEditOpenURL(CCmdUI* pCmdUI)
{
	if (pCmdUI->m_pSubMenu)
	{
		pCmdUI->m_pMenu->EnableMenuItem(pCmdUI->m_nIndex, MF_BYPOSITION |
										((view.telnet && view.telnet->sel_start != view.telnet->sel_end) ? MF_ENABLED : MF_GRAYED));
	}
}

void CMainFrame::OnUpdateSetCharset(CCmdUI* pCmdUI)
{
	if (pCmdUI->m_pSubMenu)
	{
		pCmdUI->m_pMenu->EnableMenuItem(pCmdUI->m_nIndex, MF_BYPOSITION |
										((view.telnet) ? MF_ENABLED : MF_GRAYED));
	}
}

void CMainFrame::OnBackupConfig()
{
	CString title;
	title.LoadString(IDS_CHOOSE_BACKUP_DIR);
	CBrowseDirDlg dlg(this, title);
	if (dlg.DoModal() == IDOK)
		AppConfig.BackupConfig(ConfigPath, CString(dlg.GetPath()) + "\\PCMan_backup\\");
}

void CMainFrame::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT pds)
{
	CFrameWnd::OnDrawItem(nIDCtl, pds);
	if (pds->CtlType == ODT_MENU)
		AppConfig.favorites.DrawItem(pds);
}

void CMainFrame::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpms)
{
	CFrameWnd::OnMeasureItem(nIDCtl, lpms);
	if (lpms->CtlType == ODT_MENU)
		AppConfig.favorites.MeasureItem(lpms);
}

void CMainFrame::OnAutoUpdate()
{
	if(!AppConfig.autoupdate_disable)
		CWinThread * controlThread = AfxBeginThread(BackgroundAutoUpdate,NULL,THREAD_PRIORITY_NORMAL);
}

void CMainFrame::OnToolLock()
{
	CPasswdDlg dlg;
	if (!AppConfig.QueryPassword(TRUE))
		return;

	AppConfig.lock_pcman = true;

	ShowWindow(SW_SHOWMINIMIZED);
	::SetMenu(m_hWnd, NULL);
	dlg.title.Empty();
	while (dlg.DoModal() != IDOK || dlg.passwd != AppConfig.passwd)
		;
	::SetMenu(m_hWnd, main_menu);
	AppConfig.lock_pcman = false;

	// 參考 Delphi VCL，發現方便還原視窗的怪招
	DefWindowProc(WM_SYSCOMMAND, SC_RESTORE, 0);
}


LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (AppConfig.lock_pcman && message != WM_QUERY_APPCONFIG)
		return DefWindowProc(message, wParam, lParam);
	return CFrameWnd::WindowProc(message, wParam, lParam);
}

LRESULT CMainFrame::OnQueryAppConfig(WPARAM w, LPARAM l)
{
	switch (w)
	{
	case AC_MULTIPCMAN:
		return AppConfig.multiple_instance;
	case AC_PCMANLOCKED:
		return AppConfig.lock_pcman;
	}
	return 0;
};


void CMainFrame::OnUpdateIsBBS(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!!view.telnet);
}

void CMainFrame::OnUpdateIsConnnected(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(view.telnet && view.telnet->is_connected);
}

void CMainFrame::OnUpdateIsSel(CCmdUI *pCmdUI)
{
	// don't disable toolbar, or the icons will be grey and very ugly.
	if (!pCmdUI->m_pMenu)
		return;

#if defined(_COMBO_)
	BOOL sel;
	if (view.con)
	{
		if (view.con->is_web)
			sel = ((CWebConn*)view.con)->web_browser.wb_ctrl.QueryStatusWB(OLECMDID_COPY) & OLECMDF_ENABLED;
		else
			sel = (view.telnet->sel_start != view.telnet->sel_end);
	}
	else
		sel = FALSE;
	pCmdUI->Enable(sel);
#else
	pCmdUI->Enable(view.telnet && view.telnet->sel_start != view.telnet->sel_end);
#endif
}

void CMainFrame::OnUpdateIsConn(CCmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)	// don't disable toolbar
		return;
#if defined(_COMBO_)
	bool enable = !!view.con;
#else
	bool enable = !!view.telnet;
#endif
	pCmdUI->Enable(enable);
}

void CMainFrame::OnUpdateIsBBSSite(CCmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)	// don't disable toolbar
		return;

	bool enable = view.telnet ? !view.telnet->is_ansi_editor : false;

	if (pCmdUI->m_nID == ID_COPYARTICLE && pCmdUI->m_pSubMenu)
	{
		pCmdUI->m_pMenu->EnableMenuItem(pCmdUI->m_nIndex, MF_BYPOSITION |
										(enable ? MF_ENABLED : MF_GRAYED));
	}
	else
		pCmdUI->Enable(enable);
}

#ifdef	_COMBO_
void CMainFrame::OnUpdateIsWebPage(CCmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)	// don't disable toolbar
		return;
	pCmdUI->Enable(view.con && view.con->is_web);
}
#endif

void CMainFrame::OnUpdatePaste(CCmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)	// don't disable toolbar
		return;

#ifdef _COMBO_
	BOOL sel;
	if (view.con)
	{
		if (view.con->is_web)
			sel = ((CWebConn*)view.con)->web_browser.wb_ctrl.QueryStatusWB(OLECMDID_PASTE) & OLECMDF_ENABLED;
		else
			sel = (view.telnet->is_ansi_editor || view.telnet->is_connected);
	}
	else
		sel = FALSE;
	pCmdUI->Enable(sel);
#else
	pCmdUI->Enable(view.telnet ? (view.telnet->is_ansi_editor || view.telnet->is_connected) : 0);
#endif
}

void CMainFrame::OnUpdateIsSite(CCmdUI *pCmdUI)
{
	if (!pCmdUI->m_pMenu)	// don't disable toolbar
		return;

#ifdef _COMBO_
	pCmdUI->Enable(view.con && !view.con->is_ansi_editor);
#else
	pCmdUI->Enable(view.telnet && !view.telnet->is_ansi_editor);
#endif
}

void CMainFrame::OnUpdateShowAnsiBar(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(AppConfig.is_full_scr ? AppConfig.full_use_ansi_bar : AppConfig.use_ansi_bar);
}

void CMainFrame::OnUpdateSelectAll(CCmdUI* pCmdUI)
{
	OnUpdateIsBBS(pCmdUI);
}

BOOL CMainFrame::OnToolTipNeedText(UINT id, NMHDR *nmhdr, LRESULT *r)
{
	NMTTDISPINFO* ptt = (NMTTDISPINFO*)nmhdr;
	if (nmhdr->hwndFrom == tab.GetToolTips()->m_hWnd)
	{
		CConn* pCon = tab.GetCon(nmhdr->idFrom);
		if (pCon)
		{
			ptt->lpszText = (LPSTR)(LPCTSTR)pCon->name;
			return TRUE;
		}
	}
#if defined(_COMBO_)
	else if (nmhdr->idFrom == ID_WEB_SEARCH)
	{
		ptt->lpszText = SearchPluginCollection.GetField(AppConfig.search_engine, CSearchPluginCollection::SHORTNAME);
		return TRUE;
	}
#endif
	return FALSE;
}

#if defined(_COMBO_)

void CMainFrame::OnBrowserFontSize(UINT id)
{
	if (view.con && view.con->is_web)
	{
		id -= ID_FONT_SMALLEST;
		COleVariant v = long(id);
		((CWebConn*)view.con)->web_browser.wb_ctrl.ExecWB(OLECMDID_ZOOM, 0, &v, NULL);
	}
}

void ImportIEFav(CFile& file, IShellFolder* pParent, LPITEMIDLIST pidl)
{
	IShellFolder* pshf = NULL;
	if (SUCCEEDED(pParent->BindToObject(pidl, NULL, IID_IShellFolder, (LPVOID*)&pshf)))
	{
		IEnumIDList* penumidl = NULL;
		if (SUCCEEDED(pshf->EnumObjects(NULL, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &penumidl)))
		{
			while (NOERROR  == penumidl->Next(1, &pidl, NULL))
			{
				char szPath[_MAX_PATH+1];
				SHGetPathFromIDList(pidl, szPath);
				strcat(szPath, "\x0d\x0a");
				file.Write(szPath, strlen(szPath));
			}
			penumidl->Release();
		}
		pshf->Release();
	}

}

void ImportIEFav(CFile& file, CString dirpath)
{
	CFileFind finder;
	dirpath += "\\*.*";

	char* url = new char[1024];
	BOOL bfound = finder.FindFile(dirpath);
	while (bfound)
	{
		bfound = finder.FindNextFile();
		CString fpath = finder.GetFilePath();
		if (finder.IsDirectory())
		{
			if (!finder.IsDots())
			{
				CString line("d");
				line += finder.GetFileTitle();
				line += "\x0d\x0a";
				file.Write(LPCTSTR(line), line.GetLength());
				ImportIEFav(file, fpath);
			}
		}
		else
		{
			DWORD len;
			if (len = GetPrivateProfileString("InternetShortcut", "URL", "", url, 1022, fpath))
			{
				CString line("w");
				line += finder.GetFileTitle();
				line += '\t';
				file.Write(LPCTSTR(line), line.GetLength());
				url[len] = 0x0d;	url[len+1] = 0x0a;
				file.Write(url, len + 2);
			}
		}
	}
	finder.Close();
	file.Write("\x0d\x0a", 2);
	delete []url;
}

void CMainFrame::OnImportIEFavorites()
{
	ImportIEFavorites();
	if (!AppConfig.use_ie_fav)
		LoadWebFavorites();
}

void CMainFrame::ImportIEFavorites()
{
	if (IDOK != MessageBox(LoadString(IDS_IMPORT_IE_FAV_PROMPT)
						   , LoadString(IDS_IMPORT_IE_FAV), MB_OKCANCEL | MB_ICONQUESTION))
		return;
	LPITEMIDLIST pidl = NULL;
	SHGetSpecialFolderLocation(NULL, CSIDL_FAVORITES, &pidl);
	CString favpath;
	SHGetPathFromIDList(pidl, favpath.GetBuffer(_MAX_PATH));
	IMalloc* pmalloc;
	if (SUCCEEDED(SHGetMalloc(&pmalloc)))
	{
		pmalloc->Free(pidl);
		pmalloc->Release();
	}
	favpath.ReleaseBuffer();

	CFile file;
	if (file.Open(ConfigPath + WWW_FAVORITE_FILENAME, CFile::modeWrite | CFile::modeCreate))
	{
		ImportIEFav(file, favpath);
		file.Close();
		MessageBox(LoadString(IDS_IMPORT_COMPLETE), LoadString(IDS_FINISHED), MB_OK | MB_ICONEXCLAMATION);
	}

}


void ExportIEFav(int& i, CString dir)
{
	CStringArray& fav = AppConfig.favorites.web_fav;
	if (!IsFileExist(dir))
		CreateDirectory(dir, NULL);
	dir += '\\';
	while (!fav[i].IsEmpty() && fav[i][0] != '-')
	{
		LPCTSTR favstr = fav[i];
		if (favstr[0] == 'd')
		{
			i++;
			ExportIEFav(i, (dir + (favstr + 1)));
		}
		else if (favstr[0] == 'w')
		{
			int p = fav[i].Find('\t', 1);
			CString fpath = dir + fav[i].Mid(1, p - 1);
			fpath += ".url";
			CString URL = fav[i].Mid(p + 1);
			if (!IsFileExist(fpath))
			{
				CFile file;
				CString content = "[InternetShortcut]\r\nURL=";
				content += URL;
				if (file.Open(fpath, CFile::modeWrite | CFile::modeCreate))
				{
					file.Write(LPCTSTR(content), content.GetLength());
					file.Close();
				}
			}
		}
		i++;
	}
	i += 4;
}

void CMainFrame::OnExportIEFavorites()
{
	LPITEMIDLIST pidl = NULL;
	SHGetSpecialFolderLocation(NULL, CSIDL_FAVORITES, &pidl);
	CString favpath;
	SHGetPathFromIDList(pidl, favpath.GetBuffer(_MAX_PATH));
	favpath.ReleaseBuffer();
	IMalloc* pmalloc;
	if (SUCCEEDED(SHGetMalloc(&pmalloc)))
	{
		pmalloc->Free(pidl);
		pmalloc->Release();
	}

	int i = 0;
	ExportIEFav(i, favpath);
	MessageBox(LoadString(IDS_EXPORT_COMPLETE), LoadString(IDS_FINISHED), MB_OK | MB_ICONINFORMATION);
}


#endif

int CMainFrame::NewTab(CConn *pCon, BOOL bActive, int idx)
{
	if (idx == -1)	idx = tab.GetItemCount();
	int img;
#if defined	_COMBO_
	if (pCon->is_web)	//如果新開瀏覽器畫面
		img = 8;
	else
	{
#endif
		if (pCon->is_ansi_editor)
			img = 6;
		else
			img = 7;
#if defined	_COMBO_
	}
#endif
	tab.InsertItem(TCIF_TEXT | TCIF_IMAGE | TCIF_PARAM, idx, pCon->name, img, LPARAM(pCon));
	tab.Invalidate();
	return idx;
}

void CMainFrame::OnConnectClose()
{
	int sel = tab.GetCurSel();
	CloseConn(sel, true);
}

void CMainFrame::OnConnectCloseAllOthers()
{
	int sel = tab.GetCurSel();
	int all = tab.GetItemCount();
	for(int i=0;i<sel;i++)
		CloseConn(0, true);
	for(i=sel+1;i<all;i++)
		CloseConn(1,true);
}

void CMainFrame::CloseConn(int i, bool confirm)
{
	CConn* pCon = tab.GetCon(i);
	if (!pCon)
		return;

	if (confirm)	// confirm before close
	{
		if ((pCon->is_connected || pCon->is_ansi_editor)
			&& AppConfig.bbs_close_query)
			if (MessageBox(LoadString(IDS_CLOSE_CONFIRM) , LoadString(IDS_CONFIRM), MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL)
				return;
	}

#ifdef _COMBO_
	if (pCon->is_web)
	{
		CString str("w");
		str += pCon->name;
		str += '\t';
		str += pCon->address;
		AddToHistoryMenu(str);
	}
#endif

	CConn* pprev_con = prev_conn;
	if (i != -1)
	{
		tab.DeleteItem(i);
		if (i == tab.GetItemCount())
			i--;
		if (i != -1)
			tab.SetCurSel(i);
	}

	int switch_to;
	if (AppConfig.switch_back_on_close && -1 != (switch_to = ConnToIndex(pprev_con)))
	{
		SwitchToConn(switch_to);
	}
	else
	{
		SwitchToConn(i);
	}

	if (pCon->is_lookup_host)
	{
		pCon->is_lookup_host = false;
		pCon->is_cancelled = true;
	}
	else
		delete pCon;
}

void CMainFrame::OnUpdateDisconnect(CCmdUI* pCmdUI)
{
	if (!pCmdUI->m_pMenu)	// Don't disable toolbar
		return;
#ifdef _COMBO_
	pCmdUI->Enable(view.con && (view.con->is_web || view.con->is_connected));
#else
	pCmdUI->Enable(view.telnet && view.telnet->is_connected);
#endif
}

void CMainFrame::OnUpdateReconnect(CCmdUI* pCmdUI)
{
	if (!pCmdUI->m_pMenu)	// Don't disable toolbar
		return;
#ifdef _COMBO_
	pCmdUI->Enable(view.con &&
				   (view.con->is_connected ||
					view.con->is_disconnected ||
					view.con->is_ansi_editor ||
					view.con->is_web));
#else
	pCmdUI->Enable(!!view.telnet);
#endif
}


void CMainFrame::OnSaveSession()
{
	AppConfig.save_session = !AppConfig.save_session;
}


void CMainFrame::OnUpdateSaveSession(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(AppConfig.save_session);
}

void CMainFrame::LoadHistory()
{
	CHistoryLoader history_loader(this);
	history_loader.Load();
}

void CMainFrame::SaveHistory()
{
	CHistoryLoader history_loader(this);
	history_loader.Save();
}


#if _MFC_VER >= 0x0700
void CMainFrame::OnActivateApp(BOOL bActive, DWORD hTask)
#else
void CMainFrame::OnActivateApp(BOOL bActive, HTASK hTask)
#endif
{
	CFrameWnd::OnActivateApp(bActive, hTask);
	if (!bActive && auto_complete.m_hWnd)
		auto_complete.Close();
}

void CMainFrame::OnSwitchBack()
{
	int prev_idx = ConnToIndex(prev_conn);
	if (prev_idx != -1)
		SwitchToConn(prev_idx);
}

void CMainFrame::OnUpdateBBSMouseCTL(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(AppConfig.use_MouseCTL);
}

void CMainFrame::OnBBSMouseCTL()
{
	AppConfig.use_MouseCTL = !AppConfig.use_MouseCTL;
	MouseCTL_Reset();
}

BOOL CMainFrame::LoadUI()
{
	CFile ui;
	if (!ui.Open(ConfigPath + UI_FILENAME, CFile::modeRead)
		&& !ui.Open(DefaultConfigPath + UI_FILENAME, CFile::modeRead))
		return FALSE;

	main_menu = CreateMenu();
	DWORD l = ui.GetLength();
	WORD count;
//取得ACC table長度
	ui.Read(&count, sizeof(WORD));
//開始建立Accelerator
	if (accels)
		delete []accels;
	DestroyAcceleratorTable(m_hAccelTable);

	accel_count = count;
	accels = new ACCEL[count];	//配置記憶體給acc table
	ui.Read(accels, count*sizeof(ACCEL));	//換算成byte

	m_hAccelTable = CreateAcceleratorTable(accels, count);
//問題出在這裡,傳入CreateAcceleratorTable的應該是ACCEL的數量,而不是大小(位元組數)!!!!!!

//		Accelerator建立結束,開始讀取 UI

	l -= count + 2;	//剩餘UI長度
	char* ui_buf = new char[l+32];
	ui.Read(ui_buf, l);
	ui.Close();

	UIAddMenu(ui_buf, main_menu);
	delete []ui_buf;
//		選單建立結束

	::SetMenu(m_hWnd, main_menu);

	edit_menu =::GetSubMenu(main_menu, 1);
#if defined(_COMBO_)
	auto_dbcs_menu = GetSubMenu(GetSubMenu(main_menu, 2), 8);
	web_fav_menu =::GetSubMenu(main_menu, 4);
	ansi_menu =::GetSubMenu(main_menu, 6);
#else
	auto_dbcs_menu = GetSubMenu(GetSubMenu(main_menu, 2), 7);
	ansi_menu =::GetSubMenu(main_menu, 5);
#endif
	bbs_fav_menu =::GetSubMenu(main_menu, 3);

	MENUITEMINFO inf;
	inf.cbSize = sizeof(inf);
	inf.fMask = MIIM_SUBMENU;

	LoadHistoryMenu();
	inf.hSubMenu = history_menu;	//連線紀錄

#if defined(_COMBO_)
	SetMenuItemInfo(main_menu, 5, TRUE, &inf);
#else
	SetMenuItemInfo(main_menu, 4, TRUE, &inf);
#endif

	LoadBBSFavorites();
#if defined(_COMBO_)
	LoadWebFavorites();
#endif

	DrawMenuBar();
	return TRUE;
}

void CMainFrame::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == ID_MOUSE_SEL_TIMER)
	{
		CPoint pt;
		CRect rc;
		GetCursorPos(&pt);
		view.ScreenToClient(&pt);
		view.OnMouseMove(MK_LBUTTON, pt);
	}
	CFrameWnd::OnTimer(nIDEvent);
}

void CMainFrame::OnShowFrequentlyUsedStr()
{
	CStringDlg dlg(this);
	if (dlg.DoModal() == IDOK)
	{
#ifdef _COMBO_
		if (!view.con || dlg.str.IsEmpty())
			return;

		if (view.con->is_web)
			((CWebConn*)view.con)->web_browser.SendString(dlg.str);
		else
#else
		if (!view.telnet || dlg.str.IsEmpty())
			return;
#endif
			SendFreqStr(dlg.str, HIBYTE(HIWORD(dlg.inf)));
	}

	if (dlg.data_changed)
		LoadFrequentlyUsedStr();
}

void CMainFrame::LoadFrequentlyUsedStr()
{
	//讀取熱鍵字串
	if (hhotstr_acc)
	{
		::DestroyAcceleratorTable(hhotstr_acc);
		hotstr.RemoveAll();
		hotstr_inf.RemoveAll();
		hotstr_acc.RemoveAll();
	}

	hotstr_acc.SetSize(0, 24);
	CMemIniFile f;
	if (f.Open(ConfigPath + FUS_FILENAME, CFile::modeRead))
	{
		WORD p, c;
		DWORD inf;
		f.Read(&p, sizeof(WORD));
		ACCEL accels;
		accels.cmd = ID_FIRST_HOTSTR;
		while (p > 0)
		{
			CString tmp(LoadString(f));
			f.Read(&c, sizeof(WORD));	//Load StringArray
			while (c > 0)
			{
				inf = 0;
				f.Read4(&inf);
				tmp = LoadString(f);
				if (LOWORD(inf))	//有熱鍵
				{
					hotstr.Add(tmp);
					hotstr_inf.Add(BYTE(inf >> 24));
					accels.key = LOWORD(inf);
					accels.fVirt = LOBYTE(HIWORD(inf)) | FVIRTKEY | FNOINVERT;

					hotstr_acc.Add(accels);

					accels.cmd++;
				}
				c--;
			}
			p--;
		}
		f.Close();
		hotstr.FreeExtra();
		hotstr_inf.FreeExtra();
		hotstr_acc.FreeExtra();

		if (hotstr_acc.GetSize() > 0)
			hhotstr_acc =::CreateAcceleratorTable(hotstr_acc.GetData(), hotstr_acc.GetSize());
		else
			hhotstr_acc = NULL;
	}
}

void CMainFrame::OnEditFind()
{
#ifdef	_COMBO_
	if (!view.con)
		return;
	if (view.con->is_web)
	{
		static_cast<CWebConn*>(view.con)->web_browser.Find();
		return;
	}
#else
	if (!view.telnet)
		return;
#endif
	view.telnet->sel_end.y = view.telnet->first_line;
	view.FindStart();
}

void CMainFrame::OnFrequentlyUsedStr(UINT id)
{
	id -= ID_FIRST_HOTSTR;
#if	defined	_COMBO_
	if (!view.con)
		return;
#endif

	if (!view.telnet)
	{
#if	defined	_COMBO_
		if (id == 4)	//F5 key
			((CWebConn*)view.con)->web_browser.wb_ctrl.Refresh();
		else
			((CWebConn*)view.con)->web_browser.SendString(hotstr.ElementAt(id));
#endif
		return;
	}

	CString str = hotstr.ElementAt(id);
	BYTE inf = hotstr_inf.ElementAt(id);
	SendFreqStr(str, inf);
}

void CMainFrame::AddToHistoryMenu(CString str)
{
	int i = 0;
	int c = AppConfig.favorites.history.GetSize();
	for (;i < c; i++)
	{
		if (AppConfig.favorites.history[i].CompareNoCase(str) == 0)
			break;
	}

	if (i < c)	//如果有找到一樣的，選單不改變，只移動內部順序
		AppConfig.favorites.history.RemoveAt(i);
	else	//如果新增選單項目
	{
		//重建選單
		for (i = 0;i < AppConfig.favorites.history.GetSize(); i++)
		{
			UINT id = ID_FIRST_HISTORY + i + 1;
			ModifyMenu(history_menu, i, MF_BYPOSITION | MF_OWNERDRAW, id, LPCTSTR(id));
		}
		InsertMenu(history_menu, 0, MF_BYPOSITION | MF_OWNERDRAW, ID_FIRST_HISTORY, LPCTSTR(ID_FIRST_HISTORY));
	}
	AppConfig.favorites.history.InsertAt(0, str);
	if (AppConfig.favorites.history.GetSize() > 10 /*AppConfig.max_history_menu)*/)
	{
		AppConfig.favorites.history.RemoveAt(10 /*AppConfig.max_history_menu*/);
		DeleteMenu(history_menu, 10 /*AppConfig.max_history_menu*/ , MF_BYPOSITION);
	}
}

void CMainFrame::AddToTypedHistory(CString address)
{
	if (address.IsEmpty())
		return;

	if (address_bar.FindStringExact(0, address) == CB_ERR)
		address_bar.AddString(address);

	if (address_bar.GetCount() > AppConfig.max_history)
		address_bar.DeleteString(0);
}

void CMainFrame::AddToHistory(CString address)
{
	if (address.IsEmpty())
		return;
	if (!AppConfig.history.Find(address))
	{
		AppConfig.history.AddHead(address);
		if (AppConfig.history.GetCount() > AppConfig.max_history)
			AppConfig.history.RemoveTail();
	}

#ifdef	_COMBO_
	if (0 == strnicmp(address, "telnet", 6))
#endif
		AddToTypedHistory(address);
}

void CMainFrame::LoadBBSFavorites()
{
	char title[64];
	GetMenuString(main_menu, 3, title, 32, MF_BYPOSITION);
	AppConfig.favorites.LoadFavorites(bbs_fav_menu, 's');
	ModifyMenu(main_menu, 3, MF_BYPOSITION | MF_STRING | MF_POPUP, (UINT)bbs_fav_menu, title);
}

#if defined(_COMBO_)
void CMainFrame::LoadWebFavorites()
{
	char title[32];
	GetMenuString(main_menu, 4, title, 32, MF_BYPOSITION);

	if (AppConfig.use_ie_fav)
		AppConfig.favorites.LoadIEFav(web_fav_menu);
	else
	{
		if (!IsFileExist(ConfigPath + WWW_FAVORITE_FILENAME))
			ImportIEFavorites();
		AppConfig.favorites.LoadFavorites(web_fav_menu, 'w');
	}

	ModifyMenu(main_menu, 4, MF_BYPOSITION | MF_STRING | MF_POPUP, (UINT)web_fav_menu, title);
}
#endif

void CMainFrame::OnFavorite(UINT id)
{
	CTelnetConn* telnet = view.telnet;

	int idir = -1, _idir;	int level = 0;
	CString dir;

	const char sep_char = '\\' ;

	CStringArray* fav;
#ifdef _COMBO_
	BOOL bWWW = id >= ID_FIRST_WEB_FAVORITE;
	fav = bWWW ? &AppConfig.favorites.web_fav : &AppConfig.favorites.bbs_fav;
	id -= bWWW ? ID_FIRST_WEB_FAVORITE : ID_FIRST_BBS_FAVORITE;
#else
	fav = &AppConfig.favorites.bbs_fav;
	id -= ID_FIRST_BBS_FAVORITE;
#endif

	for (_idir = id; _idir >= 0; _idir--)
	{
		for (; _idir >= 0; _idir--)
		{
			if (fav->ElementAt(_idir).IsEmpty())
				level--;
			else if (fav->ElementAt(_idir)[0] == 'd')
			{
				if (level == 0)
				{
					if (idir == -1)
						idir = _idir;
					dir = (LPCTSTR(fav->ElementAt(_idir)) + 1) + (sep_char + dir);
					break;
				}
				level++;
			}
		}
	}
#ifdef _COMBO_
	if (bWWW)
		dir = (AppConfig.use_ie_fav ? CFavMenu::GetIEFavDir() : LoadString(IDS_WWW_FAVORITE_NAME)) + (sep_char + dir);
	else
#endif
		dir = (LoadString(IDS_BBS_FAVORITE_NAME) + (PATH_SEPARATOR + dir));

	CString name = fav->ElementAt(id);
#ifdef _COMBO_
	if (name[0] != 's' && name[0] != 'w')
#else
	if (name[0] != 's')
#endif
	{
		switch (name[0])
		{
		case 'a':
			{
#ifdef _COMBO_
				if (!view.con)
					return;
				if (bWWW)
				{
					if (view.con->is_web)
					{
						CWebPageDlg dlg(this);
						dlg.m_URL = ((CWebConn*)view.con)->web_browser.GetLocationURL();
						dlg.m_Name = view.con->name;
						if (dlg.DoModal() == IDOK)
						{
							if (AppConfig.use_ie_fav)
							{
								LPSTR pstr = (LPSTR)(LPCTSTR)dlg.m_Name;
								while (*pstr)
								{
									//check illegal filename except DBCS
									if (*pstr >= '\x81' && *pstr <= '\xfe') //0x81 to 0xfe (Big5)
									{
										pstr++;
										if (! *pstr)
											break;
									}
									else if (*pstr == '\\' || *pstr == '/' || *pstr == ':'
											 || *pstr == '?' || *pstr == '<' || *pstr == '>'
											 || *pstr == '|' || *pstr == '*' || *pstr == '\"')
										*pstr = '-';
									pstr++;
								}
								WritePrivateProfileString("InternetShortcut", "URL", dlg.m_URL, dir + dlg.m_Name + ".url");
							}
							else
							{
								name = 'w';
								name += dlg.m_Name;
								name += '\t';
								name += dlg.m_URL;
								fav->InsertAt(id - 1, name);
								AppConfig.favorites.SaveFavorites(FALSE);
							}
							LoadWebFavorites();
						}
					}
				}
				else
#endif
					if (telnet && !telnet->is_web && !telnet->is_ansi_editor)
					{
						name = 's';	name += telnet->name;
						name += '\t';
#ifdef _COMBO_
						name += telnet->address.Mid(9);
#else
						name += telnet->address;
#endif
						if (telnet->port != 23 && telnet->port > 0)
						{
							char port_str[16];
							sprintf(port_str, ":%d", telnet->port);
							name += port_str;
						}
						fav->InsertAt(id - 1, name);
						AppConfig.favorites.SaveFavorites(TRUE);
						LoadBBSFavorites();
					}
			}
			break;
		case 'e':
			{
				CString strPath = dir.Left(dir.GetLength() - 1);
#ifdef	_COMBO_
				if (bWWW && AppConfig.use_ie_fav)
				{
					typedef	DWORD (WINAPI DOFD)(HWND, LPCTSTR);
					DOFD* pfunc = NULL;
					HMODULE hmod = LoadLibrary("Shdocvw.dll");
					pfunc = (DOFD*)GetProcAddress(hmod, "DoOrganizeFavDlg");
					if (!hmod || !pfunc)
						return;
					pfunc(m_hWnd, LPCTSTR(strPath));
					FreeLibrary(hmod);
					LoadWebFavorites();
				}
				else
#endif
				{
					CListDlg dlg(&view);
					dlg.m_InitPath = strPath;
					dlg.DoModal();
				}
				break;
			}
		case 'o':
			{
				id -= 2;
				for (level = 0, idir++; idir < int(id) ; idir++)
				{
					if (*LPCTSTR(fav->ElementAt(idir)) == 'd')
					{
						level++;
						continue;
					}
					else if (fav->ElementAt(idir) == "-")
					{
						level--;
						idir += 4;
						continue;
					}
					if (level == 0)
						view.ConnectStr(fav->ElementAt(idir), dir);
				}
			}
			break;
		}
		return;
	}

	view.ConnectStr(name, dir);
}


void CMainFrame::OnViewFullscr()
{
	if (AppConfig.is_full_scr)	//如果已經是全螢幕
	{
		ModifyStyle(WS_POPUP, WS_OVERLAPPEDWINDOW);
		ShowWindow(SW_HIDE);
		view.ModifyStyleEx(0, WS_EX_CLIENTEDGE);
		AppConfig.is_full_scr = 0;
		ShowWindow(SW_SHOWNORMAL);
		ShowWindow(SW_HIDE);
		ShowWindow(showcmd);
	}
	else	//如果不是全螢幕
	{
		WINDOWPLACEMENT wpm;
		GetWindowPlacement(&wpm);
		showcmd = wpm.showCmd;

		ShowWindow(SW_SHOWNORMAL);
		ModifyStyle(GetStyle(), WS_POPUP);
		view.ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
		AppConfig.is_full_scr = 1;
		ShowWindow(SW_SHOWMAXIMIZED);
		CRect rc;
		GetWindowRect(rc);
		rc.top = -GetSystemMetrics(SM_CYMENU);
		rc.bottom = GetSystemMetrics(SM_CYSCREEN);
		MoveWindow(rc);
	}
	SetFocus();
}

void CMainFrame::OnToolSymbols()
{
	if (!::FindWindow("Symbols_main_window", NULL))
		ShellExecute(m_hWnd, "open", AppPath + "Symbols.exe", NULL, NULL, SW_SHOW);
}


extern int _afxComCtlVersion;
DWORD AFXAPI _AfxGetComCtlVersion();

void CMainFrame::OnViewConfig()
{
	BYTE autofont = AppConfig.auto_font;
	BYTE tab_button = AppConfig.tab_button;
	BYTE tab_add_num = AppConfig.tab_add_number;

	CPropertySheet configdlg(IDS_PCMAN_ALL_SETTINGS, NULL);
	configdlg.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	CGeneralPage page0;
	CSitePage page1;
	page1.show_use_global = 0;
	page1.psettings = &AppConfig.site_settings;

	COtherPage page2;
	CHyperLinkPage page3;
	page0.m_psp.dwFlags &= ~PSP_HASHELP;
	page1.m_psp.dwFlags = page2.m_psp.dwFlags = page3.m_psp.dwFlags = page0.m_psp.dwFlags;

#ifdef	_COMBO_
	CWebCfgPage page4;
	BYTE autowrap_favorite = AppConfig.autowrap_favorite;
	BYTE autosort_favorite = AppConfig.autosort_favorite;
	BYTE use_ie_fav = AppConfig.use_ie_fav;
#endif

	configdlg.AddPage(&page0);
	configdlg.AddPage(&page1);
	configdlg.AddPage(&page2);
	configdlg.AddPage(&page3);
#ifdef	_COMBO_
	configdlg.AddPage(&page4);
#endif

	int r = configdlg.DoModal();
	if (r != IDOK)
		return;

	if (autofont != AppConfig.auto_font)
	{
		if (AppConfig.auto_font)
		{
			CRect rc;
			view.GetClientRect(rc);
			view.AdjustFont(rc.right, rc.bottom);
		}
		else
		{
			MessageBox(LoadString(IDS_SET_FONT_PROMPT), LoadString(IDS_ATTENTION), MB_OK | MB_ICONEXCLAMATION);
			OnFont();
		}
	}

	if (tab_add_num != AppConfig.tab_add_number)
		tab.UpdateNumberFrom(0);

	if (AppConfig.tab_button != tab_button)
	{
		if (_afxComCtlVersion >= MAKELONG(0, 6))	//Win XP IE 6.0
		{
			MessageBox(LoadString(IDS_TAB_STYLE_ERR_MSG), LoadString(IDS_ATTENTION), MB_OK | MB_ICONINFORMATION);
		}
		else
		{
			if (AppConfig.tab_button)
			{
				tab.ModifyStyle(TCS_BOTTOM, (TCS_FLATBUTTONS | TCS_BUTTONS | TCS_HOTTRACK));
				tab.ModifyStyleEx(0, TCS_EX_FLATSEPARATORS);
			}
			else
				tab.ModifyStyle(TCS_FLATBUTTONS | TCS_BUTTONS, AppConfig.kktab ? TCS_BOTTOM : 0);
		}
	}

	while (AppConfig.max_history < AppConfig.history.GetCount())
		AppConfig.history.RemoveTail();

	while (AppConfig.max_history < address_bar.GetCount())
		address_bar.DeleteString(0);

#ifdef	_COMBO_
	if (use_ie_fav != AppConfig.use_ie_fav
		|| autowrap_favorite != AppConfig.autowrap_favorite
		|| autosort_favorite != AppConfig.autosort_favorite)
		LoadWebFavorites();
#endif

	view.Invalidate(FALSE);
}

void CMainFrame::LoadHistoryMenu()
{
	history_menu = CreatePopupMenu();	int i;
	for (i = 0;i < AppConfig.favorites.history.GetSize();i++)
		AppendMenu(history_menu, MF_OWNERDRAW,
				   ID_FIRST_HISTORY + i, LPCTSTR(ID_FIRST_HISTORY + i));

	AppConfig.favorites.favmenus.Add(history_menu);

	for (POSITION p = AppConfig.history.GetHeadPosition(); p; AppConfig.history.GetNext(p))
	{
		CString& address = AppConfig.history.GetAt(p);
#ifdef	_COMBO_
		if (0 == strncmp(address, "telnet", 6))
#endif
			AddToTypedHistory(address);
	}
}

void CMainFrame::OnColorConfig()
{
	CColorConfigDlg dlg;
	memcpy(dlg.colormap, AppConfig.colormap, sizeof(AppConfig.colormap));
	if (dlg.DoModal() == IDOK)
	{
		memcpy(AppConfig.colormap, dlg.colormap, sizeof(AppConfig.colormap));
		view.Invalidate(FALSE);
	}
}

void CMainFrame::OnClearHistory()
{
	if (MessageBox(LoadString(IDS_CLEAR_ADS_CONFIRM), LoadString(IDS_CONFIRM), MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		AppConfig.history.RemoveAll();
		address_bar.ResetContent();
		UpdateAddressBar();
	}

	if (MessageBox(LoadString(IDS_CLEAR_HISTORY_CONFIRM), LoadString(IDS_CONFIRM), MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		int c = GetMenuItemCount(history_menu);
		while (c--)
			DeleteMenu(history_menu, 0, MF_BYPOSITION);
		AppConfig.favorites.history.RemoveAll();
	}
	SaveHistory();
}

void CMainFrame::OnCustomize()
{
	CCustomizeDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		HMENU tmp = main_menu;
		RemoveMenu(tmp, 4, MF_BYPOSITION);
		DestroyMenu(main_menu);
		LoadUI();
	}
}

void CMainFrame::OnAutoDBCSDetect()
{
	CRect rc;
	toolbar.GetItemRect(toolbar.CommandToIndex(ID_AUTO_DBCS), rc);
	toolbar.ClientToScreen(rc);
	::TrackPopupMenu(auto_dbcs_menu, TPM_LEFTALIGN,
					 rc.left, rc.bottom, 0, m_hWnd, NULL);
}

void CMainFrame::OnAddToFavorite()
{
#if defined	_COMBO_
	if (!view.con || view.con->is_ansi_editor)
		return;
	OnFavorite(
		(
			view.con->is_web ?
			(ID_FIRST_WEB_FAVORITE + AppConfig.favorites.web_fav.GetSize())
			: (ID_FIRST_BBS_FAVORITE + AppConfig.favorites.bbs_fav.GetSize())
		) - 4);
#else
	if (!view.telnet || view.telnet->is_ansi_editor)
		return;
	OnFavorite(ID_FIRST_BBS_FAVORITE + AppConfig.favorites.bbs_fav.GetSize() - 4);
#endif
}

void CMainFrame::OnSitesList()
{
	CListDlg list(&view);
	list.parent = this;
	list.m_InitPath = AppConfig.last_bbslist_item;
	list.DoModal();
	AppConfig.last_bbslist_item = list.m_InitPath;
}

int CMainFrame::ConnToIndex(CConn *conn)
{
	int idx = 0;
	TCITEM item;
	item.mask = TCIF_PARAM;

	while (tab.GetItem(idx, &item) && item.lParam != (LPARAM)conn)
		idx++;
	return (idx >= tab.GetItemCount()) ? -1 : idx;
}

void CMainFrame::SwitchToConn(int index)
{
	CConn *newcon = tab.GetCon(index);

	MouseCTL_Reset();

#ifdef	_COMBO_
	prev_conn = view.con;
#else
	prev_conn = view.telnet;
	if (prev_conn)
	{
		if (prev_conn->is_telnet)
			reinterpret_cast<CTelnetConn*>(prev_conn)->is_getting_article = false;
		view.KillTimer(ID_MOVIETIMER);
	}
#endif

	if (!newcon)
	{
#if defined _COMBO_ //////////////////////
		view.con = NULL;
#endif /////////////////////////////////
		view.telnet = NULL;
		view.SetFocus();
		view.SetCaretPos(CPoint(view.left_margin, view.top_margin + view.lineh - 2));
		CRect &view_rect = view.view_rect;
		view.GetClientRect(view_rect);
		view.AdjustFont(view_rect.right, view_rect.bottom);
		UpdateUI();
		SetWindowText(CMainFrame::window_title + 3);
		UpdateAddressBar();
		UpdateStatus();
		view.Invalidate(FALSE);
#if defined	_COMBO_ ///////////////////
		view.ShowWindow(SW_SHOW);
#endif //////////////////////////////////
		return;
	}

#if defined _COMBO_ //////////////////////
	if (newcon == view.con)
		return;
#else
	if (newcon == view.telnet)
		return;
#endif /////////////////////////////////

	int idx = ConnToIndex(newcon);

	int cols_per_page = 0;
	int lines_per_page = 0;

#if defined _COMBO_/////////////////////////
	if (view.telnet)	//如果原本是BBS，記錄原本的行列數
	{
		cols_per_page = view.telnet->site_settings.cols_per_page;
		lines_per_page = view.telnet->site_settings.lines_per_page;
	}
	else	//如果原本是網頁，或是根本沒畫面
	{
		if (view.con)
		{
			((CWebConn*)view.con)->web_browser.ShowWindow(SW_HIDE);
			((CWebConn*)view.con)->web_browser.EnableWindow(FALSE);
		}
	}
	view.con = newcon;

	if (newcon->is_web)	//如果新畫面是WWW
	{
		view.telnet = NULL;
		view.GetWindowRect(view.view_rect);
		::MapWindowPoints(NULL, m_hWnd, (LPPOINT)&view.view_rect, 2);
		((CWebConn*)view.con)->web_browser.MoveWindow(view.view_rect);
		((CWebConn*)view.con)->web_browser.ShowWindow(SW_SHOW);
		view.ShowWindow(SW_HIDE);
		((CWebConn*)view.con)->web_browser.EnableWindow(TRUE);
		((CWebConn*)view.con)->web_browser.SetFocus();
	}
	else	//如果新畫面是BBS
	{
#endif///////////////////////////////////
		view.telnet = (CTelnetConn*)newcon;
		SetFocus();
		if (view.telnet->site_settings.cols_per_page != cols_per_page || view.telnet->site_settings.lines_per_page != lines_per_page)
		{
			CRect& view_rect = view.view_rect;
			view.GetClientRect(view_rect);
			view.AdjustFont(view_rect.right, view_rect.bottom);
		}
#if defined _COMBO_/////////////////////////
		view.ShowWindow(SW_SHOW);
#endif////////////////////////////////////
		view.Invalidate(FALSE);

		TCITEM item;
		item.mask = TCIF_IMAGE;
		if (view.telnet->is_connected)
		{
			item.iImage = 0;
			tab.SetItem(idx, &item);
		}
		view.telnet->UpdateCursorPos();
#if defined _COMBO_////////////////////////
	}
#endif///////////////////////////////////

	tab.SetCurSel(idx);
	UpdateStatus();
	UpdateAddressBar();
	UpdateStatus();
	UpdateUI();

	SetWindowText(newcon->name + CMainFrame::window_title);
}

void CMainFrame::OnPrevConn()
{
	int sel = tab.GetCurSel() - 1;
	if (sel < 0)
	{
		int last = tab.GetItemCount() - 1;
		if (AppConfig.scrolltab && last > 0)
			SwitchToConn(last);
		return;
	}
	SwitchToConn(sel);
}

void CMainFrame::OnNextConn()
{
	int sel = tab.GetCurSel() + 1;
	if (sel >= tab.GetItemCount())
	{
		if (AppConfig.scrolltab && tab.GetItemCount() > 0)
			SwitchToConn(0);
		return;
	}
	SwitchToConn(sel);
}

void CMainFrame::OnHotkeySwitch(UINT id)
{
	id -= ID_SWITCHCON1;
	int c = tab.GetItemCount();
	if ((int)id < c)
	{
		if ((int)id != tab.GetCurSel())
			SwitchToConn(id);
	}

}

void CMainFrame::OnLastCon()
{
	SwitchToConn(tab.GetItemCount() - 1);
}

void CMainFrame::UpdateUI()
{
	if (view.telnet)
	{
		CReBarCtrl& rbc = rebar.GetReBarCtrl();
		if (AppConfig.use_ansi_bar || view.telnet->is_ansi_editor)
			rbc.ShowBand(rbc.IDToIndex(2), TRUE);
		else
			rbc.ShowBand(rbc.IDToIndex(2), FALSE);
#if defined	_COMBO_
		progress_bar.ShowWindow(SW_HIDE);
#endif
	}
	else
	{
		CReBarCtrl& rbc = rebar.GetReBarCtrl();
		if (AppConfig.use_ansi_bar)
			rbc.ShowBand(rbc.IDToIndex(2), TRUE);
		else
			rbc.ShowBand(rbc.IDToIndex(2), FALSE);

		view.ShowScrollBar(SB_VERT, AppConfig.site_settings.showscroll);
#if defined	_COMBO_
		if (view.con)
		{
			progress_bar.SetRange32(0, ((CWebConn*)view.con)->web_browser.pgsmax);
			progress_bar.SetPos(((CWebConn*)view.con)->web_browser.pgs);
			progress_bar.ShowWindow(SW_SHOW);
		}
		else
			progress_bar.ShowWindow(SW_HIDE);
#endif
	}
	view.SetScrollBar();
}

void CMainFrame::OnAddToHome()
{
	CString txt;

#ifdef _COMBO_
	if (!view.con || view.con->is_ansi_editor)
		return;
	if (view.con->is_web)
	{
		txt = "w" + view.con->name;
		txt += '\t';
		txt += view.con->address;
	}
	else
	{
		txt = 's' + view.telnet->name;
		txt += '\t';
		txt += view.telnet->address.Mid(9);
		if (view.telnet->port != 23)
		{
			char port_str[16];
			sprintf(port_str, ":%d", view.telnet->port);
			txt += port_str;
		}
	}
#else
	if (!view.telnet || view.telnet->is_ansi_editor)
		return;

	txt = "s" + view.telnet->name;
	txt += '\t';
	txt += view.telnet->address;
	if (view.telnet->port != 23)
	{
		char port_str[16];
		sprintf(port_str, ":%d", view.telnet->port);
		txt += port_str;
	}
#endif

	txt += "\x0d\x0a";
	CFile file;
	char* data = NULL;
	DWORD len = 0;
	if (file.Open(ConfigPath + HOMEPAGE_FILENAME, CFile::modeRead))
	{
		len = file.GetLength();
		data = new char[len+2];
		file.Read(data, len);
		file.Abort();
		if (strstr(data, txt))
		{
			delete []data;
			return;
		}
		data[len] = 0;
	}

	if (file.Open(ConfigPath + HOMEPAGE_FILENAME, CFile::modeCreate | CFile::modeWrite))
	{
		if (data)
		{
			for (char *pline = strtok(data, "\x0d\x0a"); pline; pline = strtok(NULL, "\x0d\x0a"))
			{
				if (*pline)
				{
					file.Write(pline, strlen(pline));
					file.Write("\x0d\x0a", 2);
				}
			}
			delete []data;
		}
		file.Write(LPCTSTR(txt), txt.GetLength());
		file.Close();
	}
}

void CMainFrame::OnNciku()
{
	CTelnetConn* telnet = view.telnet;
	if (!telnet)
	{
#if defined	_COMBO_
		CConn* con = view.con;
		if (con)
		{
			((CWebConn*)view.con)->web_browser.SetFocus();
			((CWebConn*)view.con)->web_browser.wb_ctrl.ExecWB(OLECMDID_COPY, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
		}
#endif
		return;
	}
	CString tmp = "http://www.nciku.com.tw/search/all/"+view.GetSelText();
#if defined _COMBO_
	((CMainFrame*)AfxGetApp()->m_pMainWnd)->view.ConnectWeb(tmp, TRUE);
#else
	ShellExecute(m_hWnd, "open", tmp, NULL, NULL, SW_SHOWMAXIMIZED);
#endif
}

void CMainFrame::OnWikipedia()
{
	CTelnetConn* telnet = view.telnet;
	if (!telnet)
	{
#if defined	_COMBO_
		CConn* con = view.con;
		if (con)
		{
			((CWebConn*)view.con)->web_browser.SetFocus();
			((CWebConn*)view.con)->web_browser.wb_ctrl.ExecWB(OLECMDID_COPY, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
		}
#endif
		return;
	}
	CString tmp = "http://zh.wikipedia.org/wiki/"+view.GetSelText();
#if defined _COMBO_
	((CMainFrame*)AfxGetApp()->m_pMainWnd)->view.ConnectWeb(tmp, TRUE);
#else
	ShellExecute(m_hWnd, "open", tmp, NULL, NULL, SW_SHOWMAXIMIZED);
#endif
}

void CMainFrame::OnCopy()
{
	CTelnetConn* telnet = view.telnet;
	if (!telnet)
	{
#if defined	_COMBO_
		CConn* con = view.con;
		if (con)
		{
			((CWebConn*)view.con)->web_browser.SetFocus();
			((CWebConn*)view.con)->web_browser.wb_ctrl.ExecWB(OLECMDID_COPY, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
		}
#endif
		return;
	}
	view.CopySelText();
}

void CMainFrame::OnFont()
{
#if defined(_COMBO_)
	CConn* con = view.con;
	if (con && con->is_web)
	{
		HMENU config = GetSubMenu(main_menu, 2);
		HMENU web_pop = GetSubMenu(config, 7);
		HMENU pop = GetSubMenu(web_pop, 2);
		CRect rc;
		toolbar.GetItemRect(toolbar.CommandToIndex(ID_FONT_BTN), rc);
		toolbar.ClientToScreen(rc);
		TrackPopupMenu(pop, TPM_LEFTALIGN, rc.left, rc.bottom, 0, m_hWnd, NULL);
		return;
	}
#endif
	OnBBSFont();
}

bool DownLoadURL(LPCTSTR url, LPCTSTR out_path, bool show_ui = false);

void CMainFrame::OnPasteTinyUrl()
{
	CString url = TINY_URL;
	CString text;
	if (!CClipboard::GetText(text)) //第一次從剪貼簿取字串 (ansi字串)
		return;
	url += text;

	if (!DownLoadURL(url, ::AppPath + TINYURL_TEMP_FILENAME))
		return;

	CFile f;
	char* str = new char[8192];
	char* found = NULL;
	if (f.Open(::AppPath + TINYURL_TEMP_FILENAME, CFile::modeRead))
	{
		CArchive* ar;
		ar = new CArchive(&f, CArchive::load);
		ar->Read(str, 8192);
		if (found = strstri(str, "Open in new window"))
		{
			char* s = str;
			char* t = NULL;
			if ((s = strstri(found - 150, "http")) && (t = strnstri(s, "</b>", 40)))
			{
				char* tinyurl = new char[t-s+1];
				strncpy(tinyurl, s, t - s);
				*(tinyurl + (t - s)) = '\0';
				view.telnet->SendString(tinyurl);
				delete []tinyurl;
			}
		}
		ar->Close();
		f.Close();
		unlink(::AppPath + TINYURL_TEMP_FILENAME);
	}
	delete []str;
}

void CMainFrame::OnCopyPaste()
{
	OnCopy();
	OnPaste();
}

void CMainFrame::OnPaste()
{
	if (!view.telnet)
	{
#if defined	_COMBO_
		if (view.con)
		{
			((CWebConn*)view.con)->web_browser.wb_ctrl.SetFocus();
			((CWebConn*)view.con)->web_browser.wb_ctrl.ExecWB(OLECMDID_PASTE, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
		}
#endif

		return;
	}

	CString text;
	if (!CClipboard::GetText(text))
		return;

	//偵測是否包含色彩碼
	//如果有色彩碼，改成彩色貼上
	if (IsContainAnsiCode(text))
		view.SendAnsiString(text);
	else	//否則正常貼上純文字
	{
		//並且如果不包含色彩碼，再重新從剪貼簿取一次字串 (改取Unicode字串)

		wchar_t* pwstr = NULL;
		if (CClipboard::GetTextW(&pwstr))
		{
			UINT cp_id = view.GetCodePage();
			int len = wcslen(pwstr) * sizeof(wchar_t) + 1;
			char* pstr = new char[ len ];
			memset(pstr, 0, len);

			if (cp_id == 950)
			{
				g_ucs2conv.Ucs22Big5(pwstr, pstr);
			}
			else
			{
				::WideCharToMultiByte(cp_id, 0, pwstr, -1, pstr, len, NULL, NULL);
			}

			text.Empty();
			text = pstr;
			if (pstr)
				delete [] pstr;
			if (pwstr)
				delete [] pwstr;
			//考慮到 server 端實際接收資料的速度不快，
			//所以這邊試圖忽略掉從剪貼簿取兩次資料的開銷損失
		}
		view.telnet->SendString(text);
	}
}

void CMainFrame::OnSelAll()
{
	if (!view.telnet)
	{
#if defined	_COMBO_
		if (view.con)
		{
			((CWebConn*)view.con)->web_browser.SetFocus();
			((CWebConn*)view.con)->web_browser.wb_ctrl.ExecWB(OLECMDID_SELECTALL, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
		}
#endif
		return;
	}
	SCROLLINFO info;
	view.GetScrollInfo(SB_VERT, &info);

	CTelnetConn *telnet = view.telnet;
	telnet->sel_start.x = 0;
	telnet->sel_start.y = info.nPos;
	telnet->sel_end.x = telnet->site_settings.cols_per_page;
	telnet->sel_end.y = info.nPos + telnet->site_settings.lines_per_page - 1;
	view.Invalidate(FALSE);
}

void CMainFrame::OnPlayMovie()
{
	if (!view.telnet)
		return;
	view.SetTimer(ID_MOVIETIMER, 1000, NULL);
}

static CDownloadArticleDlg* download_article_dlg = NULL;

void CMainFrame::OnCopyArticle()
{
	if (!view.telnet)
		return;
	view.telnet->CopyArticle(false, false);
}

void CMainFrame::OnCopyArticleWithAnsi()
{
	if (!view.telnet)
		return;
	view.telnet->CopyArticle(true, false);
}

void CMainFrame::OnDownloadArticle()
{
	if (!view.telnet)
		return;
	view.telnet->CopyArticle(true, true);
}

void CMainFrame::OnExit()
{
	OnClose();
	// PostMessage(WM_CLOSE);
}

void CMainFrame::OnHelp()
{
#if defined	_COMBO_
	view.ConnectWeb("http://pcman.ptt.cc/pcman_help.html", TRUE);
#else
//	if((long)ShellExecute(m_hWnd,"open",AppPath+"pcman.html",NULL,NULL,SW_SHOWMAXIMIZED)<=32)
	ShellExecute(m_hWnd, "open", "http://pcman.ptt.cc/pcman_help.html", NULL, NULL, SW_SHOWMAXIMIZED);
#endif
}

void CMainFrame::OpenHomepage()
{
//開啟首頁
	CString str, adv;
	CFile cfgf;
	if (cfgf.Open(ConfigPath + HOMEPAGE_FILENAME, CFile::modeRead))
	{
		adv = LoadString(IDS_HOMEPAGE_NAME) + PATH_SEPARATOR;
		CArchive ar(&cfgf, CArchive::load);
		while (ar.ReadString(str))
		{
			if (str.IsEmpty())
				continue;

			// FIXME: prevent duplicated items
			if (AppConfig.save_session)
			{
				int p = str.Find('\t');
				if (p < 0)
					break;
				CString address = str.Mid(p + 1);
				int port = 0;
				CTelnetConn* telnet = NULL;
				if (str[0] == 's')	// telnet
				{
					p = address.Find(':');
					if (p > 0)
					{
						port = atoi((LPCTSTR(address) + p + 1));
						address = address.Left(p);
					}
					else
						port = 23;

#if defined(_COMBO_)
					address = "telnet://" + address;
#endif
				}
				int i;
				int c = tab.GetItemCount();
				for (i = 0; i < c; i++)
				{
					CConn* pcon = tab.GetCon(i);
					// The same site has been opened
					if (0 == pcon->address.CompareNoCase(address))
					{
						if (pcon->is_telnet)	// this is a telnet connection
						{
							CTelnetConn* telnet = static_cast<CTelnetConn*>(pcon);
							if (port == telnet->port)  // both address and port are duplicated
							{
								// If the site has been loaded with advanced settings, or
								// we cannot provide more useful advanced settings over the loaded one,
								// just skip this site.
								if (! telnet->cfg_path.IsEmpty() || adv.IsEmpty())
									break;
							}
						}
						else
						{
							break;
						}
					}
				}
				if (i < c)
					continue;
			}
			view.ConnectStr(str, adv);
		}

		ar.Close();
		cfgf.Close();
	}
}


void CMainFrame::OpenLastSession()
{
	CString str;
	CString adv;
	CFile logf;
	if (logf.Open(ConfigPath + SESSION_FILENAME, CFile::modeRead))
	{
		DWORD len = logf.GetLength();
		char* buf = new char[len + 1];
		logf.Read(buf, len);
		buf[len] = '\0';
		logf.Close();

		char *line, *nextline = NULL;
		for (line = buf; line; line = nextline)
		{
			nextline = strnextline(line);
			if (!*line)
				continue;
			str = line;

			adv.Empty();
			if (*line == 's')
			{
				// FIXME: what's this??
				if (str[str.GetLength()-1] == PATH_SEPARATOR)	// has adv info
				{
					int p = str.ReverseFind('\t');
					adv = str.Mid(p + 1);
					str = str.Left(p);
				}
			}
			view.ConnectStr(str, adv);
			switch(AppConfig.saved_charset)
			{
				case 0:
					break;
	
				case 1:
					this->OnSetCharset(ID_SET_CHARSET_DEFAULT);
					break;
				case 2:
					this->OnSetCharset(ID_SET_CHARSET_CP950);
					break;
				case 3:
					this->OnSetCharset(ID_SET_CHARSET_CP936);
					break;
				case 4:
					this->OnSetCharset(ID_SET_CHARSET_CP932);
					break;
				case 5:
					this->OnSetCharset(ID_SET_CHARSET_UTF8);
					break;
			}
		}
		delete []buf;
		//Restore Charset Setting
	}
		
}

void CMainFrame::OnNewConn()
{
	CAddressDlg dlg;

	if (dlg.DoModal() != IDOK)
	{
		dlg.name.Empty();
		dlg.address.Empty();
		return;
	}

	view.Connect(dlg.address, dlg.name, dlg.port);
	dlg.name.Empty();
	dlg.address.Empty();
	
	
	if(!setCharset)
	{
		switch(AppConfig.saved_charset)
		{
			case 0:
				break;
			
			case 1:
				this->OnSetCharset(ID_SET_CHARSET_DEFAULT);
				break;
			case 2:
				this->OnSetCharset(ID_SET_CHARSET_CP950);
				break;
			case 3:
				this->OnSetCharset(ID_SET_CHARSET_CP936);
				break;
			case 4:
				this->OnSetCharset(ID_SET_CHARSET_CP932);
				break;
			case 5:
				this->OnSetCharset(ID_SET_CHARSET_UTF8);
				break;
		}	
		
	}

}

void CMainFrame::OnSetCharset(UINT nID)
{
//#ifdef	_COMBO_
//	return;
//#endif
	if (!view.telnet)
		return;

	static UINT prev_id = 0;

	switch (nID)
	{
	case ID_SET_CHARSET_DEFAULT:
		view.SetCodePage(::GetACP());
		AppConfig.saved_charset = 1;
		break;
	case ID_SET_CHARSET_CP950:
		view.SetCodePage(950);
		AppConfig.saved_charset = 2;
		break;
	case ID_SET_CHARSET_CP936:
		view.SetCodePage(936);
		AppConfig.saved_charset = 3;
		break;
	case ID_SET_CHARSET_CP932:
		view.SetCodePage(932);
		AppConfig.saved_charset = 4;
		break;
	case ID_SET_CHARSET_UTF8:
		view.SetCodePage(65001);
		AppConfig.saved_charset = 5;
		break;
	}
	setCharset = true;

	CMenu* menu = GetMenu();
	menu->CheckMenuItem(prev_id, MF_UNCHECKED);
	menu->CheckMenuItem(nID, MF_CHECKED);
	view.Invalidate(FALSE);
	prev_id = nID;
}

void CMainFrame::OnBBSFont()
{
	CTelnetConn* telnet = view.telnet;

	CFontDialog dlg;
	dlg.m_cf.lpLogFont = &AppConfig.font_info;
	dlg.m_cf.Flags |= AppConfig.old_textout ?
					  CF_FIXEDPITCHONLY | CF_INITTOLOGFONTSTRUCT : CF_INITTOLOGFONTSTRUCT;
	dlg.m_cf.Flags &= ~CF_EFFECTS;
	if (dlg.DoModal() == IDOK)
	{
		LOGFONT &font_info = view.font_info;
		fnt.DeleteObject();
		dlg.GetCurrentFont(&font_info);
		if (!*font_info.lfFaceName)
			strcpy(font_info.lfFaceName, LoadString(IDS_DEFAULT_FONT_FACE));
		fnt.CreateFontIndirect(&AppConfig.font_info);

		int cols_per_page = telnet ? telnet->site_settings.cols_per_page : AppConfig.site_settings.cols_per_page;
		int lines_per_page = telnet ? telnet->site_settings.lines_per_page : AppConfig.site_settings.lines_per_page;
		CRect rc;
		view.GetClientRect(&rc);
		if (AppConfig.auto_font)	//如果使用動態字體調整
		{
			view.AdjustFont(rc.right, rc.bottom);
		}
		else
		{
			CWindowDC dc(this);
			CGdiObject* old = dc.SelectObject(&fnt);
			CSize& sz = dc.GetTextExtent(LoadString(IDS_DOUBLE_SPACE_CHAR) , 2);	// 全形空白font的size
			dc.SelectObject(&old);
			view.chw = sz.cx / 2;//半型字寬
			view.lineh = sz.cy;//字高
			view.left_margin = (rc.right - view.chw * cols_per_page) / 2;
			view.top_margin = (rc.bottom - view.lineh * lines_per_page) / 2;
			view.CreateCaret();
			view.ShowCaret();
			if (telnet)
				telnet->UpdateCursorPos();
			else
				SetCaretPos(CPoint(view.left_margin, view.top_margin + view.lineh - 2));
		}
		view.Invalidate(FALSE);
	}
}

void CMainFrame::OnDownloadPage()
{
	const char url[] = "http://of.openfoundry.org/projects/744/download";
#ifdef	_COMBO_
	((CMainFrame*)AfxGetApp()->GetMainWnd())->view.ConnectWeb(url, TRUE);
#else
	ShellExecute(m_hWnd, "open", url , NULL, NULL, SW_SHOW);
#endif
}


void CMainFrame::OnInitMenuPopup(CMenu* pMenu,UINT nIndex,BOOL bSysMenu)
{
	if(nIndex==6)
	{
		int c = pMenu->GetMenuItemCount();
		CString text,result;
		pMenu->GetMenuString(1,text,MF_BYPOSITION);
		if(strlen(text)==0||text[0] != '-')
			return;
		
		pMenu->GetMenuString(0,text,MF_BYPOSITION);
		CSearchPlugin *plugin_2 = new CSearchPlugin();
		CString URLCString = _T("http://www.e390.com/cgi-bin/e390dic?MYINPUT=" + text + "&&CMP=1&UID=%23%23UID");
		result = plugin_2->GetWebPage(URLCString);
		if (result.GetLength() > 0)
		{
			result = plugin_2->ProcessContent(result);
			result.TrimLeft();
			if (result.GetLength() > 0)
				result.TrimRight();
			else
				result = _T("查無此翻譯");
		}
		else
		{			
			result = _T("查無此翻譯");
		}
		pMenu->RemoveMenu(1, MF_BYPOSITION);
		pMenu->InsertMenu(1, MF_BYPOSITION, CSearchPluginCollection::ID_TRANSLATION, result);
	}
	CFrameWnd::OnInitMenuPopup(pMenu, nIndex, bSysMenu);
}

void CMainFrame::OnCheckUpdate()
{
	CWinThread * controlThread = AfxBeginThread(BackgroundAutoUpdate,NULL,THREAD_PRIORITY_NORMAL);
	//TODO: 沒有更新的話不會有任何訊息
}