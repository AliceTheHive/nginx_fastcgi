#include "errcode.h"


#define FCGI_CODE_ACTION(name, desc) desc,
static char *gs_fcgicode_desc[] =
{
	FCGI_CODE(FCGI_CODE_ACTION)
};
#undef FCGI_CODE_ACTION


char *GetFcgiCodeStr(FcgiCode code)
{
	if(code <= FC_MIN || code >= FC_MAX)
	{
		return "unknown";
	}

	return gs_fcgicode_desc[code];
}
