#ifndef __PACKET_H__
#define __PACKET_H__


#include <vector>


namespace Network
{
	class CPacket
	{
	public:
		CPacket();
		virtual ~CPacket();
	};

	typedef std::vector<CPacket *> CPackets;
};


#endif
