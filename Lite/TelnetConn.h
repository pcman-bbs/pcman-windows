#if !defined(AFX_MYSOCKET_H__1837C9F6_3C2A_11D5_A2FD_946114C10000__INCLUDED_)
#define AFX_MYSOCKET_H__1837C9F6_3C2A_11D5_A2FD_946114C10000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TelnetConn.h : header file
//

#include <memory>
#include <atomic>

#include <winsock2.h>
// #include <WS2TCPIP.h> // for future IPV6 support

#include "Conn.h"
#include "SiteSettings.h"
#include "KeyMap.h"	// Added by ClassView
#include "TriggerList.h"	// Added by ClassView
#include "ConnIO.h"
#include "TcpSocket.h"

/////////////////////////////////////////////////////////////////////////////

class CTermView;
class CDownloadArticleDlg;

struct CConnEvent
{
	enum EventType
	{
		EVENT_CONNECT = 0,
		EVENT_CLOSE = 1,
		EVENT_DATA = 2,
		EVENT_CONNECT_FAILED = 3,
	};
	uint64_t connection_id;
	EventType event_type;
	std::string data;

	CConnEvent(uint64_t connection_id, EventType event_type)
		: connection_id(connection_id)
		, event_type(event_type)
	{}

	static std::unique_ptr<CConnEvent> MakeUnique(
		uint64_t connection_id, EventType event_type) {
		return std::unique_ptr<CConnEvent>(new CConnEvent(
			connection_id, event_type));
	}
};

int find_sub_str(char* str, char* sub);

class CTelnetConn : public CConn
{
	friend class CDownloadArticleDlg;
// Attributes
public:
	enum
	{
		MAX_LINES_PER_PAGE = 64,
		MAX_COLS_PER_PAGE = 160,
		MIN_LINE_PER_PAGE = 24,
		MIN_COLS_PER_PAGE = 40,
		MAX_LINE_COUNT = 32767,
		MIN_LINE_COUNT = 24
	};

	CString cfg_path;	//���x�i���]�w�ɬ۹���|
	CKeyMap* key_map;	//��L��M

//	Socket handle
	SOCKET telnet;
	uint64_t connection_id;
	std::shared_ptr<CConnIO> conn_io;
	std::shared_ptr<CTcpSocket> tcp_socket;

//	Screen Data
	LPSTR *screen;	//screen buffer
	long first_line;	//first line index
	long last_line;		//last line index
	CPoint cursor_pos;	//cursor position
	int scr_top;	//screen scroll region
	int scr_bottom;	//screen scroll region

//	Mouse Selection
	CPoint sel_start;
	CPoint sel_end;
	bool sel_block;

//	Site Settings
	CSiteSettings site_settings;	// BBS Site Settings

//	Runtime Internal Data
	static BYTE buffer[4096];	//����������ƪ�buffer�A�Ҧ�socket�@��
	BYTE *buf;
	BYTE *last_byte;	//End of buffer
	ULONG time;		//�s�u�ɶ�
	ULONG idle_time;	//���m�ɶ�
	int scroll_pos;	//�������b��m

	char ansi_param[64];	//�ΨӳB�zANSI�m��r�ꪺ�Ȧsbuffer
	char* pansi_param;		//�ΨӾާ@ansi_param��ƪ�����
	CPoint old_cursor_pos;	//saved position
	BYTE saved_attr;		//saved attributes
	BYTE cur_attr;			//�ثe���ù��r���ݩ�
	BYTE attr_flags;	//�ù��r���ݩʥΪ��Ȧsflag�A�x�s�O�_�Ϭ۵���T

BYTE ansi_mode: 1;
BYTE insert_mode: 1;

BYTE is_getting_article : 1;
BYTE get_article_with_ansi : 1;
BYTE get_article_in_editor : 1;

//	Delay Send Data
	CList<CTelnetConnDelayedSend, CTelnetConnDelayedSend> delay_send;

	static CString downloaded_article;
    static int current_download_line;

	static std::atomic<uint64_t> connection_counter;

public:
	CTelnetConn();
	virtual ~CTelnetConn();

// Operations
public:
	void CopyArticleComplete(bool cancel = false);
	void CopyArticle(bool with_color, bool in_editor);
	CString GetLineWithAnsi(long line);
	int IsEndOfArticleReached();
	void SendNaws();
	void SendMacroString(CString str);
	inline int GetLineBufLen();
	inline int GetLineBufLen(int _cols_per_page);
	inline LPBYTE GetLineAttr(const char* line);
	inline LPBYTE GetLineAttr(const int line);
	inline LPBYTE GetLineAttr(const int line, const int len);
	inline void InitNewLine(char* line);
	void Home();
	void End();
	void Delete(int num = 1);
	inline void SetHyperLink(long i, BOOL haslink = TRUE);
	inline BOOL GetHyperLink(const char* line);
	inline BOOL GetHyperLink(long i);
	inline void SetUpdateWholeLine(long line);
	inline BOOL GetUpdateLine(long line);
	inline void RemoveUpdateLine(long line);
	void SetUpdateLine(long line, BYTE curx);
	void Back(int num = 1);
	void EditorLineBack(LPSTR newline, LPBYTE newlineatb, int l);
	void EditorCarriageRetiurn();
	void EditorLineFeed(LPSTR newline, LPBYTE newlineatb, int l);
	BOOL IsEmptyLine(LPSTR line, int len);
	inline char* AllocNewLine();
	char* ResizeLine(int line, int newl);
	void ReSizeBuffer(long new_line_count, int new_cols_per_page, int new_lines_per_page);
	inline void ProcessAnsiEscapeSequence();
	inline void SetCurrentAttributes(USHORT clr);
	inline void GoUp(int p);
	inline void GoDown(int p);
	void GoRight(int p);
	inline void GoLeft(int p);
	inline void LineFeed(int param);
	inline void ScrollUp();
	inline void ScrollDown();
	inline void SetScrollRange(int pt, int progress_bar);
	inline void GotoXY(int line, int col);
	inline void InsertLines(int num);
	void ClearScreen(int param);
	inline void ClearCurrentLine(int param);
	inline void SaveCursorPos();
	inline void RestoreCursorPos();
	inline void InsertChar(int n);
	inline void DeleteLines(int n);
	inline void DeleteChar(int n);
	inline void NextPages(int n);
	inline void PrevPages(int n);
	inline void BackTab(int n);
	inline void SetMode(int p);
	inline void ResetMode(int p);
	inline void DeviceStatusReport(int p);

	size_t GetConnectionID() const { return connection_id; }
	bool HasSocket() const { return tcp_socket.operator bool(); }
	SOCKET GetSocket() const { return tcp_socket->GetSocket(); }
	bool IsSecureConn() const { return conn_io && conn_io->IsSecure(); }
	void OnSocket(WPARAM wparam, LPARAM lparam);
	int Close();
	int Shutdown();
	void HandleConnEvent(std::unique_ptr<CConnEvent> event);
	void Connect(sockaddr* addr, int len);
	void ConnectWebsocket();
	BOOL Create();
	int SendString(LPCTSTR str);
	void LocalEcho(void* str, int len);
	void ProcessData(int len);
	void CreateBuffer();
	void CheckStrTrigger();
	int Send(const void* lpBuf, int nBufLen);
	void CheckHyperLinks();
	static void SetFgColor(BYTE &attr, BYTE fg);
	static void SetBgColor(BYTE& attr, BYTE bk);
	void LineFeed();
	void UpdateLine(int line);
	void OnClose();
	void UpdateCursorPos();
	void OnText();
	void OnIAC();

	void ClearAllFlags()
	{
		CConn::ClearAllFlags();
		is_telnet = true;

		scr_top = 0;
		scr_bottom = site_settings.lines_per_page - 1;
	}

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTelnetConn)
public:
	virtual void OnReceive(int len);
	virtual void OnConnect(bool success);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CTelnetConn)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
};

inline void CTelnetConn::SetBgColor(BYTE &attr, BYTE bk)
{
	attr &= 143;		//1000,1111b=143d;
	attr |= (bk << 4);
}

inline void CTelnetConn::SetFgColor(BYTE &attr, BYTE fg)
{
	attr &= 248;		//1111,1000b=248d
	attr |= fg;
}

inline BOOL CTelnetConn::GetHyperLink(long i)
{	return GetLineAttr(i)[site_settings.cols_per_page+1];	}

inline BOOL CTelnetConn::GetHyperLink(const char* line)
{	return GetLineAttr(line)[site_settings.cols_per_page+1];	}

inline void CTelnetConn::SetHyperLink(long i, BOOL haslink /*=TRUE*/)
{	GetLineAttr(i)[site_settings.cols_per_page+1] = (BYTE)haslink;	}


inline void CTelnetConn::Home()
{
	cursor_pos.x = 0;
	UpdateCursorPos();
}


inline LPBYTE CTelnetConn::GetLineAttr(const char *line)
{
	return LPBYTE(line + site_settings.cols_per_page + 5);
}

inline LPBYTE CTelnetConn::GetLineAttr(const int line)
{
	return LPBYTE(screen[line] + site_settings.cols_per_page + 5);
}

inline LPBYTE CTelnetConn::GetLineAttr(const int line, const int len)
{
	return LPBYTE(screen[line] + len + 5);
}

inline void CTelnetConn::InitNewLine(char *line)
{
	LPBYTE attr = GetLineAttr(line);
	memset(line, ' ', site_settings.cols_per_page);
	*(DWORD*)(line + site_settings.cols_per_page) = 0;
	memset(attr, 7, site_settings.cols_per_page);
	*(DWORD*)(attr + site_settings.cols_per_page) = 0;
	*(DWORD*)(attr - 4) = 0;
}

inline char* CTelnetConn::AllocNewLine()
{
	char* str = new char[GetLineBufLen()];
	InitNewLine(str);
	return str;
}


inline int CTelnetConn::GetLineBufLen()
{	return site_settings.cols_per_page*2 + 10;	}

inline int CTelnetConn::GetLineBufLen(int _cols_per_page)
{	return _cols_per_page*2 + 10;	}

inline BYTE GetAttrBkColor(BYTE attr)
{
	BYTE bk = (attr & 112) >> 4;		//0111,0000b=112d;
	return bk;
}

inline BYTE GetAttrFgColor(BYTE attr)
{
	BYTE fg = attr & 7;		//0000,0111b=7;
	if (attr&8)
		fg += 8;		//0000,1000b=8d;
	return fg;
}

inline bool IsAttrBlink(BYTE attr)
{
	return !!(attr&128);	//1000,0000b=128d
}

CString AttrToStr(BYTE prevatb, BYTE attr);

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYSOCKET_H__1837C9F6_3C2A_11D5_A2FD_946114C10000__INCLUDED_)
