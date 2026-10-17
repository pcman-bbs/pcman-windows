#pragma once
#include <string>

// Streaming parser for 7-bit ESC/CSI sequences and bounded OSC commands.
// No platform or UI dependencies. OSC payload is never executed as CSI.
// Outside OSC, bytes >= 0x80 remain text (Big5/UAO and UTF-8 are not C1 controls).
// OSC interpretation belongs to the consumer; DCS remains outside this scope.
class AnsiSequenceParser
{
public:
	enum Result { Text, Consumed, Dispatch, SyncBegin, SyncEnd, Osc, AutoWrapOn, AutoWrapOff };
	static const unsigned int MaxOscLength = 4096;
	AnsiSequenceParser() { Reset(); }

	void Reset()
	{
		state = Ground;
		length = 0;
		supported = true;
		number = 0;
		overflow = false;
		sequence[0] = 0;
		osc.clear();
		oscValid = true;
	}

	Result Feed(unsigned char byte)
	{
		// Handle OSC before generic ESC/C0/high-byte processing. Payload may
		// contain any of them, but must not draw, ring BEL or dispatch CSI.
		// Keep discarding oversized/unfinished strings through their terminator.
		if (state == OscString || state == OscEscape)
		{
			if (byte == 0x07 || byte == 0x18 || byte == 0x1a ||
				(state == OscEscape && byte == '\\'))
			{
				if (byte == 0x18 || byte == 0x1a) oscValid = false;
				state = Ground;
				return Osc; // Only the completed command is exposed to the consumer.
			}
			if (state == OscEscape) AppendOsc(0x1b);
			if (byte != 0x1b) AppendOsc(byte);
			state = byte == 0x1b ? OscEscape : OscString;
			return Consumed;
		}
		if (byte == 0x1b)
		{
			state = Escape;
			length = 0;
			supported = true;
			number = 0;
			overflow = false;
			return Consumed;
		}
		if (byte == 0x18 || byte == 0x1a) // CAN / SUB cancel a sequence.
		{
			state = Ground;
			return Consumed;
		}
		if (state == Ground)
			return Text;
		if (byte == 0x7f)
			return Consumed;
		if (byte < 0x20) // Execute C0 without losing the pending sequence.
			return Text;
		if (byte >= 0x80)
		{
			state = Ground; // Invalid in a 7-bit sequence; preserve text bytes.
			return Text;
		}

		if (state == Escape && byte == ']')
		{
			osc.clear();
			oscValid = true;
			state = OscString;
			return Consumed;
		}
		if (state == Escape && byte == '[')
		{
			Append(byte);
			state = Parameters;
			return Consumed;
		}
		if (state == Escape || state == EscapeIntermediate)
		{
			Append(byte);
			if (byte >= 0x20 && byte <= 0x2f)
			{
				state = EscapeIntermediate;
				supported = false;
				return Consumed;
			}
			return Finish();
		}

		// CSI: parameter bytes 0x30..0x3f, intermediates 0x20..0x2f,
		// then exactly one final byte 0x40..0x7e. Unknown/oversized
		// sequences are consumed through the final, never partly executed.
		Append(byte);
		if (byte >= 0x40 && byte <= 0x7e)
			return Finish();
		if (byte >= 0x20 && byte <= 0x2f)
		{
			state = Intermediate;
			supported = false;
		}
		else if (state != Parameters || (byte != ';' && (byte < '0' || byte > '9')))
			supported = false; // Private prefixes, subparameters or invalid order.
		else if (byte == ';')
			number = 0;
		else if (supported)
		{
			number = number * 10 + byte - '0';
			// Bound input before legacy atoi and coordinate/count arithmetic.
			if (number > 32767)
				supported = false;
		}
		return Consumed;
	}

	// Valid only when Feed returns Dispatch, and until the next Feed call.
	const char* Sequence() const { return sequence; }
	unsigned int Length() const { return length; }
	// Valid after Osc until the next OSC starts or Reset. Invalid commands
	// (overflow/cancellation) must never be interpreted as a truncated URL.
	const std::string& OscSequence() const { return osc; }
	bool OscValid() const { return oscValid; }

private:
	enum State { Ground, Escape, EscapeIntermediate, Parameters, Intermediate, OscString, OscEscape };
	State state;
	char sequence[64]; // Includes the '[' for CSI and a trailing NUL.
	unsigned int length;
	bool supported;
	unsigned int number;
	bool overflow;
	std::string osc;
	bool oscValid;

	void AppendOsc(unsigned char byte)
	{
		if (osc.size() < MaxOscLength) osc += static_cast<char>(byte);
		else oscValid = false;
	}

	void Append(unsigned char byte)
	{
		if (length < sizeof(sequence) - 1)
			sequence[length++] = static_cast<char>(byte);
		else
		{
			supported = false;
			overflow = true;
		}
	}

	Result Finish()
	{
		state = Ground;
		sequence[length] = 0;
		// Recognized private modes bypass the legacy numeric dispatcher.
		// Validate the entire scalar parameter, including leading zeros; lists,
		// intermediates, subparameters and oversized input remain ignored.
		if (!overflow && length >= 4 && sequence[0] == '[' && sequence[1] == '?' &&
			(sequence[length - 1] == 'h' || sequence[length - 1] == 'l'))
		{
			unsigned int mode = 0;
			for (unsigned int i = 2; i < length - 1; ++i)
			{
				if (sequence[i] < '0' || sequence[i] > '9') return Consumed;
				mode = mode * 10 + sequence[i] - '0';
				if (mode > 32767) return Consumed;
			}
			const bool enabled = sequence[length - 1] == 'h';
			if (mode == 7) return enabled ? AutoWrapOn : AutoWrapOff;
			if (mode == 2026) return enabled ? SyncBegin : SyncEnd;
		}
		if (!supported)
			return Consumed;
		// Only the existing command family reaches the legacy dispatcher.
		const char finalByte = sequence[length - 1];
		const char* commands = sequence[0] == '[' ? "mKHfrJELABCDsu@MPZhln" : "DME78";
		for (const char* command = commands; *command; ++command)
			if (*command == finalByte)
				return Dispatch;
		return Consumed;
	}
};
