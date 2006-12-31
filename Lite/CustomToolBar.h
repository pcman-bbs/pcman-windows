#if !defined(AFX_CUSTOMTOOLBAR_H__14ABF8CF_C7BD_46CE_8835_506BF5BFFAD7__INCLUDED_)
#define AFX_CUSTOMTOOLBAR_H__14ABF8CF_C7BD_46CE_8835_506BF5BFFAD7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CustomToolBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCustomToolBar window

class CCustomToolBar;

class CCustomToolBarInfo	//目前支援最多255個按鈕
{
public:
	int count;
	int allcount;
	BYTE* index;
	TBBUTTON* pbtns;

	CCustomToolBarInfo(TBBUTTON* _pbtns, int _allcount)
	{	pbtns=_pbtns;	allcount=_allcount;	}

	~CCustomToolBarInfo()
	{	delete index;	}

	inline void SaveToFile(CFile& f)
	{
		f.Write(&count,sizeof(count));
		f.Write(index,count*sizeof(*index));
	}

	inline void LoadDefault()
	{
		index=new BYTE[allcount];
		count=allcount;
		for(int i=0;i<count;i++)
			index[i]=i;
	}

	inline void LoadFromFile(CFile& f)
	{
		f.Read(&count,sizeof(count));
		index=new BYTE[count];
		f.Read(index,count*sizeof(*index));
	}

	int LookupIndex(int id)
	{
		for(int i=0;i<allcount;i++)
			if(pbtns[i].idCommand==id)
				return i;
		return -1;
	}

	int LookupImage(int id)
	{
		for(int i=0;i<allcount;i++)
			if(pbtns[i].idCommand==id)
				return pbtns[i].iBitmap;
		return -1;
	}

	inline void GetState(CToolBar& ctb)
	{
		count=ctb.GetToolBarCtrl().GetButtonCount();
		delete []index;
		index= new BYTE[count];
		for(int i=0;i<count;i++)
			index[i]=LookupIndex(ctb.GetItemID(i));
	}
};

class CCustomToolBar : public CToolBar
{
// Construction
public:
	CCustomToolBar();

// Attributes
public:
	CCustomToolBarInfo* inf;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomToolBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	int SaveState();
	void DeleteAllButtons();
	void OnToolBarBeginAdjust(NMHDR *pnmhdr, LRESULT *r);
	void Default();
	BOOL LoadToolBar(CCustomToolBarInfo* _inf);
	virtual ~CCustomToolBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCustomToolBar)
	afx_msg void OnCustomize();
	afx_msg void OnToolBarGetButtonInfo(NMHDR *pnmhdr, LRESULT *r);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnToolBarEndAdjust(NMHDR *pnmhdr, LRESULT *r);
	afx_msg void OnToolBarQueryDelete(NMHDR *pnmhdr, LRESULT *r);
	afx_msg void OnToolBarQueryInsert(NMHDR *pnmhdr, LRESULT *r);
	afx_msg void OnToolBarReset(NMHDR *pnmhdr, LRESULT *r);
	afx_msg void OnToolBarChange(NMHDR *pnmhdr, LRESULT *r);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUSTOMTOOLBAR_H__14ABF8CF_C7BD_46CE_8835_506BF5BFFAD7__INCLUDED_)
