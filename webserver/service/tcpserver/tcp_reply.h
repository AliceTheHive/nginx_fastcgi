#ifndef __TCP_REPLY_H__
#define __TCP_REPLY_H__


#include "dispatcher.h"
#include "task.h"


class CTcpReply : public CReplyDispatcher<CTask>
{
public:
	CTcpReply();
	virtual ~CTcpReply();

public:
	virtual void ReplyNotify(CTask *task);
};


#endif
