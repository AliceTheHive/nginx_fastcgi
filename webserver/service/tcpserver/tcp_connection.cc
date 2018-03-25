#include "tcp_connection.h"

#include "request_packet.h"
#include "response_packet.h"
#include "log.h"
#include "task.h"


class CClientReply : public CReplyDispatcher<CTask>
{
public:
	CClientReply()
	{
		
	}
	virtual ~CClientReply()
	{
		
	}

public:
	virtual void ReplyNotify(CTask *data);
};


void CClientReply::ReplyNotify(CTask *data)
{
	log_debug("CClientReply::ReplyNotify");
	
	CTcpConnection *connection = task->GetConnection();
	if(NULL != connection)
	{
		connection->SendPacket(data->GetResponsePacket());
	}
}


CTcpConnection::CTcpConnection(CClientProcess *client, int32_t fd) : CConnection(fd)
{
	m_successor = client;
}

CTcpConnection::~CTcpConnection()
{
	
}

void CTcpConnection::OnPacket(CPacket *packet)
{
	CRequestPacket *req_packet = dynamic_cast<CRequestPacket *>(packet);
	CTask *task = new CTask();
	task->SetConnection(this);
	//task->PushReplyDispatcher();
	task->SetRequestPacket(req_packet);
	m_successor->TaskNotify(task);
}

int64_t CTcpConnection::Decode(CPackets &packets)
{
	int64_t decode_length = 0;
	while(true)
	{
		CPacket *packet = NULL;
		int64_t result = DecodePacket(decode_length, packet);
		if(result > 0)
		{
			decode_length += result;
			packets.push_back(packet);
		}
		else if(0 == result)
		{
			break;
		}
		else
		{
			if(decode_length <= 0)
			{
				decode_length = result;
			}
			break;
		}
	}

	return decode_length;
}

int64_t CTcpConnection::DecodePacket(int64_t pos, CPacket *&packet)
{
	packet = NULL;
	CPacket *decode_packet = new CRequestPacket();
	if(NULL == decode_packet)
	{
		log_error("Not enough memory for new CRequestPacket.");
		return 0;
	}

	int64_t decode_length = decode_packet->Decode(m_recv_buffers, pos);
	if(decode_length > 0)
	{
		packet = decode_packet;
	}
	else
	{
		delete decode_packet;
		decode_packet = NULL;
	}
	
	return decode_length;
}
