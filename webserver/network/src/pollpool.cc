#include "pollpool.h"

#include <errno.h>

#include "log.h"


namespace Network
{
	CPollPool::CPollPool(uint64_t epoll_size /* = 4096 */)
	{
		m_epoll_fd = -1;
		m_epoll_size = epoll_size;
		m_epoll_events = NULL;
	}

	CPollPool::~CPollPool()
	{
		if(NULL != m_epoll_events)
		{
			delete[] m_epoll_events;
			m_epoll_events = NULL;
		}

		if(-1 != m_epoll_fd)
		{
			close(m_epoll_fd);
			m_epoll_fd = -1;
		}
	}

	int32_t CPollPool::Init()
	{
		m_epoll_events = new epoll_event[m_epoll_size];
		if(NULL == m_epoll_events)
		{
			log_error("Not enough memory for new [%llu] epoll_event.", static_cast<unsigned long long>(m_epoll_size));
			return -1;
		}

		if(-1 == (m_epoll_fd = epoll_create(m_epoll_size)))
		{
			log_error("Create epoll fd failed, errno [%d], strerror [%s].", errno, strerror(errno));
			return -1;
		}

		return 0;
	}

	int32_t CPollPool::Attach(CPollUnit *unit)
	{
		if(m_poll_units.size() >= m_epoll_size || NULL == unit)
		{
			return -1;
		}

		epoll_event *event = unit->GetEpollEvent();
		if(NULL == event)
		{
			return -1;
		}

		if(0 != (epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, unit->GetFD(), event)))
		{
			log_error("Epoll EPOLL_CTL_ADD fd [%d] failed.", unit->GetFD());
			return -1;
		}

		unit->m_owner = this;
		m_poll_units[unit->GetFD()] = unit;
		return 0;
	}

	int32_t CPollPool::Detach(CPollUnit *unit)
	{
		if(NULL == unit)
		{
			return -1;
		}

		if(m_poll_units.end() == m_poll_units.find(unit->GetFD()))
		{
			log_error("This pollunit [%d] not find in pollpool.", unit->GetFD());
			return -1;
		}

		if(0 != (epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, unit->GetFD(), unit->GetEpollEvent())))
		{
			log_error("Epoll EPOLL_CTL_DEL fd [%d] failed.", unit->GetFD());
			return -1;
		}

		unit->m_owner = NULL;
		m_poll_units.erase(unit->GetFD());
		return 0;
	}

	int32_t CPollPool::EpollCtl(int32_t op, int32_t fd, epoll_event *ep_event)
	{
		if(0 != (epoll_ctl(m_epoll_fd, op, fd, ep_event)))
		{
			log_error("Epoll ctl [%d] fd [%d] failed.", op, fd);
			return -1;
		}

		return 0;
	}

	int32_t CPollPool::EpollCtl(int32_t op, CPollUnit *unit)
	{
		return EpollCtl(op, unit->GetFD(), unit->GetEpollEvent())
	}

	int32_t CPollPool::EpollCtl(int32_t op, CPollUnit *unit, epoll_event *ep_event)
	{
		if(NULL == ep_event)
		{
			return EpollCtl(op, unit);
		}

		unit->m_events = ep_event->events;
		return EpollCtl(op, unit);
	}

	int32_t CPollPool::EpollCtl(int32_t op, CPollUnit *unit, uint32_t events)
	{
		unit->m_events = events;
		return EpollCtl(op, unit);
	}

	int32_t CPollPool::EpollProcess(int32_t timeout /* = 3000 */)
	{
		int32_t event_count = epoll_wait(m_epoll_fd, m_epoll_events, m_epoll_size, timeout);
		for(int32_t event_index = 0; event_index < event_count; ++ event_index)
		{
			epoll_event *one_event = m_epoll_events[event_index];
			POLLUNIT_MAP::iterator one_unit_it = m_poll_units.find(one_event->data.fd);
			if(m_poll_units.end() == one_unit_it)
			{
				log_error("This fd [%d] is invalid.", one_event->data.fd);
				EpollCtl(EPOLL_CTL_DEL, one_event->data.fd, one_event);
				close(one_event->data.fd);
				continue;
			}

			CPollUnit *unit = one_unit_it->second;

			if(one_event->events & (EPOLLHUP | EPOLLERR))
			{
				unit->HangupNotify();
				continue;
			}
			
			if(one_event->events & EPOLLOUT)
			{
				unit->OutputNotify();
			}

			if(one_event->events & EPOLLIN)
			{
				unit->InputNotify();
			}
		}

		return event_count;
	}
};
