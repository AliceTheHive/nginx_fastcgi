#ifndef __ERRCODE_H__
#define __ERRCODE_H__


#define FCGI_CODE(ACTION) \
	ACTION(FC_SUCCESS,			"success") \
	ACTION(FC_NETWORK_ERROR,	"network error") \


#define FCGI_CODE_ACTION(name, desc) ##name,
enum FcgiCode
{
	FC_MIN = -1,
	FCGI_CODE(FCGI_CODE_ACTION)
	FC_MAX,
};
#undef FCGI_CODE_ACTION


char *GetFcgiCodeStr(FcgiCode code);


#endif
