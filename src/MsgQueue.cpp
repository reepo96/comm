// {{{RME classifier 'Logical View::MsgQueue'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "MsgQueue.h"
#endif

#include <RTSystem/Communication.h>
#include <MsgQueue.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* 文件名称：MsgQueue.cpp
* 文件标识：
* 摘    要：任务队列类
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

#pragma warning( disable : 4786 )

const int   QUEUE_MAXSIZE = 200;    //定义队列最大元素个数

// }}}USR
// }}}RME

// {{{RME operation 'MsgQueue()'
MsgQueue::MsgQueue( void )
	// {{{RME tool 'OT::Cpp' property 'ConstructorInitializer'
	// {{{USR

	// }}}USR
	// }}}RME
{
	// {{{USR
	m_hEvent = ::CreateEvent(NULL,FALSE,TRUE,NULL);
	// }}}USR
}
// }}}RME

// {{{RME operation '~MsgQueue()'
MsgQueue::~MsgQueue( void )
{
	// {{{USR
	if(m_hEvent != NULL)
	{
		::SetEvent(m_hEvent);
		::CloseHandle(m_hEvent);
		m_hEvent = NULL;
	}

	while(!m_msgQue.empty())
	{
		Task *pTask = m_msgQue.front();
		m_msgQue.pop_front();
		delete pTask;
		pTask = NULL;
	}

	// }}}USR
}
// }}}RME

// {{{RME operation 'SendMsg(Task*)'
bool MsgQueue::SendMsg( Task * task )
{
	// {{{USR
	if(m_hEvent != NULL)
	{
		::WaitForSingleObject(m_hEvent,INFINITE);
	}

	bool    bResult = true;

	if(m_msgQue.size() >= QUEUE_MAXSIZE)
	{
		bResult = false;
	}
	else
	{
		m_msgQue.push_front(task);
		bResult = true;
	}

	if(m_hEvent != NULL)
	{
		::SetEvent(m_hEvent);
	}
	return bResult;
	// }}}USR
}
// }}}RME

// {{{RME operation 'ReceiveMsg()'
Task * MsgQueue::ReceiveMsg( void )
{
	// {{{USR
	if(m_hEvent != NULL)
	{
		::WaitForSingleObject(m_hEvent,INFINITE);
	}

	Task *pTask = NULL;
	if(!m_msgQue.empty())
	{
		pTask = m_msgQue.back();
		m_msgQue.pop_back();
	}

	if(m_hEvent != NULL)
	{
		::SetEvent(m_hEvent);
	}

	return pTask;
	// }}}USR
}
// }}}RME

// {{{RME operation 'ReplaceOrAddMsg(Task*)'
bool MsgQueue::ReplaceOrAddMsg( Task * task )
{
	// {{{USR
	if(m_hEvent != NULL)
	{
		::WaitForSingleObject(m_hEvent,INFINITE);
	}

	bool bHasReplace = false;

	deque< Task * >::iterator it = m_msgQue.begin();
	for(;it != m_msgQue.end();it++)
	{
		if(task->cmd == (*it)->cmd)
		{
			Task *pTmpTask = (*it);
			(*it) = task;
			delete pTmpTask;
			pTmpTask = NULL;

			bHasReplace = true;
			break;
		}
	}

	if(m_hEvent != NULL)
	{
		::SetEvent(m_hEvent);
	}

	if(bHasReplace)
	{
		return true;
	}
	else
	{
		return SendMsg(task);
	}
	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR
bool MsgQueue::IsEmpty()
{
    if(m_msgQue.size() == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool MsgQueue::IsFull()
{
    if(m_msgQue.size() == QUEUE_MAXSIZE)
    {
        return true;
    }
    else
    {
        return false;
    }
}

Task * MsgQueue::FindMsg( int msgId )
{
	if(m_hEvent != NULL)
	{
		::WaitForSingleObject(m_hEvent,INFINITE);
	}

	Task *pTask = NULL;
	deque< Task * >::iterator it = m_msgQue.begin();
	for(;it != m_msgQue.end();it++)
	{
		if( msgId == (*it)->cmd.msgId)
		{
			pTask = (*it);
			m_msgQue.erase(it);
			break;
		}
	}

	if(m_hEvent != NULL)
	{
		::SetEvent(m_hEvent);
	}

	return pTask;
}

void MsgQueue::ClearAll()
{
	Task *pTask = ReceiveMsg();
	while( pTask != NULL )
	{
		delete pTask;
		pTask = NULL;
		pTask = ReceiveMsg();
	}
}
// }}}USR
// }}}RME

// }}}RME
