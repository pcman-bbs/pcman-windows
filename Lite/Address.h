#pragma once

#include <cassert>

#include "stdafx.h"

class CAddress
{
public:
	CAddress();
	explicit CAddress(const CString& url);
	~CAddress();

	bool Parse(const CString& url);

	bool IsValid() const { return valid_; }
	const CString& URL() const { return url_; }
	const CString& Protocol() const { assert(valid_); return protocol_; }
	const CString& Server() const { assert(valid_); return server_; }
	unsigned short Port() const {
		assert(valid_);
		return port_ ? port_ : DefaultPort();
	}
	unsigned short DefaultPort() const;
	const CString& Path() const { assert(valid_); return path_; }

	CString Description() const;

private:
	bool valid_;
	CString url_;
	CString protocol_;
	CString server_;
	unsigned short port_;
	CString path_;
};