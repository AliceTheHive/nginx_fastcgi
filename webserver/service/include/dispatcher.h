#ifndef __DISPATCHER_H__
#define __DISPATCHER_H__


#include <list>

#include "log.h"
#include "pollpool.h"


template<typename T>
class CTaskPipe;


using namespace Network;


template<typename T>
class CTaskDispatcher
{
public:
	CTaskDispatcher(CPollPool *pool = NULL)
	{
		m_owner = pool;
		m_successor = NULL;
	}

	virtual ~CTaskDispatcher()
	{
		
	}

public:
	void AttachPool(CPollPool *pool)
	{
		if(NULL == m_owner)
		{
			m_owner = pool;
		}
	}

	CPollPool *GetOwner()
	{
		return m_owner;
	}

	void BindDispatcher(CTaskDispatcher<T> *successor)
	{
		if(m_owner == successor->GetOwner())
		{
			m_successor = successor;
		}
		else
		{
			CTaskPipe<T> *tp = new CTaskPipe<T>();
			tp->BindDispatcher(this, successor);
		}
	}

public:
	virtual void TaskNotify(T *data) = 0;

protected:
	CPollPool *m_owner;
	CTaskDispatcher<T> *m_successor;
};

template<typename T>
class CReplyDispatcher
{
public:
	CReplyDispatcher()
	{
		
	}

	virtual ~CReplyDispatcher()
	{
		
	}

public:
	virtual void ReplyNotify(T *data) = 0;
};

template<typename T>
class CTaskReplyList : public std::list<CReplyDispatcher<T> *>
{
public:
	CTaskReplyList()
	{
		
	}

	virtual ~CTaskReplyList()
	{
		
	}

public:
	void Clear()
	{
		this->clear();
	}

	int32_t PushFront(CReplyDispatcher<T> *proc)
	{
		this->push_front(proc);
		return 0;
	}

	int32_t PushBack(CReplyDispatcher<T> *proc)
	{
		this->push_back(proc);
		return 0;
	}

	CReplyDispatcher<T> *PopFront()
	{
		if(this->empty())
		{
			return NULL;
		}
		else
		{
			CReplyDispatcher<T> *rd = this->front();
			this->pop_front();
			return rd;
		}
	}

	CReplyDispatcher<T> *PopBack()
	{
		if(this->empty())
		{
			return NULL;
		}
		else
		{
			CReplyDispatcher<T> *rd = this->back();
			this->pop_back();
			return rd;
		}
	}

	int32_t PushReplyDispatcher(CReplyDispatcher<T> *proc)
	{
		if(NULL == proc)
		{
			log_error("PushReplyDispatcher failed, because the dispatcher is NULL.");
			return -1;
		}
		else
		{
			if(0 != PushFront(proc))
			{
				log_error("PushReplyDispatcher failed, because push queue failed.");
				return -1;
			}

			log_debug("PushReplyDispatcher success.");
		}

		return 0;
	}

	void ReplyNotify()
	{
		CReplyDispatcher<T> *proc = PopFront();
		if(NULL == proc)
		{
			log_error("ReplyNotify failed, because no more dispatcher.");
		}
		else
		{
			proc->ReplyNotify(static_cast<T *>(this));
		}
	}
};


#endif
