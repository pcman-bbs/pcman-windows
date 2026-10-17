#include "../Lite/AnsiSequenceParser.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

static unsigned int checks = 0;
static void Check(bool success, const char* label)
{
	++checks;
	if (!success)
	{
		std::fprintf(stderr, "FAIL: %s (check %u)\n", label, checks);
		std::exit(1);
	}
}

struct Capture
{
	AnsiSequenceParser parser;
	std::string text;
	std::vector<std::string> commands;
	std::vector<AnsiSequenceParser::Result> syncEvents;
	void Feed(const std::string& bytes)
	{
		for (unsigned char byte : bytes)
		{
			const auto result = parser.Feed(byte);
			if (result == AnsiSequenceParser::Text)
				text += static_cast<char>(byte);
			else if (result == AnsiSequenceParser::Dispatch)
			{
				Check(parser.Length() < 64, "bounded command");
				Check(parser.Sequence()[parser.Length()] == 0, "terminated command");
				commands.emplace_back(parser.Sequence(), parser.Length());
			}
			else if (result == AnsiSequenceParser::SyncBegin || result == AnsiSequenceParser::SyncEnd)
				syncEvents.push_back(result);
		}
	}
};

// Run every possible two-packet split, plus one byte per packet. The parser
// instance must persist exactly as it does between OnText/ProcessData calls.
static void Expect(const std::string& input, const std::string& text,
	const std::vector<std::string>& commands, const char* label,
	const std::vector<AnsiSequenceParser::Result>& syncEvents = {})
{
	for (size_t split = 0; split <= input.size(); ++split)
	{
		Capture capture;
		capture.Feed(input.substr(0, split));
		capture.Feed(input.substr(split));
		Check(capture.text == text, label);
		Check(capture.commands == commands && capture.syncEvents == syncEvents, label);
	}
	Capture capture;
	for (char byte : input)
		capture.Feed(std::string(1, byte));
	Check(capture.text == text, label);
	Check(capture.commands == commands && capture.syncEvents == syncEvents, label);
}

static void OscStrings()
{
	const std::string osc = "\x1b]", st = "\x1b\\";
	const std::string open = osc + "8;id=example;https://example.com/path?a=1&b=2" + st;
	const std::string close = osc + "8;;" + st;
	Expect("BEFORE" + open + "LABEL" + close + "AFTER", "BEFORELABELAFTER", {}, "OSC 8 label only");
	Expect(open + "\x1b[31mRED" + close + "\x1b[m", "RED", { "[31m", "[m" }, "label SGR preserved");
	Expect(osc + "8;;https://example.com\aLABEL" + osc + "8;;\aEND", "LABELEND", {}, "BEL ends OSC silently");
	Expect(osc + "0;window title" + st + "OK" + osc + "52;clipboard payload\a!", "OK!", {}, "unknown OSC ignored");
	Expect(osc + st + "OK", "OK", {}, "empty OSC");
	Expect(open + close + "OK", "OK", {}, "empty link label");
	const std::string encoded = "\xa4\xa4\xa4\xe5\xe6\x97\xa5\xe6\x9c\xac";
	Expect(osc + "8;;https://example.com/" + encoded + st + encoded + close,
		encoded, {}, "encoded URL hidden and Big5/UTF8 label bytes intact");
	Expect(osc + "0;\x1b[31m\x1b[2J\x1b[?2026h\x1b[?2026l\x1b]nested" + st +
		"OK\x1b[32m!", "OK!", { "[32m" }, "OSC payload cannot execute CSI or sync events");
	std::string bytes;
	for (unsigned int byte = 0; byte <= 255; ++byte)
		if (byte != 7 && byte != 0x18 && byte != 0x1a)
			bytes += static_cast<char>(byte);
	Expect(osc + bytes + st + "OK", "OK", {}, "OSC controls/high bytes are not executed or leaked");
	Expect(osc + "0;\x9cnot ST" + st + "OK", "OK", {}, "8-bit ST is payload, not an encoded-text control");
	Expect(osc + "0;payload\x1b\x1b\\OK", "OK", {}, "repeated ESC before ST");
	Expect(osc + "0;payload\x1b\aOK", "OK", {}, "BEL after pending ESC");
	for (char cancel : { '\x18', '\x1a' })
	{
		Expect(osc + "0;payload" + cancel + "OK\x1b[32m!", "OK!", { "[32m" }, "OSC cancelled");
		Expect(osc + "0;payload\x1b" + cancel + "OK", "OK", {}, "OSC cancelled after pending ESC");
	}
	Expect(osc + "8;;unterminated\r\n\t\b\x1b[31mHIDDEN", "", {}, "unfinished OSC stays hidden");
	Expect(osc + "0;" + std::string(200, 'x') + st + "OK", "OK", {}, "OSC beyond CSI buffer size");
	Capture large;
	large.Feed(osc + "8;;");
	for (unsigned int chunk = 0; chunk < 256; ++chunk)
		large.Feed(std::string(4096, 'x'));
	large.Feed("\x1b[?2026h\x1b[31m");
	Check(large.text.empty() && large.commands.empty() && large.syncEvents.empty(), "1 MiB OSC remains ignored");
	large.Feed(st + "OK\x1b[32m!");
	Check(large.text == "OK!" && large.commands == std::vector<std::string>{ "[32m" }, "large OSC terminates and recovers");
	Capture first, second;
	first.Feed(osc + "8;;partial\x1b");
	second.Feed("OTHER\x1b[31m");
	first.Feed("\\LABEL" + close);
	Check(first.text == "LABEL" && first.commands.empty(), "OSC state persists within its connection");
	Check(second.text == "OTHER" && second.commands == std::vector<std::string>{ "[31m" }, "OSC connection isolation");
	for (const std::string& pending : { osc + "8;;partial", osc + "8;;partial\x1b", std::string("\x1b[31") })
	{
		Capture reset;
		reset.Feed(pending);
		reset.parser.Reset(); // Used by the production connection lifecycle.
		reset.Feed("NEW\x1b[32mSESSION");
		Check(reset.text == "NEWSESSION" && reset.commands == std::vector<std::string>{ "[32m" },
			"connection reset discards unfinished OSC/CSI");
	}
}

int main()
{
	OscStrings();
	Expect("plain\r\ntext", "plain\r\ntext", {}, "plain text");
	Expect("\x1b[0;1;37;44mTITLE\x1b[0;30;47mRIGHT\x1b[m", "TITLERIGHT",
		{ "[0;1;37;44m", "[0;30;47m", "[m" }, "PTT header colors");
	const std::string finals = "KHfrJELABCDsu@MPZhln";
	for (char finalByte : finals)
	{
		const std::string command = std::string("[2") + finalByte;
		Expect("\x1b" + command + "OK", "OK", { command }, "existing CSI family");
	}
	Expect("\x1b[12;34H\x1b[;H\x1b[H", "", { "[12;34H", "[;H", "[H" }, "positions/defaults");
	Expect("\x1b" "7\x1b" "8\x1b" "D\x1b" "M\x1b" "E", "",
		{ "7", "8", "D", "M", "E" }, "legacy ESC commands");
	Expect("\x1b[?2026hHELLO\x1b[?2026l", "HELLO", {}, "DEC2026 never enters legacy dispatcher",
		{ AnsiSequenceParser::SyncBegin, AnsiSequenceParser::SyncEnd });
	for (const char* mode : { "1000", "1002", "1003", "1006" })
		Expect(std::string("\x1b[?") + mode + "hX\x1b[?" + mode + "l", "X", {}, "mouse ignored");
	Expect("\x1b[6nOK", "OK", { "[6n" }, "CPR reaches existing no-op handler");
	Expect("\x1b[31mRED\x1b[>4;2mSTILL RED", "REDSTILL RED", { "[31m" }, "private m cannot reset color");
	for (char prefix : std::string("<=>?"))
		for (char finalByte = '@'; finalByte <= '~'; ++finalByte)
			Expect(std::string("\x1b[") + prefix + "1;2" + finalByte + "OK", "OK", {}, "all private finals ignored");
	for (char intermediate = 0x20; intermediate <= 0x2f; ++intermediate)
		Expect(std::string("\x1b[1") + intermediate + "AOK", "OK", {}, "intermediate cannot move cursor");
	Expect("\x1b[1 2AOK", "OK", {}, "invalid parameter order");
	Expect("\x1b[38:2:255:0:0mOK", "OK", {}, "unsupported colon subparameters");
	Expect("\x1b[1qOK", "OK", {}, "unknown final");
	Expect("\x1b(BO\x1b MK", "OK", {}, "ESC intermediates ignored");
	const std::string largest = "[" + std::string(61, '0') + "m";
	Expect("\x1b" + largest, "", { largest }, "exact buffer boundary");
	Expect("\x1b[" + std::string(62, '0') + "mOK\x1b[32m", "OK", { "[32m" }, "overflow recovery");
	Expect("\x1b[" + std::string(200, ';') + "HOK", "OK", {}, "long sequence ignored");
	Expect("\x1b[32767C", "", { "[32767C" }, "numeric boundary");
	Expect("\x1b[32768COK\x1b[999999999999999999m!", "OK!", {}, "numeric overflow");
	Expect("\x1b[31\x1b[32mOK", "OK", { "[32m" }, "ESC restart");
	Expect("\x1b\x1b[32mOK", "OK", { "[32m" }, "repeated ESC");
	Expect("\x1b[31\x18OK\x1b[32\x1a!", "OK!", {}, "CAN/SUB cancellation");
	Expect("\x1b[3\r\n\t\b\a1mX", "\r\n\t\b\aX", { "[31m" }, "C0 during CSI");
	Expect(std::string("\x1b[3\0", 4) + "1mX", std::string(1, '\0') + "X", { "[31m" }, "NUL during CSI");
	Expect("\x1b[3\x7f" "1mX", "X", { "[31m" }, "DEL during CSI");
	const std::string encoded = "\xa4\xa4\xa4\xe5\xef\xbe\x9b";
	Expect(encoded, encoded, {}, "Big5/UTF8 bytes remain text");
	Expect("\x1b[31" + encoded + "OK", encoded + "OK", {}, "invalid high-byte recovery");
	Expect("\x1b[31", "", {}, "unfinished sequence does not dispatch");
	Capture first, second;
	first.Feed("\x1b[3");
	second.Feed("\x1b[32mB");
	first.Feed("1mA");
	Check(first.commands == std::vector<std::string>{ "[31m" } && first.text == "A", "connection isolation A");
	Check(second.commands == std::vector<std::string>{ "[32m" } && second.text == "B", "connection isolation B");
	std::printf("PASS: %u checks (all split points and bytewise input)\n", checks);
	return 0;
}
