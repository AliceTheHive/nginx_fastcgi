#ifndef __BUFFERS_H__
#define __BUFFERS_H__


#include <stdint.h>

#include <list>


namespace Network
{
	class CBuffer;

	
	class CBuffers : public std::list<CBuffer *>
	{
	public:
		CBuffers();
		~CBuffers();

	public:
		void PushFront(CBuffer *buffer);
		void PushBack(CBuffer *buffer);
		void Clear(bool clean = true);
		uint64_t DataLength() const;
		uint64_t GetData(uint64_t pos, uint64_t length, void *data) const;
		uint64_t EraseFront(uint64_t count);

	private:
		uint64_t m_data_length;
	};
	
};


#endif
