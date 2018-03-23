#ifndef __RESPONSE_PACKET_H__
#define __RESPONSE_PACKET_H__


#include "packet.h"
#include "web_structure.h"


using namespace Network;


class CResponsePacket : public CPacket
{
public:
	CResponsePacket();
	virtual ~CResponsePacket();

public:
	virtual int64_t Decode(const CBuffers &buffers, uint64_t pos);
	virtual int64_t Encode(CBuffers &buffers) const;

public:
	ProtocolHeader m_header;
	const char *m_body;
};


#endif
