#include "buffer.h"


namespace Network
{
	CBuffer::CBuffer(uint64_t size /* = kDefaultBufferLength */)
	{
		m_data = new uint8_t[size];
		m_buffer_length = size;
		m_data_end = 0;
		m_data_offset = 0;
	}

	CBuffer::~CBuffer()
	{
		if(NULL != m_data)
		{
			delete[] m_data;
			m_data = NULL;
		}
	}

	uint64_t CBuffer::BufferLength()
	{
		return m_buffer_length;
	}

	uint64_t CBuffer::DataLength()
	{
		return (m_data_end - m_data_offset);
	}

	uint64_t CBuffer::EraseFront(uint64_t count)
	{
		uint64_t erase_size = 0;
		if(DataLength() >= count)
		{
			erase_size = count;
		}
		else
		{
			erase_size = DataLength();
		}

		m_data_offset += erase_size;

		return erase_size;
	}

	uint64_t CBuffer::EraseBack(uint64_t count)
	{
		uint64_t erase_size = 0;
		if(DataLength() >= count)
		{
			erase_size = count;
		}
		else
		{
			erase_size = DataLength();
		}

		m_data_end -= erase_size;

		return erase_size;
	}

	void CBuffer::SetData(const void *data, uint64_t length)
	{
		if(BufferLength() < length)
		{
			if(NULL != m_data)
			{
				delete[] m_data;
			}
			m_data = new uint8_t[length];
			m_buffer_length = length;
		}

		memcpy(m_data, data, length);
		m_data_end = length;
		m_data_offset = 0;
	}

	void *CBuffer::GetData()
	{
		return reinterpret_cast<void *>(m_data + m_data_offset);
	}

	void CBuffer::Clear()
	{
		m_data_end = 0;
		m_data_offset = 0;
	}
};
