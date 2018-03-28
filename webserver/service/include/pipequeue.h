#ifndef __PIPE_QUEUE_H__
#define __PIPE_QUEUE_H__


#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "pollpool.h"
#include "pollunit.h"


using namespace Network;


const uint32_t kCommonConstant = 32;


template<typename T>
class CPipeQueueReader : public CPollUnit
{
public:
	CPipeQueueReader()
		: m_data_num_offset(0)
		, m_data_num_end(0)
		, m_remaining_size(0)
	{
		
	}

	virtual ~CPipeQueueReader()
	{
		
	}

public:
	int32_t AttachPool(CPollPool *pool, int32_t fd)
	{
		m_fd = fd;
		SetNonBlock();
		EnableInput();
		DisableOutput();
		return pool->AttachUnit(this);
	}

public:
	virtual void HangupNotify()
	{
		
	}

	virtual void RDHupNotify()
	{
		
	}
	
	virtual void InputNotify()
	{
		log_debug("CPipeQueueReader::InputNotify [%d] begin.", m_fd);
		T data;
		uint32_t task_num = 0;
		while(++task_num <= (kCommonConstant * 2) && (data = Pop()) != NULL)
		{
			this->TaskNotify(data);
		}
		log_debug("CPipeQueueReader::InputNotify [%d] end.", m_fd);
	}

	virtual void OutputNotify()
	{
		
	}

	virtual void TaskNotify(T data)
	{
		
	}

private:
	T Pop()
	{
		if(m_data_num_offset >= m_data_num_end)
		{
			if(m_remaining_size > 0)
			{
				memmove(reinterpret_cast<void *>(m_buf), reinterpret_cast<void *>(m_buf + m_data_num_offset), m_remaining_size);
			}
			m_data_num_offset = 0;
			m_data_num_end = 0;
			
			int32_t read_size = read(m_fd, reinterpret_cast<void *>(reinterpret_cast<char *>(m_buf) + m_remaining_size), sizeof(m_buf) - m_remaining_size);
			if(read_size <= 0)
			{
				return NULL;
			}
			m_remaining_size += read_size;
			m_data_num_end = m_remaining_size / sizeof(T);
			log_debug("The fd [%d] read [%d] bytes data.", m_fd, read_size);
		}

		m_remaining_size -= sizeof(T);
		return m_buf[m_data_num_offset++];
	}

private:
	int32_t m_data_num_offset;
	int32_t m_data_num_end;
	int32_t m_remaining_size;
	T m_buf[kCommonConstant];
};


template<typename T>
class CPipeQueueWriter : public CPollUnit
{
public:
	CPipeQueueWriter()
		: m_buf(NULL)
		, m_data_size_offset(0)
		, m_data_size_end(0)
		, m_buf_volume(kCommonConstant)
	{
		m_buf = reinterpret_cast<T *>(malloc(m_buf_volume * sizeof(T)));
	}

	virtual ~CPipeQueueWriter()
	{
		if(NULL != m_buf)
		{
			free(m_buf);
		}
	}

public:
	virtual void InputNotify()
	{
		
	}
	virtual void OutputNotify()
	{
		log_debug("CPipeQueueWriter::OutputNotify [%d] begin.", m_fd);
		if(m_data_size_end > m_data_size_offset)
		{
			int32_t write_size = write(m_fd, reinterpret_cast<void *>(m_buf + m_data_size_offset), m_data_size_end - m_data_size_offset);
			if(write_size > 0)
			{
				m_data_size_offset += write_size;
			}

			log_debug("The fd [%d] write [%d] bytes data.", m_fd, write_size);
		}
		if((m_data_size_end - m_data_size_offset) <= 0)
		{
			m_data_size_end = m_data_size_offset = 0;
			DisableOutput();
		}

		GetOwner()->ModifyEvent(this);
		log_debug("CPipeQueueWriter::OutputNotify [%d] end.", m_fd);
	}

public:
	int32_t AttachPool(CPollPool *pool, int32_t fd)
	{
		m_fd = fd;
		SetNonBlock();
		DisableInput();
		DisableOutput();
		return pool->AttachUnit(this);
	}

	int32_t Push(T data)
	{
		if(m_data_size_end >= (m_buf_volume * sizeof(T)))
		{
			if(m_data_size_offset > sizeof(T))
			{
				memmove(m_buf, reinterpret_cast<void *>(reinterpret_cast<char *>(m_buf) + m_data_size_offset),  m_data_size_end - m_data_size_offset);
				m_data_size_end -= m_data_size_offset;
			}
			else
			{
				T *newbuf = reinterpret_cast<T *>(realloc(m_buf, 2 * m_buf_volume * sizeof(T)));
				if(NULL == newbuf)
				{
					return -1;
				}

				m_buf = newbuf;
				m_buf_volume *= 2;
			}

			m_data_size_offset = 0;
		}

		reinterpret_cast<T *>(reinterpret_cast<char *>(m_buf) + m_data_size_end)[0] = data;
		m_data_size_end += sizeof(T);
		EnableOutput();
		GetOwner()->ModifyEvent(this);
		
		if(((m_data_size_end - m_data_size_offset) >= (kCommonConstant * sizeof(T))) && (0 == m_data_size_end % (kCommonConstant * sizeof(T))))
		{
			OutputNotify();
		}

		return 0;
	}

private:
	T *m_buf;
	int32_t m_data_size_offset;
	int32_t m_data_size_end;
	int32_t m_buf_volume;
};

template<typename T>
class CPipeQueue : public CPipeQueueReader<T>
{
	
public:
	CPipeQueue()
	{
		
	}
	virtual ~CPipeQueue()
	{
		
	}

public:
	int32_t AttachPool(CPollPool *from, CPollPool *to)
	{
		int fd[2];
		int ret = pipe(fd);
		if(0 != ret)
		{
			return ret;
		}
		CPipeQueueReader<T>::AttachPool(to, fd[0]);
		m_writer.AttachPool(from, fd[1]);

		return 0;
	}

	int32_t Push(T data)
	{
		return m_writer.Push(data);
	}

private:
	CPipeQueueWriter<T> m_writer;
};


#endif
