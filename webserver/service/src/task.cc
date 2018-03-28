#include "task.h"

#include <arpa/inet.h>


CTask::CTask()
{
	m_connection = NULL;
	m_req_packet = NULL;
}

CTask::~CTask()
{
	m_connection = NULL;
	if(NULL != m_req_packet)
	{
		delete m_req_packet;
		m_req_packet = NULL;
	}
}

void CTask::SetConnection(CTcpConnection *connection)
{
	m_connection = connection;
}

CTcpConnection *CTask::GetConnection()
{
	return m_connection;
}

void CTask::SetRequestPacket(CRequestPacket *req_packet)
{
	m_req_packet = req_packet;
	m_res_packet.m_header = req_packet->m_header;
	m_res_packet.m_header.data_length = 0;
}

void CTask::SetResponseHeader(const ProtocolHeader &header)
{
	m_res_packet.m_header = header;
}

void CTask::SetResponseBody(const char *body, uint32_t length /* = 0 */)
{
	m_res_packet.m_body = body;
	m_res_packet.m_header.data_length = htonl(length);
}

void CTask::SetResponseDataLength(uint32_t length)
{
	m_res_packet.m_header.data_length = htonl(length);
}

const CResponsePacket &CTask::GetResponsePacket()
{
	return m_res_packet;
}
