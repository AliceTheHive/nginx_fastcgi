#include "pollunit.h"

#include <fcntl.h>
#include <unistd.h>

#include <sys/socket.h>

#include "log.h"
#include "pollpool.h"


namespace Network
{
	CPollUnit::CPollUnit()
	{
		m_owner = NULL;
		m_fd = -1;
		m_events = 0;
	}

	CPollUnit::~CPollUnit()
	{
		log_debug("CPollUnit::~CPollUnit [%d] begin.", m_fd);
		if(NULL != m_owner)
		{
			m_owner->DetachUnit(this);
			m_owner = NULL;
		}

		if(m_fd > 0)
		{
			close(m_fd);
			m_fd = -1;
		}

		m_events = 0;
		log_debug("CPollUnit::~CPollUnit end.");
	}

	void CPollUnit::EnableInput()
	{
		m_events |= EPOLLIN;
	}

	void CPollUnit::DisableInput()
	{
		m_events &= ~EPOLLIN;
	}

	void CPollUnit::EnableOutput()
	{
		m_events |= EPOLLOUT;
	}

	void CPollUnit::DisableOutput()
	{
		m_events &= ~EPOLLOUT;
	}

	void CPollUnit::EnableRDHup()
	{
		m_events |= EPOLLRDHUP;
	}

	void CPollUnit::DisableRDHup()
	{
		m_events &= ~EPOLLRDHUP;
	}

	void CPollUnit::EnableET()
	{
		m_events |= EPOLLET;
	}

	void CPollUnit::DisableET()
	{
		m_events &= ~EPOLLET;
	}

	void CPollUnit::EnableOneShot()
	{
		m_events |= EPOLLONESHOT;
	}

	void CPollUnit::DisableOneShot()
	{
		m_events &= ~EPOLLONESHOT;
	}

	int32_t CPollUnit::SetCloseExec()
	{
		int flags = fcntl(m_fd, F_GETFD);
		if(-1 == flags)
		{
			return -1;
		}

		flags |= FD_CLOEXEC;
		if(-1 == fcntl(m_fd, F_SETFD, flags))
		{
			return -1;
		}

		return 0;
	}

	int32_t CPollUnit::ClearCloseExec()
	{
		int flags = fcntl(m_fd, F_GETFD);
		if(-1 == flags)
		{
			return -1;
		}

		flags &= ~FD_CLOEXEC;
		if(-1 == fcntl(m_fd, F_SETFD, flags))
		{
			return -1;
		}

		return 0;
	}

	int32_t CPollUnit::SetFlag(int option)
	{
		int flags = fcntl(m_fd, F_GETFL);
		if(-1 == flags)
		{
			return -1;
		}

		flags |= option;
		if(-1 == fcntl(m_fd, F_SETFL, flags))
		{
			return -1;
		}

		return 0;
	}

	int32_t CPollUnit::ClearFlag(int option)
	{
		int flags = fcntl(m_fd, F_GETFL);
		if(-1 == flags)
		{
			return -1;
		}

		flags &= ~option;
		if(-1 == fcntl(m_fd, F_SETFL, flags))
		{
			return -1;
		}

		return 0;
	}

	int32_t CPollUnit::SetNonBlock()
	{
		return SetFlag(O_NONBLOCK);
	}

	int32_t CPollUnit::ClearNonBlock()
	{
		return ClearFlag(O_NONBLOCK);
	}

	CPollPool *CPollUnit::GetOwner()
	{
		return m_owner;
	}

	int32_t CPollUnit::GetFD()
	{
		return m_fd;
	}

	uint32_t CPollUnit::GetEvents()
	{
		return m_events;
	}

	epoll_event *CPollUnit::GetEpollEvent()
	{
		m_epoll_event.events = m_events;
		m_epoll_event.data.fd = m_fd;
		return (&m_epoll_event);
	}

	void CPollUnit::Close()
	{
		Clear();
		
		if(m_fd > 0)
		{
			shutdown(m_fd, SHUT_WR);
		}
	}

	bool CPollUnit::DoSomething()
	{
		
	}

	void CPollUnit::HangupNotify()
	{
		delete this;
	}

	void CPollUnit::RDHupNotify()
	{
		delete this;
	}

	void CPollUnit::Clear()
	{
		
	}
};
