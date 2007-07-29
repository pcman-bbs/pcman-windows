// FavMenu.cpp: implementation of the CFavMenu class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcman.h"
#include "FavMenu.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFavMenu::CFavMenu()
{
	NONCLIENTMETRICS nc;	nc.cbSize = sizeof(nc);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &nc, 0);
	menu_font = CreateFontIndirect(&nc.lfMenuFont);
}

CFavMenu::~CFavMenu()
{
	DeleteObject(menu_font);
}

inline BOOL CFavMenu::AppendMenu(HMENU hMenu, UINT uFlags, UINT uIDNewItem, LPCTSTR lpNewItem, int& height)
{
#ifdef	_COMBO_
	if (AppConfig.autowrap_favorite && (height + item_height) > max_height)
#else
	if ((height + item_height) > max_height)
#endif
	{
		uFlags |= (MF_OWNERDRAW | MF_MENUBARBREAK);
		height = 0;
	}
	else
	{
		uFlags &= ~MF_MENUBARBREAK;
		uFlags |= MF_OWNERDRAW;
	}
	height += item_height;
	return ::AppendMenu(hMenu, uFlags, uIDNewItem, lpNewItem);
}

static CString add_to_fav;
static CString open_all;
static CString org_fav;
static CString add_to_home;
static CString sep = "-";

HMENU CFavMenu::DoLoadFavorites(CArchive& ar, CStringArray& data, UINT& id, int& menu_height)
{
	max_height = GetSystemMetrics(SM_CYSCREEN) - 64;
	item_height = GetSystemMetrics(SM_CYMENU);
	bool is_toplevel = (data.GetSize() == 0);

	HMENU menu = CreatePopupMenu();
	CString str;
	for (; ar.ReadString(str); ++id)
	{
		if (str.IsEmpty())	// end of dir
		{
			data.Add(sep);	// add separator
			::AppendMenu(menu, MF_SEPARATOR, id, NULL);
			++id;

			if (! is_toplevel)
			{
				// Add to Favorites
				data.Add(add_to_fav);
				AppendMenu(menu, MF_OWNERDRAW, id, LPCTSTR(id), menu_height);
				++id;

				// Open All
				data.Add(open_all);
				AppendMenu(menu, MF_OWNERDRAW, id, LPCTSTR(id), menu_height);
				++id;

				// Organize Favorites
				data.Add(org_fav);
				AppendMenu(menu, MF_OWNERDRAW, id, LPCTSTR(id), menu_height);
				++id;

				// end of sub menu
				data.Add(_T(""));		// afxEmptyString is no longer in mfc7 and above
			}
			break;
		}
		data.Add(str);
		if (str[0] == 'd') // dir
		{
			UINT submenu_id = id;	// reserve id for the menu item
			++id;
			int submenu_height = 0;
			HMENU sub = DoLoadFavorites(ar, data, id, submenu_height);	// create sub menu
			AppendMenu(menu, MF_OWNERDRAW | MF_POPUP,
					   UINT(sub), LPCTSTR(submenu_id), menu_height);
		}
		else
			AppendMenu(menu, MF_OWNERDRAW, id, LPCTSTR(id), menu_height);
	}
	favmenus.Add(menu);
	return menu;
}

void CFavMenu::LoadFavorites(HMENU &fav_menu, char type)
{
	max_height = GetSystemMetrics(SM_CYSCREEN) - 64;
	item_height = GetSystemMetrics(SM_CYMENU);

	MENUITEMINFO mi;	mi.cbSize = sizeof(mi);	mi.fMask = MIIM_DATA;	mi.dwItemData = 0;
	int last = GetMenuItemCount(fav_menu) - 1;
	CString _add_to_fav("a"), _open_all("o"), _org_fav("e");
	add_to_home = "h";
	if (GetMenuItemInfo(fav_menu, last, TRUE, &mi) && mi.dwItemData)
	{
		last = bbs_fav.GetSize() - 1;
		add_to_home = bbs_fav[last--];
		_org_fav = bbs_fav[last--];
		_open_all = bbs_fav[last--];
		_add_to_fav = bbs_fav[last];
		DestroyMenu(fav_menu);
	}
	else
	{
		CMenu tmp;	tmp.Attach(fav_menu);	CString str;
		tmp.GetMenuString(last--, str, MF_BYPOSITION);	add_to_home += str;
		tmp.GetMenuString(last--, str, MF_BYPOSITION);	_org_fav += str;
		tmp.GetMenuString(last--, str, MF_BYPOSITION);	_open_all += str;
		tmp.GetMenuString(last, str, MF_BYPOSITION);		_add_to_fav += str;
	}

//  Favorite 內容經過修改後，menu 必須重建，而原來的 favorite menu 會被刪除，
//  所以先前建立的 menu 都已經無效，必須從 CFavMenu 的 array 中移除掉。
//	favmenus.RemoveAll();
//	錯誤，這樣會把其他favorite和history的CFavMenu <-> HMENU對應刪除
//	改成下列方式，因為上面已經 DestroyMenu( fav_menu )，所以可以很投機取巧的
//	利用 IsMenu 來檢查，移除 favmenu 中已經無效的 Handles
	int i;	//	此 i 在稍後的程式還有使用
	for (i = favmenus.GetSize() - 1;i >= 0;i--)	//搜尋看是否為CFavMenu
		if (!IsMenu(reinterpret_cast<HMENU>(favmenus[i])))
			favmenus.RemoveAt(i);
	//	刪除特定位置第i個 item 後，其後的array都會往前 shift 1，
	//	但是不用管他，因為是由後往前檢查，所以i後面的都已經檢查過

	if ((i = _add_to_fav.Find('\t')) >= 0)
		add_to_fav = _add_to_fav.Left(i);
	else
		add_to_fav = _add_to_fav;

	if ((i = _open_all.Find('\t')) >= 0)
		open_all = _open_all.Left(i);
	else
		open_all = _open_all;

	if ((i = _org_fav.Find('\t')) >= 0)
		org_fav = _org_fav.Left(i);
	else
		org_fav = _org_fav;

	CFile file;
#if defined(_COMBO_)
	if (file.Open(ConfigPath + (type == 's' ?
								BBS_FAVORITE_FILENAME : WWW_FAVORITE_FILENAME), CFile::modeRead))
#else
	if (file.Open(ConfigPath + BBS_FAVORITE_FILENAME, CFile::modeRead))
#endif
	{
		UINT id;
#if defined(_COMBO_)
		CStringArray* data;
		if (type == 's')
		{
			data = &bbs_fav;
			id = ID_FIRST_BBS_FAVORITE;
		}
		else
		{
			data = &web_fav;
			id = ID_FIRST_WEB_FAVORITE;
		}
#else
		CStringArray* data = &bbs_fav;
		id = ID_FIRST_BBS_FAVORITE;
#endif
		data->RemoveAll();

		CArchive ar(&file, CArchive::load);
		int menu_height = 0;
		fav_menu = DoLoadFavorites(ar, *data, id, menu_height);
		data->Add(_add_to_fav);
		AppendMenu(fav_menu, MF_OWNERDRAW, ID_ADDTOFAVORITE, LPCTSTR(id), menu_height);
		++id;

		data->Add(_open_all);
		AppendMenu(fav_menu, MF_OWNERDRAW, id/*ID_FAVORITE_OPENALL*/, LPCTSTR(id), menu_height);
		++id;

		data->Add(_org_fav);
		AppendMenu(fav_menu, MF_OWNERDRAW, id/*ID_FAVORITE_EDIT*/, LPCTSTR(id), menu_height);
		++id;

		data->Add(add_to_home);
		AppendMenu(fav_menu, MF_OWNERDRAW, ID_ADDTOHOME, LPCTSTR(id), menu_height);
	}
}


inline char GetMenuKey(LPCTSTR str)
{
	const char* pch = str;
	while (*pch && *pch != '\t' && *pch != '&')
		pch++;
	return toupper(*pch == '&' ? pch[1] : str[1]);
}

LRESULT CFavMenu::OnMenuChar(UINT nChar, UINT nFlags, CMenu *pMenu)
{
	int delta;
	CStringArray& data = GetData(pMenu->m_hMenu, delta);
	int i;	int c = pMenu->GetMenuItemCount();
	for (i = 0; i < c;i++)
		if (MF_HILITE & pMenu->GetMenuState(i, MF_BYPOSITION))
		{
			i++;
			break;
		}
	int cur = -1, next = -1;
	MENUITEMINFO mi;	memset(&mi, 0, sizeof(mi));
	mi.cbSize = sizeof(mi);	mi.fMask = MIIM_DATA | MIIM_TYPE;
	for (int i2 = 0; i2 < c; i2++, i++)
	{
		int idx = i >= c ? (i - c) : i;

		if (GetMenuItemInfo(pMenu->m_hMenu, idx, TRUE, &mi) &&
			!(mi.fType & MFT_SEPARATOR) &&
			GetMenuKey(data[mi.dwItemData-delta]) == toupper(nChar))
		{
			if (cur >= 0)
			{
				next = idx;
				break;
			}
			else
				cur = idx;
		}
	}
	if (cur >= 0)
		return MAKELONG(cur, (next >= 0) ? MNC_SELECT : MNC_EXECUTE);
	return 0;
}


void CFavMenu::DrawItem(LPDRAWITEMSTRUCT pds)
{
	CDC dc;	dc.Attach(pds->hDC);
	CRect& rc = (CRect)pds->rcItem;
	int delta;
	CStringArray& data = GetData(pds->itemData, delta);
	CString& str = data.ElementAt(pds->itemData - delta);
	COLORREF bkcolor;	COLORREF textcolor;
	if (pds->itemState & ODS_SELECTED)
	{
		textcolor = GetSysColor(COLOR_HIGHLIGHTTEXT);
		bkcolor = GetSysColor(COLOR_HIGHLIGHT);
	}
	else
	{
		bkcolor = GetSysColor(COLOR_MENU);
		textcolor = GetSysColor(COLOR_MENUTEXT);
	}

	int	topmargin = (rc.Height() - 16) / 2;
	dc.FillSolidRect(rc, bkcolor);
#if defined(_COMBO_)
	int iimg = (str[0] == 'd' ? 3 : (str[0] == 's' ? 4 : (str[0] == 'w' ? 8 : -1)));
#else
	int iimg = (str[0] == 'd' ? 3 : (str[0] == 's' ? 4 : -1));
#endif
	CMainFrame* mainfrm = (CMainFrame*)AfxGetMainWnd();
	ImageList_Draw(mainfrm->img_icons, iimg, dc.m_hDC,
				   rc.left + 2, rc.top + topmargin,
				   ILD_NORMAL | ILD_TRANSPARENT);
	rc.left += 24;

	dc.SetBkMode(TRANSPARENT);
	HGDIOBJ fold = SelectObject(dc.m_hDC, menu_font);

	if (pds->itemState & ODS_GRAYED)
		textcolor = GetSysColor(COLOR_3DSHADOW);

	dc.SetTextColor(textcolor);
	rc.right -= 8;
#if defined(_COMBO_)
	dc.DrawText(LPCTSTR(str) + 1, (iimg != 4 && iimg != 8) ? str.GetLength() - 1 : str.Find('\t') - 1,
				rc, DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_EXPANDTABS);
#else
	dc.DrawText(LPCTSTR(str) + 1, (iimg != 4) ? str.GetLength() - 1 : str.Find('\t') - 1,
				rc, DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_EXPANDTABS);
#endif
	SelectObject(dc.m_hDC, fold);
	dc.Detach();
}

void CFavMenu::MeasureItem(LPMEASUREITEMSTRUCT pms)
{
	HDC dc = GetDC(AfxGetMainWnd()->m_hWnd);
	HGDIOBJ fold = SelectObject(dc, menu_font);
	CRect rc;	rc.left = 0;	rc.top = 0;
	int delta;
	CStringArray& data = GetData(pms->itemData, delta);
	CString& _str = data.ElementAt(pms->itemData - delta);

	CString str;
#if defined(_COMBO_)
	int i = (_str[0] == 's' || _str[0] == 'w') ? _str.Find('\t') : -1;
#else
	int i = _str[0] == 's' ? _str.Find('\t') : -1;
#endif

	str = (i == -1) ? _str.Mid(1) : _str.Mid(1, i);
	int len = str.GetLength();
	if (len > 40 && ID_FIRST_HISTORY > pms->itemID && str.GetLength())	//不是連線紀錄且太長則截斷
		len = 40;

	DrawText(dc, LPCTSTR(str), len, rc, DT_SINGLELINE | DT_CALCRECT | DT_EXPANDTABS);
	pms->itemHeight = GetSystemMetrics(SM_CYMENU);	//( rc.bottom>16 ? (rc.bottom+4):20);
	pms->itemWidth = rc.right + 16 + 8 + 4;
	SelectObject(dc, fold);
	ReleaseDC(AfxGetMainWnd()->m_hWnd, dc);
}


void CFavMenu::SaveFavorites(BOOL bBBS)
{
	CFile file;
#ifdef _COMBO_
	CStringArray& fav = bBBS ? AppConfig.favorites.bbs_fav : AppConfig.favorites.web_fav;
	if (file.Open(ConfigPath + (bBBS ? BBS_FAVORITE_FILENAME : WWW_FAVORITE_FILENAME),
				  CFile::modeWrite | CFile::modeCreate))
#else
	CStringArray& fav = AppConfig.favorites.bbs_fav;
	if (file.Open(ConfigPath + BBS_FAVORITE_FILENAME, CFile::modeWrite | CFile::modeCreate))
#endif
	{
		for (int i = 0;i < fav.GetSize() - 4; i++)
		{
			CString& str = fav.ElementAt(i);
			if (*LPCTSTR(str) == '-')
			{
				i += 3;
				continue;
			}
			file.Write(LPCTSTR(str), str.GetLength());
			file.Write("\x0d\x0a", 2);
		}
		file.Write("\x0d\x0a", 2);
		file.Close();
	}
}

inline CStringArray& CFavMenu::GetData(UINT id, int& delta)
{
	if (id <= ID_LAST_BBS_FAVORITE)	// bbs_fav
	{
		delta = ID_FIRST_BBS_FAVORITE;
		return bbs_fav;
	}
#ifdef	_COMBO_
	if (id <= ID_LAST_WEB_FAVORITE)	// web_fav
	{
		delta = ID_FIRST_WEB_FAVORITE;
		return web_fav;
	}
#endif
	delta = ID_FIRST_HISTORY;	// history menu
	return history;
}

inline CStringArray& CFavMenu::GetData(HMENU menu, int& delta)
{
	MENUITEMINFO mi;	mi.cbSize = sizeof(mi);	mi.fMask = MIIM_DATA;
	GetMenuItemInfo(menu, 0, TRUE, &mi);
	return GetData(mi.dwItemData, delta);
}

int FavItemCompare(const void* item1, const void* item2)
{
	return strcmpi(*(CString*)item1, *(CString*)item2);
}

#if defined( _COMBO_ )

void CFavMenu::AddToIEFav(HMENU fmenu, CString favpath, UINT &id)
{
	static short menu_level = 0;
	menu_level++;
	favmenus.Add(fmenu);

	CStringArray favs;

	DWORD count = 0;
	LPSTR pdata = NULL;
	BYTE* data = NULL;

	favs.SetSize(count, 64);

	CFileFind finder;
	BOOL found = finder.FindFile(favpath + "\\*.*");
	while (found)
	{
		found = finder.FindNextFile();
		if (!finder.IsDots() && !finder.IsHidden())
		{
			CString title = finder.GetFileTitle();
			BOOL bdir = finder.IsDirectory();

			CString item;
			if (finder.IsDirectory())
			{
				//取得有"."的資料夾名稱 by NomedEmag 07/06/08
				title = finder.GetFileName();
				//
				item = 'd' + title;
			}
			else
			{
				item = 'w' + title;
				item += '\t';
				CString ads;
				DWORD retl = GetPrivateProfileString("InternetShortcut", "URL", "",
													 ads.GetBuffer(2048), 2048 , finder.GetFilePath());
				ads.ReleaseBuffer();
				item += ads;
			}

			favs.Add(item);
		}
	}
	finder.Close();

	if (data)
		delete []data;

	//利用特殊技巧 qsort，這是依賴 MFC 的內部實做方式，將來可能有變
	if (AppConfig.autosort_favorite)
		qsort(favs.GetData(), favs.GetSize(), sizeof(CString), FavItemCompare);

//	long menuh=GetSystemMetrics(SM_CYMENU)*4+64;

//	UINT id = ID_FIRST_WEB_FAVORITE;
	int menu_height = 0;
	for (int i = 0; i < favs.GetSize(); ++i)
	{
		if (!favs[i].IsEmpty())
		{
			web_fav.Add(favs[i]);
			if ('d' == favs[i][0])
			{
				HMENU submenu = CreatePopupMenu();
				AppendMenu(fmenu, MF_OWNERDRAW | MF_POPUP, (UINT)submenu, LPCTSTR(id), menu_height);
				++id;
				AddToIEFav(submenu, (favpath + '\\' + (LPCTSTR(favs[i]) + 1)), id);
			}
			else
			{
				AppendMenu(fmenu, MF_OWNERDRAW, id, LPCTSTR(id), menu_height);
				++id;
			}
		}
	}
	web_fav.Add(sep);
	++id;
	::AppendMenu(fmenu, MF_SEPARATOR, 0, 0);

	web_fav.Add(add_to_fav);
	AppendMenu(fmenu, MF_OWNERDRAW, id/*ID_ADDTOFAVORITE*/, LPCTSTR(id), menu_height);	id++;

	web_fav.Add(open_all);
	AppendMenu(fmenu, MF_OWNERDRAW, id/*ID_FAVORITE_OPENALL*/, LPCTSTR(id), menu_height);	id++;

	web_fav.Add(org_fav);
	AppendMenu(fmenu, MF_OWNERDRAW, id/*ID_FAVORITE_EDIT*/, LPCTSTR(id), menu_height);	id++;

	menu_level--;

	if (!menu_level)
	{
		web_fav.Add(add_to_home);
		AppendMenu(fmenu, MF_OWNERDRAW, ID_ADDTOHOME, LPCTSTR(id), menu_height);
		++id;
	}
	else
	{
		//  AfxGetEmptyString();
		web_fav.Add(_T(""));	// afxEmptyString is no-longer in mfc7 and above
		++id;
	}
}

void CFavMenu::LoadIEFav(HMENU &fav_menu)
{
//	錯誤，這樣會把其他favorite和history的CFavMenu <-> HMENU對應刪除
//	favmenus.RemoveAll();
//	改成下列方式，因為上面已經 DestroyMenu( fav_menu )，所以可以很投機取巧的
//	利用 IsMenu 來檢查，移除 favmenu 中已經無效的 Handles
	int i;	//	此 i 在稍後的程式還有使用
	for (i = favmenus.GetSize() - 1;i >= 0;i--)	//搜尋看是否為CFavMenu
		if (!IsMenu(reinterpret_cast<HMENU>(favmenus[i])))
			favmenus.RemoveAt(i);
	//	刪除特定位置第i個 item 後，其後的array都會往前 shift 1，
	//	但是不用管他，因為是由後往前檢查，所以i後面的都已經檢查過

	DestroyMenu(fav_menu);
	web_fav.RemoveAll();

	CString favdir = GetIEFavDir();

	max_height = GetSystemMetrics(SM_CYSCREEN) - 64;
	item_height = GetSystemMetrics(SM_CYMENU);

	UINT id = ID_FIRST_WEB_FAVORITE;
	fav_menu = CreatePopupMenu();
	AddToIEFav(fav_menu, favdir, id);
}


CString CFavMenu::GetIEFavDir()
{
	ITEMIDLIST *pidl;
	SHGetSpecialFolderLocation(NULL, CSIDL_FAVORITES, &pidl);
	CString favdir;
	SHGetPathFromIDList(pidl, favdir.GetBuffer(_MAX_PATH));
	favdir.ReleaseBuffer();
	IMalloc* pmlc = NULL;
	SHGetMalloc(&pmlc);
	pmlc->Free(pidl);
	pmlc->Release();
	return favdir;
}
#endif

