#include "tcp_client_unit.h"

#include "log.h"
#include "poll_thread.h"
#include "tcp_client.h"
#include "tcp_client_unit_pool.h"
#include "web_constant.h"


CTcpClientUnit::CTcpClientUnit(const std::string &thread_name, const std::string &ip, uint16_t port)
{
	m_thread_name = thread_name;
	m_ip = ip;
	m_port = port;
	m_pollthread = NULL;
	m_tcpclient = NULL;
	m_owner = NULL;
}

CTcpClientUnit::~CTcpClientUnit()
{
	Close();
}

bool CTcpClientUnit::Init()
{
	m_pollthread = new CPollThread(m_thread_name, 4096, 3000, true, false);
	if(NULL == m_pollthread)
	{
		log_error("Not enough memory for new [%s] CPollThread.", m_thread_name.c_str());
		return false;
	}
	if(false == m_pollthread->Start())
	{
		log_error("The [%s] thread start failed.");
		return false;
	}

	m_tcpclient = new CTcpClient(this, m_ip, m_port);
	if(NULL == m_tcpclient)
	{
		log_error("Not enough memory for new CTcpClient.");
		return false;
	}
	if(false == m_tcpclient->Connect())
	{
		log_error("The tcpclient connect failed.");
		return false;
	}

	if(0 != m_pollthread->AttachUnit(m_tcpclient))
	{
		log_error("The tcpclient attach [%s] thread failed.", m_thread_name.c_str());
		return false;
	}

	log_debug("The CTcpClientUnit init success.");
	return true;
}

void CTcpClientUnit::Close()
{
	DetachPool();
	
	if(NULL != m_pollthread)
	{
		m_pollthread->Interrupt(NULL);
		m_pollthread->UnInit();
	}

	if(NULL != m_tcpclient)
	{
		m_tcpclient->Close();
		delete m_tcpclient;
	}

	DELETE(m_pollthread);	
}

bool CTcpClientUnit::AttachPool(CTcpClientUnitPool *pool)
{
	if(NULL != m_owner)
	{
		log_error("The poll had exist.");
		return false;
	}
	
	m_owner = pool;
	m_owner->Insert(this);
	return true;
}

bool CTcpClientUnit::DetachPool()
{
	if(NULL != m_owner)
	{
		m_owner->Erase(this);
		m_owner = NULL;
	}
	return true;
}

bool CTcpClientUnit::AttachClient(CTcpClient *client)
{
	if(NULL == m_pollthread)
	{
		return false;
	}

	return m_pollthread->AttachUnit(client);
}
