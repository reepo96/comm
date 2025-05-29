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
* 文件名称：QueRegister.h
* 文件标识：
* 摘    要：队列注册类
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
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
	已经注册的，由queue名和queue组成的map
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
	    Description:    获得QueRegister的实例
	    Calls:          
	    Called By:      
	    Input:          
	                    
	                    
	    Output:                             
	    Return:         返回QueRegister的实例
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
	    Description:    通过名称注册queue，如果该名称的queue已经存在，则返回存在的queue，否则创建新的queue并返回
	    Calls:          
	    Called By:      
	    Input:          queName：队列名称
	                    
	                    
	    Output:                             
	    Return:         返回MsgQueue的实例
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
