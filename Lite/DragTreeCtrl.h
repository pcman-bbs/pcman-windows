#if !defined(AFX_DRAGTREECTRL_H__FCC01CB7_C649_4E6E_A9F9_9194D6E04F57__INCLUDED_)
#define AFX_DRAGTREECTRL_H__FCC01CB7_C649_4E6E_A9F9_9194D6E04F57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DragTreeCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDragTreeCtrl window

class CDragTreeCtrl : public CTreeCtrl
{
// Construction
public:
	CDragTreeCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDragTreeCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	CString GetItemPath(HTREEITEM item, TCHAR separator = '\\');
	HTREEITEM PathToItem(LPCTSTR path, HTREEITEM root, TCHAR separato = '\\');
	virtual BOOL CanDrag(HTREEITEM item) = 0;
	int GetIndex(HTREEITEM item, HTREEITEM from);
	void SetAutoExpandTarget(int time);
	void EndDrag();
	virtual void MoveItem(HTREEITEM item, HTREEITEM target, bool up, bool bcopy);
	virtual ~CDragTreeCtrl();

	// Generated message map functions
protected:
	HTREEITEM dragitem;
	CImageList* dragimg;
	int hovertime;
	HTREEITEM hoveritem;
	int auto_expand_time;
	HCURSOR cursor;
	//{{AFX_MSG(CDragTreeCtrl)
	afx_msg void OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnExpanded(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRAGTREECTRL_H__FCC01CB7_C649_4E6E_A9F9_9194D6E04F57__INCLUDED_)
