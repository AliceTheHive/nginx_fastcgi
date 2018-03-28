#ifndef __WEB_CONSTANT_H__
#define __WEB_CONSTANT_H__


#include <stdint.h>


#define DELETE(pointer) \
	do \
	{ \
	if(NULL != pointer) \
	{ \
	delete pointer; \
	pointer = NULL; \
	} \
	} while(0)


const uint16_t kProtocolMagic = 0xFDFC;


enum CmdType
{
	CT_MIN = 0,
	CT_A = 1,
	CT_B,
	CT_C,
	CT_D,
	CT_MAX
};


#endif
