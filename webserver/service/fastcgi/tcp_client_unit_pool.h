#ifndef __TCP_CLIENT_POOL_H__
#define __TCP_CLIENT_POOL_H__


#include <stdint.h>

#include <vector>


class CTcpClientUnit;


class CTcpClientUnitPool
{
public:
	CTcpClientUnitPool();
	~CTcpClientUnitPool();

public:
	uint32_t GetInitNum()
	{
		return m_initnum;
	}

public:
	bool Init(CConfig &config);
	void Insert(CTcpClientUnit *clientunit);
	void Erase(CTcpClientUnit *clientunit);
	CTcpClientUnit *GetOneClientUnit();
	void CloseOneClientUnit(CTcpClientUnit *clientunit);
	void CloseAllClientUnit();

private:
	uint32_t m_initnum;
	uint32_t m_cursor;
	std::vector<CTcpClientUnit *> m_tcpclientunits;
};


#endif
