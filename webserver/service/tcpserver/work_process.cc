#include "work_process.h"

#include "log.h"


CWorkProcess::CWorkProcess(CPollPool *pool /* = NULL */)
	: CTaskDispatcher<CTask>(pool)
{
	
}

CWorkProcess::~CWorkProcess()
{
	
}

void CWorkProcess::TaskNotify(CTask *data)
{
	log_debug("CWorkProcess::TaskNotify");
	data->ReplyNotify();
}
