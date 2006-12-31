// PCMan4.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PCMan.h"

#include "MainFrm.h"

#include <wininet.h>
#include "WinUtils.h"

#ifdef	_COMBO_
//	#include <..\src\occimpl.h>
	#include "..\Combo\CustSite.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString AppPath;
CString ConfigPath;

/////////////////////////////////////////////////////////////////////////////
// CApp

BEGIN_MESSAGE_MAP(CApp, CWinApp)
	//{{AFX_MSG_MAP(CApp)
	ON_COMMAND(ID_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CApp construction

CApp::CApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CApp object

CApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CApp initialization

BOOL CApp::InitInstance()
{
	::GetModuleFileName(AfxGetInstanceHandle(),AppPath.GetBuffer(MAX_PATH),_MAX_PATH);
	AppPath.ReleaseBuffer();
	AppPath=AppPath.Left(AppPath.ReverseFind('\\')+1);
	ConfigPath=AppPath+CONFIG_DIR;

//	OFSTRUCT ofs;
/*
	if(IsWinNT())	//如果是NT才支援多人環境
	{
		LPITEMIDLIST pidl;	CString PersonalConfigPath;
		::SHGetSpecialFolderLocation(NULL,CSIDL_PERSONAL,&pidl);
		BOOL personal=(::SHGetPathFromIDList(pidl,PersonalConfigPath.GetBuffer(_MAX_PATH))==NOERROR);
		IMalloc* im;
		SHGetMalloc(&im);
		im->Free(pidl);
		im->Release();
		PersonalConfigPath.ReleaseBuffer();
		PersonalConfigPath+="\\PCMan\\";
		PersonalConfigPath;
		//如果沒有個人設定檔
//		if(OpenFile(PersonalConfigPath+UI_FILENAME,&ofs,OF_EXIST)==HFILE_ERROR)
		if(!IsFileExist(PersonalConfigPath))
			AppConfig.BackupConfig(ConfigPath,PersonalConfigPath);
		ConfigPath=PersonalConfigPath;
	}
*/

	HWND mainwnd=GetTopWindow(HWND_DESKTOP);
	while( (mainwnd=::FindWindowEx(HWND_DESKTOP,mainwnd,CMainFrame::mainfrm_class_name,NULL)) )
	{
		if(	!::SendMessage(mainwnd,WM_QUERY_APPCONFIG,AC_MULTIPCMAN,0) &&
			!::SendMessage(mainwnd,WM_QUERY_APPCONFIG,AC_PCMANLOCKED,0) )
		{
			if(*m_lpCmdLine)
			{
				int l=strlen(m_lpCmdLine)+1;
				COPYDATASTRUCT d;
				d.lpData=m_lpCmdLine;
				d.cbData=l;
				d.dwData=0;
				::SendMessage(mainwnd,WM_COPYDATA,(WPARAM)NULL,(LPARAM)&d);
			}
			if(IsIconic(mainwnd))
				ShowWindow(mainwnd,SW_RESTORE);
			BringWindowToTop(mainwnd);
			SetForegroundWindow(mainwnd);
			return FALSE;
		}
	}

#if defined (_COMBO_)
	//IShellUIHandle
	CCustomOccManager *pMgr = new CCustomOccManager;

	// Create an IDispatch class for extending the Dynamic HTML Object Model 
//	m_pDispOM = new CImpIDispatch;
	//Drop target
//	m_pDropTarget = new CImpIDropTarget;

	// Set our control containment up but using our control container 
	// management class instead of MFC's default
	AfxEnableControlContainer(pMgr);
#endif

	AppConfig.Load(ConfigPath + CONFIG_FILENAME);

	CMainFrame* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;

	WSADATA wsadata;
    WSAStartup(MAKEWORD(2,2),&wsadata);

	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);

	//Register Hotkey
	BOOL r = RegisterHotKey(pFrame->m_hWnd,1,AppConfig.pcman_hotkey_mod,AppConfig.pcman_hotkey);

	pFrame->view.OnInitialUpdate();
	pFrame->SwitchToConn( 0 );
	//Restore Main Window Position
	AppConfig.mainwnd_state.Restore(pFrame->m_hWnd);
	pFrame->UpdateWindow();

	if(!*m_lpCmdLine)	//如果沒有命令列參數則開啟首頁
		pFrame->OpenHomepage();

	if( AppConfig.save_session )
		pFrame->OpenLastPages();

	if(*m_lpCmdLine)
		pFrame->OnNewConnectionAds(m_lpCmdLine);

//如果只允許執行一個 PCMan，則把User data設為1
	SetWindowLong(m_pMainWnd->m_hWnd,GWL_USERDATA,!AppConfig.multiple_instance);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CApp message handlers


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	static char web[];
	//{{AFX_MSG(CAboutDlg)
	afx_msg void OnMail();
	afx_msg void OnWeb();
	afx_msg void OnHelp();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

char CAboutDlg::web[]="http://pcman.ptt.cc/pcman_help.html";

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_BN_CLICKED(IDC_MAIL, OnMail)
	ON_BN_CLICKED(IDC_WEB, OnWeb)
	ON_BN_CLICKED(IDB_HELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CApp message handlers


void CAboutDlg::OnMail() 
{
	ShellExecute(m_hWnd,"open","mailto:hzysoft@sina.com.tw",NULL,NULL,SW_SHOW);	
}

void CAboutDlg::OnWeb() 
{
	web[20]=0;
#ifdef	_COMBO_
	((CMainFrame*)AfxGetApp()->m_pMainWnd)->view.ConnectWeb(web, TRUE);
#else
	ShellExecute(m_hWnd,"open",web,NULL,NULL,SW_SHOW);	
#endif
	web[20]='p';
}

void CAboutDlg::OnHelp() 
{
#ifdef	_COMBO_
	((CMainFrame*)AfxGetApp()->m_pMainWnd)->view.ConnectWeb(web, TRUE);
#else
	if((long)ShellExecute(m_hWnd,"open",AppPath+"pcman.html",NULL,NULL,SW_SHOWMAXIMIZED)<=32)
		ShellExecute(m_hWnd,"open",web,NULL,NULL,SW_SHOWMAXIMIZED);
#endif
}

int CApp::ExitInstance() 
{
	WSACleanup();
	return CWinApp::ExitInstance();
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CString about;
	about.LoadString(IDS_ABOUT_INFO);
	GetDlgItem(IDC_EDIT)->SetWindowText( about );
	return TRUE;
}

