#include "response_packet.h"

#include <arpa/inet.h>

#include "buffer.h"
#include "log.h"


CResponsePacket::CResponsePacket()
{
	m_body = NULL;
}

CResponsePacket::~CResponsePacket()
{
	if(NULL != m_body)
	{
		delete[] m_body;
		m_body = NULL;
	}
}

int64_t CResponsePacket::Decode(const CBuffers &buffers, uint64_t pos)
{
	return 0;
}

int64_t CResponsePacket::Encode(CBuffers &buffers) const
{
	CBuffer *header_buffer = new CBuffer(sizeof(m_header));
	if(NULL == header_buffer)
	{
		log_error("Not enough memory for new [%lu] CBuffer.", sizeof(m_header));
		return -1;
	}

	uint32_t body_length = ntohl(m_header.data_length);
	CBuffer *body_buffer = new CBuffer(body_length);
	if(NULL == body_buffer)
	{
		log_error("Not enough memory for new [%u] CBuffer.", body_length);
		delete header_buffer;
		header_buffer = NULL;
		return -1;
	}

	header_buffer->SetData(&m_header, sizeof(m_header));
	body_buffer->SetData(m_body, body_length);
	buffers.push_back(header_buffer);
	buffers.push_back(body_buffer);

	return (sizeof(m_header) + body_length);
}
