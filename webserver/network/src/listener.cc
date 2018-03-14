#include "listener.h"

#include <errno.h>
#include <string.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>

#include "connection.h"
#include "log.h"
#include "pollpool.h"


namespace Network
{
	CListener::CListener(std::string address, uint16_t port, int32_t rcvbuf_size /* = 0 */, int32_t sndbuf_size /* = 0 */, int32_t reuse /* = 1 */, int32_t nodelay /* = 1 */, int32_t defer_accept /* = 1 */)
	{
		m_address = address;
		m_port = port;
		m_rcvbuf_size = rcvbuf_size;
		m_sndbuf_size = sndbuf_size;
		m_reuse = reuse;
		m_nodelay = nodelay;
		m_defer_accept = defer_accept;
		m_fd = -1;
	}

	CListener::CListener(int32_t fd)
	{
		m_address = "";
		m_port = 0;
		m_rcvbuf_size = 0;
		m_sndbuf_size = 0;
		m_reuse = 1;
		m_nodelay = 1;
		m_defer_accept = 0;
		m_fd = fd;
		
		if(m_fd < 0)
		{
			return;
		}

		sockaddr_in my_addr;
		socklen_t length = static_cast<socklen_t>(sizeof(my_addr));
		if(0 == getsockname(m_fd, reinterpret_cast<sockaddr *>(&my_addr), &length))
		{
			m_address = inet_ntoa(my_addr.sin_addr);
			m_port = ntohs(my_addr.sin_port);
		}

		length = static_cast<socklen_t>(sizeof(m_rcvbuf_size));
		getsockopt(m_fd, SOL_SOCKET, SO_RCVBUF, static_cast<void *>(&m_rcvbuf_size), &length);
		
		length = static_cast<socklen_t>(sizeof(m_sndbuf_size));
		getsockopt(m_fd, SOL_SOCKET, SO_SNDBUF, static_cast<void *>(&m_sndbuf_size), &length);

		length = static_cast<socklen_t>(sizeof(m_reuse));
		getsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, static_cast<void *>(&m_reuse), &length);
		
		length = static_cast<socklen_t>(sizeof(m_nodelay));
		getsockopt(m_fd, SOL_TCP, TCP_NODELAY, static_cast<void *>(&m_nodelay), &length);

		length = static_cast<socklen_t>(sizeof(m_defer_accept));
		getsockopt(m_fd, SOL_TCP, TCP_DEFER_ACCEPT, static_cast<void *>(&m_defer_accept), &length);
	}

	CListener::~CListener()
	{
		m_address = "";
		m_port = 0;
	}

	int32_t CListener::Listen(int32_t backlog /* = 1024 */)
	{
		if(m_fd > 0)
		{
			log_debug("The listen fd [%d] has exist.", m_fd);
			return 0;
		}

		sockaddr_in listen_addr;
		listen_addr.sin_family = AF_INET;
		listen_addr.sin_addr.s_addr = inet_addr(m_address.c_str());
		listen_addr.sin_port = htons(m_port);
		bzero(&(listen_addr.sin_zero), sizeof(listen_addr.sin_zero));

		m_fd = socket(AF_INET, SOCK_STREAM, 0);
		if(-1 == m_fd)
		{
			log_error("Create socket fd failed, errno [%d], strerror [%s].", errno, strerror(errno));
			return -1;
		}

		int32_t result = 0;
		if(m_reuse > 0)
		{
			result = setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, static_cast<void *>(&m_reuse), sizeof(m_reuse));
			if(-1 == result)
			{
				log_error("The fd [%d] setsockopt SO_REUSEADDR failed, errno [%d], strerror [%s].", m_fd, errno, strerror(errno));
				close(m_fd);
				m_fd = -1;
				return -1;
			}
		}

		if(m_nodelay > 0)
		{
			result = setsockopt(m_fd, SOL_TCP, TCP_NODELAY, static_cast<void *>(&m_nodelay), sizeof(m_nodelay));
			if(-1 == result)
			{
				log_error("The fd [%d] setsockopt TCP_NODELAY failed, errno [%d], strerror [%s].", m_fd, errno, strerror(errno));
				close(m_fd);
				m_fd = -1;
				return -1;
			}
		}

		if(m_defer_accept > 0)
		{
			result = setsockopt(m_fd, SOL_TCP, TCP_DEFER_ACCEPT, static_cast<void *>(&m_defer_accept), sizeof(m_defer_accept));
			if(-1 == result)
			{
				log_error("The fd [%d] setsockopt TCP_DEFER_ACCEPT failed, errno [%d], strerror [%s].", m_fd, errno, strerror(errno));
				close(m_fd);
				m_fd = -1;
				return -1;
			}
		}
		
		result = bind(m_fd, reinterpret_cast<sockaddr *>(&listen_addr), sizeof(listen_addr));
		if(-1 == result)
		{
			log_error("The fd [%d] bind failed, errno [%d], strerror [%s].", m_fd, errno, strerror(errno));
			close(m_fd);
			m_fd = -1;
			return -1;
		}

		result = listen(m_fd, backlog);
		if(-1 == result)
		{
			log_error("The fd [%d] listen failed, errno [%d], strerror [%s].", m_fd, errno, strerror(errno));
			close(m_fd);
			m_fd = -1;
			return -1;
		}

		if(m_rcvbuf_size > 0)
		{
			result = setsockopt(m_fd, SOL_SOCKET, SO_RCVBUF, static_cast<void *>(&m_rcvbuf_size), sizeof(m_rcvbuf_size));
			if(-1 == result)
			{
				log_error("The fd [%d] setsockopt SO_RCVBUF failed, errno [%d], strerror [%s].", m_fd, errno, strerror(errno));
				close(m_fd);
				m_fd = -1;
				return -1;
			}
		}

		if(m_sndbuf_size > 0)
		{
			result = setsockopt(m_fd, SOL_SOCKET, SO_SNDBUF, static_cast<void *>(&m_sndbuf_size), sizeof(m_sndbuf_size));
			if(-1 == result)
			{
				log_error("The fd [%d] setsockopt SO_SNDBUF failed, errno [%d], strerror [%s].", m_fd, errno, strerror(errno));
				close(m_fd);
				m_fd = -1;
				return -1;
			}
		}

		return 0;
	}

	CConnection *CListener::Accept()
	{
		sockaddr_in remote_addr;
		socklen_t sock_length = sizeof(remote_addr);
		int32_t new_fd = accept(m_fd, reinterpret_cast<sockaddr *>(&remote_addr), &sock_length);

		CConnection *connection = NULL;
		if(-1 == new_fd)
		{
			log_error("The fd [%d] accept one connection failed, errno [%d], strerror [%s].", m_fd, errno, strerror(errno));
		}
		else
		{
			connection = CreateConnection(new_fd);
			if(NULL == connection)
			{
				log_error("Create CConnection for new fd [%d] failed.", new_fd);
				close(new_fd);
			}
			else
			{
				if(-1 == connection->SetCloseExec()
				   || -1 == connection->SetNonBlock())
				{
					log_error("The fd [%d] connection SetCloseExec or SetNonBlock failed.", new_fd);
					delete connection;
					connection = NULL;
				}
			}
		}

		return connection;
	}

	void CListener::InputNotify()
	{
		CConnection *connection = this->Accept();
		if(NULL == connection)
		{
			log_error("Accept one connection failed.");
			return;
		}
		if(-1 == (GetOwner()->Attach(connection)))
		{
			log_error("Attach connection to owner failed.");
			delete connection;
			connection = NULL;
			return;
		}

		if(m_defer_accept > 0)
		{
			connection->InputNotify();
		}
	}

	void CListener::OutputNotify()
	{
		
	}
};
