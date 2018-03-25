#ifndef __WORK_PROCESS_H__
#define __WORK_PROCESS_H__


#include "dispatcher.h"
#include "task.h"


class CWorkProcess : public CTaskDispatcher<CTask>
{
public:
	CWorkProcess(CPollPool *pool = NULL);
	virtual ~CWorkProcess();

public:
	virtual void TaskNotify(CTask *data);
};


#endif
