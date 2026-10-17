#pragma once

#include <cstddef>
#include <string>
#include "AnsiSequenceParser.h"

// PTT's SGR 66 captures the current attribute for the next character's
// left half. Subsequent SGR parameters still change the ordinary attribute.
namespace Sgr66
{
    enum Format { Auto = 0, Legacy = 1, Compatible = 2 };
    inline int NormalizeFormat(int format)
    {
        return format >= Auto && format <= Compatible ? format : Auto;
    }
    inline bool Enabled(int format, bool ptt, bool editor)
    {
        format = NormalizeFormat(format);
        return format == Compatible || (format == Auto && (ptt || editor));
    }

    class PendingAttribute
    {
        bool pending = false;
        unsigned char left = 7;
    public:
        void Capture(unsigned char attr) { left = attr; pending = true; }
        void Reset() { pending = false; }
        unsigned char Take(unsigned char current)
        {
            const unsigned char result = pending ? left : current;
            pending = false;
            return result;
        }
    };

    inline bool Lead(unsigned char c) { return c >= 0x81 && c <= 0xfe; }
    inline bool Trail(unsigned char c)
    {
        return (c >= 0x40 && c <= 0x7e) || (c >= 0xa1 && c <= 0xfe);
    }
    inline std::size_t CharacterLength(const char* text, std::size_t count, unsigned int cp)
    {
        if (!count) return 0;
        const unsigned char first = static_cast<unsigned char>(text[0]);
        if (cp == 950)
            return count >= 2 && Lead(first) && Trail(static_cast<unsigned char>(text[1])) ? 2 : 1;
        if (cp != 65001 || first < 0x80) return 1;
        const std::size_t size = first >= 0xc2 && first <= 0xdf ? 2 :
            first >= 0xe0 && first <= 0xef ? 3 : first >= 0xf0 && first <= 0xf4 ? 4 : 1;
        if (size > count || size == 1) return 1;
        for (std::size_t i = 1; i < size; ++i)
            if ((static_cast<unsigned char>(text[i]) & 0xc0) != 0x80) return 1;
        const unsigned char second = static_cast<unsigned char>(text[1]);
        if ((first == 0xe0 && second < 0xa0) || (first == 0xed && second >= 0xa0) ||
            (first == 0xf0 && second < 0x90) || (first == 0xf4 && second >= 0x90)) return 1;
        return size;
    }

    inline std::string Parameters(unsigned char attr)
    {
        std::string result = "0;";
        if (attr & 8) result += "1;";
        if (attr & 128) result += "5;";
        result += "3"; result += static_cast<char>('0' + (attr & 7));
        result += ";4"; result += static_cast<char>('0' + ((attr >> 4) & 7));
        return result;
    }

    // Bounds are byte offsets into the complete row. Incomplete selected
    // characters are omitted rather than exporting an unselected half.
    inline std::string EncodeRange(const char* text, const unsigned char* attrs,
        int width, int begin, int end, unsigned int cp)
    {
        if (!text || !attrs || width <= 0) return std::string();
        if (begin < 0) begin = 0;
        if (end > width) end = width;
        if (begin >= end) return "\x1b[m";
        int alignedBegin = begin, alignedEnd = end;
        for (int i = 0; i < width; )
        {
            const int next = i + static_cast<int>(CharacterLength(text + i, width - i, cp));
            if (begin > i && begin < next) alignedBegin = next;
            if (end > i && end < next) alignedEnd = i;
            i = next;
        }
        begin = alignedBegin; end = alignedEnd;
        while (end > begin && text[end - 1] == ' ' && (attrs[end - 1] & 0x70) == 0) --end;
        std::string result = "\x1b[m";
        unsigned char current = 7;
        for (int i = begin; i < end; )
        {
            const int size = static_cast<int>(CharacterLength(text + i, end - i, cp));
            const unsigned char left = attrs[i];
            const unsigned char right = cp == 950 && size == 2 ? attrs[i + 1] : left;
            if (left != right)
            {
                result += "\x1b[" + Parameters(left) + ";66;" + Parameters(right) + "m";
                current = right;
            }
            else if (current != left)
            {
                result += "\x1b[" + Parameters(left) + "m";
                current = left;
            }
            // Retain the existing omission of buffer NULs, not a byte stream
            // terminator; callers supply the actual row width.
            for (int j = 0; j < size; ++j) if (text[i + j]) result += text[i + j];
            i += size;
        }
        if (current != 7) result += "\x1b[m";
        return result;
    }

    inline std::size_t SgrLength(const std::string& text, std::size_t start)
    {
        if (start + 2 >= text.size() || text[start] != '\x1b' || text[start + 1] != '[') return 0;
        AnsiSequenceParser parser;
        for (std::size_t i = start; i < text.size() && i - start < 65; ++i)
        {
            const auto parsed = parser.Feed(static_cast<unsigned char>(text[i]));
            if (i >= start + 2 && text[i] >= 0x40 && text[i] <= 0x7e)
                return text[i] == 'm' && parsed == AnsiSequenceParser::Dispatch ? i - start + 1 : 0;
            if (i >= start + 2 && text[i] != ';' && (text[i] < '0' || text[i] > '9')) return 0;
        }
        return 0;
    }

    // Never inspect hidden OSC/DCS/PM/APC/SOS payload as Big5 picture data.
    inline std::size_t ControlEnd(const std::string& text, std::size_t start)
    {
        if (start + 1 >= text.size()) return text.size();
        const char type = text[start + 1];
        if (type == ']' || type == 'P' || type == '^' || type == '_' || type == 'X')
        {
            for (std::size_t i = start + 2; i < text.size(); ++i)
            {
                if ((type == ']' && text[i] == '\x07') || text[i] == '\x18' || text[i] == '\x1a') return i + 1;
                if (text[i] == '\x1b' && i + 1 < text.size() && text[i + 1] == '\\') return i + 2;
            }
            return text.size();
        }
        if (type == '[')
        {
            for (std::size_t i = start + 2; i < text.size(); ++i)
                if (text[i] >= 0x40 && text[i] <= 0x7e) return i + 1;
            return text.size();
        }
        return start + 2;
    }

    // Hoist only complete plain numeric SGRs from between a valid Big5
    // pair. Keep every other control sequence and all payload bytes intact.
    inline std::string ConvertSplitBig5(const std::string& text, unsigned int cp)
    {
        if (cp != 950) return text;
        std::string result;
        result.reserve(text.size());
        for (std::size_t i = 0; i < text.size(); )
        {
            if (text[i] == '\x1b')
            {
                const std::size_t end = ControlEnd(text, i);
                result.append(text, i, end - i); i = end; continue;
            }
            if (Lead(static_cast<unsigned char>(text[i])))
            {
                std::size_t j = i + 1;
                std::string params;
                while (const std::size_t size = SgrLength(text, j))
                {
                    params += ';'; params.append(text, j + 2, size - 3); j += size;
                }
                if (j > i + 1 && j < text.size() && Trail(static_cast<unsigned char>(text[j])))
                {
                    if (params.size() <= 58) result += "\x1b[66" + params + "m";
                    else { result += "\x1b[66m"; result.append(text, i + 1, j - i - 1); }
                    result += text[i]; result += text[j]; i = j + 1; continue;
                }
                // An intact pair can contain an ASCII-range trail byte.
                if (j == i + 1 && j < text.size() && Trail(static_cast<unsigned char>(text[j])))
                { result.append(text, i, 2); i += 2; continue; }
            }
            result += text[i++];
        }
        return result;
    }
}
