#include "client_process.h"

#include "log.h"


CClientProcess::CClientProcess(CPollPool *pool /* = NULL */)
	: CTaskDispatcher<CTask>(pool)
{
	
}

CClientProcess::~CClientProcess()
{
	
}

void CClientProcess::TaskNotify(CTask *data)
{
	log_debug("CClientProcess::TaskNotify");
	m_successor->TaskNotify(data);
}
