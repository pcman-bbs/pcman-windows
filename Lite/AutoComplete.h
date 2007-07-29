#if !defined(AFX_AUTOCOMPLETE_H__3F354EDE_457B_4613_B13B_6A8A46A3F796__INCLUDED_)
#define AFX_AUTOCOMPLETE_H__3F354EDE_457B_4613_B13B_6A8A46A3F796__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AutoComplete.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAutoComplete window

class CAutoComplete : public CListCtrl
{
// Construction
public:
	CAutoComplete();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoComplete)
protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	static int CompareItem(const void* p1, const void* p2);
	void UpdateEditText();
	void Close();
	void Open();
	void DetachEdit();
	void AttachEdit(HWND hedit, CStringList* src, HWND hCombo = NULL);
	CString GetDisplayedItemText(int i){ return m_pSrc->GetAt(m_DisplayedItems[i].strpos); }
	virtual ~CAutoComplete();

	// Generated message map functions
protected:
	void FilterDisplayedItems();
	static LRESULT CALLBACK MouseProc(int code, WPARAM w, LPARAM l);
	WNDPROC m_OldEditProc;
	struct ListItemData
	{
		ListItemData(){}
		ListItemData(POSITION _strpos, UINT _strstrpos)
				: strpos(_strpos), strstrpos(_strstrpos){}
		POSITION strpos;
		int strstrpos;
	};
	CArray<ListItemData, ListItemData> m_DisplayedItems;
	CStringList* m_pSrc;
	HWND m_hEdit;
	HWND m_hCombo;
	static HHOOK m_MouseHook;
	static LRESULT CALLBACK EditProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l);
	static CAutoComplete* m_pOpenedList;
	void DrawItem(LPDRAWITEMSTRUCT lpds);
	//{{AFX_MSG(CAutoComplete)
	afx_msg int OnMouseActivate(CWnd* pDetelnetopWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	//}}AFX_MSG

#if _MFC_VER >= 0x0700
	afx_msg void OnActivateApp(BOOL bActive, DWORD hTask);
#else
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
#endif

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOCOMPLETE_H__3F354EDE_457B_4613_B13B_6A8A46A3F796__INCLUDED_)
