#ifndef __TASK_PIPE_H__
#define __TASK_PIPE_H__


#include <list>

#include "pipequeue.h"


template<typename T>
class CTaskDispatcher;

template<typename T>
class CReplyDispatcher;


template<typename T>
class CIncomingPipe : public CPipeQueue<T *>
{
public:
	CIncomingPipe()
	{
		
	}

	virtual ~CIncomingPipe()
	{
		
	}

public:
	virtual void TaskNotify(T *data)
	{
		m_successor->TaskNotify(data);
	}

public:
	CTaskDispatcher<T> *m_successor;
};


template<typename T>
class CReturnPipe : public CPipeQueue<T *>
{
public:
	CReturnPipe()
	{
		
	}

	virtual ~CReturnPipe()
	{
		
	}

public:
	virtual void TaskNotify(T *data)
	{
		data->ReplyNotify();
	}
};

template<typename T>
class CTaskPipe : public CTaskDispatcher<T>, public CReplyDispatcher<T>
{
public:
	CTaskPipe()
	{
		s_pipelist.push_back(this);
	}

	virtual ~CTaskPipe()
	{
		
	}

public:
	virtual void TaskNotify(T *data)
	{
		data->PushReplyDispatcher(this);
		m_incoming_pipe.Push(data);
	}

	virtual void ReplyNotify(T *data)
	{
		m_return_pipe.Push(data);
	}

public:
	int32_t BindDispatcher(CTaskDispatcher<T> *from, CTaskDispatcher<T> *to)
	{
		CTaskDispatcher<T>::m_owner = from->GetOwner();
		m_incoming_pipe.AttachPool(from->GetOwner(), to->GetOwner());
		m_return_pipe.AttachPool(to->GetOwner(), from->GetOwner());
		from->BindDispatcher(this);
		m_incoming_pipe.m_successor = to;
		return 0;
	}

public:
	static void DestroyAllPipe()
	{
		CTaskPipe<T> *tp = NULL;
		while(!s_pipelist.empty())
		{
			tp = s_pipelist.front();
			if(NULL != tp)
			{
				delete tp;
			}
			s_pipelist.pop_front();
		}
	}

private:
	CIncomingPipe<T> m_incoming_pipe;
	CReturnPipe<T> m_return_pipe;
	
private:
	static std::list<CTaskPipe<T> *> s_pipelist;
};

template<typename T>
std::list<CTaskPipe<T> *> CTaskPipe<T>::s_pipelist;


#endif
