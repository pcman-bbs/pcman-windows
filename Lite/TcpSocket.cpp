#include "TcpSocket.h"

CTcpSocket::CTcpSocket(
	sockaddr addr, int addr_len,
	std::shared_ptr<CConnEventDelegate> delegate)
	: addr_(addr)
	, addr_len_(addr_len)
	, socket_(socket(AF_INET, SOCK_STREAM, 0))
	, delegate_(delegate)
{
}

CTcpSocket::~CTcpSocket()
{
}

bool CTcpSocket::Connect()
{
	::connect(socket_, &addr_, addr_len_);
	return true;
}

int CTcpSocket::Send(const void* data, size_t length)
{
	return ::send(socket_, (const char*)data, length, 0);
}

void CTcpSocket::Shutdown()
{
	::shutdown(socket_, SD_SEND);
}

void CTcpSocket::Close()
{
	::closesocket(socket_);
}

void CTcpSocket::OnSocketMessage(WPARAM wparam, LPARAM lparam)
{
	WORD err = HIWORD(lparam);

	switch (LOWORD(lparam))
	{
	case FD_READ:
	{
		char buffer[4095];
		int len = ::recv(socket_, (char*)buffer, sizeof(buffer), 0);
		if (len <= 0)
			return;
		delegate_->OnReceive(buffer, len);
		break;
	}

	case FD_CONNECT:
		delegate_->OnConnect(err == 0);
		break;

	case FD_CLOSE:
		delegate_->OnClose();
	}
}