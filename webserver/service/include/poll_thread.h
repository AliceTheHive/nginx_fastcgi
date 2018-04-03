#ifndef __POLL_THREAD_H__
#define __POLL_THREAD_H__


#include "pollpool.h"
#include "thread_base.h"
#include "thread_condition.h"
#include "thread_mutex.h"


using namespace Network;


class CPollThread : public CPollPool, public CThreadBase
{
public:
	CPollThread(const std::string &threadname, uint64_t epoll_size = 4096, int32_t epoll_timeout = 3000, bool is_use_signal = false, bool is_suspend = false);
	virtual ~CPollThread();

public:
	void Suspend();
	void Active();
	
public:
	virtual bool Initialize();
	virtual void Prepare();
	virtual void *Run();
	virtual bool Interrupt(void **msg);

private:
	int32_t m_epoll_timeout;
	bool m_is_use_signal;
	bool m_is_suspend;
	CThreadMutex m_ctrl_mutex;
	CThreadCondition m_ctrl_condition;
};


#endif
