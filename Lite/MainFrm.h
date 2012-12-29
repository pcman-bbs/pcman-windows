// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__9231A868_8865_11D5_B2E5_140094CD0000__INCLUDED_)
#define AFX_MAINFRM_H__9231A868_8865_11D5_B2E5_140094CD0000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TermView.h"
#include "ListDlg.h"	// Added by ClassView
#include "MemIniFile.h"
#include "CustomTabCtrl.h"
#include "CustomToolBar.h"
#include "AutoComplete.h"	// Added by ClassView

#include "AutoUpdate.h"
#include "InstantTranDlg.h"

#if defined _COMBO_
#include "../Combo/adlist.h"
#include "../Combo/aditem.h"
#include "../Combo/SearchBar.h"
#endif

#define	CT_MENU		1
#define	CT_HAS_SUB	2
#define	CT_EMPTY_SUB	2
#define	CT_LAST		4
#define	CT_CMD		8

const WPARAM AC_MULTIPCMAN	= 0;
const WPARAM AC_PCMANLOCKED	= 1;
const char TINY_URL[] = "http://tinyurl.com/create.php?url=";
const char TINYURL_TEMP_FILENAME[] = "Tinyurl";


#define ID_MOUSE_SEL_TIMER	100

class CMainFrame : public CFrameWnd
{
public:
	CMainFrame();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void RecalcLayout(BOOL bNotify = TRUE);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	int ConnToIndex(CConn* conn);
	void SwitchToConn(int index);
	void SwitchToConn(CConn* conn) { SwitchToConn(ConnToIndex(conn)); }

	void UpdateAddressBar();
	void OnKKmanStyleTab();
	void OnShowAddressBar();

	void OnInstantTranslation();

	void OnNewConnectionAds(LPCTSTR cmdline);
	void OnRClickTab(NMHDR *pNMHDR, LRESULT *pResult);
	LRESULT OnNewConnection(WPARAM wparam, LPARAM lparam);
	void OnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
	void UpdateStatus();
	void UpdateUI();

	void SendFreqStr(CString str , BYTE inf);
	void LoadFrequentlyUsedStr();
	void AddToHistoryMenu(CString str);
	void AddToTypedHistory(CString address) ;
	void AddToHistory(CString address);

	void LoadBBSFavorites();
	void LoadHistoryMenu();
#ifdef	_COMBO_
	void LoadWebFavorites();
#endif

	void OpenHomepage();
	void OpenLastSession();

	virtual ~CMainFrame();

	static LPSTR mainfrm_class_name;

	CCustomToolBar	toolbar;
	CCustomTabCtrl	tab;
	CTermView view;
	CFont bar_font;
	CStatic status_bar;
	CComboBox address_bar;
#ifdef	_COMBO_
	CSearchBar search_bar;
#endif

	HMENU main_menu;
	HMENU edit_menu;
	HMENU auto_dbcs_menu;
	HMENU bbs_fav_menu;
#ifdef	_COMBO_
	HMENU web_fav_menu;
#endif
	HMENU ansi_menu;
	HMENU history_menu;

	HMENU tab_popup_menu;

	CImageList img_toolbar;
	CImageList img_icons;
	HICON icon;
	CReBar rebar;
	CAnsiBar ansi_bar;
	static const char* window_title;
	CAutoComplete auto_complete;

	ACCEL* accels;
	int accel_count;
	CToolBar close_btn;

	CStringArray hotstr;
	CByteArray hotstr_inf;
	CArray<ACCEL, ACCEL&> hotstr_acc;
	HACCEL hhotstr_acc;

	UINT showcmd;
	CConn* prev_conn;

#if defined _COMBO_
	HMENU webtab_popup_menu;
	CImageList img_webbar;
	CCustomToolBar	web_bar;
	CBitmap web_bar_bkgnd;
	CProgressCtrl progress_bar;
	HWND focus;
#endif

// Generated message map functions
public:
	BOOL LoadUI();
	void OnSwitchBack();
	void SaveHistory();
	void LoadHistory();
	void CloseConn(int i, bool confirm = false);
	int NewTab(CConn* pCon, BOOL bActive = FALSE, int idx = -1);
	void OnUpdateShowAnsiBar(CCmdUI* pCmdUI);
	void OnUpdateIsSite(CCmdUI* pCmdUI);
	void OnUpdatePaste(CCmdUI* pCmdUI);
	void OnUpdateIsBBSSite(CCmdUI *pCmdUI);
	void OnUpdateIsConn(CCmdUI* pCmdUI);
	void OnUpdateIsSel(CCmdUI *pCmdUI);
	void OnUpdateIsConnnected(CCmdUI *pCmdUI);
	void OnUpdateIsBBS(CCmdUI* pCmdUI);
	void OnUpdateSetCharset(CCmdUI* pCmdUI);
	void RestoreWindow();
	LRESULT OnNotifyIcon(WPARAM wp, LPARAM lp);
	LRESULT OnHotKey(WPARAM wp, LPARAM lp);
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnShowTabBar();
	afx_msg void OnShowToolbar();
	afx_msg void OnCloseBtn();
	afx_msg void OnShowStatusbar();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnPgd();
	afx_msg void OnPgu();
	afx_msg void OnAddressFocus();
	afx_msg void OnShowAnsiBar();
	afx_msg void OnShowScroll();
	afx_msg void OnSetDefaultProgram();
	afx_msg void OnAutoSwitch();
	afx_msg void OnUpdateEditorInsertMode(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAutoswitch(CCmdUI* pCmdUI);
	afx_msg void OnUpdateKKmanStyleTab(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowScroll(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSmoothDraw(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowStatusbar(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowTabBar(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowToolbar(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAutoDBCSArrow(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAutoDBCSBackspace(CCmdUI* pCmdUI);
	afx_msg void OnAutoDBCSBackspace();
	afx_msg void OnAutoDBCSArrow();
	afx_msg void OnAutoDBCSDel();
	afx_msg void OnUpdateAutoDBCSDel(CCmdUI* pCmdUI);
	afx_msg void OnAutoDBCSMouse();
	afx_msg void OnUpdateAutoDBCSMouse(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowAddressBar(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCloseBtn(CCmdUI* pCmdUI);
	afx_msg void OnAddressBarComboOK();
	afx_msg void OnAnsiBarBk();
	afx_msg void OnAnsiBarFg();
	afx_msg void OnAnsiBarBlink();
	afx_msg void OnAnsiBarSend();
	afx_msg void OnAnsiBarSendDropDown();
	afx_msg void OnUpdateAnsiBarBlink(CCmdUI* pCmdUI);
	afx_msg void OnCustomizeMainToolbar();
	afx_msg void OnGetLocalIP();
	afx_msg LRESULT OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu);
	afx_msg void OnUpdateEditOpenURL(CCmdUI* pCmdUI);
	afx_msg void OnBackupConfig();
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpms);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpds);
	afx_msg void OnToolLock();
	afx_msg void OnUpdateSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnConnectClose();
	afx_msg void OnConnectCloseAllOthers();
	afx_msg void OnUpdateDisconnect(CCmdUI* pCmdUI);
	afx_msg void OnUpdateReconnect(CCmdUI* pCmdUI);
	afx_msg void OnSaveSession();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnShowFrequentlyUsedStr();
	afx_msg void OnEditFind();
	afx_msg void OnFrequentlyUsedStr(UINT id);
	afx_msg void OnViewFullscr();
	afx_msg void OnToolSymbols();
	afx_msg void OnViewConfig();
	afx_msg void OnClearHistory();
	afx_msg void OnColorConfig();
	afx_msg void OnCustomize();
	afx_msg void OnAutoDBCSDetect();
	afx_msg void OnAddToFavorite();
	afx_msg void OnAddToHome();
	afx_msg void OnSitesList();
	afx_msg void OnNewConn();
	afx_msg void OnPrevConn();
	afx_msg void OnNextConn();
	afx_msg void OnLastCon();
	afx_msg void OnHotkeySwitch(UINT id);
	afx_msg void OnCopy();
	afx_msg void OnCopyPaste();
	afx_msg void OnCopyArticle();
	afx_msg void OnCopyArticleWithAnsi();
	afx_msg void OnDownloadArticle();
	afx_msg void OnPlayMovie();
	afx_msg void OnFont();
	afx_msg void OnBBSFont();
	afx_msg void OnPaste();
	afx_msg void OnPasteTinyUrl();
	afx_msg void OnSelAll();
	afx_msg void OnExit();
	afx_msg void OnHelp();
	afx_msg void OnSetCharset(UINT nID);
	afx_msg void OnWebSearch();
	afx_msg void OnUpdateBBSMouseCTL(CCmdUI* pCmdUI);
	afx_msg void OnBBSMouseCTL();
	afx_msg void OnAutoUpdate();
	afx_msg LRESULT OnCommitUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDownLoadUpdateComplete(WPARAM wParam, LPARAM lParam);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu,UINT nIndex,BOOL bSysMenu);
	afx_msg void OnCheckUpdate();
	afx_msg void OnNciku();
	afx_msg void OnWikipedia();
	//}}AFX_MSG

	void OnFavorite(UINT id);
    LRESULT OnDownloadPage(WPARAM, LPARAM);

#if	_MFC_VER >= 0x0700
	afx_msg void OnActivateApp(BOOL bActive, DWORD hTask);
#else
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
#endif

	void OnAddressComboCancel();
	LRESULT OnQueryAppConfig(WPARAM w, LPARAM l);

#ifdef _COMBO_

	afx_msg void OnGoBack();
	afx_msg void OnGoForward();
	afx_msg void OnWebPageAddToAdFilter();
	afx_msg void OnWebHome();
	afx_msg void OnAdsHttp();
	afx_msg void OnAdsTelnet();
	afx_msg void OnNewWebConn();
	afx_msg void OnNewHome();
	afx_msg void OnEditAdFilter();
	afx_msg void OnIESetup();
	afx_msg void OnWebPagePrint();
	afx_msg void OnWebPagePrintSettings();
	afx_msg void OnWebPageOpen();
	afx_msg void OnWebPageViewSrc();
	afx_msg void OnNewCurPage();
	afx_msg void OnNewCurPageInIE();
	afx_msg void OnWebPageSaveAs();
	afx_msg void OnBlockPopup();
	afx_msg void OnSearchbarCleanup();
	afx_msg void OnAdsOpenNew();
	afx_msg void OnUpdateAddressBarOpenNew(CCmdUI* pCmdUI);
	afx_msg void OnImportIEFavorites();
	afx_msg void OnExportIEFavorites();
	afx_msg void OnUpdateBlockPopup(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSearchbarCleanup(CCmdUI* pCmdUI);
	afx_msg void OnCustomizeWebBar();
	afx_msg void OnShowWebBar();
	afx_msg void OnUpdateShowWebBar(CCmdUI* pCmdUI);
	afx_msg void OnShowSearchBar();
	afx_msg void OnUpdateShowSearchBar(CCmdUI* pCmdUI);
	afx_msg void OnUpdateIsWebPage(CCmdUI* pCmdUI);
	afx_msg void OnSearchBarFocus();

	void ImportIEFavorites();

	BOOL FindAdFilter(LPCTSTR title, LPCTSTR address);
	CEdit edit;
	LRESULT OnRemoveWebConn(WPARAM wparam, LPARAM lparam);
	BOOL FilterWebConn(CWebConn* web_conn);
	void OnToolbarMenuDropDown(NMHDR* pNMHDR, LRESULT* pResult);
	void OnSearchBarCancel();
#endif

	DECLARE_MESSAGE_MAP()
protected:
	void OnUpdateSaveSession(CCmdUI* pCmdUI);
#if defined(_COMBO_)
	void OnBrowserFontSize(UINT id);
#endif

	afx_msg BOOL OnToolTipNeedText(UINT id, NMHDR* nmhdr, LRESULT* r);
	inline void MinimizeToTray();
	static LRESULT CALLBACK AddressBarWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	WNDPROC old_address_bar_proc;

private:
	bool setCharset;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__9231A868_8865_11D5_B2E5_140094CD0000__INCLUDED_)
