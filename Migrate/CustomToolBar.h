#if !defined(AFX_CUSTOMTOOLBAR_H__14ABF8CF_C7BD_46CE_8835_506BF5BFFAD7__INCLUDED_)
#define AFX_CUSTOMTOOLBAR_H__14ABF8CF_C7BD_46CE_8835_506BF5BFFAD7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CustomToolBar.h : header file
//

#include <afxwin.h>

/////////////////////////////////////////////////////////////////////////////
// CCustomToolBar window

struct CCustomToolBarInfo	//目前支援最多255個按鈕
{
	int count;
	int allcount;
	BYTE* index;
	TBBUTTON* pbtns;

	~CCustomToolBarInfo()
	{	delete index;	}

	inline void SaveToFile(CFile& f)
	{
		f.Write(&count,sizeof(count));
		f.Write(index,count*sizeof(*index));
	}

	inline void LoadFromFile(CFile& f)
	{
		f.Read(&count,sizeof(count));
		index=new BYTE[count];
		f.Read(index,count*sizeof(*index));
	}
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUSTOMTOOLBAR_H__14ABF8CF_C7BD_46CE_8835_506BF5BFFAD7__INCLUDED_)
