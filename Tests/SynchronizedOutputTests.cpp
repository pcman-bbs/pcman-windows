#include "../Lite/AnsiSequenceParser.h"
#include "../Lite/SynchronizedOutput.h"
#include "../Lite/OscHyperlinks.h"
#include <cstdio>
#include <cstdlib>
#include <string>

static unsigned int checks = 0;
static std::uint64_t testTime = 0;
static std::uint64_t GetTickCount64() { return testTime; }
static const int FALSE = 0;
static void Check(bool ok, const char* label)
{
    ++checks;
    if (!ok) { std::fprintf(stderr, "FAIL: %s\n", label); std::exit(1); }
}
class CTelnetConn;
struct FakeView
{
    CTelnetConn* telnet = nullptr;
    int hidden = 0, repaints = 0;
    void HideCaret() { ++hidden; }
    void Invalidate(int) { ++repaints; }
};
class CTelnetConn
{
public:
    AnsiSequenceParser parser;
    SynchronizedOutput sync_output;
    OscHyperlinks osc_links, osc_snapshot;
    char model[4] = { 'O', 'L', 'D', 7 };
    char* screen[1] = { model };
    int scroll_pos = 0, sync_scroll_pos = 0, first_line = 0, last_line = 0;
    bool is_ansi_editor = false;
    struct Settings { int lines_per_page = 1, cols_per_page = 3; } site_settings;
    FakeView ownView;
    FakeView* view = &ownView;
    unsigned int begins = 0, ends = 0, commands = 0, dirty = 0, cursors = 0;
    CTelnetConn() { ownView.telnet = this; }
    void CheckHyperLinks() {}
    int GetLineBufLen() { return 4; }
    void SetUpdateWholeLine(int) { ++dirty; }
    void UpdateCursorPos() { if (!sync_output.Active()) ++cursors; }
    void BeginSynchronizedOutput();
    void EndSynchronizedOutput();
    void RefreshSynchronizedOutput();
    void CheckSynchronizedOutputTimeout();
    void Feed(const std::string& data)
    {
        CheckSynchronizedOutputTimeout();
        for (unsigned char c : data)
        {
            const auto result = parser.Feed(c);
            if (result == AnsiSequenceParser::Osc) osc_links.Command(parser.OscSequence(), parser.OscValid());
            else if (result == AnsiSequenceParser::SyncBegin) { ++begins; BeginSynchronizedOutput(); }
            else if (result == AnsiSequenceParser::SyncEnd) { ++ends; EndSynchronizedOutput(); }
            else if (result == AnsiSequenceParser::Dispatch) ++commands;
            else if (result == AnsiSequenceParser::Text) { osc_links.Write(model, 0, 3); model[0] = static_cast<char>(c); }
        }
    }
    char Display() { return sync_output.Active() ? sync_output.Row(0)[0] : model[0]; }
};
#include "ProductionSyncMethods.inc"

int main()
{
    const std::string begin = "\x1b[?2026h", end = "\x1b[?2026l";
    const std::string input = begin + "N" + end;
    for (std::size_t split = 0; split <= input.size(); ++split)
    {
        CTelnetConn t;
        t.Feed(input.substr(0, split));
        if (split >= begin.size() && split < input.size())
            Check(t.Display() == 'O' && !t.sync_output.CopyReady(), "partial frame not presented/captured");
        t.Feed(input.substr(split));
        Check(t.Display() == 'N' && t.sync_output.CopyReady(), "every split completes");
        Check(t.begins == 1 && t.ends == 1 && t.commands == 0, "sync bypasses legacy dispatcher");
        Check(t.view->repaints == 1 && t.view->hidden == 1, "one release and caret barrier");
    }
    {
        CTelnetConn t;
        for (char c : begin + "N") t.Feed(std::string(1, c));
        t.model[3] = 2;
        Check(t.Display() == 'O' && t.sync_output.Row(0)[3] == 7, "text and attributes deeply copied");
        t.screen[0] = nullptr;
        Check(t.sync_output.Row(0)[0] == 'O', "snapshot independent of pointer swaps");
        for (char c : end) t.Feed(std::string(1, c));
        Check(t.Display() == 'N', "bytewise end releases snapshot");
    }
    {
        CTelnetConn a, b;
        a.Feed(begin + "N"); testTime = 1900; a.Feed(begin);
        testTime = 1999; a.CheckSynchronizedOutputTimeout();
        Check(a.sync_output.Active(), "before deadline");
        testTime = 2000; a.CheckSynchronizedOutputTimeout();
        Check(!a.sync_output.Active(), "repeated begin cannot extend deadline");
        Check(a.Display() == 'N' && !a.sync_output.CopyReady(), "timeout releases display only");
        Check(a.view->repaints == 1, "timer repaints without receiving bytes");
        a.CheckSynchronizedOutputTimeout(); Check(a.view->repaints == 1, "expire once");
        Check(b.sync_output.CopyReady() && b.Display() == 'O', "connection isolation");
        a.Feed(end); Check(a.sync_output.CopyReady(), "late end recovers");
        a.Feed(begin); a.sync_output.Abort();
        Check(!a.sync_output.Active() && !a.sync_output.CopyReady(), "resize not completion");
        a.sync_output.Reset(); Check(a.sync_output.CopyReady(), "reset permits new session");
        Check(!a.sync_output.End(), "stray end harmless");
        testTime = 0;
    }
    {
        CTelnetConn t;
        t.Feed(begin + "A" + end + begin + "B");
        Check(t.Display() == 'A', "multiple frames retain last complete model");
        t.Feed(end); Check(t.Display() == 'B', "second frame release");
    }
    {
        CTelnetConn t;
        t.Feed(begin + "N\x1b]0;title" + end + "\x1b[2J");
        Check(t.Display() == 'O' && t.begins == 1 && t.ends == 0 && t.commands == 0,
            "OSC payload cannot dispatch or release presentation barrier");
        t.Feed("\x1b\\" + end);
        Check(t.Display() == 'N' && t.ends == 1 && t.sync_output.CopyReady(),
            "real sync end after OSC releases presentation barrier");
    }
    {
        CTelnetConn t; t.view->telnet = nullptr;
        t.Feed(begin + "N" + end);
        Check(t.dirty == 1 && t.view->repaints == 0, "background connection does not repaint active tab");
        t.is_ansi_editor = true; t.Feed(begin);
        Check(!t.sync_output.Active(), "local ANSI editor unaffected");
    }
    {
        CTelnetConn t;
        t.Feed("\x1b]8;;https://old.example\x1b\\O\x1b]8;;\x1b\\" + begin);
        t.Feed("\x1b]8;;https://new.example\x1b\\N\x1b]8;;\x1b\\");
        const auto oldTarget = t.osc_snapshot.At(t.sync_output.Row(0), 0);
        Check(oldTarget && *oldTarget == "https://old.example" && t.Display() == 'O',
            "snapshot retains visible old OSC link while model receives new target");
        t.Feed(end);
        Check(t.osc_links.At(t.model, 0) && *t.osc_links.At(t.model, 0) == "https://new.example" && t.Display() == 'N',
            "completed frame presents new OSC target with new text");
    }
    for (const std::string& bad : { std::string("\x1b[?2026;1000h"), std::string("\x1b[?2026 h"),
        std::string("\x1b[>2026h"), std::string("\x1b[2026h"), std::string("\x1b[?2026$p"),
        std::string("\x1b[?2026\x18h"), std::string("\x1b[?2026\x1a" "l"),
        std::string("\x1b[?") + std::string(100, '0') + "2026h" })
    {
        CTelnetConn t; t.Feed(bad);
        Check(t.begins == 0 && t.ends == 0, "malformed/unknown modes cannot open barrier");
        t.Feed(begin + "N" + end);
        Check(t.begins == 1 && t.ends == 1 && t.sync_output.CopyReady(), "parser recovers");
    }
    std::printf("PASS: %u synchronized output checks (real sync methods, fake UI/clock)\n", checks);
}
