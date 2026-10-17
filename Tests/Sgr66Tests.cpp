#include <afx.h>
#include <commctrl.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include "../Lite/Sgr66.h"
#include "../Lite/SynchronizedOutput.h"
#include "../Lite/KeepAlive.h"
#include "../Lite/OscHyperlinks.h"
#include "../Resource/resource.h"

static unsigned int checks = 0;
static void Check(bool ok, const char* why)
{
    ++checks;
    if (!ok) { std::fprintf(stderr, "FAIL: %s\n", why); std::exit(1); }
}
static BYTE GetAttrBkColor(BYTE value) { return (value >> 4) & 7; }
CString AttrToStr(BYTE, BYTE);
class CTelnetConn;
struct Parent
{
    HWND m_hWnd = nullptr;
    struct Tab { void SetItem(int, TCITEM*) {} } tab;
    int ConnToIndex(CTelnetConn*) { return 0; }
    void SwitchToConn(CTelnetConn*) {}
    void SetForegroundWindow() {}
    void BringWindowToTop() {}
};
struct CTermView
{
    UINT codePage = 950;
    CTelnetConn* telnet = nullptr;
    Parent frame;
    Parent* parent = &frame;
    bool doflash = false, auto_switch = false;
    void HideCaret() {}
    void ShowCaret() {}
    void UpdateWindow() {}
    UINT GetCodePage() { return codePage; }
    CString GetSelAnsi();
    void SendAnsiString(CString);
};
struct { struct { BOOL paste_autowrap = TRUE; } site_settings; int ansi_output_format = 0, sound = 0; CString wavepath; } AppConfig;
class CTelnetConn
{
public:
    BYTE cur_attr = 7, attr_flags = 0;
    Sgr66::PendingAttribute sgr66_attribute;
    struct Settings { int cols_per_page = 8; long line_count = 2; bool line_wrap = false; CString esc_convert = "\x15"; } site_settings;
    char text[9] = {};
    BYTE colors[8] = {};
    char second[9] = {};
    BYTE secondColors[8] = {};
    char* rows[2] = { text, second };
    char** screen = rows;
    CTermView ui;
    CTermView* view = &ui;
    CTelnetConn() { ui.telnet = this; }
    bool is_ansi_editor = false;
    struct Address
    {
        CString server = "ptt.cc";
        bool valid = true;
        bool IsValid() const { return valid; }
        const CString& Server() const { return server; }
    } address;
    CPoint sel_start = CPoint(0, 0), sel_end = CPoint(8, 0);
    bool sel_block = false;
    SynchronizedOutput sync_output;
    OscHyperlinks osc_links;
    AnsiSequenceParser ansi_parser;
    BYTE* buf = nullptr;
    BYTE* last_byte = nullptr;
    char ansi_param[64] = {};
    char* pansi_param = ansi_param;
    CPoint cursor_pos = CPoint(0,0), old_cursor_pos = CPoint(0,0);
    int first_line = 0, last_line = 1;
    BYTE saved_attr = 7;
    bool insert_mode = false, is_getting_article = false;
    void OnText();
    void ProcessAnsiEscapeSequence();
    void CheckSynchronizedOutputTimeout() {}
    void BeginSynchronizedOutput() { sync_output.Begin(0, screen, 2, 9); }
    void EndSynchronizedOutput() { sync_output.End(); }
    void CheckStrTrigger() {}
    void UpdateCursorPos() {}
    void CheckHyperLinks() {}
    bool GetUpdateLine(int) { return false; }
    void UpdateLine(int) {}
    void RemoveUpdateLine(int) {}
    void ContinueCopyArticle() {}
    void SetUpdateLine(int, BYTE) {}
    void SetUpdateWholeLine(int) {}
    void EditorCarriageRetiurn() { cursor_pos.y = 1; }
    void LineFeed(int = 1) { cursor_pos.y = 1; }
    void Back() { if(cursor_pos.x>0) --cursor_pos.x; }
    void ClearCurrentLine(int) {}
    void GotoXY(int,int) {}
    void SetScrollRange(int,int) {}
    void ClearScreen(int) {}
    void InsertLines(int) {}
    void GoUp(int) {}
    void GoDown(int) {}
    void GoRight(int) {}
    void GoLeft(int) {}
    void SaveCursorPos() {}
    void RestoreCursorPos() {}
    void InsertChar(int) {}
    void DeleteLines(int) {}
    void DeleteChar(int) {}
    void BackTab(int) {}
    void SetMode(int) {}
    void ResetMode(int) {}
    void DeviceStatusReport(int) {}
    void ScrollUp() {}
    void ScrollDown() {}
    void Feed(std::string input)
    {
        buf = reinterpret_cast<BYTE*>(&input[0]); last_byte = buf + input.size(); OnText();
    }
    std::string sent;
    BOOL wrapDuringSend = TRUE;
    int SendString(const char* value) { sent = value; wrapDuringSend = AppConfig.site_settings.paste_autowrap; return 0; }
    bool UseSgr66() const;
    BYTE* GetLineAttr(const char* row) { return row == text ? colors : secondColors; }
    BYTE* GetLineAttr(long row) { return row == 0 ? colors : secondColors; }
    static void SetBgColor(BYTE& attr, BYTE color) { attr = (attr & 143) | (color << 4); }
    static void SetFgColor(BYTE& attr, BYTE color) { attr = (attr & 248) | color; }
    void SetCurrentAttributes(USHORT);
    CString GetLineWithAnsi(long);
};
// Keep real native menu APIs; intercept only the modal help window.
class CMainFrame
{
public:
    HWND m_hWnd = nullptr;
    void OnAnsiOutputFormatHelp();
};
static std::wstring helpText, helpTitle;
static UINT helpFlags = 0;
static int WINAPI CaptureMessageBoxW(HWND owner, LPCWSTR text, LPCWSTR title, UINT flags)
{
    Check(owner == nullptr, "help uses the frame owner");
    helpText = text; helpTitle = title; helpFlags = flags;
    return IDOK;
}
#define MessageBoxW CaptureMessageBoxW
#include "ProductionSgr66Methods.inc"
#undef MessageBoxW

static std::wstring MenuText(HMENU menu, int position)
{
    const int length = ::GetMenuStringW(menu, position, nullptr, 0, MF_BYPOSITION);
    std::vector<wchar_t> text(static_cast<std::size_t>(length) + 1);
    ::GetMenuStringW(menu, position, text.data(), length + 1, MF_BYPOSITION);
    return text.data();
}

static std::wstring MenuSnapshot(HMENU menu, int limit = -1)
{
    const int count = limit < 0 ? ::GetMenuItemCount(menu) : limit;
    std::wstring result = std::to_wstring(count) + L"{";
    for (int i = 0; i < count; ++i)
    {
        result += std::to_wstring(::GetMenuItemID(menu, i)) + L":" +
            std::to_wstring(::GetMenuState(menu, i, MF_BYPOSITION)) + L":" + MenuText(menu, i) + L";";
        HMENU child = ::GetSubMenu(menu, i);
        if (child) result += MenuSnapshot(child);
    }
    return result + L"}";
}

static int CountMenuCommand(HMENU menu, UINT command)
{
    int result = 0;
    for (int i = 0; i < ::GetMenuItemCount(menu); ++i)
    {
        if (::GetMenuItemID(menu, i) == command) ++result;
        HMENU child = ::GetSubMenu(menu, i);
        if (child) result += CountMenuCommand(child, command);
    }
    return result;
}

static void CheckOutputMenu()
{
    // Load the built application's actual menu resources, without launching it.
    wchar_t executable[MAX_PATH] = {};
    Check(::GetModuleFileNameW(nullptr, executable, MAX_PATH) != 0, "locate test executable");
    std::wstring root = executable;
    for (int i = 0; i < 4; ++i) root.erase(root.find_last_of(L"\\/"));
#ifdef _DEBUG
    const std::wstring application = root + L"/Lite/Debug/PCMan/PCMan.exe";
#else
    const std::wstring application = root + L"/Lite/Release/PCMan/PCMan.exe";
#endif
    HMODULE resources = ::LoadLibraryExW(application.c_str(), nullptr,
        LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
    Check(resources != nullptr, "load matching built PCMan resources without execution");
    for (int reload = 0; reload < 3; ++reload)
    {
        HMENU menu = ::LoadMenuW(resources, MAKEINTRESOURCEW(IDR_BUILD_UI));
        Check(menu != nullptr, "load actual main menu");
        const int count = ::GetMenuItemCount(menu);
        HMENU options = ::GetSubMenu(menu, 2);
        const int optionsCount = ::GetMenuItemCount(options);
        HMENU dbcs = ::GetSubMenu(options, 7);
        Check(::GetMenuItemID(dbcs, 0) == ID_AUTODBCS_MOUSE, "original fixed DBCS index");
        std::vector<HMENU> handles;
        std::vector<std::wstring> labels, snapshots;
        for (int i = 0; i < count; ++i)
        {
            HMENU child = ::GetSubMenu(menu, i);
            handles.push_back(child);
            labels.push_back(MenuText(menu, i));
            snapshots.push_back(child ? MenuSnapshot(child) : L"");
        }
        AppConfig.ansi_output_format = reload;
        AppendAnsiOutputMenu(menu); // Unmodified production code, real Windows menus.
        Check(AppConfig.ansi_output_format == reload, "menu creation preserves saved format");
        Check(::GetMenuItemCount(menu) == count, "all top-level menu positions preserved");
        Check(::GetMenuItemCount(options) == optionsCount + 1, "append exactly one options submenu");
        Check(::GetSubMenu(options, 7) == dbcs, "DBCS submenu handle and position preserved");
        for (int i = 0; i < count; ++i)
        {
            HMENU child = ::GetSubMenu(menu, i);
            Check(child == handles[i], "original top-level submenu handle preserved");
            Check(MenuText(menu, i) == labels[i], "original top-level label preserved");
            Check((child ? MenuSnapshot(child, i == 2 ? optionsCount : -1) : L"") == snapshots[i],
                "every original command, separator, nested menu, state and position preserved");
        }
        Check(MenuText(options, optionsCount) == L"彩色內容輸出格式（全域）", "global scope in submenu title");
        HMENU format = ::GetSubMenu(options, optionsCount);
        Check(::GetMenuItemCount(format) == 5, "three formats plus separator and help");
        const UINT commands[] = { ID_ANSI_FORMAT_AUTO, ID_ANSI_FORMAT_LEGACY, ID_ANSI_FORMAT_SGR66 };
        const wchar_t* names[] = { L"自動（PTT／PTT2／ANSI 編輯器使用 SGR 66）", L"傳統 Big5 格式", L"SGR 66 相容格式" };
        for (int i = 0; i < 3; ++i)
        {
            Check(::GetMenuItemID(format, i) == commands[i], "original format command IDs and order retained");
            Check(MenuText(format, i) == names[i], "format label is readable Unicode");
            Check(CountMenuCommand(menu, commands[i]) == 1, "exactly one entry for each format");
            Check(CountMenuCommand(::GetSubMenu(menu, 5), commands[i]) == 0, "no format left in ANSI editor menu");
        }
        Check((::GetMenuState(format, 3, MF_BYPOSITION) & MF_SEPARATOR) != 0, "help separated from radio choices");
        Check(::GetMenuItemID(format, 4) == ID_ANSI_FORMAT_HELP && MenuText(format, 4) == L"說明…", "help command present");
        Check(CountMenuCommand(menu, ID_ANSI_FORMAT_HELP) == 1, "help command unique");
        CMainFrame frame;
        frame.OnAnsiOutputFormatHelp();
        Check(AppConfig.ansi_output_format == reload, "reading help does not change format");
        Check(helpTitle == L"彩色內容輸出格式（全域）說明", "help dialog title");
        Check(helpFlags == (MB_OK | MB_ICONINFORMATION), "help uses informational message box");
        for (const wchar_t* phrase : { L"全域設定", L"所有 BBS", L"ANSI 編輯器", L"不影響接收畫面",
            L"自動（預設）", L"PTT／PTT2 及其子網域", L"傳統 Big5 格式", L"不把既有 SGR 66 轉回舊格式",
            L"SGR 66 相容格式", L"目標站台支援", L"UTF-8", L"不轉換文字編碼" })
            Check(helpText.find(phrase) != std::wstring::npos, "help explains each mode, scope and limitations");
        Check(::DestroyMenu(menu) != FALSE, "release menu and appended popup on reload");
    }
    Check(::FreeLibrary(resources) != FALSE, "release resource module");
    AppendAnsiOutputMenu(nullptr); // An invalid parent must not create an orphan popup.
    AppConfig.ansi_output_format = Sgr66::Auto;
}

struct Decoded
{
    std::string text;
    std::vector<BYTE> colors;
};
// Actual parser and production SGR handler; only the screen/UI is replaced.
static Decoded Decode(const std::string& input, std::size_t split)
{
    CTelnetConn conn;
    AnsiSequenceParser parser;
    Decoded output;
    for (const std::string part : { input.substr(0, split), input.substr(split) })
        for (unsigned char byte : part)
        {
            const auto result = parser.Feed(byte);
            if (result == AnsiSequenceParser::Dispatch)
            {
                const std::string seq = parser.Sequence();
                if (seq[0] != '[' || seq.back() != 'm') { conn.sgr66_attribute.Reset(); continue; }
                std::size_t begin = 1;
                for (std::size_t i = 1; i < seq.size(); ++i)
                    if (seq[i] == ';' || seq[i] == 'm')
                    {
                        conn.SetCurrentAttributes(static_cast<USHORT>(std::atoi(seq.substr(begin, i - begin).c_str())));
                        begin = i + 1;
                    }
            }
            else if (result == AnsiSequenceParser::Text)
            {
                if (byte < 0x20) { conn.sgr66_attribute.Reset(); continue; }
                output.text += static_cast<char>(byte);
                output.colors.push_back(conn.sgr66_attribute.Take(conn.cur_attr));
            }
        }
    return output;
}

static void CheckRoundTrip(const std::string& encoded, const std::string& text,
    const std::vector<BYTE>& colors, bool allSplits = false)
{
    for (std::size_t split = 0; split <= (allSplits ? encoded.size() : 0); ++split)
    {
        const Decoded decoded = Decode(encoded, split);
        Check(decoded.text == text, "production parser round-trip text");
        Check(decoded.colors == colors, "production SGR handler round-trip half colors");
    }
}

int main()
{
    CheckOutputMenu();
    CTelnetConn conn;
    std::memset(conn.text, ' ', 8);
    std::memset(conn.colors, 7, 8);
    conn.text[0] = '\xa4'; conn.text[1] = '\xa4';
    conn.colors[0] = 15; conn.colors[1] = 11;
    const CString output = conn.GetLineWithAnsi(0);
    Check(output.Find("66;") >= 0, "real article export emits SGR 66 for split Big5 colors");
    Check(output.Find("\xa4\xa4") >= 0, "real article export never splits the Big5 character");
    CheckRoundTrip(LPCTSTR(output), "\xa4\xa4", {15, 11}, true);
    CheckRoundTrip("\x1b[1;37;66;33m\xa4\xa4" "X", "\xa4\xa4" "X", {15,11,11}, true);
    CheckRoundTrip("\x1b[1;37;66;0;31m\xa4\xa4", "\xa4\xa4", {15,1}, true);
    CheckRoundTrip("\x1b[31;66m\x1b[32m\xa4\xa4\xa4\xa4", "\xa4\xa4\xa4\xa4", {1,2,2,2}, true);
    CheckRoundTrip("\x1b[31;66;32;66;34m\xa4\xa4", "\xa4\xa4", {2,4}, true);
    CheckRoundTrip("\x1b[31;66;32mXY", "XY", {1,2}, true);
    CheckRoundTrip("\x1b[31;66;32m\r\nXY", "XY", {2,2}, true);
    for (unsigned int left = 0; left < 256; ++left)
        for (unsigned int right = 0; right < 256; ++right)
        {
            const unsigned char attrs[] = {static_cast<BYTE>(left), static_cast<BYTE>(right)};
            const std::string encoded = Sgr66::EncodeRange("\xa4\xa4", attrs, 2, 0, 2, 950);
            Check(encoded.find("\xa4\xa4") != std::string::npos, "every attribute pair keeps Big5 bytes contiguous");
            CheckRoundTrip(encoded, "\xa4\xa4", {attrs[0], attrs[1]});
        }
    AppConfig.ansi_output_format = Sgr66::Legacy;
    const CString legacy = conn.GetLineWithAnsi(0);
    Check(legacy.Find("66;") < 0 && legacy.Find("\xa4\xa4") < 0, "legacy article output retained");
    CheckRoundTrip(LPCTSTR(legacy), "\xa4\xa4", {15,11});
    std::memset(conn.text, ' ', 8);
    std::memset(conn.colors, 7, 8);
    Check(conn.GetLineWithAnsi(0) == "\x1b[m", "blank legacy row bounded");
    AppConfig.ansi_output_format = Sgr66::Auto;
    Check(conn.GetLineWithAnsi(0) == "\x1b[m", "blank compatible row bounded");
    conn.colors[7] = 0x17;
    CheckRoundTrip(LPCTSTR(conn.GetLineWithAnsi(0)), std::string(8, ' '), {7,7,7,7,7,7,7,0x17});

    const unsigned char attrs[] = {15,11,7,7,7,7,7,7};
    Check(Sgr66::EncodeRange("\xa4\xa4" "ABC   ", attrs, 8, 1, 5, 950) == "\x1b[mABC", "partial starting character omitted");
    Check(Sgr66::EncodeRange("\xa4\xa4" "ABC   ", attrs, 8, 0, 1, 950) == "\x1b[m", "partial ending character omitted");
    Check(Sgr66::EncodeRange("        ", attrs, 8, -20, 99, 950) == "\x1b[m", "range bounds clamped");
    const std::string utf8 = "\xe4\xb8\xad";
    Check(Sgr66::EncodeRange(utf8.data(), attrs, 3, 0, 3, 65001).find(utf8) != std::string::npos, "UTF8 character kept intact");
    Check(Sgr66::EncodeRange(utf8.data(), attrs, 3, 1, 3, 65001) == "\x1b[m", "partial UTF8 character omitted");
    for (const std::string value : { std::string("\xc2\xa2"), utf8, std::string("\xf0\x9f\x98\x80") })
        Check(Sgr66::CharacterLength(value.data(), value.size(), 65001) == value.size(), "valid UTF8 length");
    for (const std::string value : { std::string("\xc0\x80"), std::string("\xed\xa0\x80"), std::string("\xf4\x90\x80\x80"), std::string("\xe4") })
        Check(Sgr66::CharacterLength(value.data(), value.size(), 65001) == 1, "invalid UTF8 is not combined");

    const std::string splitAnsi = "\x1b[1;37m\xa4\x1b[33m\xa4";
    const std::string converted = Sgr66::ConvertSplitBig5(splitAnsi, 950);
    Check(converted == "\x1b[1;37m\x1b[66;33m\xa4\xa4", "legacy split ANSI hoisted to SGR66");
    CheckRoundTrip(converted, "\xa4\xa4", {15,11}, true);
    Check(Sgr66::ConvertSplitBig5(converted,950) == converted, "SGR66 paste is idempotent");
    Check(Sgr66::ConvertSplitBig5(splitAnsi,65001) == splitAnsi, "UTF8 not misread as Big5");
    for (const std::string control : { std::string("\x1b]0;") + splitAnsi + "\x07", std::string("\x1bP") + splitAnsi + "\x1b\\", std::string("\x1b_abc") + splitAnsi })
        Check(Sgr66::ConvertSplitBig5(control,950) == control, "opaque control string preserved");
    for (const std::string invalid : { std::string("\xa4\x1b[31"), std::string("\xa4\x1b[31m!"), std::string("\xa4\x1b[?31m\xa4"), std::string("\xa4\x1b[999999m\xa4") })
        Check(Sgr66::ConvertSplitBig5(invalid,950) == invalid, "incomplete or unsupported split preserved");
    std::string many = "\x1b[37m\xa4";
    for (int i = 0; i < 50; ++i) many += "\x1b[31m";
    many += "\xa4";
    CheckRoundTrip(Sgr66::ConvertSplitBig5(many,950), "\xa4\xa4", {7,1}, true);
    for (const char* host : {"ptt.cc", "ws.ptt2.cc", "example.com", "ptt.cc.evil.example"})
    {
        const bool ptt = KeepAlive::IsPttHost(host);
        Check(Sgr66::Enabled(Sgr66::Auto,ptt,false) == ptt, "auto format host decision");
        Check(Sgr66::Enabled(Sgr66::Auto,ptt,true), "editor defaults to new format");
        Check(!Sgr66::Enabled(Sgr66::Legacy,ptt,true), "explicit legacy wins");
        Check(Sgr66::Enabled(Sgr66::Compatible,ptt,false), "explicit new format wins");
    }
    Check(Sgr66::NormalizeFormat(-1) == Sgr66::Auto && Sgr66::NormalizeFormat(100) == Sgr66::Auto, "invalid preference normalized");

    std::memset(conn.text, ' ', 8); std::memset(conn.colors, 7, 8);
    conn.text[0]='\xa4'; conn.text[1]='\xa4'; conn.text[2]='X';
    conn.colors[0]=15; conn.colors[1]=11;
    std::memset(conn.second, ' ', 8); std::memset(conn.secondColors, 7, 8);
    conn.second[0]='Y'; conn.second[1]='\xa4'; conn.second[2]='\xa4';
    conn.secondColors[1]=1; conn.secondColors[2]=2;
    CheckRoundTrip(LPCTSTR(conn.ui.GetSelAnsi()), "\xa4\xa4" "X", {15,11,7});
    conn.sel_start=CPoint(0,0); conn.sel_end=CPoint(3,1);
    const CString multi=conn.ui.GetSelAnsi();
    Check(multi.Find("\r\n")>=0, "real selection keeps line boundaries");
    CheckRoundTrip(LPCTSTR(multi), "\xa4\xa4" "XY\xa4\xa4", {15,11,7,7,1,2});
    std::swap(conn.sel_start,conn.sel_end);
    Check(conn.ui.GetSelAnsi()==multi, "reverse selection preserves complete range");
    conn.sel_block=true;
    conn.sel_start=CPoint(1,0); conn.sel_end=CPoint(3,1);
    CheckRoundTrip(LPCTSTR(conn.ui.GetSelAnsi()), "X\xa4\xa4", {7,1,2});
    conn.sel_start=conn.sel_end;
    Check(conn.ui.GetSelAnsi().IsEmpty(), "empty selection does not export resets");
    conn.sel_start=CPoint(0,0); conn.sel_end=CPoint(3,1);
    conn.sync_output.Begin(0, conn.screen, 2, 9);
    Check(conn.ui.GetSelAnsi().IsEmpty(), "synchronized output blocks selection export");
    conn.sync_output.Reset();

    conn.ui.SendAnsiString(CString(splitAnsi.c_str()));
    Check(conn.sent == std::string("\x15[1;37m\x15[66;33m\xa4\xa4"), "real paste converts before ESC substitution");
    Check(!conn.wrapDuringSend && AppConfig.site_settings.paste_autowrap, "paste restores auto-wrap setting");
    AppConfig.ansi_output_format=Sgr66::Legacy;
    conn.ui.SendAnsiString(CString(splitAnsi.c_str()));
    Check(conn.sent==std::string("\x15[1;37m\xa4\x15[33m\xa4"), "legacy paste unchanged except existing ESC substitution");
    AppConfig.ansi_output_format=Sgr66::Auto;
    conn.is_ansi_editor=true;
    conn.ui.SendAnsiString(CString(splitAnsi.c_str()));
    Check(conn.sent==converted, "editor paste keeps actual ESC for readback");
    conn.is_ansi_editor=false;
    conn.address.server="example.com";
    Check(!conn.UseSgr66(), "real format selection preserves other BBS default");
    conn.address.server="ptt.cc.evil.example";
    Check(!conn.UseSgr66(), "real format selection rejects lookalike host");
    conn.address.server="ws.ptt2.cc";
    Check(conn.UseSgr66(), "real format selection recognizes PTT2");
    conn.ui.codePage=932;
    Check(!conn.UseSgr66(), "unsupported codepage keeps legacy path");
    conn.ui.codePage=65001;
    Check(conn.UseSgr66(), "UTF8 output uses character-safe path");

    const std::string received="\x1b[1;37;66;33m\xa4\xa4" "X";
    for (std::size_t split=0; split<=received.size(); ++split)
    {
        CTelnetConn receiver;
        receiver.Feed(received.substr(0,split)); receiver.Feed(received.substr(split));
        Check(std::string(receiver.text,3)==std::string("\xa4\xa4" "X"), "real OnText receives every split");
        Check(receiver.colors[0]==15 && receiver.colors[1]==11 && receiver.colors[2]==11, "real OnText applies captured color once");
    }
    CTelnetConn bytewise;
    for(char byte:received) bytewise.Feed(std::string(1,byte));
    Check(bytewise.colors[0]==15 && bytewise.colors[1]==11, "real OnText bytewise dual color");
    for(const std::string cancel:{std::string("\r"),std::string("\t"),std::string("\x18"),std::string("\x1a"),std::string("\x1b[K")})
    {
        CTelnetConn receiver;
        receiver.Feed("\x1b[31;66;32m"+cancel+"X");
        const int x=cancel=="\t" ? 7:0;
        Check(receiver.colors[x]==2, "real OnText controls cancel pending half attribute");
    }
    CTelnetConn utf8Receiver;
    utf8Receiver.ui.codePage=65001;
    utf8Receiver.Feed("\x1b[1;37;66;33m"+utf8);
    Check(utf8Receiver.colors[0]==11 && utf8Receiver.colors[1]==11 && utf8Receiver.colors[2]==11, "UTF8 receive keeps documented single-color fallback");
    CTelnetConn opaque;
    opaque.Feed("\x1b]0;\x1b[31;66;32m\x07X");
    Check(opaque.colors[0]==7 && opaque.text[0]=='X', "real OnText OSC cannot inject SGR66");
    bytewise.sgr66_attribute.Capture(1); bytewise.sgr66_attribute.Reset();
    Check(bytewise.sgr66_attribute.Take(7)==7, "pending reset for connection lifecycle");
    AppConfig.ansi_output_format=Sgr66::Legacy;
    CTelnetConn blank;
    std::memset(blank.text,' ',8); std::memset(blank.colors,7,8);
    std::memset(blank.second,' ',8); std::memset(blank.secondColors,7,8);
    Check(blank.ui.GetSelAnsi()=="\x1b[m", "legacy blank selection stays within row");
    blank.sel_end=CPoint(0,1);
    Check(blank.ui.GetSelAnsi()=="\x1b[m\r\n", "legacy selection ending at column zero stays within row");
    blank.text[0]='\xa4'; blank.text[1]='\xa4'; blank.colors[0]=15; blank.colors[1]=11;
    blank.sel_end=CPoint(8,0);
    CheckRoundTrip(LPCTSTR(blank.ui.GetSelAnsi()),"\xa4\xa4",{15,11});
    std::printf("PASS: %u SGR 66 checks\n", checks);
}
