#ifndef __POLL_POOL_H__
#define __POLL_POOL_H__


#include <stdint.h>

#include <sys/epoll.h>

#include <map>


namespace Network
{
	class CPollUnit;
	
	
	class CPollPool
	{
	public:
		CPollPool(uint64_t epoll_size = 4096);
		~CPollPool();

	public:
		int32_t Init();
		void UnInit();
		int32_t Attach(CPollUnit *unit);
		int32_t Detach(CPollUnit *unit);
		int32_t EpollCtl(int32_t op, int32_t fd, epoll_event *ep_event);
		int32_t EpollCtl(int32_t op, CPollUnit *unit);
		int32_t EpollCtl(int32_t op, CPollUnit *unit, epoll_event *ep_event);
		int32_t EpollCtl(int32_t op, CPollUnit *unit, uint32_t events);
		int32_t EpollProcess(int32_t timeout = 3000);

	private:
		typedef std::map<int32_t, CPollUnit *> POLLUNIT_MAP;
		
	private:
		int32_t m_epoll_fd;
		uint64_t m_epoll_size;
		epoll_event *m_epoll_events;
		POLLUNIT_MAP m_poll_units;
	};
};


#endif
