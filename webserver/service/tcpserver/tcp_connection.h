#ifndef __TCP_CONNECTION_H__
#define __TCP_CONNECTION_H__


#include "connection.h"


using namespace Network;


class CClientProcess;


class CTcpConnection : public CConnection
{
public:
	CTcpConnection(CClientProcess *client, int32_t fd);
	virtual ~CTcpConnection();

public:
	virtual void OnPacket(CPacket *packet);
	virtual int64_t Decode(CPackets &packets);

private:
	int64_t DecodePacket(int64_t pos, CPacket *&packet);

private:
	CClientProcess *m_successor;
};


#endif
