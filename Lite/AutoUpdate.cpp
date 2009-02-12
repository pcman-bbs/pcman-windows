// AutoUpdater.cpp: implementation of the CAutoUpdater class.
//
//////////////////////////////////////////////////////////////////////

#include "stdAfx.h"
#include "AutoUpdate.h"
#include "AutoUpdateDlg.h"
#include "AppConfig.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define TRANSFER_SIZE 4096



CAutoUpdater::CAutoUpdater()
{
	// Initialize WinInet
	hInternet = InternetOpen("AutoUpdateAgent", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);	
}

CAutoUpdater::~CAutoUpdater()
{
	if (hInternet) {
		InternetCloseHandle(hInternet);
	}
}

// Check if an update is required
//
CAutoUpdater::ErrorType CAutoUpdater::CheckForUpdate()
{		
	if (!InternetOkay())
	{
		return InternetConnectFailure;
	}


	bTransferSuccess = false;


	// First we must check the remote configuration file to see if an update is necessary
	CString URL = CString(UPDATE_CHECK_URL) + CString(LOCATION_UPDATE_FILE_CHECK);
	HINTERNET hSession = GetSession(URL);
	if (!hSession)
	{
		return InternetSessionFailure;
	}

	BYTE pBuf[TRANSFER_SIZE];
	memset(pBuf, NULL, sizeof(pBuf));
	bTransferSuccess = DownloadConfig(hSession, pBuf, TRANSFER_SIZE);
	InternetCloseHandle(hSession);
	if (!bTransferSuccess)
	{
		return ConfigDownloadFailure;
	}

	// Get the version number of our executable and compare to see if an update is needed
	CString executable = GetExecutable();
	CString fileVersion = GetFileVersion(executable);\
	if (fileVersion.IsEmpty())
	{
		return NoExecutableVersion;
	}

	CString updateVersion = (char *) pBuf;
	if (CompareVersions(updateVersion, fileVersion) != 1)
	{	
		return UpdateNotRequired;
	}

	// At this stage an update is required	
	TCHAR path[MAX_PATH];
	GetTempPath(MAX_PATH, path);
	CString directory = path;
	
	// Download the updated file
	/*
	TCHAR *pToken = strtok(updateVersion.GetBuffer(256),_T("."));
	while(pToken!=NULL)
	{	
		if(IsDigits(pToken[0]))
			URL.Insert(URL.GetLength(),pToken[0]);
		pToken = strtok(NULL, _T("."));
	}
	*/
	updateVersion.TrimLeft();
	updateVersion.TrimRight();
	updateVersion.TrimRight(_T("."));
	URL = CString(UPDATE_DOWNLOAD_URL) + updateVersion + CString("/") + CString(InstallerName) + CString(".exe");
	updateVersion.ReleaseBuffer();
	DBG_PRINT(URL);
	
	hSession = GetSession(URL);
	if (!hSession)
	{
		return InternetSessionFailure;
	}

	CAutoUpdateDlg autoupdate_dlg;
	int nResult = autoupdate_dlg.DoModal();
	if (nResult==0)
	{
		AppConfig.autoupdate_disable=1;
		return UpdateNotComplete;	
	}
	else if (nResult==2)
	{
		return UpdateNotComplete;
	}

	// Proceed with the update
	CString updateFileLocation = directory+InstallerName+CString(".exe");
	download_update_dlg = new CDownloadUpdateDlg();
	CAutoUpdater_DownloadInfo * Info = new CAutoUpdater_DownloadInfo(hSession, updateFileLocation,download_update_dlg);
	

   	CWinThread * controlThread = AfxBeginThread(DownloadUpdateFile,Info,THREAD_PRIORITY_NORMAL);
	download_update_dlg->DoModal();
	
	//download_update_dlg->OnCancel();
	LPDWORD lpExitCode = new unsigned long;
	*lpExitCode = STILL_ACTIVE;
	//while(*lpExitCode==STILL_ACTIVE)
	//{
	//	GetExitCodeThread(controlThread->m_hThread,lpExitCode);
	//	::Sleep(1000);
	//}
	//bTransferSuccess = (int)(*lpExitCode);
	
	//InternetCloseHandle(hSession);
//	if (!bTransferSuccess)
//	{
//		return FileDownloadFailure;
//	}	

    MessageBox(AfxGetMainWnd()->m_hWnd, LoadString(IDS_INSTALL_UPDATE), LoadString(IDS_PCMAN_CLOSE), MB_ICONINFORMATION|MB_OK);
	if (!::ShellExecute(AfxGetMainWnd()->m_hWnd, "open", updateFileLocation, NULL, NULL,
						   SW_SHOWNORMAL))
	{
		return UpdateNotComplete;
	}
   
	
	ASSERT(AfxGetMainWnd() != NULL);
    SetActiveWindow(AfxGetMainWnd()->m_hWnd);
	

	AfxGetMainWnd()->SendMessage(WM_COMMIT_UPDATE);

	
	return Success;
}

// Ensure the internet is ok to use
//
bool CAutoUpdater::InternetOkay()
{
	if (hInternet == NULL) {
		return false;
	}

	// Important step - ensure we have an internet connection. We don't want to force a dial-up.
	DWORD dwType;
	if (!InternetGetConnectedState(&dwType, 0))
	{
		return false;
	}

	return true;
}

// Get a session pointer to the remote file
//
HINTERNET CAutoUpdater::GetSession(CString &URL)
{
	// Canonicalization of the URL converts unsafe characters into escape character equivalents
	TCHAR canonicalURL[1024];
	DWORD nSize = 1024;
	InternetCanonicalizeUrl(URL, canonicalURL, &nSize, ICU_BROWSER_MODE);		
	
	DWORD options = INTERNET_FLAG_NEED_FILE|INTERNET_FLAG_HYPERLINK|INTERNET_FLAG_RESYNCHRONIZE|INTERNET_FLAG_RELOAD;
	HINTERNET hSession = InternetOpenUrl(hInternet, canonicalURL, NULL, NULL, options, 0);
	URL = canonicalURL;
    
	return hSession;
}

// Download a file into a memory buffer
//
bool CAutoUpdater::DownloadConfig(HINTERNET hSession, BYTE *pBuf, DWORD bufSize)
{	
	DWORD	dwReadSizeOut;
	InternetReadFile(hSession, pBuf, bufSize, &dwReadSizeOut);
	if (dwReadSizeOut <= 0)
	{
		return false;
	}

	
	return true;
}

// Download a file to a specified location
//
UINT DownloadUpdateFile(LPVOID pParam)
{	
    CAutoUpdater_DownloadInfo* pObject = (CAutoUpdater_DownloadInfo*)pParam;

	HINTERNET hSession = pObject->hSession;
	LPCTSTR localFile = pObject->localFile;
	

	HANDLE	hFile;
	BYTE	pBuf[TRANSFER_SIZE];
	DWORD	dwReadSizeOut, dwTotalReadSize = 0;

	hFile = CreateFile(localFile, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {return false;}

	do {
		DWORD dwWriteSize, dwNumWritten;
		BOOL bRead = InternetReadFile(hSession, pBuf, TRANSFER_SIZE, &dwReadSizeOut);
		dwWriteSize = dwReadSizeOut;

		if (bRead && dwReadSizeOut > 0) {
			dwTotalReadSize += dwReadSizeOut;
			WriteFile(hFile, pBuf, dwWriteSize, &dwNumWritten, NULL); 
			// File write error
			if (dwWriteSize != dwNumWritten) {
				CloseHandle(hFile);	
				//download_update_dlg->OnCancel();
				return 0;
			}
		}
		else {
			if (!bRead)
			{
				// Error
				CloseHandle(hFile);	
				//download_update_dlg->OnCancel();
				return 0;
			}			
			break;
		}
	} while(1);
	CloseHandle(hFile);
	SendMessage(pObject->hWnd,WM_DOWNLOAD_UPDATE_COMPLETE,(WPARAM)pObject->dlg,0);
	//AfxGetMainWnd()->SendMessage(WM_DOWNLOAD_UPDATE_COMPLETE);
	return 1;
}

// Get the version of a file
//
CString CAutoUpdater::GetFileVersion(LPCTSTR file)
{
	CString version;
	VS_FIXEDFILEINFO *pVerInfo = NULL;
	DWORD	dwTemp, dwSize, dwHandle = 0;
	BYTE	*pData = NULL;
	UINT	uLen;

	try {
		dwSize = GetFileVersionInfoSize((LPTSTR) file, &dwTemp);
		if (dwSize == 0) throw 1;

		pData = new BYTE[dwSize];
		if (pData == NULL) throw 1;

		if (!GetFileVersionInfo((LPTSTR) file, dwHandle, dwSize, pData))
			throw 1;

		if (!VerQueryValue(pData, _T("\\"), (void **) &pVerInfo, &uLen)) 
			throw 1;

		DWORD verMS = pVerInfo->dwFileVersionMS;
		DWORD verLS = pVerInfo->dwFileVersionLS;

		int ver[4];
		ver[0] = HIWORD(verMS);
		ver[1] = LOWORD(verMS);
		ver[2] = HIWORD(verLS);
		ver[3] = LOWORD(verLS);

		// Are lo-words used?
		if (ver[2] != 0 || ver[3] != 0)
		{
			version.Format(_T("%d.%d.%d"), ver[0], ver[1], ver[2]);
		}
		else if (ver[0] != 0 || ver[1] != 0)
		{
			version.Format(_T("%d.%d"), ver[0], ver[1]);
		}

		delete pData;
		return version;
	}
	catch(...) {
		return _T("");
	}	
}

// Compare two versions 
//
int CAutoUpdater::CompareVersions(CString ver1, CString ver2)
{
	int  wVer1[4], wVer2[4];
	int	 i;
	TCHAR *pVer1 = ver1.GetBuffer(256);
	TCHAR *pVer2 = ver2.GetBuffer(256);

	for (i=0; i<4; i++)
	{
		wVer1[i] = 0;
		wVer2[i] = 0;
	}

	// Get version 1 to DWORDs
	TCHAR *pToken = strtok(pVer1, _T("."));
	if (pToken == NULL)
	{
		return -21;
	}

	i=2;
	while(pToken != NULL&&i>=0)
	{
		if (!IsDigits(pToken)) 
		{			
			return -21;	// Error in structure, too many parameters
		}		
		wVer1[i] = atoi(pToken);
		pToken = strtok(NULL, _T("."));
		i--;
	}
	ver1.ReleaseBuffer();

	// Get version 2 to DWORDs
	pToken = strtok(pVer2, _T("."));
	if (pToken == NULL)
	{
		return -22;
	}

	i=2;
	while(pToken != NULL && i>=0)
	{
		if (!IsDigits(pToken)) 
		{
			return -22;	// Error in structure, too many parameters
		}		
		wVer2[i] = atoi(pToken);
		pToken = strtok(NULL, _T("."));
		i--;
	}
	ver2.ReleaseBuffer();

	// Compare the versions
	for (i=2; i>=0; i--)
	{
		if (wVer1[i] > wVer2[i])
		{
			return 1;		// ver1 > ver 2
		}
		else if (wVer1[i] < wVer2[i])
		{
			return -1;
		}
	}

	return 0;	// ver 1 == ver 2
}

// Ensure a string contains only digit characters
//
bool CAutoUpdater::IsDigits(CString text)
{
	for (int i=0; i<text.GetLength(); i++)
	{
		TCHAR c = text.GetAt(i);
		if (c >= _T('0') && c <= _T('9'))
		{
		}
		else
		{
			return false;
		}
	}

	return true;
}


CString CAutoUpdater::GetExecutable()
{
	HMODULE hModule = ::GetModuleHandle(NULL);
    ASSERT(hModule != 0);
    
    TCHAR path[MAX_PATH];
    VERIFY(::GetModuleFileName(hModule, path, MAX_PATH));
    return path;
}
