#ifndef __WEB_STRUCTURE_H__
#define __WEB_STRUCTURE_H__


#include "web_constant.h"


struct ProtocolHeader
{
	uint16_t magic;
	uint16_t cmd;
	uint32_t data_length;

	ProtocolHeader()
	{
		magic = kProtocolMagic;
		cmd = CT_MIN;
		data_length = 0;
	}

	void CopyData(ProtocolHeader *self, const ProtocolHeader *other)
	{
		self->magic = other->magic;
		self->cmd = other->cmd;
		self->data_length = other->data_length;
	}

	ProtocolHeader(const ProtocolHeader &other)
	{
		CopyData(this, &other);
	}

	const ProtocolHeader &operator =(const ProtocolHeader &other)
	{
		CopyData(this, &other);
		return (*this);
	}
};


#endif
