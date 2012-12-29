#include "TelnetSSHConnSurrogate.h"
#include <cassert>

#include "TelnetSSHState.h"

TelnetSSHConnSurrogate::TelnetSSHConnSurrogate(void) :
    m_addrlen(0)
{
    m_isSSH = true;
}


TelnetSSHConnSurrogate::~TelnetSSHConnSurrogate(void)
{
}


void TelnetSSHConnSurrogate::OnReceive( int nErrorCode )
{
    CTelnetConn::OnReceive(nErrorCode);

    m_self->OnReceive(nErrorCode);
}


void TelnetSSHConnSurrogate::OnConnect( int nErrorCode )
{
    CTelnetConn::OnConnect(nErrorCode);
    m_self->OnConnect(nErrorCode);
}


void TelnetSSHConnSurrogate::ConnectImpl( sockaddr *addr, int len )
{
    std::unique_ptr<TelnetSSHLoginInfoLoadState> self(
        new TelnetSSHLoginInfoLoadState(
        this->telnet,
        std::bind(&TelnetSSHConnSurrogate::NextState, this),
        std::bind(&TelnetSSHConnSurrogate::ReceiveData, this),
        std::bind(&TelnetSSHConnSurrogate::SetDisconnect, this)
        ));
    //std::unique_ptr<TelnetSSHConnectState> self(
    //    new TelnetSSHConnectState(
    //    this->telnet, "bbs", "",
    //    std::bind(&TelnetSSHConnSurrogate::NextState, this),
    //    std::bind(&TelnetSSHConnSurrogate::SetDisconnect, this)
    //    ));
    m_self = std::move(self);

    m_addr = *addr;
    m_addrlen = len;
    m_self->Connect(&m_addr, m_addrlen);
}


int TelnetSSHConnSurrogate::RecvImpl( void *buf, int len )
{
    return m_self->Recv(buf, len);
}


int TelnetSSHConnSurrogate::SendImpl( const void *buf, int len )
{
    return m_self->Send(buf, len);
}


int TelnetSSHConnSurrogate::CloseImpl()
{
    int result = CTelnetConn::CloseImpl();
    m_self->Close();
    return result;
}


void TelnetSSHConnSurrogate::NextState()
{
    m_self = m_self->Next();
}

void TelnetSSHConnSurrogate::SetDisconnect()
{
    this->is_connected = false;
    this->is_disconnected = true;
}
