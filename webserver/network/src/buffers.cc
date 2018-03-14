#include "buffers.h"

#include <string.h>

#include "buffer.h"


namespace Network
{
	CBuffers::CBuffers()
	{
		m_data_length = 0;
	}

	CBuffers::~CBuffers()
	{
		Clear();
	}

	void CBuffers::PushFront(CBuffer *buffer)
	{
		if(NULL != buffer)
		{
			m_data_length += buffer->DataLength();
			this->push_front(buffer);
		}
	}

	void CBuffers::PushBack(CBuffer *buffer)
	{
		if(NULL != buffer)
		{
			m_data_length += buffer->DataLength();
			this->push_back(buffer);
		}
	}

	void CBuffers::Clear(bool clean /* = true */)
	{
		if(true == clean)
		{
			for(std::list<CBuffer *>::iterator it = this->begin(); it != this->end(); ++ it)
			{
				if(NULL != (*it))
				{
					delete (*it);
				}
			}
		}

		this->clear();
		this->m_data_length = 0;
	}

	uint64_t CBuffers::DataLength() const
	{
		return m_data_length;
	}

	uint64_t CBuffers::GetData(uint64_t pos, uint64_t length, void *data)
	{
		if(pos >= DataLength() || 0 == length || NULL == data)
		{
			return 0;
		}

		uint64_t cur_pos = 0;
		uint64_t end_pos = pos + length;
		uint64_t prev_pos = 0;
		uint64_t real_length = 0;
		for(std::list<CBuffer *>::const_iterator it = this->begin(); it != this->end(); ++ it)
		{
			cur_pos += (*it)->DataLength();
			if(cur_pos > pos)
			{
				uint64_t offset = (pos > prev_pos) ? (pos - prev_pos) : 0;
				uint64_t copy_length = (cur_pos < end_pos) ? (cur_pos - prev_pos - offset) : (end_pos - prev_pos - offset);
				memcpy(static_cast<uint8_t *>(data) + real_length, static_cast<uint8_t *>((*it)->GetData()) + offset, copy_length);
				real_length += copy_length;
			}

			prev_pos = cur_pos;
			if(end_pos <= cur_pos)
			{
				break;
			}
		}

		return real_length;
	}

	uint64_t CBuffers::EraseFront(uint64_t count)
	{
		if(0 == count)
		{
			return 0;
		}

		uint64_t erase_length = 0;
		while(erase_length < count && !this->empty())
		{
			if((erase_length + this->front()->DataLength()) > count)
			{
				this->front()->EraseFront(count - erase_length);
				erase_length = count;
			}
			else
			{
				erase_length += this->front()->DataLength();
				CBuffer *buffer = this->front();
				this->pop_front();
				delete buffer;
			}
		}

		m_data_length -= erase_length;
		return erase_length;
	}
};
