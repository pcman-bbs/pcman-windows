#include "TelnetSSHConnSurrogate.h"

#include <libssh2.h>

#include <iterator>
#include <string>
#include <sstream>
#include <cassert>

#include "TelnetSSHState.h"

#undef min

TelnetSSHState::TelnetSSHState( void )
{

}


TelnetSSHState::~TelnetSSHState( void )
{

}


namespace
{


const std::string gLoginUsernameMessage = "SSH LoginUsername: ";
const std::string gLoginPasswordMessage = "SSH LoginPassword: ";


void SetCourseCommand(unsigned column, unsigned row, std::string &buffer)
{
    //<ESC>[{ROW};{COLUMN}H
    std::stringstream conv;
    conv << '\x1b' << '[' << column << ';' << row << 'H';
    buffer += conv.str();
}


} //namespace


TelnetSSHLoginInfoLoadState::TelnetSSHLoginInfoLoadState( 
    SOCKET socket,
    const fnTypeNextState &fnNextState,
    const fnTypeNotifyReceive &fnNotifyReceive,
    const fnTypeSetDisconnect &fnSetDisconnect) : 

    m_target(&m_username)
{
    this->SetSocket(socket);
    this->SetFnNextState(fnNextState);
    this->SetFnNotifyReceive(fnNotifyReceive);
    this->SetFnSetDisconnect(fnSetDisconnect);

    SetCourseCommand(1, 1, m_textShowOnSceen);
    m_textShowOnSceen += gLoginUsernameMessage;
    assert(m_target);
}


TelnetSSHLoginInfoLoadState::TelnetSSHLoginInfoLoadState( 
    const TelnetSSHConnectState &conn )
{
    this->SetSocket(conn.GetSocket());
    this->SetFnNextState(conn.GetFnNextState());
    this->SetFnNotifyReceive(conn.GetFnNotifyReceive());
    this->SetFnSetDisconnect(conn.GetFnSetDisconnect());
}


TelnetSSHLoginInfoLoadState::~TelnetSSHLoginInfoLoadState( void )
{
}


void TelnetSSHLoginInfoLoadState::OnReceive( int /*nErrorCode*/ )
{
}


void TelnetSSHLoginInfoLoadState::OnConnect( int /*nErrorCode*/ )
{
}


void TelnetSSHLoginInfoLoadState::Connect( sockaddr *addr, int len )
{
    m_addr = addr;
    m_addrlen = len;

    this->NotifyReceive();
}


int TelnetSSHLoginInfoLoadState::Recv( void *buf, int len )
{
    assert(buf);

    if (nullptr == buf || len <= 0)
    {
        return 0;
    }

    std::copy(m_textShowOnSceen.begin(), m_textShowOnSceen.end(), 
        stdext::checked_array_iterator<char *>(static_cast<char *>(buf), len));

    return std::min(m_textShowOnSceen.size(), static_cast<size_t>(len));
}


int TelnetSSHLoginInfoLoadState::Send( const void *buf, int len )
{
    assert(buf);

    if (nullptr == buf || len <= 0)
    {
        return 0;
    }

    const char *text = static_cast<const char *>(buf);
    m_textShowOnSceen.clear();

    switch(*text)
    {
    case 13:
        {
            if (m_username.empty())
            {
                SetCourseCommand(1, 1, m_textShowOnSceen);
                m_textShowOnSceen += gLoginUsernameMessage;
                m_target = &m_username;
            }
            else if (&m_username == m_target)
            {
                SetCourseCommand(2, 1, m_textShowOnSceen);
                m_textShowOnSceen += gLoginPasswordMessage;
                m_target = &m_password;
            }
            else //!m_username.empty() && !m_password.empty()
            {
                m_fnNextState();
                return len;
            }
        }
        break;

    case 8:
    case 127:
        {
            if (!m_target->empty())
            {
                const char *backspace = "\b \b";
                m_textShowOnSceen = backspace;
                m_target->pop_back();
            }
        }
        break;

    default:
        if (m_target == &m_password)
        {
            m_textShowOnSceen.assign("*", len);
        }
        else
        {
            m_textShowOnSceen.assign(text, len);
        }
        m_target->append(text, len);
    }

    this->NotifyReceive();

    return len;
}


void TelnetSSHLoginInfoLoadState::Close()
{
}


std::unique_ptr<TelnetSSHState> TelnetSSHLoginInfoLoadState::Next()
{
    std::unique_ptr<TelnetSSHConnectState> state(
        new TelnetSSHConnectState(*this));
    state->Connect(m_addr, m_addrlen);

    return std::move(state);
}


void TelnetSSHLoginInfoLoadState::NotifyReceive()
{
    m_fnNotifyReceive();
}


struct TelnetSSHConnectState::TelnetSSHConnectStateImpl
{
    void Init();
    void Clear();

    bool initialSuccess;
    bool connected;
    LIBSSH2_SESSION *session;
    LIBSSH2_CHANNEL *channal;
};


void TelnetSSHConnectState::TelnetSSHConnectStateImpl::Init()
{
    this->Clear();

    if (0 == libssh2_init(0) )
    {
        this->initialSuccess = true;
    }
}


void TelnetSSHConnectState::TelnetSSHConnectStateImpl::Clear()
{
    this->initialSuccess = false;
    this->connected = false;
    this->session = nullptr;
    this->channal = nullptr;
}


TelnetSSHConnectState::TelnetSSHConnectState( 
    SOCKET socket, 
    const std::string &username, 
    const std::string &password, 
    const fnTypeNextState &fnNextState, 
    const fnTypeSetDisconnect &fnSetDisconnect ) :
    m_impl(new TelnetSSHConnectState::TelnetSSHConnectStateImpl)
{
    assert(m_impl);

    this->SetSocket(socket);
    this->SetUsername(username);
    this->SetPassword(password);
    this->SetFnNextState(fnNextState);
    this->SetFnSetDisconnect(fnSetDisconnect);

    m_impl->Init();
}


TelnetSSHConnectState::TelnetSSHConnectState( 
    TelnetSSHLoginInfoLoadState &loginState ) : 
    m_impl(new TelnetSSHConnectState::TelnetSSHConnectStateImpl)
{
    assert(m_impl);

    this->SetSocket(loginState.GetSocket());
    this->SetUsername(loginState.GetUsername());
    this->SetPassword(loginState.GetPassword());
    this->SetFnNextState(loginState.GetFnNextState());
    this->SetFnNotifyReceive(loginState.GetFnNotifyReceive());
    this->SetFnSetDisconnect(loginState.GetFnSetDisconnect());

    m_impl->Init();
}


TelnetSSHConnectState::~TelnetSSHConnectState( void )
{
    libssh2_exit();
}


void TelnetSSHConnectState::OnReceive( int nErrorCode )
{
}


void TelnetSSHConnectState::OnConnect( int nErrorCode )
{
    if (0 != nErrorCode)
    {
        return;
    }

    LIBSSH2_SESSION *session = libssh2_session_init();

    if(int err = libssh2_session_handshake(session, this->GetSocket()))
    {
        return;
    }

    //what fingerprint can do?
    const char *fingerprint = libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);

    if (libssh2_userauth_password(
        session, 
        this->GetUsername().c_str(), 
        this->GetPassword().c_str()))
    {
        this->GetFnSetDisconnect()();
        return;
    }

    LIBSSH2_CHANNEL *channel = libssh2_channel_open_session(session);
    if (nullptr == channel)
    {
        return;
    }

    if (libssh2_channel_request_pty(channel, "vt100"))
    {
        return;
    }

    if (libssh2_channel_shell(channel))
    {
        return;
    }

    libssh2_session_set_blocking(session, 0);
    m_impl->session = session;
    m_impl->channal = channel;

    m_impl->connected = true;
}



void TelnetSSHConnectState::Connect( sockaddr *addr, int len )
{
    ::connect(this->GetSocket(), addr, len);
}


int TelnetSSHConnectState::Send( const void* buf, int len )
{
    if (!m_impl->initialSuccess)
    {
        return 0;
    }

    if (!m_impl->connected)
    {
        return 0;
    }

    int returnLen = libssh2_channel_write(m_impl->channal, static_cast<const char *>(buf), len);

    if (LIBSSH2_ERROR_EAGAIN == returnLen)
    {
        //TODO.
    }

    if (LIBSSH2_ERROR_ALLOC == returnLen)
    {
        //TODO.
    }

    if (LIBSSH2_ERROR_SOCKET_SEND == returnLen)
    {
        //TODO.
    }

    if (LIBSSH2_ERROR_CHANNEL_CLOSED == returnLen)
    {
        //TODO.
    }

    if (LIBSSH2_ERROR_CHANNEL_EOF_SENT == returnLen)
    {
        //TODO.
    }

    return returnLen;
}


int TelnetSSHConnectState::Recv( void* buf, int len )
{
    if (!m_impl->initialSuccess)
    {
        return 0;
    }

    if (!m_impl->connected)
    {
        return 0;
    }

    if (nullptr == m_impl->channal)
    {
        return 0;
    }

    int returnLen = libssh2_channel_read(m_impl->channal, static_cast<char *>(buf), len);

    if (LIBSSH2_ERROR_EAGAIN == returnLen)
    {
        return 0;
    }

    if (LIBSSH2_ERROR_SOCKET_SEND == returnLen)
    {
        //TODO.
    }

    if (LIBSSH2_ERROR_CHANNEL_CLOSED == returnLen)
    {
        //TODO.
    }


    if (0 >= returnLen)
    {
        if (libssh2_channel_eof(m_impl->channal))
        {
            if (m_impl->channal)
            {
                libssh2_channel_free(m_impl->channal);
                m_impl->channal = nullptr;
            }

            if (m_impl->session)
            {
                libssh2_session_disconnect(m_impl->session, "disconnect");
                libssh2_session_free(m_impl->session);
            }
        }
    }

    return returnLen;
}


void TelnetSSHConnectState::Close()
{
}


std::unique_ptr<TelnetSSHState> TelnetSSHConnectState::Next()
{
    std::unique_ptr<TelnetSSHLoginInfoLoadState> loginState(
        new TelnetSSHLoginInfoLoadState(*this));

    return std::move(loginState);
}
