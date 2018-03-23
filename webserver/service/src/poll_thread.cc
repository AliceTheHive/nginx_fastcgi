#include "poll_thread.h"

#include "log.h"


CPollThread::CPollThread(const std::string &threadname, uint64_t epoll_size /* = 4096 */, int32_t epoll_timeout /* = 3000 */)
	: CPollPool(epoll_size)
	, CThreadBase(threadname)
	, m_epoll_timeout(epoll_timeout)
{
	
}

CPollThread::~CPollThread()
{
	
}

bool CPollThread::Initialize()
{
	int32_t result = CPollPool::Init();
	if(0 != result)
	{
		log_error("CPollPool init failed, when initialize CPollThread.");
		return false;
	}

	return true;
}

void *CPollThread::Run()
{
	while(!IsNeedStop())
	{
		EpollProcess(m_epoll_timeout);
	}

	return static_cast<void *>(const_cast<char *>(GetExitMsg().c_str()));
}
