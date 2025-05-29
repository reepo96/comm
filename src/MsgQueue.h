// {{{RME classifier 'Logical View::MsgQueue'

#ifndef MsgQueue_H
#define MsgQueue_H

#ifdef PRAGMA
#pragma interface "MsgQueue.h"
#endif

#include <RTSystem/Communication.h>

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
#include "public.h"
#include "Task.h"
#include <deque>
using namespace std;

/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* 文件名称：MsgQueue.h
* 文件标识：
* 摘    要：任务队列类
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

struct Task;

#pragma warning( disable : 4786 )

// }}}USR
// }}}RME

class MsgQueue
{
public:
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR

		HANDLE m_hEvent;

		bool IsEmpty();
		bool IsFull();

		Task * FindMsg( int msgId );
		void ClearAll();

	// }}}USR
	// }}}RME

protected:
	// {{{RME tool 'OT::Cpp' property 'ProtectedDeclarations'
	// {{{USR

	// }}}USR
	// }}}RME

private:
	// {{{RME tool 'OT::Cpp' property 'PrivateDeclarations'
	// {{{USR

	// }}}USR
	// }}}RME
	// {{{RME classAttribute 'm_msgQue'
	// {{{RME general 'documentation'
	/* {{{USR
	保存任务信息的队列
	   }}}USR */
	// }}}RME
	deque< Task * > m_msgQue;
	// }}}RME

public:
	// {{{RME operation 'MsgQueue()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	MsgQueue( void );
	// }}}RME
	// {{{RME operation '~MsgQueue()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	virtual ~MsgQueue( void );
	// }}}RME
	// {{{RME operation 'SendMsg(Task*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       SendMsg
	    Description:    将任务放到队列中
	    Calls:          
	    Called By:      
	    Input:          task：任务
	                    
	    Output:         
	    Return:         成功返回true，失败返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	bool SendMsg( Task * task );
	// }}}RME
	// {{{RME operation 'ReceiveMsg()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       ReceiveMsg
	    Description:    从消息队列中获取任务
	    Calls:          
	    Called By:      
	    Input:          
	                    
	    Output:         
	    Return:         成功返回任务，失败返回NULL
	    *************************************************	
	   }}}USR */
	// }}}RME
	Task * ReceiveMsg( void );
	// }}}RME
	// {{{RME operation 'ReplaceOrAddMsg(Task*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       ReplaceOrAddMsg
	    Description:    任务队列中有相同的任务，则将旧任务替换掉，否则添加新任务到队列中
	    Calls:          
	    Called By:      
	    Input:          task：任务
	                    
	    Output:         
	    Return:         成功返回true，失败返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	bool ReplaceOrAddMsg( Task * task );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* MsgQueue_H */

// }}}RME
