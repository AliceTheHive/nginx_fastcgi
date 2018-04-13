#include "pollpool.h"

#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "log.h"
#include "pollunit.h"


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

	void CPollPool::UnInit()
	{
		while(m_poll_units.size() > 0)
		{
			POLLUNIT_MAP::iterator pit = m_poll_units.begin();
			if(m_poll_units.end() != pit)
			{				
				CPollUnit *unit = pit->second;
				this->DetachUnit(unit);
			}
			else
			{
				break;
			}
		}
	}

	int32_t CPollPool::AttachPreUnit(CPollUnit *unit)
	{
		m_poll_pres.push_back(unit);
		return 0;
	}

	int32_t CPollPool::AttachUnit(CPollUnit *unit)
	{
		if(m_poll_units.size() >= m_epoll_size || NULL == unit)
		{
			return -1;
		}

		if(0 != (EpollCtl(EPOLL_CTL_ADD, unit)))
		{
			log_error("Epoll EPOLL_CTL_ADD fd [%d] failed.", unit->GetFD());
			return -1;
		}

		unit->m_owner = this;
		m_poll_units[unit->GetFD()] = unit;
		return 0;
	}

	int32_t CPollPool::DetachUnit(CPollUnit *unit)
	{
		log_debug("CPollPool::DetachUnit [%d] begin.", unit->GetFD());
		if(NULL == unit)
		{
			return -1;
		}

		if(m_poll_units.end() == m_poll_units.find(unit->GetFD()))
		{
			log_error("This pollunit [%d] not find in pollpool.", unit->GetFD());
			return -1;
		}

		if(0 != (EpollCtl(EPOLL_CTL_DEL, unit)))
		{
			log_error("Epoll EPOLL_CTL_DEL fd [%d] failed.", unit->GetFD());
			return -1;
		}

		unit->m_owner = NULL;
		m_poll_units.erase(unit->GetFD());
		log_debug("CPollPool::DetachUnit [%d] end.", unit->GetFD());
		return 0;
	}

	int32_t CPollPool::EpollCtl(int32_t op, int32_t fd, epoll_event *ep_event)
	{
		if(0 != (epoll_ctl(m_epoll_fd, op, fd, ep_event)))
		{
			log_error("Epoll ctl [%d] fd [%d] failed, errno [%d], strerror [%s].", op, fd, errno, strerror(errno));
			return -1;
		}

		return 0;
	}

	int32_t CPollPool::EpollCtl(int32_t op, CPollUnit *unit)
	{
		return EpollCtl(op, unit->GetFD(), unit->GetEpollEvent());
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

	int32_t CPollPool::ModifyEvent(CPollUnit *unit)
	{
		return EpollCtl(EPOLL_CTL_MOD, unit);
	}

	int32_t CPollPool::DoOtherSomething()
	{
		for(std::list<CPollUnit *>::iterator pit = m_poll_pres.begin(); pit != m_poll_pres.end();)
		{
			CPollUnit *pre_unit = (*pit);
			if(NULL != pre_unit)
			{
				if(true == pre_unit->DoSomething())
				{
					pit = m_poll_pres.erase(pit);
				}
			}
			else
			{
				pit = m_poll_pres.erase(pit);
			}
		}

		return 0;
	}

	int32_t CPollPool::EpollProcess(int32_t timeout /* = 3000 */)
	{
		if(m_poll_units.size() <= 0)
		{
			return 0;
		}
		int32_t event_count = epoll_wait(m_epoll_fd, m_epoll_events, m_epoll_size, timeout);
		for(int32_t event_index = 0; event_index < event_count; ++ event_index)
		{
			epoll_event *one_event = &m_epoll_events[event_index];
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

			if(one_event->events & EPOLLRDHUP)
			{
				unit->RDHupNotify();
				continue;
			}
			
			if(one_event->events & EPOLLOUT)
			{
				log_debug("The fd [%d] EPOLLOUT.", one_event->data.fd);
				unit->OutputNotify();
			}

			if(one_event->events & EPOLLIN)
			{
				log_debug("The fd [%d] EPOLLIN.", one_event->data.fd);
				unit->InputNotify();
			}
		}

		return event_count;
	}
};
