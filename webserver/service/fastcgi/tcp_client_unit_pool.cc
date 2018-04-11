#include "tcp_client_unit_pool.h"

#include "log.h"
#include "tcp_client_unit.h"


CTcpClientUnitPool::CTcpClientUnitPool()
{
	m_initnum = 0;
	m_cursor = 0;
	m_tcpclientunits.clear();
}

CTcpClientUnitPool::~CTcpClientUnitPool()
{
	CloseAllClient();
	m_initnum = 0;
	m_cursor = 0;
	m_tcpclientunits.clear();
}

bool CTcpClientUnitPool::Init(const CConfig &config)
{
	if(config.isMember("service") && config["service"].isArray())
	{
		const Json::Value &service = config["service"];
		if(service.size() <= 0)
		{
			log_error("The service field is invalid, config is [%s].", config.toStyledString().c_str());
			return false;
		}

		for(uint32_t service_index = 0; service_index < service.size(); ++ service_index)
		{
			const Json::Value &one_service = service[service_index];
			if(one_service.isMember("ip") && one_service["ip"].isString()
			   && one_service.isMember("port") && one_service["port"].isUInt()
			   && one_service.isMember("thread_num") && one_service["thread_num"].isUInt())
			{
				std::string ip = one_service["ip"].asString();
				uint16_t port = one_service["port"].asString();
				uint32_t thread_num = one_service["thread_num"].asUInt();
				std::ostringstream oss;
				oss << ip << ":" << port << "|";
				std::string prefix = oss.str();
				for(uint32_t thread_index = 0; thread_index < thread_num; ++ thread_index)
				{
					oss.str("");
					oss << prefix << thread_index;
					CTcpClientUnit *one_clientunit = new CTcpClientUnit(oss.str(), ip, port);
					if(NULL == one_clientunit)
					{
						log_error("Not enough memory for new CTcpClientUnit.");
						return false;
					}
					if(false == one_clientunit->Init())
					{
						log_error("Init tcp clientunit failed.");
						return false;
					}
					one_clientunit->AttachPool(this);
				}
			}
			else
			{
				log_error("The ip or port or thread_num field is inexistence or invalid, config is [%s].", config.tostyledstring().c_str());
				return false;
			}
		}
	}
	else
	{
		log_error("The service field is inexistence or invalid, config is [%s].", config.toStyledString().c_str());
		return false;
	}

	return true;
}

void CTcpClientUnitPool::Insert(CTcpClientUnit *clientunit)
{
	m_tcpclientunits.push_back(clientunit);
}

void CTcpClientUnitPool::Erase(CTcpClientUnit *clientunit)
{
	for(std::vector<CTcpClientUnit *>::iterator tcit = m_tcpclientunits.begin(); tcit != m_tcpclientunits.end(); ++ tcit)
	{
		CTcpClientUnit *one_clientunit = (*tcit);
		if(clientunit == one_clientunit)
		{
			m_tcpclientunits.erase(tcit);
			break;
		}
	}
}

CTcpClientUnit *CTcpClientUnitPool::GetOneClientUnit()
{
	CTcpClientUnit *one_clientunit = m_tcpclientunits.at(m_cursor ++);
	m_cursor %= m_tcpclientunits.size();
}

void CTcpClientUnitPool::CloseOneClientUnit(CTcpClientUnit *clientunit)
{
	if(NULL != clientunit)
	{
		clientunit->Close();
		delete clientunit;
	}
}

void CTcpClientUnitPool::CloseAllClientUnit()
{
	while(!m_tcpclientunits.empty())
	{
		CTcpClientUnit *clientunit = (*(m_tcpclientunits.begin()));
		if(NULL == clientunit)
		{
			m_tcpclientunits.erase(m_tcpclientunits.begin());
			continue;
		}
		CloseOneClientUnit(clientunit);
	}
}
