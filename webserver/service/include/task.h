#ifndef __TASK_H__
#define __TASK_H__


#include "dispatcher.h"
#include "request_packet.h"
#include "response_packet.h"


class CWebConnection;


class CTask : public CTaskReplyList<CTask>
{
public:
	CTask();
	virtual ~CTask();

public:
	void SetConnection(CWebConnection *connection);
	CWebConnection *GetConnection();
	void SetRequestPacket(CRequestPacket *req_packet);
	void SetResponseHeader(const ProtocolHeader &header);
	void SetResponseBody(const char *body);
	const CResponsePacket &GetResponsePacket();

private:
	CWebConnection *m_connection;
	CRequestPacket *m_req_packet;
	CResponsePacket m_res_packet;
};


#endif
