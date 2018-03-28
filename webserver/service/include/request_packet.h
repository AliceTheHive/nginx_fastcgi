#ifndef __REQUEST_PACKET_H__
#define __REQUEST_PACKET_H__


#include "web_structure.h"
#include "packet.h"


using namespace Network;


class CTask;


class CRequestPacket : public CPacket
{
public:
	friend class CTask;
	
public:
	CRequestPacket();
	virtual ~CRequestPacket();
public:
	virtual int64_t Decode(const CBuffers &buffers, uint64_t pos);
	virtual int64_t Encode(CBuffers &buffers) const;

private:
	int64_t DecodeHeader(const CBuffers &buffers, uint64_t pos);
	int64_t DecodeBody(const CBuffers &buffers, uint64_t pos, uint32_t data_length);

private:
	ProtocolHeader m_header;
	char *m_body;
};


#endif
