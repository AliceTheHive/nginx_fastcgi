#include "tcp_client.h"

#include "log.h"
#include "web_constant.h"


CTcpClient::CTcpClient(CTcpClientUnit *unit
					   , const std::string &ip
					   , uint16_t port
					   , uint32_t max_req_count /* = 5000 */)
{
	m_unit = unit;
	m_server_ip = ip;
	m_server_port = port;
	m_max_req_count = max_req_count;
	m_cur_request = NULL;
	while(!m_requests.empty())
	{
		m_requests.pop();
	}
}

CTcpClient::~CTcpClient()
{
	m_unit = NULL;
	m_server_ip = "";
	m_server_port = 0;
	m_max_req_count = 0;
	m_cur_request = NULL;
	while(!m_requests.empty())
	{
		CRequest *request = m_requests.front();
		m_requests.pop();
		DELETE(request);
	}
}

bool CTcpClient::Connect()
{
	if(m_fd > 0)
	{
		return true;
	}
	
	m_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == m_fd)
	{
		log_error("Create socket fd failed, errno [%d], strerror [%s].", errno, strerror(errno));
		return false;
	}
	if(0 != SetCloseExec())
	{
		log_error("Set fd [%d] CloseExec flag failed.", m_fd);
		return false;
	}
	if(0 != SetNonBlock())
	{
		log_error("Set fd [%d] NonBlock flag failed.", m_fd);
		return false;
	}

	sockaddr_in their_addr;
	their_addr.sin_family = AF_INET;
	their_addr.sin_addr.s_addr = inet_addr(const_cast<char *>(m_server_ip.c_str()));
	their_addr.sin_port = htons(m_server_port);
	bzero(&(their_addr.sin_zero), sizeof(their_addr.sin_zero));

	int result = connect(m_fd, reinterpret_cast<sockaddr *>(&their_addr), sizeof(their_addr));
	if(0 == result)
	{
		log_debug("The fd [%d] connect success.", m_fd);
	}
	else
	{
		if(EINPROGRESS == errno)
		{
			pollfd pfd;
			pfd.fd = m_fd;
			pfd.events = POLLOUT;
			if(poll(&pfd, 1, 5000) > 0)
			{
				result = 0;
				log_debug("The fd [%d] connect EINPROGRESS , but success.", m_fd);
			}
			else
			{
				log_error("The fd [%d] connect EINPROGRESS , and failed.", m_fd);
			}
		}
		else
		{
			log_error("The fd [%d] connect faield.", m_fd);
		}
	}

	if(0 != result)
	{
		return false;
	}

	return true;
}

void CTcpClient::Close()
{
	CPollUnit::Close();
	if(m_fd > 0)
	{
		close(m_fd);
		m_fd = -1;
	}
}

bool CTcpClient::DispatchRequest(CRequest *request)
{
	
}

void CTcpClient::HangupNotify()
{
	ErrorNotify();
}

void CTcpClient::RDHupNotify()
{
	ErrorNotify();
}

void CTcpClient::InputNotify()
{
	
}

void CTcpClient::OutputNotify()
{
	
}

void CTcpClient::ErrorNotify()
{
	m_owner->Suspend();
	m_owner->DetachUnit(this);
	if(m_fd > 0)
	{
		close(m_fd);
		m_fd = -1;
	}
}
