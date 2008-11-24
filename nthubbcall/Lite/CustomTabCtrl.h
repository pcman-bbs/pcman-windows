#if !defined(AFX_MYTABCTRL_H__28CE1D49_4B37_11D7_BEA1_0040F427D59A__INCLUDED_)
#define AFX_MYTABCTRL_H__28CE1D49_4B37_11D7_BEA1_0040F427D59A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CustomTabCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCustomTabCtrl window

class CCustomTabCtrl : public CTabCtrl
{
// Construction
public:
	CCustomTabCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomTabCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetItemText(int i, CString Text);

	inline CConn* GetCon(int i)
	{
		TCITEM item;
		item.mask = TCIF_PARAM;
		item.lParam = NULL;
		GetItem(i, &item);
		return (CConn*)item.lParam;
	}

	void BeginDrag(CPoint point);
	CImageList* CreateDragImage(int i);
	CImageList* dragimg;
	void UpdateNumberFrom(int idx);
	BOOL DeleteItem(int nItem);
	CPoint oldp;
	UINT md;
	int sel;
	CMainFrame* parent;
	virtual ~CCustomTabCtrl();

	BOOL InsertItem(int nItem, TCITEM* pTabCtrlItem);
	BOOL InsertItem(UINT nMask, int nItem, LPCTSTR lpszItem,
					int nImage, LPARAM lParam);

	// Generated message map functions
protected:
	void DrawItem(LPDRAWITEMSTRUCT LPDS);
	//{{AFX_MSG(CCustomTabCtrl)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYTABCTRL_H__28CE1D49_4B37_11D7_BEA1_0040F427D59A__INCLUDED_)
