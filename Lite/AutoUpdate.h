// AutoUpdater.h: interface for the CAutoUpdater class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUTOUPDATER_H__227B2B21_B6AE_4164_B3A5_BFDAAF13D85D__INCLUDED_)
#define AFX_AUTOUPDATER_H__227B2B21_B6AE_4164_B3A5_BFDAAF13D85D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Wininet.h>

#define UPDATE_CHECK_URL "http://of.openfoundry.org/download_path/pcman/update.feed/"
#define UPDATE_DOWNLOAD_URL "http://of.openfoundry.org/download_path/pcman/"

#ifdef	_COMBO_
	#define UPDATE_FILE "update_combo.txt"
	#define InstallerName "PCManCB"
#else	
	#define UPDATE_FILE "update_lite.txt"
	#define InstallerName "PCMan"
#endif

#define LOCATION_UPDATE_FILE_CHECK _T(UPDATE_FILE)
static UINT NEAR WM_COMMIT_UPDATE = RegisterWindowMessage("COMMIT_UPDATE");
static UINT NEAR WM_DOWNLOAD_UPDATE_COMPLETE = RegisterWindowMessage("DOWNLOAD_UPDATE_COMPLETE");

class CAutoUpdater  
{
public:
	CAutoUpdater();
	virtual ~CAutoUpdater();

	enum ErrorType { Success, InternetConnectFailure, InternetSessionFailure, 
						ConfigDownloadFailure, FileDownloadFailure, NoExecutableVersion,
						UpdateNotRequired, UpdateNotComplete };

	ErrorType CheckForUpdate();	
	HINTERNET GetSession(CString &URL);

	bool InternetOkay();
	bool DownloadConfig(HINTERNET hSession, BYTE *pBuf, DWORD bufSize);

	CString GetFileVersion(LPCTSTR file);
	int		CompareVersions(CString ver1, CString ver2);
	bool	IsDigits(CString text);
	CString GetExecutable();
	bool	Switch(CString executable, CString update, bool WaitForReboot);
	int bTransferSuccess;

private:
	HINTERNET hInternet;
	
};
class CDownloadUpdateDlg : public CDialog
{
public:
	CDownloadUpdateDlg()
			: CDialog(IDD_DOWNLOADING_UPDATE,AfxGetMainWnd())
	{
	}

	BOOL OnInitDialog()
	{
		CDialog::OnInitDialog();
		return TRUE;
	}

	void OnCancel()
	{
	    CDialog::OnCancel();
	}

};



class CAutoUpdater_DownloadInfo
{
public:
	HINTERNET hSession;
	HWND hWnd;
	LPCTSTR localFile;
	CDownloadUpdateDlg *dlg;
	CAutoUpdater_DownloadInfo(HINTERNET hSession,LPCTSTR localFile,CDownloadUpdateDlg *dlg)
	{
		this->hSession = hSession;
		this->localFile = localFile;
		hWnd=AfxGetMainWnd()->GetSafeHwnd();
		this->dlg =  dlg;
	}
};


UINT DownloadUpdateFile(LPVOID pParam);
static CDownloadUpdateDlg *download_update_dlg;
#endif // !defined(AFX_AUTOUPDATER_H__227B2B21_B6AE_4164_B3A5_BFDAAF13D85D__INCLUDED_)
