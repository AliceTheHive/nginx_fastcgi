#include "task.h"


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

void CTask::SetConnection(CWebConnection *connection)
{
	m_connection = connection;
}

CWebConnection *CTask::GetConnection()
{
	return m_connection;
}

void CTask::SetRequestPacket(CRequestPacket *req_packet)
{
	m_req_packet = req_packet;
}

void CTask::SetResponseHeader(const ProtocolHeader &header)
{
	m_res_packet.m_header = header;
}

void CTask::SetResponseBody(const char *body)
{
	m_res_packet.m_body = body;
}

const CResponsePacket &CTask::GetResponsePacket()
{
	return m_res_packet;
}
