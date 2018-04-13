#include "poll_thread.h"

#include "log.h"


CPollThread::CPollThread(const std::string &threadname
						 , uint64_t epoll_size /* = 4096 */
						 , int32_t epoll_timeout /* = 3000 */
						 , bool is_use_signal /* = false */
						 , bool is_suspend /* = false */)
	: CPollPool(epoll_size)
	, CThreadBase(threadname)
	, m_epoll_timeout(epoll_timeout)
	, m_is_use_signal(is_use_signal)
	, m_is_suspend(is_suspend)
	, m_ctrl_mutex()
	, m_ctrl_condition(&m_ctrl_mutex)
{
	
}

CPollThread::~CPollThread()
{
	
}

void CPollThread::Suspend()
{
	m_is_suspend = true;
}

void CPollThread::Active()
{
	m_is_suspend = false;
	m_ctrl_condition.Signal();
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

void CPollThread::Prepare()
{
	
}

void *CPollThread::Run()
{
	m_ctrl_mutex.Lock();
	
	while(!IsNeedStop())
	{
		if(true == m_is_use_signal)
		{
			if(true == m_is_suspend)
			{
				m_ctrl_condition.Wait();
				continue;
			}
		}
		
		DoOtherSomething();

		if(true == m_is_use_signal)
		{
			if(true == m_is_suspend)
			{
				m_ctrl_condition.Wait();
				continue;
			}
		}

		EpollProcess(m_epoll_timeout);
	}

	m_ctrl_mutex.UnLock();

	return static_cast<void *>(const_cast<char *>(GetExitMsg().c_str()));
}

bool CPollThread::Interrupt(void **msg)
{
	if(true == m_is_use_signal)
	{
		if(NULL == this || m_stop || !GetThreadID())
		{
			return true;
		}
		SetStopFlag(1);
		m_ctrl_condition.Signal();
		bool res = Join(msg);
		do
		{
			if(false == res)
			{
				log_error("Join [%s] thread failed, when interrupt.", GetThreadName().c_str());
				break;
			}

			log_error("The [%s] thread normal stopped.", GetThreadName.c_str());
		} while(0);

		return res;
	}
	else
	{
		return CThreadBase::Interrupt(msg);
	}
}
