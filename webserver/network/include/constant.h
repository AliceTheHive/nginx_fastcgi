#ifndef __CONSTANT_H__
#define __CONSTANT_H__


#include <stdint.h>


namespace Network
{
	const uint64_t kDefaultBufferLength = 1024 * 1024;
	const uint64_t kDefaultPollerSize = 4096;
	const uint32_t kDefaultIOVCount = 128;
};


#endif
