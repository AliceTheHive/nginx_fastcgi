#ifndef __TCP_LISTENER_H__
#define __TCP_LISTENER_H__


#include "listener.h"


using namespace Network;


class CClientProcess;


class CTcpListener : public CListener
{
public:
	CTcpListener(CClientProcess *client, std::string address, uint16_t port, int32_t rcvbuf_size = 0, int32_t sndbuf_size = 0, int32_t reuse = 1, int32_t nodelay = 1, int32_t defer_accept = 1);
	CTcpListener(CClientProcess *client, int32_t fd);
	virtual ~CTcpListener();

public:
	virtual CConnection *CreateConnection(int32_t fd);

private:
	CClientProcess *m_successor;
};


#endif
