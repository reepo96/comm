// {{{RME classifier 'Logical View::QueRegister'

#ifndef QueRegister_H
#define QueRegister_H

#ifdef PRAGMA
#pragma interface "QueRegister.h"
#endif

#include <RTSystem/Communication.h>

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR

/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* �ļ����ƣ�QueRegister.h
* �ļ���ʶ��
* ժ    Ҫ������ע����
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
*
**************************************************************************/

#include "public.h"
#include "MsgQueue.h"
#include <map>
#include <string>
using namespace std;

#pragma warning( disable : 4786 )

// }}}USR
// }}}RME

class QueRegister
{
public:
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR

	// }}}USR
	// }}}RME

protected:
	// {{{RME tool 'OT::Cpp' property 'ProtectedDeclarations'
	// {{{USR
	HANDLE m_hEvent;
	// }}}USR
	// }}}RME

private:
	// {{{RME tool 'OT::Cpp' property 'PrivateDeclarations'
	// {{{USR

	// }}}USR
	// }}}RME
	// {{{RME classAttribute '_instance'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	static QueRegister _instance;
	// }}}RME
	// {{{RME classAttribute 'queueMap'
	// {{{RME general 'documentation'
	/* {{{USR
	�Ѿ�ע��ģ���queue����queue��ɵ�map
	   }}}USR */
	// }}}RME
	map< string, MsgQueue * > queueMap;
	// }}}RME
	// {{{RME operation 'QueRegister()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	QueRegister( void );
	// }}}RME

public:
	// {{{RME operation '~QueRegister()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	virtual ~QueRegister( void );
	// }}}RME
	// {{{RME operation 'GetInstance()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetInstance
	    Description:    ���QueRegister��ʵ��
	    Calls:          
	    Called By:      
	    Input:          
	                    
	                    
	    Output:                             
	    Return:         ����QueRegister��ʵ��
	    *************************************************
	   }}}USR */
	// }}}RME
	static QueRegister & GetInstance( void );
	// }}}RME
	// {{{RME operation 'RegisterQue(const string)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       RegisterQue
	    Description:    ͨ������ע��queue����������Ƶ�queue�Ѿ����ڣ��򷵻ش��ڵ�queue�����򴴽��µ�queue������
	    Calls:          
	    Called By:      
	    Input:          queName����������
	                    
	                    
	    Output:                             
	    Return:         ����MsgQueue��ʵ��
	    *************************************************	
	   }}}USR */
	// }}}RME
	MsgQueue * RegisterQue( const string queName );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* QueRegister_H */

// }}}RME
