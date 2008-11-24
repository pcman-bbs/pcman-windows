// FavMenu.h: interface for the CFavMenu class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FAVMENU_H__E773E801_8FF9_4F3B_9DE6_63E8DC347CA4__INCLUDED_)
#define AFX_FAVMENU_H__E773E801_8FF9_4F3B_9DE6_63E8DC347CA4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AppConfig.h"
#include <afxcoll.h>

class CFavMenu
{
	friend class CTermView;
	friend class CMainFrame;
public:
	HMENU DoLoadFavorites(CArchive& ar, CStringArray& data, UINT& id, int& menu_height);
#ifdef	_COMBO_
	static CString GetIEFavDir();
	void LoadIEFav(HMENU &fav_menu);
#endif
	void SaveFavorites(BOOL bBBS);
	inline int Lookup(HMENU hm)
	{
		for (int i = favmenus.GetSize() - 1;i >= 0;i--)	//搜尋看是否為CFavMenu
			if (favmenus[i] == hm)	//如果有找到，開始OnMenuChar
				return i;
		return -1;
	}

	CFavMenu();
	virtual ~CFavMenu();
//	void AddToFavMenu( HMENU fav_menu, CStringArray& data, CArchive& ar, UINT& id );
	void LoadFavorites(HMENU &fav_menu, char type);
	void DrawItem(LPDRAWITEMSTRUCT pds);
	void MeasureItem(LPMEASUREITEMSTRUCT pms);
	LRESULT OnMenuChar(UINT nChar, UINT nFlags, CMenu *pMenu);

	CStringArray bbs_fav;
	CStringArray history;
#ifdef	_COMBO_
	CStringArray web_fav;
#endif

protected:
#ifdef	_COMBO_
	void AddToIEFav(HMENU fmenu, CString favpath, UINT &id);
#endif
	BOOL AppendMenu(HMENU hMenu, UINT uFlags, UINT uIDNewItem, LPCTSTR lpNewItem, int& height);

	CPtrArray favmenus;
	inline CStringArray& GetData(UINT id, int& delta);
	inline CStringArray& GetData(HMENU menu, int& delta);
	HFONT menu_font;

	int item_height;
	int max_height;
};


#endif // !defined(AFX_FAVMENU_H__E773E801_8FF9_4F3B_9DE6_63E8DC347CA4__INCLUDED_)
