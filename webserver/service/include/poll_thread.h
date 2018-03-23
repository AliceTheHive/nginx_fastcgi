#ifndef __POLL_THREAD_H__
#define __POLL_THREAD_H__


#include "pollpool.h"
#include "thread_base.h"


using namespace Network;


class CPollThread : public CPollPool, public CThreadBase
{
public:
	CPollThread(const std::string &threadname, uint64_t epoll_size = 4096, int32_t epoll_timeout = 3000);
	virtual ~CPollThread();

public:
	virtual bool Initialize();
	virtual void *Run();

private:
	int32_t m_epoll_timeout;
};


#endif
