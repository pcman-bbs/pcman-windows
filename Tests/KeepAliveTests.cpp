#include <afx.h>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>
#include "../Lite/KeepAliveConfig.h"
#include "../Lite/AnsiSequenceParser.h"
#include "../Lite/OscHyperlinks.h"

static unsigned int checks = 0;
static void Check(bool passed, const char* description)
{
	++checks;
	if (!passed) { std::fprintf(stderr, "FAIL: %s\n", description); std::exit(1); }
}

struct FakeTransport
{
	std::string sent;
	int Send(const void* bytes, int count)
	{
		sent.append(static_cast<const char*>(bytes), count);
		return count;
	}
};

// Minimal UI-free host for methods extracted from TelnetConn.cpp at build
// time. OnIAC, ProcessData and Send are NOT copies maintained by this test.
class CTelnetConn
{
public:
	BYTE buffer[4096] = {};
	BYTE* buf = nullptr;
	BYTE* last_byte = nullptr;
	char ansi_param[64] = {};
	char* pansi_param = ansi_param;
	unsigned long idle_time = 0;
	bool is_ansi_editor = false;
	struct Settings { CString termtype = "VT100"; } site_settings;
	std::shared_ptr<FakeTransport> conn_io = std::make_shared<FakeTransport>();
	std::string text;
	std::vector<std::string> csi;
	std::vector<AnsiSequenceParser::Result> syncEvents;
	AnsiSequenceParser parser;
	OscHyperlinks osc_links;
	unsigned int naws = 0;
	void SendNaws() { ++naws; }
	void OnIAC();
	void ProcessData(int len);
	int Send(const void* bytes, int count);
	void OnText()
	{
		while (buf < last_byte)
		{
			if (*buf == IAC) { --buf; break; }
			const auto result = parser.Feed(*buf);
			if (result == AnsiSequenceParser::Osc) osc_links.Command(parser.OscSequence(), parser.OscValid());
			if (result == AnsiSequenceParser::Text)
			{
				osc_links.Write(this, static_cast<int>(text.size()), 4096);
				text += static_cast<char>(*buf);
			}
			if (result == AnsiSequenceParser::Dispatch) csi.emplace_back(parser.Sequence());
			if (result == AnsiSequenceParser::SyncBegin || result == AnsiSequenceParser::SyncEnd)
				syncEvents.push_back(result);
			++buf;
		}
	}
	void Feed(const std::string& input)
	{
		Check(input.size() <= sizeof(buffer), "test receive buffer limit");
		memcpy(buffer, input.data(), input.size());
		ProcessData(static_cast<int>(input.size()));
	}
};

#include "ProductionTelnetMethods.inc"

static std::string Bytes(std::initializer_list<unsigned char> bytes)
{
	return std::string(reinterpret_cast<const char*>(bytes.begin()), bytes.size());
}

int main()
{
	for (const char* host : { "ptt.cc", "ptt2.cc", "ws.ptt.cc", "ws.ptt2.cc", "PTT.CC", "ws.ptt.cc." })
	{
		Check(KeepAlive::UseTimingMark(KeepAlive::Auto, host), "PTT migration via auto");
		Check(!KeepAlive::UseTimingMark(KeepAlive::Custom, host), "explicit custom mode preserved");
	}
	for (const char* host : { "", "notptt.cc", "ptt.cc.evil.example", "example.com", "127.0.0.1", "ptt2.ccx" })
	{
		Check(!KeepAlive::UseTimingMark(KeepAlive::Auto, host), "non-PTT retains legacy string");
		Check(KeepAlive::UseTimingMark(KeepAlive::TimingMark, host), "explicit timing mode");
	}
	Check(KeepAlive::NormalizeMode(-1) == KeepAlive::Auto, "invalid mode default");
	Check(KeepAlive::NormalizeMode(999) == KeepAlive::Auto, "invalid large mode");
	for (unsigned long interval : { 0ul, 1ul, 29ul, 86401ul, 0xfffffffful })
		Check(KeepAlive::NormalizeInterval(interval) == 180, "invalid interval normalized");
	for (const char* input : { "", "0", "29", "-180", "+180", "180s", "180.0", "86401", "999999999999999999999999999" })
	{
		unsigned long value = 999;
		Check(!KeepAlive::ParseInterval(input, value) && value == 999, "strict invalid interval");
	}
	for (const char* input : { "30", "180", "300", "86400", "000180" })
	{
		unsigned long value = 0;
		Check(KeepAlive::ParseInterval(input, value), "valid interval");
	}
	unsigned long idle = 0;
	for (unsigned int cycle = 0; cycle < 3; ++cycle)
	{
		for (unsigned int second = 1; second < 180; ++second)
			Check(!KeepAlive::Tick(true, 180, idle), "not early");
		Check(KeepAlive::Tick(true, 180, idle) && idle == 0, "one keepalive per interval");
	}
	idle = 0xfffffffful;
	Check(KeepAlive::Tick(true, 180, idle) && idle == 0, "no counter overflow/burst");
	idle = 100;
	Check(!KeepAlive::Tick(false, 0, idle) && idle == 0, "disabled/zero safe");
	idle = 179;
	Check(KeepAlive::Tick(true, 0, idle), "runtime invalid interval fallback");

	DWORD interval = 180;
	int mode = KeepAlive::Auto;
	KeepAliveIntervalSetting intervalSetting(interval);
	KeepAliveModeSetting modeSetting(mode);
	CString value;
	for (const char* input : { "0", "-1", "", "999999999999999999999" })
	{
		CString source(input);
		intervalSetting.Load(source.GetBuffer());
		source.ReleaseBuffer();
		Check(interval == 180, "INI invalid interval fallback");
	}
	char validInterval[] = " 300 ";
	intervalSetting.Load(validInterval);
	intervalSetting.Save(value);
	Check(interval == 300 && value == "300", "INI interval roundtrip");
	for (int expected = 0; expected <= 2; ++expected)
	{
		CString source;
		source.Format("%d", expected);
		modeSetting.Load(source.GetBuffer());
		source.ReleaseBuffer();
		modeSetting.Save(value);
		Check(mode == expected && source == value, "INI mode roundtrip");
	}
	char invalidMode[] = "999999999999999999999";
	modeSetting.Load(invalidMode);
	Check(mode == KeepAlive::Auto, "INI invalid mode fallback");

	CTelnetConn sender;
	sender.idle_time = 100;
	sender.Send(KeepAlive::Request, sizeof(KeepAlive::Request));
	Check(sender.conn_io->sent == Bytes({ 255, 253, 6 }), "exact raw request, no IAC escaping");
	Check(sender.idle_time == 0, "send resets idle timer");
	sender.Send("", 0);
	Check(sender.conn_io->sent.size() == 3, "empty custom sends nothing");
	sender.idle_time = 100;
	sender.Send("x", 1);
	Check(sender.idle_time == 0, "normal key resets timer");

	for (unsigned char reply : { static_cast<unsigned char>(WILL), static_cast<unsigned char>(WONT) })
	{
		const auto packet = Bytes({ IAC, reply, TIMING_MARK });
		const std::string input = "BEFORE\x1b[3" + packet + "1mAFTER" + packet;
		for (size_t split = 0; split <= input.size(); ++split)
		{
			CTelnetConn conn;
			conn.Feed(input.substr(0, split));
			conn.Feed(input.substr(split));
			Check(conn.text == "BEFOREAFTER", "reply hidden, surrounding text intact");
			Check(conn.csi == std::vector<std::string>{ "[31m" }, "reply inside split CSI");
			Check(conn.conn_io->sent.empty(), "no DONT/reply loop");
		}
		CTelnetConn conn;
		for (char byte : input) conn.Feed(std::string(1, byte));
		Check(conn.text == "BEFOREAFTER" && conn.conn_io->sent.empty(), "bytewise reply");
	}
	CTelnetConn legacy;
	legacy.Feed(Bytes({ IAC, WILL, ECHO, IAC, WILL, SUPRESS_GO_AHEAD, IAC, WILL, 42, IAC, DO, NAWS }));
	Check(legacy.conn_io->sent == Bytes({ IAC, DO, ECHO, IAC, DO, SUPRESS_GO_AHEAD, IAC, DONT, 42, IAC, WILL, NAWS }), "other negotiations unchanged");
	Check(legacy.naws == 1 && legacy.text.empty(), "NAWS preserved");
	CTelnetConn a, b;
	a.Feed(Bytes({ IAC, WILL }));
	b.Feed("B");
	a.Feed(Bytes({ TIMING_MARK }) + "A");
	Check(a.text == "A" && b.text == "B", "per-connection state");
	const std::string synchronized = std::string("\x1b[?20") + Bytes({ IAC, WONT, TIMING_MARK }) +
		"26hNEW\x1b[?2026l";
	for (size_t split = 0; split <= synchronized.size(); ++split)
	{
		CTelnetConn conn;
		conn.Feed(synchronized.substr(0, split));
		conn.Feed(synchronized.substr(split));
		Check(conn.syncEvents == std::vector<AnsiSequenceParser::Result>{
			AnsiSequenceParser::SyncBegin, AnsiSequenceParser::SyncEnd }, "IAC inside split DEC2026");
		Check(conn.text == "NEW" && conn.conn_io->sent.empty(), "sync and keepalive do not leak or reply");
	}
	for (unsigned char reply : { static_cast<unsigned char>(WILL), static_cast<unsigned char>(WONT) })
	{
		// IAC negotiation can arrive inside an OSC payload or between ESC and
		// the backslash of ST. Neither may end OSC or leak protocol bytes.
		const auto packet = Bytes({ IAC, reply, TIMING_MARK });
		const std::string osc = std::string("\x1b[?2026hBEFORE\x1b]8;;https://example.com/") +
			packet + "\x1b[?2026l\x1b" + packet + "\\LABEL\x1b]8;;\x1b\\AFTER\x1b[?2026l";
		for (size_t split = 0; split <= osc.size(); ++split)
		{
			CTelnetConn conn;
			conn.Feed(osc.substr(0, split));
			conn.Feed(osc.substr(split));
			Check(conn.text == "BEFORELABELAFTER" && conn.csi.empty(), "OSC and IAC payload never reaches text/CSI");
			Check(conn.syncEvents == std::vector<AnsiSequenceParser::Result>{
				AnsiSequenceParser::SyncBegin, AnsiSequenceParser::SyncEnd }, "OSC cannot prematurely end sync");
			Check(conn.conn_io->sent.empty(), "no OSC or timing-mark response");
		}
		CTelnetConn conn;
		for (char byte : osc) conn.Feed(std::string(1, byte));
		Check(conn.text == "BEFORELABELAFTER" && conn.csi.empty() && conn.conn_io->sent.empty(), "OSC and IAC bytewise receive");
		Check(conn.syncEvents == std::vector<AnsiSequenceParser::Result>{
			AnsiSequenceParser::SyncBegin, AnsiSequenceParser::SyncEnd }, "OSC and IAC bytewise sync isolation");
	}
	for (unsigned char reply : { static_cast<unsigned char>(WILL), static_cast<unsigned char>(WONT) })
	{
		const std::string input = std::string("\x1b]8;;https://example.com/\x1b") +
			Bytes({ IAC, reply, TIMING_MARK }) + "\\LABEL\x1b]8;;\x1b\\END";
		for (size_t split = 0; split <= input.size(); ++split)
		{
			CTelnetConn conn;
			conn.Feed(input.substr(0, split)); conn.Feed(input.substr(split));
			Check(conn.osc_links.At(&conn, 0) && *conn.osc_links.At(&conn, 0) == "https://example.com/" &&
				!conn.osc_links.At(&conn, 5), "OSC target survives IAC inside ST, ends before unlinked suffix");
			Check(conn.text == "LABELEND" && conn.conn_io->sent.empty(), "no URL or protocol bytes displayed/replied");
		}
	}
	std::printf("PASS: %u KeepAlive checks (real receive/negotiation/send methods)\n", checks);
	return 0;
}
