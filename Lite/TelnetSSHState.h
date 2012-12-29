#ifndef _PCMANNOVUS_COMBO_TELNETSSHCONN_H_
#define _PCMANNOVUS_COMBO_TELNETSSHCONN_H_


#include <memory>
#include <string>
#include <functional>

#include <WinSock.h>

class TelnetSSHState
{
public:
    TelnetSSHState(void);

    virtual ~TelnetSSHState(void);

    virtual void OnReceive(int nErrorCode) = 0;

    virtual void OnConnect(int nErrorCode) = 0;

    virtual void Connect(sockaddr *addr, int len) = 0;

    virtual int Recv(void *buf, int len) = 0;

    virtual int Send(const void *buf, int len) = 0;

    virtual void Close() = 0;

    virtual std::unique_ptr<TelnetSSHState> Next() = 0;
};


class TelnetSSHConnectState;
class TelnetSSHDefaultState : 
    public TelnetSSHState
{
public:
    typedef std::function<void (void)> fnTypeNextState;
    typedef std::function<void (void)> fnTypeNotifyReceive;
    typedef std::function<void (void)> fnTypeSetDisconnect;

    void SetSocket(SOCKET socket)
    {
        m_socket = socket;
    }

    SOCKET GetSocket() const
    {
        return m_socket;
    }

    template<class T>
    void SetUsername(T &&username)
    {
        m_username = std::forward<T>(username);
    }

    const std::string &GetUsername() const
    {
        return m_username;
    }

    template<class T>
    void SetPassword(T &&password)
    {
        m_password = std::forward<T>(password);
    }

    const std::string &GetPassword() const
    {
        return m_password;
    }

    template<class T>
    void SetFnNextState(T &&fnNextState)
    {
        m_fnNextState = std::forward<T>(fnNextState);
    }

    const fnTypeNextState &GetFnNextState() const
    {
        return m_fnNextState;
    }

    template<class T>
    void SetFnNotifyReceive(T &&fnNotifyReceive)
    {
        m_fnNotifyReceive = std::forward<T>(fnNotifyReceive);
    }

    const fnTypeNotifyReceive &GetFnNotifyReceive() const
    {
        return m_fnNotifyReceive;
    }

    template<class T>
    void SetFnSetDisconnect(T &&fnSetDisconnect)
    {
        m_fnSetDisconnect = std::forward<T>(fnSetDisconnect);
    }

    const fnTypeSetDisconnect &GetFnSetDisconnect() const
    {
        return m_fnSetDisconnect;
    }

protected:
    SOCKET m_socket;
    std::string m_username;
    std::string m_password;
    fnTypeNextState m_fnNextState;
    fnTypeNotifyReceive m_fnNotifyReceive;
    fnTypeSetDisconnect m_fnSetDisconnect;
};


class TelnetSSHLoginInfoLoadState : 
    public TelnetSSHDefaultState
{
public:
    TelnetSSHLoginInfoLoadState(
        SOCKET socket,
        const fnTypeNextState &fnNextState, 
        const fnTypeNotifyReceive &fnNotifyReceive,
        const fnTypeSetDisconnect &fnSetDisconnect);
    TelnetSSHLoginInfoLoadState(const TelnetSSHConnectState &conn);
    virtual ~TelnetSSHLoginInfoLoadState(void);

    virtual void OnReceive(int nErrorCode) override;

    virtual void OnConnect(int nErrorCode) override;

    virtual void Connect(sockaddr *addr, int len) override;

    virtual int Recv(void *buf, int len) override;

    virtual int Send(const void *buf, int len) override;

    virtual void Close() override;

    virtual std::unique_ptr<TelnetSSHState> Next() override;

    void NotifyReceive();

private:
    sockaddr *m_addr;
    int m_addrlen;
    std::string m_textShowOnSceen;

    std::string *m_target;
};


class TelnetSSHConnectState :
    public TelnetSSHDefaultState
{
public:
    TelnetSSHConnectState(
        SOCKET socket, 
        const std::string &username, 
        const std::string &password,
        const fnTypeNextState &fnNextState,
        const fnTypeSetDisconnect &fnSetDisconnect);

    TelnetSSHConnectState(
        TelnetSSHLoginInfoLoadState &loginState);

    virtual ~TelnetSSHConnectState(void);

    virtual void OnReceive(int nErrorCode) override;

    virtual void OnConnect(int nErrorCode) override;

    virtual void Connect(sockaddr *addr, int len);

    virtual int Recv(void *buf, int len) override;

    virtual int Send(const void *buf, int len) override;

    virtual void Close() override;

    virtual std::unique_ptr<TelnetSSHState> Next() override;

private:
    struct TelnetSSHConnectStateImpl;
    std::unique_ptr<TelnetSSHConnectStateImpl> m_impl;
};


#endif
