#include "web_listener.h"

#include "log.h"
#include "web_connection.h"


CWebListener::CWebListener(CClientProcess *client, std::string address, uint16_t port, int32_t rcvbuf_size /* = 0 */, int32_t sndbuf_size /* = 0 */, int32_t reuse /* = 1 */, int32_t nodelay /* = 1 */, int32_t defer_accept /* = 1 */) : CListener(address, port, rcvbuf_size, sndbuf_size, reuse, nodelay, defer_accept)
{
	m_successor = client;
}

CWebListener::CWebListener(CClientProcess *client, int32_t fd) : CListener(fd)
{
	m_successor = client;
}

CWebListener::~CWebListener()
{
	
}

CConnection *CWebListener::CreateConnection(int32_t fd)
{
	CConnection *connection = new CWebConnection(m_successor, fd);
	if(NULL == connection)
	{
		log_error("Not enough memory for fd [%d] new CWebConnection.", fd);
	}

	return connection;
}
