#include "web_reply.h"

#include "web_connection.h"


CWebReply::CWebReply()
{
	
}

CWebReply::~CWebReply()
{
	
}

void CWebReply::ReplyNotify(CTask *task)
{
	do
	{
		CWebConnection *connection = task->GetConnection();
		if(NULL == connection)
		{
			break;
		}

		connection->SendPacket(task->GetResponsePacket());
	} while(0);
	
	delete task;
}
