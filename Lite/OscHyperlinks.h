#pragma once
#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <vector>

// Per-connection OSC 8 targets, attached to terminal row identities and byte
// positions just like the existing text/colour model. Shared targets are freed
// when their last cell, active scope or presentation snapshot is discarded.
class OscHyperlinks
{
public:
    using Target = std::shared_ptr<const std::string>;
    static const size_t MaxStoredTargets = 4096;
    static const size_t MaxStoredBytes = 1024 * 1024;

    static std::string SafeUrl(const std::string& uri)
    {
        if (uri.size() > 4096) return {};
        std::string scheme = uri.substr(0, 8);
        for (char& c : scheme) if (c >= 'A' && c <= 'Z') c += 'a' - 'A';
        const size_t start = scheme.compare(0, 7, "http://") == 0 ? 7 :
            (scheme == "https://" ? 8 : 0);
        if (!start || uri.size() <= start) return {};
        const size_t end = uri.find_first_of("/?#", start);
        const std::string authority = uri.substr(start, end - start);
        if (authority.empty() || authority.find('@') != std::string::npos) return {};
        size_t hostEnd = authority.find(':');
        if (authority[0] == '[')
        {
            hostEnd = authority.find(']');
            if (hostEnd == std::string::npos || hostEnd == 1) return {};
            ++hostEnd;
            if (hostEnd < authority.size() && authority[hostEnd] != ':') return {};
        }
        if (hostEnd == 0) return {};
        if (hostEnd != std::string::npos && hostEnd < authority.size())
        {
            if (hostEnd + 1 == authority.size()) return {};
            unsigned int port = 0;
            for (size_t i = hostEnd + 1; i < authority.size(); ++i)
            {
                if (authority[i] < '0' || authority[i] > '9') return {};
                port = port * 10 + authority[i] - '0';
                if (port > 65535) return {};
            }
        }
        std::string safe;
        const char hex[] = "0123456789ABCDEF";
        for (size_t i = 0; i < uri.size(); ++i)
        {
            const unsigned char c = static_cast<unsigned char>(uri[i]);
            // Reject controls, shell/browser ambiguity, and malformed URI escapes.
            if (c <= 0x20 || c == 0x7f || c == '\\' || c == '"' || c == '<' || c == '>') return {};
            if (c == '%' && (i + 2 >= uri.size() || !Hex(uri[i+1]) || !Hex(uri[i+2]))) return {};
            if (c >= 0x80)
            {
                safe += '%'; safe += hex[c >> 4]; safe += hex[c & 15];
            }
            else safe += static_cast<char>(c);
        }
        return safe;
    }

    void Command(const std::string& payload, bool valid)
    {
        if (!valid) { active.reset(); return; }
        if (payload.compare(0, 2, "8;") != 0) return; // Unknown OSC cannot alter the current link.
        const size_t separator = payload.find(';', 2);
        if (separator == std::string::npos) { active.reset(); return; }
        const std::string uri = payload.substr(separator + 1);
        if (uri.empty()) active.reset();
        else active = MakeTarget(SafeUrl(uri));
        // An invalid nonempty target still owns its label: no auto-link fallback.
    }

    bool Write(const void* row, int column, int width)
    {
        if (column < 0 || column >= width) return false;
        const Target before = At(row, column);
        if (active || before)
        {
            auto& cells = rows[row];
            cells.resize(width);
            cells[column] = active;
            Prune(row);
        }
        return before != active;
    }

    Target At(const void* row, int column) const
    {
        const auto it = rows.find(row);
        return it != rows.end() && column >= 0 && static_cast<size_t>(column) < it->second.size()
            ? it->second[column] : Target();
    }

    bool Overlaps(const void* row, int column, int count) const
    {
        const auto it = rows.find(row);
        if (it == rows.end() || column < 0 || count <= 0 || static_cast<size_t>(column) >= it->second.size()) return false;
        const size_t end = (std::min)(it->second.size(), static_cast<size_t>(column) + count);
        for (size_t i = column; i < end; ++i) if (it->second[i]) return true;
        return false;
    }

    void Erase(const void* row, int column, int count)
    {
        auto it = rows.find(row);
        if (it == rows.end() || column < 0 || count <= 0 || static_cast<size_t>(column) >= it->second.size()) return;
        const size_t end = (std::min)(it->second.size(), static_cast<size_t>(column) + count);
        std::fill(it->second.begin() + column, it->second.begin() + end, Target());
        Prune(row);
    }

    void Copy(const void* destination, int to, const void* source, int from, int count, int width)
    {
        CopyFrom(*this, destination, to, source, from, count, width);
    }

    void CopyFrom(const OscHyperlinks& other, const void* destination, int to,
        const void* source, int from, int count, int width)
    {
        if (to < 0 || from < 0 || to >= width || from >= width || count <= 0) return;
        count = (std::min)(count, (std::min)(width - to, width - from));
        // Copy first, because text edits can overlap within the same row.
        std::vector<Target> copy;
        for (int i = 0; i < count; ++i) copy.push_back(other.At(source, from + i));
        if (!Overlaps(destination, to, count) &&
            std::none_of(copy.begin(), copy.end(), [](const Target& t) { return !!t; })) return;
        auto& cells = rows[destination];
        cells.resize(width);
        std::copy(copy.begin(), copy.end(), cells.begin() + to);
        Prune(destination);
    }

    void Clear(const void* row) { rows.erase(row); }
    void Reset() { rows.clear(); active.reset(); }

private:
    struct Budget { size_t count = 0, bytes = 0; };
    std::shared_ptr<Budget> budget = std::make_shared<Budget>();
    Target MakeTarget(const std::string& safe)
    {
        static const Target blocked = std::make_shared<const std::string>();
        if (safe.empty() || budget->count >= MaxStoredTargets ||
            safe.size() > MaxStoredBytes - budget->bytes) return blocked;
        const auto accounting = budget;
        const auto value = new std::string(safe);
        ++accounting->count;
        accounting->bytes += safe.size();
        return Target(value, [accounting](const std::string* target) {
            --accounting->count;
            accounting->bytes -= target->size();
            delete target;
        });
    }
    static bool Hex(char c)
    {
        return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
    }
    std::map<const void*, std::vector<Target>> rows;
    Target active;
    void Prune(const void* row)
    {
        const auto it = rows.find(row);
        if (it != rows.end() && std::none_of(it->second.begin(), it->second.end(),
            [](const Target& t) { return !!t; })) rows.erase(it);
    }
};
