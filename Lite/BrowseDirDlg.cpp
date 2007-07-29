#include "stdafx.h"

#include "BrowseDirDlg.h"

int CBrowseDirDlg::DoModal()
{
	LPITEMIDLIST pidl = ::SHBrowseForFolder(&bi);
	if (pidl)
	{
		::SHGetPathFromIDList(pidl, bi.pszDisplayName);
		IMalloc* pmlc;
		SHGetMalloc(&pmlc);
		pmlc->Free(pidl);
		pmlc->Release();
		return IDOK;
	}
	return IDCANCEL;
}

CBrowseDirDlg::CBrowseDirDlg(CWnd* pParent , LPCTSTR lpstrTitle = NULL)
{
	bi.hwndOwner = pParent->GetSafeHwnd();
	bi.pidlRoot = NULL;
	bi.pszDisplayName = new char[MAX_PATH];
	bi.pszDisplayName[0] = 0;
	bi.lpszTitle = lpstrTitle;
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	bi.lpfn = NULL;
	bi.lParam = NULL;
}

CBrowseDirDlg::~CBrowseDirDlg()
{
	if (bi.pszDisplayName)
		delete []bi.pszDisplayName;
}
