#ifndef __BUFFER_H__
#define __BUFFER_H__


#include "constant.h"


namespace Network
{
	class CBuffer
	{
	public:
		CBuffer(uint64_t size = kDefaultBufferLength);
		~CBuffer();

	public:
		uint64_t BufferLength();
		uint64_t DataLength();
		uint64_t EraseFront(uint64_t count);
		uint64_t EraseBack(uint64_t count);
		void SetData(const void *data, uint64_t length);
		void *GetData();
		void Clear();

	private:
		CBuffer(const CBuffer &other);
		const CBuffer &operator=(const CBuffer &other);
		
	private:
		uint64_t m_buffer_length;
		uint64_t m_data_end;
		uint64_t m_data_offset;
		uint8_t *m_data;
	};

};


#endif
