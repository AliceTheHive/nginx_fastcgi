#include "pollunit.h"


namespace Network
{
	CPollUnit::CPollUnit(CPollPool *owner /* = NULL */, int32_t fd /* = -1 */)
	{
		m_owner = owner;
		m_fd = fd;
		m_events = 0;
	}

	CPollUnit::~CPollUnit()
	{
		if(NULL != m_owner)
		{
			m_owner->Detach(this);
			m_owner = NULL;
		}

		if(m_fd > 0)
		{
			close(m_fd);
			m_fd = -1;
		}

		m_events = 0;
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
		m_epoll_event.fd = m_fd;
		return (&m_epoll_event);
	}
};
