#ifndef __WEB_REPLY_H__
#define __WEB_REPLY_H__


#include "dispatcher.h"
#include "task.h"


class CWebReply : public CReplyDispatcher<CTask>
{
public:
	CWebReply();
	virtual ~CWebReply();

public:
	virtual void ReplyNotify(CTask *task);
};


#endif
