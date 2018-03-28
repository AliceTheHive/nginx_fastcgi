#include "work_process.h"

#include <string.h>

#include <string>

#include "log.h"


CWorkProcess::CWorkProcess(CPollPool *pool /* = NULL */)
	: CTaskDispatcher<CTask>(pool)
{
	
}

CWorkProcess::~CWorkProcess()
{
	
}

void CWorkProcess::TaskNotify(CTask *data)
{
	log_debug("CWorkProcess::TaskNotify");
	std::string res = "{\"code\":1}";
	char *res_char = new char[res.length()];
	memmove(res_char, res.c_str(), res.length());
	data->SetResponseBody(res_char, res.length());
	data->ReplyNotify();
}
