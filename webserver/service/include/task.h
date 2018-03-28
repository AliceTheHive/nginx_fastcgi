#ifndef __TASK_H__
#define __TASK_H__


#include "dispatcher.h"
#include "request_packet.h"
#include "response_packet.h"


class CTcpConnection;


class CTask : public CTaskReplyList<CTask>
{
public:
	CTask();
	virtual ~CTask();

public:
	void SetConnection(CTcpConnection *connection);
	CTcpConnection *GetConnection();
	void SetRequestPacket(CRequestPacket *req_packet);
	void SetResponseHeader(const ProtocolHeader &header);
	void SetResponseBody(const char *body, uint32_t length = 0);
	void SetResponseDataLength(uint32_t length);
	const CResponsePacket &GetResponsePacket();

private:
	CTcpConnection *m_connection;
	CRequestPacket *m_req_packet;
	CResponsePacket m_res_packet;
};


#endif
