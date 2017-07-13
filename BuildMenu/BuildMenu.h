// BuildMenu.h : main header file for the BUILDMENU application
//

#if !defined(AFX_BUILDMENU_H__9A10E364_EDCD_40CB_B3F1_4708EF5E5F33__INCLUDED_)
#define AFX_BUILDMENU_H__9A10E364_EDCD_40CB_B3F1_4708EF5E5F33__INCLUDED_

#include <memory>
#include <vector>

#include "..\Lite\StdAfx.h"

class CBuffer {
public:
	void Write(const void *buf, size_t len)
	{
		size_t currlen = buf_.size();
		buf_.resize(currlen + len);
		memcpy(&buf_.at(currlen), buf, len);
	}

	size_t Read(void *buf, size_t len)
	{
		size_t n = min(len, buf_.size() - offset);
		if (n)
		{
			memcpy(buf, &buf_.at(offset), len);
			offset += n;
		}
		return n;
	}

	template <class F>
	size_t ReadFrom(F& f)
	{
		const size_t kBatchSize = 8192;
		size_t i = buf_.size(), n = 0;
		do {
			buf_.resize(i + kBatchSize);
			i += (n = f.Read(&buf_.at(i), kBatchSize));
		} while (n == kBatchSize);
		buf_.resize(i);
		return i;
	}

	size_t GetLength() const { return buf_.size(); }

private:
	std::vector<BYTE> buf_;
	size_t offset = 0;
};

std::unique_ptr<CBuffer> GetUIBuffer();

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BUILDMENU_H__9A10E364_EDCD_40CB_B3F1_4708EF5E5F33__INCLUDED_)
