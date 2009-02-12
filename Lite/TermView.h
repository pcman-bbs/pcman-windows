// TermView.h : interface of the CTermView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TVIEW_H__EF878A2E_6FEC_11D5_A2FD_D45914C10000__INCLUDED_)
#define AFX_TVIEW_H__EF878A2E_6FEC_11D5_A2FD_D45914C10000__INCLUDED_

#include "..\CHICONV\ChiConv.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include"TelnetConn.h"
#include"addressdlg.h"
#include "AnsiBar.h"	// Added by ClassView
#include "Ucs2Conv.h"
#include <afxtempl.h>

/*
#ifdef	_COMBO_
	#include"../Combo/menumapitem.h"
#endif
*/

bool find_link(char* type, char* str, int& start, int& end);
//	bool find_email(char* str,int& start,int& end);

class CMainFrame;


#ifdef	_COMBO_
class CWebConn;
struct MENUMAPITEM;
#else	//在不支援 Web 的版本中直接把TCon定義成MySocket省去麻煩
//	typedef		CTelnetConn	CConn;
#endif

class CTermView : public CWnd
{
	friend class CTelnetConn;

// Attributes
protected:
	static LPSTR ctviewcls;

public:
#ifdef	_COMBO_
	CStringList favorite_ads;
	BOOL autosort_favorite;
	CConn* con;
#endif

	CTelnetConn* telnet;
	int lineh;
	int chw;

	BOOL blight;

	int left_margin;
	int top_margin;

	CMainFrame* parent;

	HBITMAP bk;
	HGDIOBJ holdobj;
	HDC memdc;
	LOGFONT font_info;
	CRect view_rect;
	HCURSOR hand_cursor;
	BOOL caret_vis;
	DWORD paste_block;
	HBITMAP draw_bmp;
	CDC drawdc;
	BOOL auto_switch;
	CFindReplaceDialog* pfinddlg;
	CString find_text;

	bool key_processed;

	struct DNSLookupData
	{
		HANDLE hTask;
		CTelnetConn* new_telnet;
		SOCKADDR_IN sockaddr;
		LPCTSTR address;
	};

// Operations
public:
	CTermView();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTermView)
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	inline UINT SetDCColors(CDC* dc, BYTE color, BOOL invirt = 0);
	void OnDisConnect();
	void SetScrollBar();
	virtual ~CTermView();

	LRESULT OnImeChar(WPARAM wparam, LPARAM lparam);
	LRESULT OnImeComposition(WPARAM wparam, LPARAM lparam);
	LRESULT _OnImeCompositionW(WPARAM wparam, LPARAM lparam);
	LRESULT _OnImeCompositionA(WPARAM wparam, LPARAM lparam);
	LRESULT OnInputLangChange(WPARAM wparam, LPARAM lparam);
	inline UINT GetCodePage(){ return cp_id; }
	void SetCodePage(UINT nCodePageId){ cp_id = nCodePageId; }
	inline BOOL IsWinNT(){ return os_ver_nt; }
	DWORD ime_prop;

	inline void ShowCaret()
	{
		if (!caret_vis)
		{
			::ShowCaret(m_hWnd);
			caret_vis = TRUE;
		}
	}

	inline void HideCaret()
	{
		if (caret_vis)
		{
			::HideCaret(m_hWnd);
			caret_vis = FALSE;
		}
	}

	inline void CreateCaret()
	{
		caret_vis = 0;
		::CreateCaret(m_hWnd, NULL, chw, 2);
	}

	char* HyperLinkHitTest(int x, int y, int& len);
	inline void PtToLineCol(POINT pt, int& x, int& y, bool adjust_x = true);

// Generated message map functions
public:
	void CopySelText();
	BOOL ExtTextOut(CDC& dc, int x, int y, UINT nOptions, LPCRECT lpRect, LPCTSTR lpszString, UINT nCount);
	static CPtrArray all_telnet_conns;
	void ConnectStr(CString name, CString dir);
	void AdjustFont(int cx, int cy);
	CString GetSelText();
	void FindStart();

#ifdef	_COMBO_
	CWebConn* ConnectWeb(CString address, BOOL act);
	void MoveWindow(int x, int y, int nWidth, int nHeight, BOOL bRepaint = TRUE);
	BOOL SetWindowPos(const CWnd* pWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags);

#endif

	void OnHistory(UINT id);
	inline void FillBkRect(CDC& dc, CRect& rc, BYTE atb, BOOL bkinvirt = 0);
	inline void DrawLink(CDC &dc, LPSTR line, LPBYTE atbline, int y);
	inline void DrawLine(CDC &dc, LPCSTR line, BYTE* pline_selstart, BYTE* pline_selend, int y);
	inline void DrawLineBlink(CDC &dc, LPCSTR line, int y);
	void DrawBlink();
	void DrawScreen(CDC& dc);
	void SendAnsiString(CString data);
	BOOL OpenAnsFile(LPCTSTR filepath);
	UINT mouse_sel_timer;
	static DWORD DNSLookupThread(LPVOID param);
	inline CConn* NewConn(CString address, CString name, unsigned short port, LPCTSTR cfg_path);
	void OnInitialUpdate();
	inline void FillBk(CDC& dc);
	void UpdateBkgnd();
	int CanUseMouseCTL();

	BOOL doflash;
	CString GetSelAnsi();

	void OnAnsiCopy();
	void ReConnect(CTelnetConn* retelnet);
	LRESULT CTermView::OnDNSLookupEnd(WPARAM found, LPARAM lparam);
	BOOL Connect(CString address, CString name, unsigned short port, LPCTSTR cfg = NULL);
	//{{AFX_MSG(CTermView)
	afx_msg LRESULT OnFind(WPARAM w, LPARAM l);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnEditPastefile();
	afx_msg void OnReconnect();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetBkgnd();
	afx_msg void OnRightNoDBCS();
	afx_msg void OnLeftNoDBCS();
	afx_msg void OnBackspaceNoDBCS();
	afx_msg void OnSelAllBuf();
	afx_msg void OnDestroyClipboard();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSmoothDraw();
	afx_msg void OnCurConSettings();
	afx_msg void OnUpdateBBSList();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

	afx_msg void OnAnsiCls();
	afx_msg void OnAnsiIns();
	afx_msg void OnAnsiSave();
	afx_msg void OnAnsiSaveAs();
	afx_msg void OnAnsiOpen();
	afx_msg void OnAnsiEditor();
	//}}AFX_MSG
	afx_msg void OnEditOpenURL(UINT id);
	afx_msg void OnSearchPlugin(UINT id);
	afx_msg void OnTranslation();

	DECLARE_MESSAGE_MAP()
protected:
	CChiConv chi_conv;
	UINT cp_id;
	BOOL os_ver_nt;
	void ConnectSocket(CTelnetConn* new_telnet);
	LRESULT OnSocket(WPARAM wparam, LPARAM lparam);

	inline void DrawLineBlinkOld(CDC &dc, LPSTR line, int y);
	inline void DrawLineOld(CDC &dc, LPSTR line, BYTE* pline_selstart, BYTE* pline_selend, int y);
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TVIEW_H__EF878A2E_6FEC_11D5_A2FD_D45914C10000__INCLUDED_)
