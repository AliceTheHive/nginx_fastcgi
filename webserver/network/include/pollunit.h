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
		CPollUnit();
		virtual ~CPollUnit();

	public:
		void EnableInput();
		void DisableInput();
		void EnableOutput();
		void DisableOutput();
		void EnableRDHup();
		void DisableRDHup();
		void EnableET();
		void DisableET();
		void EnableOneShot();
		void DisableOneShot();
		int32_t SetCloseExec();
		int32_t ClearCloseExec();
		int32_t SetFlag(int option);
		int32_t ClearFlag(int option);
		int32_t SetNonBlock();
		int32_t ClearNonBlock();
		CPollPool *GetOwner();
		int32_t GetFD();
		uint32_t GetEvents();
		epoll_event *GetEpollEvent();

	public:
		void Close();

	public:
		virtual bool DoSomething();
		virtual void HangupNotify();
		virtual void RDHupNotify();
		virtual void Clear();
		
	public:
		virtual void InputNotify() = 0;
		virtual void OutputNotify() = 0;
			
	protected:
		CPollPool *m_owner;
		int32_t m_fd;
		uint32_t m_events;
		epoll_event m_epoll_event;
	};
};


#endif
