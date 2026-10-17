#include <afx.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <iterator>
#include <vector>
#include "../Lite/ArticleProgress.h"
#include "../Lite/SynchronizedOutput.h"

static unsigned int checks = 0;
static int messages = 0;
static void Check(bool ok, const char* why)
{
    ++checks;
    if (!ok) { std::fprintf(stderr, "FAIL: %s\n", why); std::exit(1); }
}
static int TestMessageBox(const char*, unsigned int) { ++messages; return 0; }
#define AfxMessageBox TestMessageBox
class CTelnetConn;
struct CUcs2Conv
{
    unsigned char* m_Big52Ucs2_Tab = nullptr;
    size_t Big52Ucs2(const char*, wchar_t*, int);
} g_ucs2conv;
struct CClipboard
{
    static CStringW text;
    static void SetTextW(HWND, const wchar_t* value) { text = value; }
};
CStringW CClipboard::text = L"unchanged";
struct CTermView
{
    HWND m_hWnd = nullptr;
    CTelnetConn* telnet = nullptr;
    UINT codePage = 950;
    int editorOpens = 0;
    UINT GetCodePage() { return codePage; }
    CStringW DecodeText(const CString&);
    void OnAnsiEditor() { ++editorOpens; }
    void LockWindowUpdate() {}
    void UnlockWindowUpdate() {}
    void SetScrollBar() {}
};
struct CDownloadArticleDlg
{
    explicit CDownloadArticleDlg(CTelnetConn*) {}
    int DoModal() { return 0; }
    void OnCancel() {}
};
static CDownloadArticleDlg* download_article_dlg = nullptr;
class CTelnetConn
{
public:
    CTermView testView;
    CTermView* view = &testView;
    POINT cursor_pos = {};
    CString editorBytes;
    CTelnetConn() { testView.telnet = this; }
    void Send(const char* bytes, int length) { editorBytes.SetString(bytes, length); }
    void UpdateCursorPos() {}
    void ProductionCopyArticleComplete(bool cancel = false);
	SynchronizedOutput sync_output;
    struct Settings { int line_count = 4; int cols_per_page = 80; } site_settings;
    struct KeyMap { const char* FindKey(int, int) { return nullptr; } } keys;
    KeyMap* key_map = &keys;
    char lines[4][81] = {};
    char* rows[4] = { lines[0], lines[1], lines[2], lines[3] };
    char** screen = rows;
    int last_line = 3, first_line = 0, scroll_pos = 0;
    int current_download_line = 0;
    bool is_getting_article = false, get_article_with_ansi = false, get_article_in_editor = false;
    CString downloaded_article;
    CString clipboard = "unchanged";
    int sends = 0, completes = 0;
    ~CTelnetConn() { delete download_article_dlg; download_article_dlg = nullptr; }
    void SendString(const char*) { ++sends; }
    CString GetLineWithAnsi(int y) { return CString("ANSI:") + screen[y]; }
    void CopyArticleComplete(bool = false)
    {
        ++completes; is_getting_article = false; clipboard = downloaded_article;
    }
    int IsEndOfArticleReached();
    ArticleProgress::Progress GetArticleProgress();
    void CopyArticle(bool, bool);
    void ContinueCopyArticle();
    void Footer(const std::string& s)
    {
        std::memset(lines[3], ' ', 80);
        std::memcpy(lines[3], s.data(), (s.size() < 80 ? s.size() : 80));
    }
};
#include "ProductionArticleMethods.inc"

static std::string Footer(int percent, int first, int last)
{
    return "page (" + std::to_string(percent) + "%) range " + std::to_string(first) +
        "~" + std::to_string(last) + " \xa6\xe6 (X/%) help";
}

static std::string Encode(const std::wstring& text, UINT codePage)
{
    const int length = WideCharToMultiByte(codePage, 0, text.data(),
        static_cast<int>(text.size()), nullptr, 0, nullptr, nullptr);
    Check(length > 0, "regression fixture encoding size");
    std::string bytes(length, '\0');
    Check(WideCharToMultiByte(codePage, 0, text.data(), static_cast<int>(text.size()),
        &bytes[0], length, nullptr, nullptr) == length, "regression fixture encoding");
    return bytes;
}

// Synthetic examples based on the two UI announcements, not captured sessions.
static void InterfaceRegression()
{
    using namespace ArticleProgress;
    for (UINT codePage : { 950u, static_cast<UINT>(CP_UTF8) })
    {
        for (const wchar_t* caption : { L"  選擇看板  ", L" 看板列表 ", L" 我的最愛 ",
            L" 分類看板 ", L" 文章選讀 ", L" 文章列表 ", L" 系列文章 ", L" 文摘列表 ",
            L" 鴻雁往返 ", L" 信件列表 ", L" 【功能鍵】 ", L" 精華列表 ", L" 精華管理 ",
            L" 標記項目 ", L" 主功能表 ", L" 看板設定與管理 - Test " })
        {
            const std::string row = Encode(std::wstring(caption) + L" (←)離開 (h)說明", codePage);
            Check(Parse(row.data(), row.size()).state == Unavailable,
                "old/new non-article captions cannot complete article copy");
        }
        for (const wchar_t* hint : { L"", L" (h)說明(→)離開 ", L" (h)說明 (←/q)離開 ",
            L" (←)離開 (h)說明", L" (y)回應 (X)推文 (←)離開 (h)說明" })
        {
            for (int percent : { 0, 50, 100 })
            {
                const std::wstring footer = L"  瀏覽 第 1/2 頁 (" + std::to_wstring(percent) +
                    L"%)  目前顯示: 第 01~03 行";
                const auto row = Encode(footer + hint, codePage);
                for (std::size_t width : { 80u, 120u })
                {
                    std::string padded = row;
                    padded.resize(width, ' ');
                    const auto progress = Parse(padded.data(), width);
                    Check(progress.state == (percent == 100 ? Complete : More) &&
                        progress.first == 1 && progress.last == 3,
                        "progress independent of old/new/hidden hints and row width");
                }
            }
        }
        const auto row = Encode(L" 文章列表  (h)說明", codePage);
        const auto body = Encode(L"  瀏覽 第 1/1 頁 (100%)  目前顯示: 第 01~03 行", codePage);
        const auto adjacent = row + body;
        Check(Parse(adjacent.data(), row.size()).state == Unavailable,
            "article-looking adjacent data cannot turn a list footer into progress");
    }
}
int main()
{
    using namespace ArticleProgress;
    // Use the shipped B2U table, not a synthetic replacement mapping.
    std::ifstream tableFile("Lite/Release/PCMan/B2U", std::ios::binary);
    Check(tableFile.good(), "run from repository root; shipped B2U readable");
    std::vector<unsigned char> table((std::istreambuf_iterator<char>(tableFile)),
        std::istreambuf_iterator<char>());
    Check(table.size() >= 38u * 157u * 2u, "B2U covers extension ranges");
    g_ucs2conv.m_Big52Ucs2_Tab = table.data();
    const CString japanese = "\xc7\xd2\xc7\xe7\xc7\x7c\xc7\xa7MAD\xad\xb5\x92\x62\xb2\xbd";
    const CStringW expectedJapanese = L"ブルアカMAD音楽祭";
    for (UINT cp : { 950u, static_cast<UINT>(CP_UTF8) })
    {
        for (bool ansi : { false, true })
        {
            CTelnetConn c;
            c.testView.codePage = cp;
            const auto utf8Bytes = Encode(static_cast<LPCWSTR>(expectedJapanese), CP_UTF8);
            const CString body = cp == 950 ? japanese : CString(utf8Bytes.c_str());
            c.downloaded_article = ansi ? CString("\x1b[31m") + body + "\x1b[m\r\n" : body + "\r\n";
            c.get_article_with_ansi = ansi;
            c.is_getting_article = true;
            c.ProductionCopyArticleComplete();
            const CStringW expected = ansi ? CStringW(L"\x1b[31m") + expectedJapanese + L"\x1b[m\r\n" : expectedJapanese + L"\r\n";
            Check(CClipboard::text == expected, "article Unicode output preserves Japanese and ANSI escapes");
            Check(c.downloaded_article.IsEmpty() && !c.is_getting_article && !c.get_article_with_ansi,
                "completion clears download state");
        }
    }
    {
        CTelnetConn c;
        c.downloaded_article = japanese;
        c.get_article_in_editor = true;
        CClipboard::text = L"keep clipboard";
        c.ProductionCopyArticleComplete();
        Check(c.editorBytes == japanese && c.testView.editorOpens == 1,
            "ANSI editor receives original station bytes");
        Check(CClipboard::text == L"keep clipboard", "editor does not overwrite clipboard");
        c.downloaded_article = japanese;
        c.ProductionCopyArticleComplete(true);
        Check(CClipboard::text == L"keep clipboard", "cancel does not overwrite clipboard");
    }
    InterfaceRegression();
    const std::string valid = Footer(50, 1, 3);
    for (const char* bad : { "", "    ", "100%", "(101%) 1~3 \xa6\xe6", "(-1%) 1~3 \xa6\xe6",
        "(50%) 0~3 \xa6\xe6", "(50%) 4~3 \xa6\xe6", "(50%) 1~ \xa6\xe6",
        "(50%) 1~3", "(50%) 1~3 ", "(50%) 1~3 \xa6", "(50%) 1~3x \xa6\xe6",
        "(50%) 1~999999999999999999999 \xa6\xe6", "(999999999999999%) 1~3 \xa6\xe6",
        "(50%) 1~80 cols, 1~3 \xa6\xe6", "(50%) no range", "(50%) (100%) 1~3 \xa6\xe6" })
        Check(Parse(bad, std::strlen(bad)).state == Unavailable, "reject invalid/incomplete footer");
    Check(Parse(nullptr, 80).state == Unavailable, "null safe");
    Check(Parse(valid.data(), 0).state == Unavailable, "zero width");
    Check(Parse(valid.data(), valid.size()).state == More, "valid non-NUL input");
    const std::string hints = valid + " ~ 999% \xa4\x7e";
    Check(Parse(hints.data(), hints.size()).state == More, "right-hand hints ignored");
    const std::string utf8 = "page (100%) range 1~3 \xe8\xa1\x8c";
    Check(Parse(utf8.data(), utf8.size()).state == Complete, "UTF8 footer");
    const std::string spaced = "page (  0%) range 01 ~ 03 \xa6\xe6";
    Check(Parse(spaced.data(), spaced.size()).state == More, "padded fields and zero percent");
    for (int p = 0; p <= 100; ++p)
    {
        const auto s = Footer(p, 1, 3);
        Check(Parse(s.data(), s.size()).state == (p == 100 ? Complete : More), "only 100 completes");
    }
    // Put the last supplied byte directly before an inaccessible memory page.
    SYSTEM_INFO info; GetSystemInfo(&info);
    char* memory = static_cast<char*>(VirtualAlloc(nullptr, info.dwPageSize * 2,
        MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE));
    Check(memory != nullptr, "guard allocation");
    DWORD old = 0;
    Check(VirtualProtect(memory + info.dwPageSize, info.dwPageSize, PAGE_NOACCESS, &old) != 0,
        "guard protection");
    for (std::size_t n = 0; n <= valid.size(); ++n)
    {
        char* input = memory + info.dwPageSize - n;
        std::memcpy(input, valid.data(), n);
        const auto result = Parse(input, n);
        Check(result.state == Unavailable || result.state == More, "bounded prefixes");
    }
    for (int n = 1; n <= 160; ++n)
    {
        char* input = memory + info.dwPageSize - n;
        std::memset(input, ' ', n);
        Check(Parse(input, n).state == Unavailable, "nonterminated blank row bounded");
        std::memset(input, '9', n);
        Check(Parse(input, n).state == Unavailable, "nonterminated digits bounded");
    }
    VirtualFree(memory, 0, MEM_RELEASE);
    Check(NewLines(Parse(valid.data(), valid.size()), 3, 3) == 0, "duplicate progress");
    for (int delta : { -1, 0, 1, 2, 3, 100 })
    {
        const auto s = Footer(60, 1 + delta, 3 + delta);
        const auto p = Parse(s.data(), s.size());
        Check(NewLines(p, 3, 3) == (delta == 1 || delta == 2 ? delta : 0), "bounded advance");
    }
    {
        CTelnetConn c;
        c.Footer("no footer"); c.CopyArticle(false, false);
        Check(c.sends == 0 && c.completes == 0 && c.clipboard == "unchanged" && messages == 1,
            "invalid start leaves clipboard and network alone");
        Check(c.IsEndOfArticleReached() == Unavailable, "unknown is distinct from complete");
        c.last_line = -1;
        Check(c.GetArticleProgress().state == Unavailable, "negative row safe");
        c.last_line = 4;
        Check(c.GetArticleProgress().state == Unavailable, "row limit safe");
    }
    for (bool ansi : { false, true })
    {
        CTelnetConn c;
        std::strcpy(c.lines[0], "one"); std::strcpy(c.lines[1], "two"); std::strcpy(c.lines[2], "three");
        c.Footer(valid); c.CopyArticle(ansi, false);
        Check(c.sends == 1 && c.is_getting_article && download_article_dlg, "start requests once and allows cancel");
        c.ContinueCopyArticle();
        Check(c.sends == 1, "same footer does not request again");
        c.Footer(Footer(90, 8, 10)); c.ContinueCopyArticle();
        Check(c.sends == 1 && c.current_download_line == 3 && c.completes == 0, "unexpected jump waits");
        c.Footer("(100%)"); c.ContinueCopyArticle();
        Check(c.sends == 1 && c.completes == 0, "missing range cannot complete");
        const auto next = Footer(75, 2, 4);
        const std::size_t unitEnd = next.find("\xa6\xe6") + 2;
        for (std::size_t n = 0; n < unitEnd; ++n)
        {
            c.Footer(next.substr(0, n)); c.ContinueCopyArticle();
            Check(c.sends == 1 && c.current_download_line == 3 && c.completes == 0, "split footer waits");
        }
        std::strcpy(c.lines[2], "four"); c.Footer(next); c.ContinueCopyArticle();
        Check(c.sends == 2 && c.current_download_line == 4, "append one line");
        c.ContinueCopyArticle(); Check(c.sends == 2, "no duplicate request");
        c.Footer(Footer(100, 2, 4)); c.ContinueCopyArticle();
        Check(c.completes == 0, "new percent with stale range cannot complete");
        std::strcpy(c.lines[1], "five"); std::strcpy(c.lines[2], "six");
        c.Footer(Footer(100, 4, 6)); c.ContinueCopyArticle();
        Check(c.completes == 1 && c.sends == 2 && !c.is_getting_article, "two lines and complete without extra Down");
        const CString expected = ansi ? "ANSI:one\r\nANSI:two\r\nANSI:three\r\nANSI:four\r\nANSI:five\r\nANSI:six\r\n" :
            "one\r\ntwo\r\nthree\r\nfour\r\nfive\r\nsix\r\n";
        Check(c.clipboard == expected, "copy content without duplicate or lost rows");
    }
    {
        CTelnetConn c; c.Footer(Footer(100, 1, 3)); c.CopyArticle(false, false);
        Check(c.completes == 1 && c.sends == 0, "single page needs no Down");
    }
    {
        CTelnetConn c; c.Footer(Footer(100, 1, 3));
        c.sync_output.Begin(0, c.rows, 4, 81);
        c.CopyArticle(false, false);
        Check(c.completes == 0 && c.sends == 0, "sync blocks initial article capture");
        c.sync_output.Expire(2000);
        c.CopyArticle(false, false);
        Check(c.completes == 0 && c.sends == 0, "timeout is not article completion");
        c.sync_output.End(); c.CopyArticle(false, false);
        Check(c.completes == 1, "real sync end permits capture");
    }
    {
        CTelnetConn c; c.Footer(Footer(50, 1, 3)); c.CopyArticle(false, false);
        c.sync_output.Begin(0, c.rows, 4, 81);
        c.Footer(Footer(100, 2, 4)); c.ContinueCopyArticle();
        Check(c.completes == 0 && c.sends == 1 && c.current_download_line == 3,
            "in-flight sync blocks continuation and additional Down");
        c.sync_output.Expire(2000); c.ContinueCopyArticle();
        Check(c.completes == 0 && c.current_download_line == 3, "timeout cannot finish continuation");
        c.sync_output.End(); c.ContinueCopyArticle();
        Check(c.completes == 1 && c.sends == 1, "real end completes pending copy once");
    }
    std::printf("PASS: %u article checks (bounded parser and real copy methods)\n", checks);
}
