// ListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcman.h"
#include "ListDlg.h"
#include "TelnetConn.h"
#include "InputNameDlg.h"
#include "addressdlg.h"
#include "mainfrm.h"
#include "AutoReplyPage.h"
#include "KeyMapDlg.h"
#include "GeneralPage.h"
#include "ConnectPage.h"

#include <wininet.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _COMBO_
#include "../Combo/WebPageDlg.h"
#endif

#define PATH_SEPARATOR	'\\'

/////////////////////////////////////////////////////////////////////////////
// CListDlg dialog

CListDlg::CListDlg(CTermView* pview /*=NULL*/)
		: CDialog(CListDlg::IDD, NULL),	view(pview), parent(pview->parent)
{
}

CListDlg::~CListDlg()
{

}

BEGIN_MESSAGE_MAP(CListDlg, CDialog)
	//{{AFX_MSG_MAP(CListDlg)
	ON_NOTIFY(TVN_SELCHANGED, IDC_SITES, OnSelchangedSites)
	ON_NOTIFY(NM_DBLCLK, IDC_SITES, OnDblclkSites)
	ON_COMMAND(ID_SITES_ADDFAVORITE, OnSitesAddfavorite)
	ON_BN_CLICKED(ID_SITES_ADDHOME, OnSitesAddhome)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_BN_CLICKED(IDC_FIND, OnFind)
	ON_BN_CLICKED(IDC_CONNECT, OnConnect)
	ON_BN_CLICKED(IDC_COPY, OnCopy)
	ON_BN_CLICKED(IDC_CUT, OnCut)
	ON_COMMAND_RANGE(IDC_PASTE, IDC_PASTE_INTO_FOLDER, OnPaste)
	ON_NOTIFY(TVN_KEYDOWN, IDC_SITES, OnKeyDownSites)
	ON_COMMAND(ID_SITES_NEW, OnAddSite)
	ON_COMMAND(ID_SITES_NEWDIR, OnAddDir)
	ON_COMMAND(ID_SITES_DEL, OnDelSite)
	ON_COMMAND(ID_SITES_EDIT, OnEditSite)
	ON_NOTIFY(NM_RCLICK, IDC_SITES, OnRclickSites)
	ON_COMMAND(ID_SITES_UPDATE, OnUpdateSitesList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListDlg message handlers

void CListDlg::OnCancel()
{
	if (sites.changed)
	{
		SaveSites();
		view->parent->LoadBBSFavorites();
#ifdef _COMBO_
		view->parent->LoadWebFavorites();
#endif
	}
	HTREEITEM item = sites.GetSelectedItem();
//	if(item != sites.bbsfavorite)
//		AppConfig.last_bbslist_item=sites.GetItemPath(item);
	m_InitPath = sites.GetItemPath(item, PATH_SEPARATOR, false);
	AppConfig.sitelist_state.Save(m_hWnd);
	CDialog::OnCancel();
}

void CListDlg::OnOK()
{
	if (::GetFocus() ==::GetDlgItem(m_hWnd, IDC_KEYWORD))
	{
		OnFind();
		return;
	}

	HTREEITEM item = sites.GetSelectedItem();
	if (!item)
		return;

	int image;
	sites.GetItemImage(item, image, image);
	if (image == 4)
	{
		CString str = sites.GetItemText(item);
		CString dir = sites.GetItemPath(sites.GetParentItem(item)) + PATH_SEPARATOR;

		AppConfig.sitelist_state.Save(m_hWnd);
		m_InitPath = sites.GetItemPath(item);

		if (sites.changed)
		{
			SaveSites();
			view->parent->LoadBBSFavorites();
#ifdef _COMBO_
			view->parent->LoadWebFavorites();
#endif
		}
		str.Replace(SEPARATOR, "\t");
		view->ConnectStr('s' + str, dir);
		CDialog::OnOK();
	}
}


static DLGLAYOUT CListDlg_layout[] =
{
	{IDC_SITES, DLA_ALL},
	{IDC_STATIC1, DLA_LEFT | DLA_TOP},
	{IDC_KEYWORD, DLA_LEFT | DLA_RIGHT | DLA_TOP},
	{IDC_FIND, DLA_RIGHT | DLA_TOP},
	{ID_SITES_NEW, DLA_RIGHT | DLA_TOP},
	{ID_SITES_NEWDIR, DLA_RIGHT | DLA_TOP},
	{ID_SITES_EDIT, DLA_RIGHT | DLA_TOP},
	{ID_SITES_DEL, DLA_RIGHT | DLA_TOP},
	{ID_SITES_UP, DLA_RIGHT | DLA_TOP},
	{ID_SITES_DOWN, DLA_RIGHT | DLA_TOP},
	{ID_SITES_ADDFAVORITE, DLA_RIGHT | DLA_TOP},
	{ID_SITES_ADDHOME, DLA_RIGHT | DLA_TOP},
	{ID_SITES_ADV, DLA_RIGHT | DLA_TOP},
	{IDC_CUT, DLA_RIGHT | DLA_TOP},
	{IDC_COPY, DLA_RIGHT | DLA_TOP},
	{IDC_PASTE, DLA_RIGHT | DLA_TOP},
	{IDC_CONNECT, DLA_RIGHT | DLA_BOTTOM},
	{IDCANCEL, DLA_RIGHT | DLA_BOTTOM},
	{IDC_STATIC2, DLA_TOP | DLA_RIGHT},
	{ID_SITES_UPDATE, DLA_RIGHT | DLA_TOP}
};

extern CFont fnt;

BOOL CListDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(parent->icon, TRUE);
	SetIcon(parent->icon, FALSE);

	dlgl.Load(m_hWnd, CListDlg_layout, sizeof(CListDlg_layout) / sizeof(DLGLAYOUT));

	sites.changed = 0;
	copyitem = NULL;
	sites.SubclassDlgItem(IDC_SITES, this);

	LoadSites();
	sites.SetItemHeight(20);
	sites.SetImageList(&parent->img_icons, TVSIL_NORMAL);
	sites.SetFocus();

	HTREEITEM item = sites.PathToItem(m_InitPath, TVI_ROOT, PATH_SEPARATOR);
	sites.SelectItem(item ? item : sites.bbsfavorite);
	sites.Expand(item, TVE_EXPAND);
	AppConfig.sitelist_state.Restore(m_hWnd);

	return FALSE;
}


void CListDlg::OnSelchangedSites(NMHDR* pNMHDR, LRESULT* pResult)
{
	UpdateCmdUI();
}

void CListDlg::AddSite(CArchive* ar, HTREEITEM parent, char* str)
{
	HTREEITEM item;
	while (ar->ReadString(str, 2048) && *str)
	{
		CString _str;
		for (const char* pstr = str; *pstr; ++pstr)
		{
			if (*pstr == '\t')
				_str += SEPARATOR;
			else
				_str += *pstr;
		}

#if defined(_COMBO_)
		if (*str == 'w')
			item = sites.InsertItem(LPCTSTR(_str) + 1, 8, 8, parent);
		else
#endif
			if (*str == 's')
				item = sites.InsertItem(LPCTSTR(_str) + 1, 4, 4, parent);
			else if (*str == 'd')
			{
				item = sites.InsertItem(LPCTSTR(_str) + 1, 3, 3, parent);
				AddSite(ar, item, str);
			}
	}
}

void CListDlg::LoadSites()
{
	CFile data;
#if defined(_COMBO_)
	if (!AppConfig.use_ie_fav)
	{
		sites.wwwfavorite = sites.InsertItem(LoadString(IDS_WWW_FAVORITE_NAME), 3, 3, TVI_ROOT);
		LoadSite(sites.wwwfavorite,::ConfigPath + WWW_FAVORITE_FILENAME);
	}
#endif
	sites.bbsfavorite = sites.InsertItem(LoadString(IDS_BBS_FAVORITE_NAME) , 3, 3, TVI_ROOT);
	LoadSite(sites.bbsfavorite,::ConfigPath + BBS_FAVORITE_FILENAME);
	sites.Expand(sites.bbsfavorite, TVE_EXPAND);
	sites.bbslist = sites.InsertItem(LoadString(IDS_BBS_LIST_NAME), 3, 3, TVI_ROOT);
	LoadSite(sites.bbslist,::AppPath + BBS_LIST_FILENAME);
	sites.Expand(sites.bbslist, TVE_EXPAND);
	CString homepage_name;
	homepage_name.LoadString(IDS_HOMEPAGE_NAME);
	sites.home = sites.InsertItem(homepage_name, 3, 3, TVI_ROOT);
	LoadSite(sites.home,::ConfigPath + HOMEPAGE_FILENAME);
	sites.Expand(sites.home, TVE_EXPAND);
}

void CListDlg::OnDblclkSites(NMHDR* pNMHDR, LRESULT* pResult)
{
	CPoint pt;
	GetCursorPos(&pt);
	sites.ScreenToClient(&pt);
	HTREEITEM item = sites.HitTest(pt);
	if (item && ::IsWindowEnabled(::GetDlgItem(m_hWnd, IDC_CONNECT)))
	{
		sites.SelectItem(item);
		OnOK();
	}
	*pResult = 0;
}


void CListDlg::SaveSites()
{
	CFile file;
#ifdef _COMBO_
	if (file.Open(::ConfigPath + WWW_FAVORITE_FILENAME, CFile::modeCreate | CFile::modeWrite))
	{
		SaveSite(file, sites.wwwfavorite);
		file.Close();
	}
#endif

	if (file.Open(::ConfigPath + BBS_FAVORITE_FILENAME, CFile::modeCreate | CFile::modeWrite))
	{
		SaveSite(file, sites.bbsfavorite);
		file.Close();
	}

	if (file.Open(::AppPath + BBS_LIST_FILENAME, CFile::modeCreate | CFile::modeWrite))
	{
		SaveSite(file, sites.bbslist);
		file.Close();
	}

	if (file.Open(::ConfigPath + HOMEPAGE_FILENAME, CFile::modeCreate | CFile::modeWrite))
	{
		SaveSite(file, sites.home);
		file.Close();
	}
}

void CListDlg::OnAddSite()
{
	HTREEITEM item = sites.GetSelectedItem();
	if (!item)
		return;
	int image;
	sites.GetItemImage(item, image, image);
	HTREEITEM newitem;

#ifdef _COMBO_
	HTREEITEM top = sites.GetTopParent(item);
	UINT type = 0;
	if (top == sites.home)
	{
		CMenu popup;	popup.LoadMenu(IDR_POPUP2);
		CRect rc;	GetDlgItem(ID_SITES_NEW)->GetWindowRect(rc);
		type = popup.GetSubMenu(1)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RETURNCMD, rc.left, rc.bottom, this);
		// WWW : type = 8
		// BBS : type = 4
		if (0 == type)
			return;
	}
	else
		type = (top == sites.wwwfavorite ? 8 : 4);

	if (image == 3)
	{
		CString text = SEPARATOR;
		if (type == 8)
			text += "http://";
		newitem = sites.InsertItem(text, type, type, item);
	}
	else
	{
		CString text = SEPARATOR;
		if (type == 8)
			text += "http://";

		newitem = sites.InsertItem(text, type, type, sites.GetParentItem(item), item);
	}
#else
	if (image == 3)
		newitem = sites.InsertItem(SEPARATOR, 4, 4, item);
	else
		newitem = sites.InsertItem(SEPARATOR, 4, 4, sites.GetParentItem(item), item);
#endif

	sites.SelectItem(newitem);
	OnEditSite();
	if (sites.GetItemText(newitem) == SEPARATOR)
	{
		sites.DeleteItem(newitem, FALSE);
		sites.SelectItem(item);
	}
}

void CListDlg::OnDelSite()
{
	HTREEITEM item = sites.GetSelectedItem();
	if (!item)
		return;
	if (MessageBox(LoadString(IDS_DEL_CONFIRM), LoadString(IDS_CONFIRM),
				   MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
	{
		sites.DeleteItem(item, TRUE);
		sites.changed = TRUE;
	}
}

void CListDlg::OnEditSite()
{
	HTREEITEM item = sites.GetSelectedItem();
	if (!item)
		return;
	int image;
	sites.GetItemImage(item, image, image);
	HTREEITEM p = sites.GetParentItem(item);
	if (image == 3)	//dir
	{
		CInputNameDlg dlg(this);
		dlg.name = sites.GetItemText(item);
		dlg.name.Replace(SEPARATOR, "\t");
		if (dlg.DoModal() == IDOK)
		{
			CString tmp = dlg.name;
			tmp.Replace("\t", SEPARATOR);
			sites.SetItemText(item, dlg.name);
			sites.changed = TRUE;
		}
	}
#ifdef _COMBO_
	else if (image == 8)
	{
		CWebPageDlg dlg(this);
		CString str = sites.GetItemText(item);
		int pos = str.Find(SEPARATOR);
		dlg.m_Name = str.Left(pos);
		dlg.m_URL = str.Mid(pos + SEPARATOR_LEN);
		if (dlg.DoModal() == IDOK)
		{
			sites.changed = TRUE;
			sites.SetItemText(item, dlg.m_Name + SEPARATOR + dlg.m_URL);
		}
	}
#endif
	else	//site
	{
		CString str = sites.GetItemText(item);
		int pos = str.Find(SEPARATOR);

		CConnectPage page0;
		CString name = page0.name = str.Left(pos);
		page0.InitWithAddress(str.Mid(pos + SEPARATOR_LEN));

		CPropertySheet dlg(IDS_SITE_SETTINGS);
		CSitePage page1;
		CSiteSettings tmpset;
		page1.psettings = &tmpset;

		//自動登入
		CAutoReplyPage page2;
		page2.triggers = &tmpset.triggers;

		dlg.AddPage(&page0);
		dlg.AddPage(&page1);
		dlg.AddPage(&page2);

		CString path1 = sites.GetItemPath(item);
		if (!tmpset.Load(path1))
			return;
		if (dlg.DoModal() == IDOK)
		{
			sites.changed = TRUE;

			CString text = page0.name + SEPARATOR + page0.GetFormattedAddress();

			sites.SetItemText(item, text);
			path1 = CSiteSettings::GetFilePath(path1);
			CString path2 = sites.GetItemPath(item);
			CString fpath2 = CSiteSettings::GetFilePath(path2);
			MoveFile(path1, fpath2);
			tmpset.Save(path2);
		}
	}
}

void CListDlg::OnAddDir()
{
	HTREEITEM item = sites.GetSelectedItem();
	if (!item)
		return;
	int image;
	sites.GetItemImage(item, image, image);
	HTREEITEM p = sites.GetParentItem(item);
	CInputNameDlg dlg(this);
	if (dlg.DoModal() == IDOK)
	{
		if (image == 3)
			item = sites.InsertItem(dlg.name, 3, 3, item);
		else
			item = sites.InsertItem(dlg.name, 3, 3, p, item);
		sites.changed = TRUE;
		/*		if( sites.GetItemText(sites.GetChildItem(p)).IsEmpty())
					CDragTreeCtrl(sites).DeleteItem(sites.GetChildItem(p));
				sites.InsertItem(NULL,4,4,item);
		*/
	}
}

void CListDlg::OnSitesAddfavorite()
{
	HTREEITEM item = sites.GetSelectedItem();
	item = sites.CopyTo(item, sites.bbsfavorite, TVI_LAST);
	sites.SelectItem(item);
}

void CListDlg::SaveSite(CFile& file, HTREEITEM parent)
{
	CString str;
	const char *crlf = "\r\n";
	for (HTREEITEM item = sites.GetChildItem(parent); item; item = sites.GetNextSiblingItem(item))
	{
		int image;		sites.GetItemImage(item, image, image);
		if (image == 3)	//dir
			str = "d";
		else if (image == 4)	//site
			str = "s";
		else if (image == 8)
			str = "w";
		CString tmp = sites.GetItemText(item);
		tmp.Replace(SEPARATOR, "\t");
		str += tmp;
		str += crlf;
		file.Write(LPCTSTR(str), str.GetLength());

		if (image == 3)	//dir
			SaveSite(file, item);
	}
	file.Write(crlf, 2);
}


void CListDlg::OnSitesAddhome()
{
	HTREEITEM item = sites.GetSelectedItem();
	item = sites.CopyTo(item, sites.home, TVI_LAST);
	sites.SelectItem(item);
}

void CListDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	dlgl.OnSize(nType, cx, cy);
}

void CListDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
	dlgl.OnGetMinMaxInfo(lpMMI);
}

void CListDlg::OnFind()
{
	CString str;
	GetDlgItem(IDC_KEYWORD)->GetWindowText(str);
	HTREEITEM sel = sites.GetSelectedItem();
	sel = FindSite(sel, str);
	if (sel)
	{
		sites.SelectItem(sel);
		sites.EnsureVisible(sel);
	}
	else
		MessageBox(LoadString(IDS_NOT_FOUND) + " " + str, LoadString(IDS_SEARCH_RESULT));
}

HTREEITEM CListDlg::FindSite(HTREEITEM item, LPCTSTR str)
{
	HTREEITEM nextitem;
	HTREEITEM parent;
	CString istr;
	istr = sites.GetItemText(item);
	if (istr.Find(str) >= 0)
		if (sites.ItemHasChildren(item))
			goto _Find_Next_Child;
		else
			goto _Find_Next_Site;

_Begin_of_FindSite:
	while (item)
	{
		istr = sites.GetItemText(item);
		if (istr.Find(str) >= 0)
			return item;

		if (sites.ItemHasChildren(item))
		{
_Find_Next_Child:
			item = sites.GetChildItem(item);
			goto _Begin_of_FindSite;
		}
_Find_Next_Site:
		nextitem = sites.GetNextSiblingItem(item);
		if (nextitem)
		{
			item = nextitem;
			goto _Begin_of_FindSite;
		}
		else
		{
			parent = sites.GetParentItem(item);
			if (!parent)
				return NULL;
			item = parent;
			goto _Find_Next_Site;
		}
	}
	return item;
}

void CListDlg::OnConnect()
{
	OnOK();
}

void CListDlg::OnCopy()
{
	if (copyitem)
		sites.SetItemState(copyitem, 0, TVIS_CUT);
	copyitem = sites.GetSelectedItem();
	UpdateCmdUI();
}

void CListDlg::OnCut()
{
	if (copyitem)
		sites.SetItemState(copyitem, 0, TVIS_CUT);
	copyitem = sites.GetSelectedItem();
	sites.SetItemState(copyitem, TVIS_CUT, TVIS_CUT);
	UpdateCmdUI();
}

void CListDlg::OnPaste(UINT id)
{
	HTREEITEM target = sites.GetSelectedItem();
	HTREEITEM after = TVI_LAST;
	bool bcut = !!sites.GetItemState(copyitem, TVIS_CUT) && target != copyitem;

	if (!sites.IsDefaultItem(target) && id != IDC_PASTE_INTO_FOLDER)
		{	after = target;	target = sites.GetParentItem(target);	}

	HTREEITEM newitem = sites.CopyTo(copyitem, target, after);
	if (bcut)	//是剪下
	{
		sites.DeleteItem(copyitem, FALSE);
		sites.SetItemState(newitem, 0, TVIS_CUT);
	}
	copyitem = NULL;
	sites.SelectItem(newitem);
}


void CListDlg::LoadSite(HTREEITEM parent, LPCTSTR fpath)
{
	char* str = new char[2048];
	CFile data;
	if (data.Open(fpath, CFile::modeRead))
	{
		CArchive ar(&data, CArchive::load);
		AddSite(&ar, parent, str);
		data.Close();
	}
	delete []str;
}


void CListDlg::UpdateCmdUI()
{
	HTREEITEM item = sites.GetSelectedItem();
	HTREEITEM pitem = sites.GetParentItem(item);
	BOOL bdef = sites.IsDefaultItem(item);
	BOOL bdir = sites.IsItemDir(item);
	BOOL is_in_home = (pitem == sites.home);
	BOOL is_in_bbsfavorite = (pitem == sites.bbsfavorite);
#if defined(_COMBO_)
	BOOL is_in_wwwfavorite = (pitem == sites.bbsfavorite);
#endif
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_CONNECT), !bdef && !bdir);
	::EnableWindow(::GetDlgItem(m_hWnd, ID_SITES_NEWDIR), !is_in_home && (item != sites.home));
	::EnableWindow(::GetDlgItem(m_hWnd, ID_SITES_ADDFAVORITE), !bdef && !is_in_bbsfavorite);
	::EnableWindow(::GetDlgItem(m_hWnd, ID_SITES_ADDHOME), !bdef && !is_in_home && !bdir);
	::EnableWindow(::GetDlgItem(m_hWnd, ID_SITES_EDIT), !bdef);
	::EnableWindow(::GetDlgItem(m_hWnd, ID_SITES_DEL), !bdef);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_CUT), !bdef);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_COPY), !bdef);
	::EnableWindow(::GetDlgItem(m_hWnd, IDC_PASTE), sites.CanPaste(copyitem));
}

void CListDlg::OnKeyDownSites(NMHDR* pNMHDR, LRESULT* pResult)
{
	TV_KEYDOWN* pTVKeyDown = (TV_KEYDOWN*)pNMHDR;
	if (pTVKeyDown->wVKey == VK_DELETE)
		if (::IsWindowEnabled(::GetDlgItem(m_hWnd, ID_SITES_DEL)))
			OnDelSite();
	*pResult = 0;
}

void CListDlg::OnRclickSites(NMHDR* pNMHDR, LRESULT* pResult)
{
	CPoint point;	GetCursorPos(&point);
	sites.ScreenToClient(&point);
	UINT flag;	HTREEITEM item = sites.HitTest(point, &flag);
	sites.SelectItem(item);

	if (item)
	{
		CMenu mnu;
		mnu.LoadMenu(IDR_POPUP);

		HTREEITEM pitem = sites.GetParentItem(item);
		BOOL bdef = sites.IsDefaultItem(item);
		BOOL bdir = sites.IsItemDir(item);
		BOOL is_in_home = (pitem == sites.home);
		BOOL is_in_bbsfavorite = (pitem == sites.bbsfavorite);
#if defined(_COMBO_)
		BOOL is_in_wwwfavorite = (pitem == sites.bbsfavorite);
#endif
		::EnableMenuItem(mnu.m_hMenu, IDC_CONNECT, MF_BYCOMMAND | (!bdef && !bdir ? MF_ENABLED : MF_GRAYED));
		::EnableMenuItem(mnu.m_hMenu, ID_SITES_NEWDIR, MF_BYCOMMAND | (!is_in_home && (item != sites.home) ? MF_ENABLED : MF_GRAYED));
		::EnableMenuItem(mnu.m_hMenu, ID_SITES_ADDFAVORITE, MF_BYCOMMAND | (!bdef && !is_in_bbsfavorite ? MF_ENABLED : MF_GRAYED));
		::EnableMenuItem(mnu.m_hMenu, ID_SITES_ADDHOME, MF_BYCOMMAND | (!bdef && !is_in_home && !bdir ? MF_ENABLED : MF_GRAYED));
		::EnableMenuItem(mnu.m_hMenu, ID_SITES_EDIT, MF_BYCOMMAND | (!bdef ? MF_ENABLED : MF_GRAYED));
		::EnableMenuItem(mnu.m_hMenu, ID_SITES_DEL, MF_BYCOMMAND | (!bdef ? MF_ENABLED : MF_GRAYED));
		::EnableMenuItem(mnu.m_hMenu, IDC_CUT, MF_BYCOMMAND | (!bdef ? MF_ENABLED : MF_GRAYED));
		::EnableMenuItem(mnu.m_hMenu, IDC_COPY, MF_BYCOMMAND | (!bdef ? MF_ENABLED : MF_GRAYED));
		BOOL canpaste = sites.CanPaste(copyitem);
		::EnableMenuItem(mnu.m_hMenu, IDC_PASTE, MF_BYCOMMAND | (canpaste ? MF_ENABLED : MF_GRAYED));
		::EnableMenuItem(mnu.m_hMenu, IDC_PASTE_INTO_FOLDER, MF_BYCOMMAND | ((canpaste) && bdir ? MF_ENABLED : MF_GRAYED));

		GetCursorPos(&point);
		mnu.GetSubMenu(1)->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	}
	*pResult = 0;
}

/*
class CDownloadingDlg : CDialog
{
public:
	CDownloadingDlg(HINTERNET& ifile)
		: CDialog(IDD_DOWNLOADING), file(ifile)
	{
	}

	BOOL OnInitDialog()
	{
		CDialog::OnInitDialog();
		return TRUE;
	}

	void OnCancel()
	{
		InternetCloseHandle( file );
	}
protected:
	HINTERNET& file;

	DECLARE_MESSAGE_MAP()
};
BEGIN_MESSAGE_MAP(CDownloadingDlg, CDialog)
END_MESSAGE_MAP()
*/

bool DownLoadURL(LPCTSTR url, LPCTSTR out_path, bool show_ui = false)
{
	HINTERNET inet = InternetOpen("PCMan", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (!inet)
		return false;

	DWORD retry = 2;
	InternetSetOption(inet, INTERNET_OPTION_CONNECT_RETRIES, &retry, sizeof(retry));

	bool ret = false;

	HINTERNET ifile = InternetOpenUrl(inet, url, NULL, 0, INTERNET_FLAG_NO_CACHE_WRITE, 0);
	if (ifile)
	{
		CFile ofile;
		if (ofile.Open(out_path, CFile::modeCreate | CFile::modeWrite))
		{
			char buf[4096];
			DWORD len = 0;
			while (InternetReadFile(ifile, buf, sizeof(buf), &len) && len > 0)
			{
				ofile.Write(buf, len);
			}
			ret = true;
		}
		InternetCloseHandle(ifile);
	}
	InternetCloseHandle(inet);
	return ret;
}

void CListDlg::OnUpdateSitesList()
{
	// URLDownloadToFile is not support before Windows xp/2000.
	CString tmp;
	GetTempPath(_MAX_PATH, tmp.GetBuffer(_MAX_PATH));
	tmp.ReleaseBuffer();
	tmp += BBS_LIST_FILENAME;
	tmp += ".tmp";
	if (DownLoadURL(SITESLIST_UPDATE_URL, tmp, true))
	{
		CString path = ::AppPath + BBS_LIST_FILENAME;
		DeleteFile(path);
		MoveFile(tmp, path);
		sites.DeleteItem(sites.bbsfavorite, false);
		sites.DeleteItem(sites.bbslist, false);
		sites.DeleteItem(sites.home, false);
		LoadSites();
		AfxMessageBox(LoadString(IDS_SITELIST_UPDATE_SUCCESS), MB_ICONINFORMATION);
	}
	else
	{
		AfxMessageBox(LoadString(IDS_SITELIST_UPDATE_FAIL), MB_ICONINFORMATION);
	}
}

