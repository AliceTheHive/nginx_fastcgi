#include "tcp_client.h"

#include "log.h"
#include "errcode.h"


CTcpClient::CTcpClient(CTcpClientUnit *unit
					   , const std::string &ip
					   , uint16_t port
					   , uint32_t max_req_count /* = 5000 */)
{
	EnableInput();
	EnableOutput();
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
		//CRequest *request = m_requests.front();
		m_requests.pop();
		//DELETE(request);
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
	m_req_mutex.Lock();
	if(static_cast<uint32_t>(m_requests.size()) >= m_max_req_count)
	{
		m_req_mutex.UnLock();
		return false;
	}

	m_requests.push(request);
	if(NULL != GetOwner())
	{
		GetOwner()->Active();
	}
	m_req_mutex.UnLock();
	return true;
}

bool CTcpClient::DoSomething()
{
	return Reconnect();
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
	bool ret = false;
	bool is_need_close_connect = false;
	if(!m_server_packet.HeaderFinish())
	{
		ret = RecvHeader();
		if(true == ret)
		{
			if(m_server_packet.HeaderFinish())
			{
				if(false == m_server_packet.DecodeHeader())
				{
					log_error("Decode header failed, need close connect.");
					is_need_close_connect = true;
				}
			}
		}
		else
		{
			log_error("Recv header failed, need close connect.");
			is_need_close_connect = true;
		}
	}
	else
	{
		ret = RecvBody();
		if(true == ret)
		{
			if(m_server_packet.BodyFinish())
			{
				ResultNotify();
				log_debug("Response result success.");
			}
		}
		else
		{
			log_error("Recv body failed, need close connect.");
			is_need_close_connect = true;
		}
	}

	EnableOutput();
	GetOwner()->ModifyEvent(this);
	if(m_requests.empty())
	{
		GetOwner()->Suspend();
	}
	
	if(true == is_need_close_connect)
	{
		ErrorNotify();
	}
}

void CTcpClient::OutputNotify()
{
	
}

bool CTcpClient::IsConnect()
{
	return (m_fd > 0);
}

bool CTcpClient::Reconnect()
{
	if(true == IsConnect())
	{
		return true;
	}

	if(false == Connect())
	{
		return false;
	}

	EnableInput();
	EnableOutput();
	
	if(false == m_unit->AttachClient(this))
	{
		Close();
		return false;
	}

	if(m_requests.empty())
	{
		GetOwner()->Suspend();
	}
	
	return true;
}

void CTcpClient::ErrorNotify()
{
	if(NULL != m_cur_request)
	{
		ResponseError(FC_NETWORK_ERROR);
	}
	
	//GetOwner()->Suspend();
	GetOwner()->AttachPreUnit(this);
	GetOwner()->DetachUnit(this);
	Close();
	ResetData();
}

void CTcpClient::ResultNotify()
{
	if(NULL != m_cur_request)
	{
		ResponseResult();
	}
	ResetData();
}

bool CTcpClient::RecvHeader()
{
	int64_t recv_length = -1;
	recv_length = read(m_fd, m_server_packet.HeaderBuffer() + m_server_packet.HeaderRecvedLength(), m_server_packet.HeaderLength() - m_server_packet.HeaderRecvedLength());
	if(recv_length > 0)
	{
		m_server_packet.SetHeaderRecvedLength(m_server_packet.HeaderRecvedLength() + recv_length);
	}
	else if(0 == recv_length)
	{
		log_error("The remote process initiative close fd [%d], remote address [%s], remote port [%u].", m_fd, m_server_ip.c_str(), m_server_port);
		return false;
	}
	else
	{
		if(EINTR != errno
		   && EAGAIN != errno
		   && EINPROGRESS != errno
		   && EWOULDBLOCK != errno)
		{
			log_error("The client [%d] recv failed, errno [%d], strerror [%s].", m_fd, errno, strerror(errno));
			return false;
		}
	}

	return true;
}

bool CTcpClient::RecvBody()
{
	int64_t recv_length = -1;
	recv_length = read(m_fd, m_server_packet.BodyBuffer() + m_server_packet.BodyRecvedLength(), m_server_packet.BodyLength() - m_server_packet.BodyRecvedLength());
	if(recv_length > 0)
	{
		m_server_packet.SetBodyRecvedLength(m_server_packet.BodyRecvedLength() + recv_length);
	}
	else if(0 == recv_length)
	{
		log_error("The remote process initiative close fd [%d], remote address [%s], remote port [%u].", m_fd, m_server_ip.c_str(), m_server_port);
		return false;
	}
	else
	{
		if(EINTR != errno
		   && EAGAIN != errno
		   && EINPROGRESS != errno
		   && EWOULDBLOCK != errno)
		{
			log_error("The client [%d] recv failed, errno [%d], strerror [%s].", m_fd, errno, strerror(errno));
			return false;
		}
	}

	return true;
}

bool CTcpClient::ResponseError(int32_t code)
{
	fcgi_ostream out_stream(m_cur_request->out);
	Json::Value response;
	response["code"] = code;
	response["message"] = GetFcgiCodeStr(code);
	out_stream << "Content-type: text/html\r\n\r\n";
	out_stream << response.toStyledString();
	CRequestFactory::Instance()->Finish(m_cur_request);
	return true;
}

bool CTcpClient::ResponseResult()
{
	fcgi_ostream out_stream(m_cur_request->out);
	out_stream.write(static_cast<char *>(m_server_packet.BodyBuffer()), m_server_packet.BodyLength());
	CRequestFactory::Instance()->Finish(m_cur_request);
	return true;
}

void CTcpClient::ResetData()
{
	m_cur_request = NULL;
	m_web_packet.Reset();
	m_server_packet.Reset();
}
