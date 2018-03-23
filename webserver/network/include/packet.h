#ifndef __PACKET_H__
#define __PACKET_H__


#include <stdint.h>

#include <vector>

#include "buffers.h"


namespace Network
{
	class CPacket
	{
	public:
		CPacket();
		virtual ~CPacket();

	public:
		virtual int64_t Decode(const CBuffers &buffers, uint64_t pos) = 0;
		virtual int64_t Encode(CBuffers &buffers) const = 0;
	};

	typedef std::vector<CPacket *> CPackets;
};


#endif
