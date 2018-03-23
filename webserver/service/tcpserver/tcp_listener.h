#ifndef __WEB_LISTENER_H__
#define __WEB_LISTENER_H__


#include "listener.h"


using namespace Network;


class CWebListener : public CListener
{
public:
	CWebListener(std::string address, uint16_t port, int32_t rcvbuf_size = 0, int32_t sndbuf_size = 0, int32_t reuse = 1, int32_t nodelay = 1, int32_t defer_accept = 1);
	CWebListener(int32_t fd);
	virtual ~CWebListener();

public:
	virtual CConnection *CreateConnection(int32_t fd);
};


#endif
