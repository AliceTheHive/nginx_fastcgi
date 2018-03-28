#include "tcp_listener.h"

#include "log.h"
#include "tcp_connection.h"


CTcpListener::CTcpListener(CClientProcess *client, std::string address, uint16_t port, int32_t rcvbuf_size /* = 0 */, int32_t sndbuf_size /* = 0 */, int32_t reuse /* = 1 */, int32_t nodelay /* = 1 */, int32_t defer_accept /* = 1 */) : CListener(address, port, rcvbuf_size, sndbuf_size, reuse, nodelay, defer_accept)
{
	m_successor = client;
}

CTcpListener::CTcpListener(CClientProcess *client, int32_t fd) : CListener(fd)
{
	m_successor = client;
}

CTcpListener::~CTcpListener()
{
	
}

CConnection *CTcpListener::CreateConnection(int32_t fd)
{
	log_debug("CTcpListener::CreateConnection");
	
	CConnection *connection = new CTcpConnection(m_successor, fd);
	if(NULL == connection)
	{
		log_error("Not enough memory for fd [%d] new CTcpConnection.", fd);
	}
	else
	{
		log_debug("Create fd [%d] CTcpConnection success.", fd);
	}

	return connection;
}
