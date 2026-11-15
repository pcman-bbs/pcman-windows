#pragma once
#include "Sgr66.h"

// FUS and SendFreqStr use station bytes; native Windows controls use UTF-16.
namespace FrequentText
{
template<class Converter>
inline CStringW Decode(const CString& bytes, UINT cp, Converter& conv)
{
	CStringW result;
	if (bytes.IsEmpty()) return result;
	const int n = bytes.GetLength();
	wchar_t* out = result.GetBuffer(n);
	const int written = cp == 950 ? static_cast<int>(conv.Big52Ucs2(bytes, out, n))
		: MultiByteToWideChar(cp, 0, bytes, n, out, n);
	result.ReleaseBuffer(written);
	return result;
}

template<class Converter>
inline CStringW Display(CString bytes, bool ansi, UINT cp, Converter& conv)
{
	if (ansi)
	{
		bytes.Replace("^[[", "\x1b[");
		const std::string intact = Sgr66::ConvertSplitBig5(std::string(bytes, bytes.GetLength()), cp);
		bytes.SetString(intact.data(), static_cast<int>(intact.size()));
	}
	CStringW result = Decode(bytes, cp, conv);
	if (ansi) result.Replace(L"\x1b[", L"^[[");
	return result;
}

template<class Converter>
inline bool Encode(const CStringW& text, UINT cp, Converter& conv, CString& bytes)
{
	bytes.Empty();
	if (text.IsEmpty()) return true;
	if (cp == 950)
	{
		if (text.GetLength() >= INT_MAX / 2) return false;
		const int size = text.GetLength() * 2 + 1;
		char* out = bytes.GetBuffer(size);
		memset(out, 0, size);
		conv.Ucs22Big5(text, out);
		bytes.ReleaseBuffer();
	}
	else
	{
		const int n = WideCharToMultiByte(cp, 0, text, text.GetLength(), NULL, 0, NULL, NULL);
		if (!n) return false;
		const int written = WideCharToMultiByte(cp, 0, text, text.GetLength(), bytes.GetBuffer(n), n, NULL, NULL);
		bytes.ReleaseBuffer(written);
	}
	return Decode(bytes, cp, conv) == text;
}
}
