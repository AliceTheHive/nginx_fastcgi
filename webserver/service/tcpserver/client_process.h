#ifndef __CLIENT_PROCESS_H__
#define __CLIENT_PROCESS_H__


#include "dispatcher.h"
#include "task.h"


class CClientProcess : public CTaskDispatcher<CTask>
{
public:
	CClientProcess(CPollPool *pool = NULL);
	virtual ~CClientProcess();

public:
	virtual void TaskNotify(CTask *data);
};


#endif
