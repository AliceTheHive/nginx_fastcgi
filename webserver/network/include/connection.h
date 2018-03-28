#ifndef __CONNECTION_H__
#define __CONNECTION_H__


#include <string>

#include "buffers.h"
#include "packet.h"
#include "pollunit.h"


namespace Network
{
	class CListener;

	
	class CConnection : public CPollUnit
	{
	public:
		CConnection(int32_t fd);
		virtual ~CConnection();

	public:
		int64_t SendPacket(const CPacket &packet);
		void SetListener(CListener *listener);
		
	public:
		virtual void InputNotify();
		virtual void OutputNotify();
		virtual void OnPacket(CPacket *packet);
		virtual void OnDecodeError();

	public:
		virtual int64_t Decode(CPackets &packets) = 0;

	protected:
		std::string m_local_address;
		uint16_t m_local_port;
		std::string m_remote_address;
		uint16_t m_remote_port;
		CBuffers m_recv_buffers;
		CBuffers m_send_buffers;

	private:
		CListener *m_listener;
	};
};


#endif
