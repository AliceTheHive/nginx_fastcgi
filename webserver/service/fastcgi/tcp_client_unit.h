#ifndef __TCP_CLIENT_UNIT_H__
#define __TCP_CLIENT_UNIT_H__


#include <stdint.h>

#include <string>


class CPollThread;
class CTcpClient;
class CTcpClientUnitPool;


class CTcpClientUnit
{
public:
	CTcpClientUnit(const std::string &thread_name, const std::string &ip, uint16_t port);
	~CTcpClientUnit();

public:
	bool Init();
	void Close();
	bool AttachPool(CTcpClientUnitPool *pool);
	bool DetachPool();
	bool AttachClient(CTcpClient *client);
	bool IsSuspend();

public:
	CPollThread *GetPollThread()
	{
		return m_pollthread;
	}

private:
	std::string m_thread_name;
	std::string m_ip;
	uint16_t m_port;
	CPollThread *m_pollthread;
	CTcpClient *m_tcpclient;
	CTcpClientUnitPool *m_owner;	
};


#endif
