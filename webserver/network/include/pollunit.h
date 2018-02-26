#ifndef __POLL_UNIT_H__
#define __POLL_UNIT_H__


#include <sys/epoll.h>


namespace Network
{
	class CPollPool;

	
	class CPollUnit
	{
	public:
		friend class CPollPool;
		
	public:
		CPollUnit(CPollPool *owner = NULL, int32_t fd = -1);
		virtual ~CPollUnit();

	public:
		void EnableInput();
		void DisableInput();
		void EnableOutput();
		void DisableOutput();
		void EnableRDHup();
		void DisableRDHup();
		CPollPool *GetOwner();
		int32_t GetFD();
		uint32_t GetEvents();
		epoll_event *GetEpollEvent();
			
	protected:
		CPollPool *m_owner;
		int32_t m_fd;
		uint32_t m_events;
		epoll_event m_epoll_event;
	};
};


#endif
