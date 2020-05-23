#pragma once

#include <string>

class CConnIO {
public:
	virtual ~CConnIO() {}

	virtual bool Connect() = 0;
	virtual int Send(const void* data, size_t length) = 0;
	virtual void Shutdown() = 0;
	virtual void Close() = 0;

	virtual bool IsSecure() const { return false; }
	virtual std::string Description() const { return ""; }
};

class CConnEventDelegate {
public:
	CConnEventDelegate() {}
	virtual ~CConnEventDelegate() {}

	virtual void OnConnect(bool success) {}
	virtual void OnClose() {}
	virtual void OnReceive(const void* data, size_t length) {}
};