#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__


#include <queue>

#include "fcgi_packet.h"
#include "pollunit.h"
#include "threadmutex.h"


using namespace Network;


class CRequest;


class CTcpClient : public CPollUnit
{
public:
	CTcpClient(CTcpClientUnit *unit, const std::string &ip, uint16_t port, uint32_t max_req_count = 5000);
	virtual ~CTcpClient();

public:
	bool Connect();
	void Close();
	bool DispatchRequest(CRequest *request);

public:
	virtual void HangupNotify();
	virtual void RDHupNotify();
	virtual void InputNotify();
	virtual void OutputNotify();

private:
	bool IsConnect();
	void ErrorNotify();
	void ResultNotify();
	bool RecvHeader();
	bool RecvBody();
	bool ResponseError(int32_t code);
	bool ResponseResult();
	void ResetData();
	
private:
	CTcpClientUnit *m_unit;
	std::string m_server_ip;
	uint16_t m_server_port;
	uint32_t m_max_req_count;
	CThreadMutex m_req_mutex;
	CRequest *m_cur_request;
	CFcgiPacket m_web_packet;
	CFcgiPacket m_server_packet;
	std::queue<CRequest *> m_requests;
};


#endif
