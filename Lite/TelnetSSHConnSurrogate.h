#ifndef _PCMAN_COMBO_TELNETSSHCONNSURROGATE_H_
#define _PCMAN_COMBO_TELNETSSHCONNSURROGATE_H_

#include <afx.h>
#include "TelnetConn.h"
#include <memory>

class TelnetSSHState;
class TelnetSSHConnSurrogate :
    public CTelnetConn
{
public:
    TelnetSSHConnSurrogate(void);
    virtual ~TelnetSSHConnSurrogate(void);

    virtual void OnReceive(int nErrorCode) override;

    virtual void OnConnect(int nErrorCode) override;

    virtual void ConnectImpl(sockaddr *addr, int len) override;

    virtual int RecvImpl(void* buf, int len) override;

    virtual int SendImpl(const void* lpBuf, int nBufLen) override;

    virtual int CloseImpl() override;

    void NextState();

    void SetDisconnect();

private:
    std::unique_ptr<TelnetSSHState> m_self;

    sockaddr m_addr;
    int m_addrlen;
};


#endif
