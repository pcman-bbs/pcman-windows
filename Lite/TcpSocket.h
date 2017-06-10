#pragma once

#include <memory>

#include <WinSock2.h>

#include "ConnIO.h"

class CTcpSocket : public CConnIO
{
public:
	CTcpSocket(
		sockaddr addr, int addr_len,
		std::shared_ptr<CConnEventDelegate> delegate);
	~CTcpSocket();

	bool Connect() override;
	int Send(const void* data, size_t length) override;
	void Shutdown() override;
	void Close() override;

	SOCKET GetSocket() const { return socket_; }
	void OnSocketMessage(WPARAM wparam, LPARAM lparam);

private:
	sockaddr addr_;
	int addr_len_;
	SOCKET socket_;
	std::shared_ptr<CConnEventDelegate> delegate_;
};