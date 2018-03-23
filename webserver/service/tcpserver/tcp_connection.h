#ifndef __TCP_CONNECTION_H__
#define __TCP_CONNECTION_H__


#include "connection.h"


using namespace Network;


class CTcpConnection : public CConnection
{
public:
	CTcpConnection(int32_t fd);
	virtual ~CTcpConnection();

public:
	virtual void OnPacket(CPacket *packet);
	virtual int64_t Decode(CPackets &packets);

private:
	int64_t DecodePacket(int64_t pos, CPacket *&packet);
};


#endif
