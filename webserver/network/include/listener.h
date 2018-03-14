#ifndef __LISTENER_H__
#define __LISTENER_H__


#include <stdint.h>

#include <string>

#include "pollunit.h"


namespace Network
{
	class CConnection;

	
	class CListener : public CPollUnit
	{
	public:
		CListener(std::string address, uint16_t port, int32_t rcvbuf_size = 0, int32_t sndbuf_size = 0, int32_t reuse = 1, int32_t nodelay = 1, int32_t defer_accept = 1);
		CListener(int32_t fd);
		virtual ~CListener();

	public:
		int32_t Listen(int32_t backlog = 1024);

	public:
		virtual void InputNotify();
		virtual void OutputNotify();

	public:
		virtual CConnection *CreateConnection(int32_t fd) = 0;

	private:
		CConnection *Accept();

	private:
		std::string m_address;
		uint16_t m_port;
		int32_t m_rcvbuf_size;
		int32_t m_sndbuf_size;
		int32_t m_reuse;
		int32_t m_nodelay;
		int32_t m_defer_accept;
	};
};


#endif
