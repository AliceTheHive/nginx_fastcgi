#ifndef __POLL_POOL_H__
#define __POLL_POOL_H__


#include <stdint.h>

#include <map>


namespace Network
{
	class CPollPool
	{
	public:
		CPollPool(uint64_t epoll_size = 4096);
		~CPollPool();

	private:
		int32_t m_epoll_fd;
		uint64_t m_epoll_size;
		std::map<int32_t, CPollUnit *> m_poll_units;
	};
};


#endif
