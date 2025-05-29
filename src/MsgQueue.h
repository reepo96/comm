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
* �ļ����ƣ�MsgQueue.h
* �ļ���ʶ��
* ժ    Ҫ�����������
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
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
	����������Ϣ�Ķ���
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
	    Description:    ������ŵ�������
	    Calls:          
	    Called By:      
	    Input:          task������
	                    
	    Output:         
	    Return:         �ɹ�����true��ʧ�ܷ���false
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
	    Description:    ����Ϣ�����л�ȡ����
	    Calls:          
	    Called By:      
	    Input:          
	                    
	    Output:         
	    Return:         �ɹ���������ʧ�ܷ���NULL
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
	    Description:    �������������ͬ�������򽫾������滻����������������񵽶�����
	    Calls:          
	    Called By:      
	    Input:          task������
	                    
	    Output:         
	    Return:         �ɹ�����true��ʧ�ܷ���false
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
