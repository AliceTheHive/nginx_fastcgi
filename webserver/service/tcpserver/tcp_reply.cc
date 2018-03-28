#include "tcp_reply.h"

#include "tcp_connection.h"


CTcpReply::CTcpReply()
{
	
}

CTcpReply::~CTcpReply()
{
	
}

void CTcpReply::ReplyNotify(CTask *task)
{
	do
	{
		CTcpConnection *connection = task->GetConnection();
		if(NULL == connection)
		{
			break;
		}

		connection->SendPacket(task->GetResponsePacket());
	} while(0);
	
	delete task;
}
