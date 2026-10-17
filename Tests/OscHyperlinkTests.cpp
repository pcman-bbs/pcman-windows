#include "../Lite/AnsiSequenceParser.h"
#include "../Lite/OscHyperlinks.h"
#include <cstdio>
#include <cstdlib>

static unsigned int checks = 0;
static void Check(bool ok, const char* label)
{
    ++checks;
    if (!ok) { std::fprintf(stderr, "FAIL: %s\n", label); std::exit(1); }
}
static const std::string uri = "https://example.com/target?q=1&v=2";
static const std::string open = "\x1b]8;id=first;" + uri + "\x1b\\";
static const std::string close = "\x1b]8;;\x1b\\";

struct Terminal
{
    AnsiSequenceParser parser;
    OscHyperlinks links;
    char row[160] = {};
    std::string text;
    unsigned int csi = 0, sync = 0;
    void Feed(const std::string& bytes)
    {
        for (unsigned char byte : bytes)
        {
            const auto result = parser.Feed(byte);
            if (result == AnsiSequenceParser::Osc) links.Command(parser.OscSequence(), parser.OscValid());
            else if (result == AnsiSequenceParser::Text)
            {
                Check(text.size() < sizeof(row), "fixture width");
                links.Write(row, static_cast<int>(text.size()), 160);
                row[text.size()] = static_cast<char>(byte);
                text += static_cast<char>(byte);
            }
            else if (result == AnsiSequenceParser::Dispatch) ++csi;
            else if (result == AnsiSequenceParser::SyncBegin || result == AnsiSequenceParser::SyncEnd) ++sync;
        }
    }
};

static void CheckLabel(const Terminal& t)
{
    Check(t.text == "XLABELY" && !t.links.At(t.row, 0) && !t.links.At(t.row, 6), "only label is linked");
    for (int x = 1; x <= 5; ++x)
        Check(t.links.At(t.row, x) && *t.links.At(t.row, x) == uri, "label hit returns hidden target");
}

int main()
{
    const std::string data = "X" + open + "LABEL" + close + "Y";
    for (size_t split = 0; split <= data.size(); ++split)
    {
        Terminal t;
        t.Feed(data.substr(0, split)); t.Feed(data.substr(split)); CheckLabel(t);
    }
    Terminal bytewise;
    for (char c : data) bytewise.Feed(std::string(1, c));
    CheckLabel(bytewise);
    for (const std::string& bad : { std::string("javascript:alert(1)"), std::string("file:///C:/Windows"),
        std::string("telnet://example.com"), std::string("data:text/html,test"), std::string(" https://example.com"),
        std::string("https://"), std::string("https:///path"), std::string("https://user@example.com"),
        std::string("http://:80/path"), std::string("http://[]/path"), std::string("http://example.com:invalid"),
        std::string("https://example.com/\r\n"), std::string("https://example.com/\\bad"),
        std::string("https://example.com/\"bad"), std::string("https://example.com/%GG"),
        std::string("https://example.com/%"), std::string("https://example.com/\x7f"),
        std::string("https://example.com/") + std::string(4096, 'x') })
    {
        Check(OscHyperlinks::SafeUrl(bad).empty(), "unsafe URI rejected");
        Terminal t; t.Feed("\x1b]8;;" + bad + "\x1b\\https://visible.example\x1b]8;;\x1b\\");
        if (bad.size() < AnsiSequenceParser::MaxOscLength)
            Check(t.links.At(t.row, 0) && t.links.At(t.row, 0)->empty() && t.links.Overlaps(t.row, 0, 23),
                "unsafe explicit link suppresses visible URL fallback");
    }
    Check(OscHyperlinks::SafeUrl("HTTP://example.com/a%20b") == "HTTP://example.com/a%20b", "HTTP and escaped URI allowed");
    Check(OscHyperlinks::SafeUrl("http://[::1]:8080/a") == "http://[::1]:8080/a", "bracketed IPv6 with port allowed");
    Check(OscHyperlinks::SafeUrl("https://example.com/\xe6\x97\xa5") == "https://example.com/%E6%97%A5", "UTF8 URL bytes percent encoded");
    {
        const std::string prefix = "8;;https://example.com/";
        const std::string exact = prefix + std::string(AnsiSequenceParser::MaxOscLength - prefix.size(), 'x');
        Terminal accepted, rejected;
        accepted.Feed("\x1b]" + exact + "\x1b\\A" + close);
        rejected.Feed("\x1b]" + exact + "x\x1b\\B" + close);
        Check(accepted.links.At(accepted.row, 0) && !accepted.links.At(accepted.row, 0)->empty(), "exact OSC limit accepted");
        Check(!rejected.links.At(rejected.row, 0) && rejected.text == "B", "one byte over OSC limit cannot create truncated link");
    }
    {
        Terminal t;
        t.Feed(open + "A\x1b]0;title\aB" + close + "C");
        Check(t.links.At(t.row, 0) == t.links.At(t.row, 1) && !t.links.At(t.row, 2), "unknown OSC does not close scope");
        t.Feed(open + "D\x1b]8;;bad\x18" + "E");
        Check(!t.links.At(t.row, 4), "cancelled command closes prior active scope");
        t.Feed(open + "F\x1b]8;;" + std::string(5000, 'x') + "\x1b\\G");
        Check(!t.links.At(t.row, 6), "overlong target never binds truncated URI or prior target");
        Check(t.csi == 0 && t.sync == 0, "OSC cannot dispatch terminal operations");
    }
    {
        Terminal t;
        const std::string label = "\xa4\xa4\xe6\x97\xa5";
        t.Feed(open + label + close);
        for (int i = 0; i < static_cast<int>(label.size()); ++i)
            Check(t.links.At(t.row, i) && *t.links.At(t.row, i) == uri, "all multibyte label bytes retain target");
        t.links.Write(t.row, 2, 160);
        Check(!t.links.At(t.row, 2) && t.links.At(t.row, 1), "unlinked overwrite clears only changed byte");
    }
    {
        Terminal t; t.Feed(open + "ABCDE" + close);
        t.links.Copy(t.row, 2, t.row, 0, 5, 160);
        t.links.Erase(t.row, 0, 2);
        Check(!t.links.At(t.row, 0) && t.links.At(t.row, 6), "insert shifts right and clears gap");
        t.links.Copy(t.row, 0, t.row, 2, 5, 160);
        t.links.Erase(t.row, 5, 155);
        Check(t.links.At(t.row, 0) && t.links.At(t.row, 4) && !t.links.At(t.row, 5), "delete shifts left and clears tail");
        char next[160] = {}, snapshotRow[160] = {}, resized[160] = {};
        t.links.Copy(next, 0, t.row, 2, 3, 160);
        Check(t.links.At(next, 0) && t.links.At(next, 2) && !t.links.At(next, 3), "row splitting copies linked range");
        const void* rows[] = { next, t.row };
        std::swap(rows[0], rows[1]);
        Check(t.links.At(rows[0], 4) && !t.links.At(rows[1], 4), "pointer-swapped rows carry their metadata");
        OscHyperlinks snapshot;
        snapshot.CopyFrom(t.links, snapshotRow, 0, t.row, 0, 160, 160);
        const auto retained = snapshot.At(snapshotRow, 0);
        t.links.Clear(t.row);
        Check(!t.links.At(t.row, 0) && retained && *retained == uri, "snapshot/borrowed target survives live erasure");
        t.links.CopyFrom(snapshot, resized, 0, snapshotRow, 0, 3, 3);
        Check(t.links.At(resized, 2) && !t.links.At(resized, 3), "resize clips link cells to retained columns");
        t.links.Reset();
        Check(!t.links.At(next, 0) && !t.links.At(resized, 0), "connection reset clears rows and active scope");
        t.links.Write(next, 0, 160);
        Check(!t.links.At(next, 0), "reset does not retain active link");
    }
    Terminal first, second;
    first.Feed(open + "A"); second.Feed("B");
    Check(first.links.At(first.row, 0) && !second.links.At(second.row, 0), "connection link isolation");
    {
        OscHyperlinks limited;
        char keys[OscHyperlinks::MaxStoredTargets + 1] = {};
        for (size_t i = 0; i < OscHyperlinks::MaxStoredTargets; ++i)
        {
            limited.Command("8;;https://example.com/" + std::to_string(i), true);
            limited.Write(keys + i, 0, 1);
        }
        limited.Command("8;;https://over-quota.example", true);
        limited.Write(keys + OscHyperlinks::MaxStoredTargets, 0, 1);
        Check(limited.At(keys + OscHyperlinks::MaxStoredTargets, 0)->empty(), "target count is bounded without auto-link fallback");
        limited.Clear(keys);
        limited.Command("8;;https://reclaimed.example", true);
        limited.Write(keys, 0, 1);
        Check(*limited.At(keys, 0) == "https://reclaimed.example", "erased cells return target budget");
    }
    {
        OscHyperlinks limited;
        char keys[512] = {};
        size_t stored = 0;
        for (; stored < sizeof(keys); ++stored)
        {
            limited.Command("8;;https://example.com/" + std::string(4000, 'x') + std::to_string(stored), true);
            limited.Write(keys + stored, 0, 1);
            if (limited.At(keys + stored, 0)->empty()) break;
        }
        Check(stored > 0 && stored < sizeof(keys), "retained URI bytes bounded below count limit");
        limited.Reset(); limited.Command("8;;https://after-reset.example", true); limited.Write(keys, 0, 1);
        Check(*limited.At(keys, 0) == "https://after-reset.example", "reset releases retained URL byte quota");
    }
    std::printf("PASS: %u OSC hyperlink checks (real parser, URI policy and row metadata)\n", checks);
}
