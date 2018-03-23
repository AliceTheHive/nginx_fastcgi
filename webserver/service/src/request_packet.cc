#include "request_packet.h"

#include <arpa/inet.h>

#include <string>

#include "log.h"


CRequestPacket::CRequestPacket()
{
	m_body = NULL;
}

CRequestPacket::~CRequestPacket()
{
	if(NULL != m_body)
	{
		delete[] m_body;
		m_body = NULL;
	}
}

int64_t CRequestPacket::Decode(const CBuffers &buffers, uint64_t pos)
{
	int64_t decode_length = 0;
	int64_t header_length = DecodeHeader(buffers, pos);
	if(header_length > 0)
	{
		int64_t body_length = DecodeBody(buffers, pos + header_length, m_header.data_length);
		if(body_length > 0)
		{
			decode_length = header_length + body_length;
		}
		else if(0 == body_length)
		{
			decode_length = 0;
		}
		else
		{
			decode_length = -1;
		}
	}
	else if(0 == header_length)
	{
		decode_length = 0;
	}
	else
	{
		decode_length = -1;
	}

	return decode_length;
}

int64_t CRequestPacket::Encode(CBuffers &buffers) const
{
	return 0;
}


int64_t CRequestPacket::DecodeHeader(const CBuffers &buffers, uint64_t pos)
{
	int64_t decode_length = 0;
	if(buffers.DataLength() < (sizeof(m_header) + pos))
	{
		return decode_length;
	}

	if(sizeof(m_header) != buffers.GetData(pos, sizeof(m_header), reinterpret_cast<void *>(&m_header)))
	{
		return decode_length;
	}

	m_header.magic = ntohs(m_header.magic);
	m_header.cmd = ntohs(m_header.cmd);
	m_header.data_length = ntohl(m_header.data_length);
	if(kProtocolMagic != m_header.magic
	   || m_header.cmd <= CT_MIN
	   || m_header.cmd >= CT_MAX)
	{
		decode_length = -1;
	}
	else
	{
		decode_length = sizeof(m_header);
	}

	return decode_length;
}

int64_t CRequestPacket::DecodeBody(const CBuffers &buffers, uint64_t pos, uint32_t data_length)
{
	int64_t decode_length = 0;
	if(buffers.DataLength() < (data_length + pos))
	{
		return decode_length;
	}

	char *body = new char[data_length];
	if(NULL == body)
	{
		log_error("Not enough memory for new [%u] char array.", data_length);
		return decode_length;
	}
	if(data_length != buffers.GetData(pos, data_length, reinterpret_cast<void *>(body)))
	{
		delete[] body;
		body = NULL;
		return decode_length;
	}

	decode_length = data_length;
	m_body = body;
	return decode_length;
}
