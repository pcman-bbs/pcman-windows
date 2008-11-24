#if !defined(AFX_CUSTOMTABCTRL_H__495042F4_8E72_4FA4_9758_E60A5C5083A4__INCLUDED_)
#define AFX_CUSTOMTABCTRL_H__495042F4_8E72_4FA4_9758_E60A5C5083A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CustomTabCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCustomTabCtrl window

class CCustomTabCtrl : public CWnd
{
public:
	struct ItemData
	{
		int image;
		CString text;	// text displayed on the item
		CString tooltip;	// tooltip
		LPVOID data;	// user data
		int desired_size;	// desired size of the item
		int size;	// real size of the item
		bool hilight;	// hilight state of the item

		ItemData(CString txt, int img, LPVOID dat);
	};

	enum CloseBtnMode
	{
		CTC_CLOSE_BTN_ACTIVE,	// only show close button on active tab
		CTC_CLOSE_BTN_ALL,	// show close button on all tabs
		CTC_CLOSE_BTN_NONE	// don't show close buttons
	};

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
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	CString GetItemTooltip(int idx);
	void SetItemTooltip(int idx, LPCTSTR text);
	BOOL RedrawItem(int idx);
	void SetMinTabSize(int min);
	void RecalcLayout();
	LPVOID SetItemData(int idx, LPVOID data);
	CImageList* SetImageList(CImageList * pImageList);
	int SetCurSel(int idx);
	LPVOID GetItemData(int idx) const;
	CImageList* GetImageList() const { return image_list; }
	int GetCurSel() const { return cur_item; }
	UINT GetItemCount() const { return items.GetSize(); }
	BOOL GetItemRect(int idx, LPRECT rc) const;
	BOOL HighlightItem(int idx, BOOL high);
	int HitTest(int x, int y, int* place = NULL);
	BOOL DeleteAllItems();
	BOOL DeleteItem(int idx);
	int InsertItem(int idx, LPCTSTR text, int image = -1, void* data = NULL);
	int AppendItem(LPCTSTR text, int image = -1, void* data = NULL) { return InsertItem(-1, text, image, data); }
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	BOOL IsLegalIndex(int idx) const { return (idx >= 0 && idx < GetItemCount()); }
	ItemData* GetItem(int idx) const { return IsLegalIndex(idx) ? (ItemData*)items[idx] : NULL; }
	void SetFixedTabSize(int size) { min_size = max_size = size; }
	virtual ~CCustomTabCtrl();

	// Generated message map functions
protected:
	void UpdateTooltips();
	void EnsureTooltips();
	void EnsureSpinButton();	// create spin button for scrolling if needed
	CToolTipCtrl* tooltips;	// tooltips
	CSpinButtonCtrl* spin;	// spin button used to scroll the tab bar
	CPtrArray items;	// data of all items
	int cur_item;	// current active item
	int hot_item;	// current hovered item
	int scroll_pos;	// scroll position
	int min_size;	// minimal size of tab
	int max_size;	// maximal size of tab
	int tab_height;	// height of all tabs
	bool homogeneous;	// whether all tabs use the same size or not
	bool can_reorder;	// whether the order of tabs can be changed by the user or not
	bool show_number;	// show numbers on the tabs
	CloseBtnMode close_btn;	// whether to show close button or not
	CImageList* image_list;	// image list providing icons for items

	LRESULT ParentNotify(int code, int id = -1);

	//{{AFX_MSG(CCustomTabCtrl)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnScrolled(NMHDR* nmhdr, LRESULT* result);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUSTOMTABCTRL_H__495042F4_8E72_4FA4_9758_E60A5C5083A4__INCLUDED_)
