// {{{RME classifier 'Logical View::SysIdleState'

#ifndef SysIdleState_H
#define SysIdleState_H

#ifdef PRAGMA
#pragma interface "SysIdleState.h"
#endif

#include <RTSystem/Communication.h>
#include <SystemState.h>

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
#include "public.h"

/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* 文件名称：SysIdleState.h
* 文件标识：
* 摘    要：通讯系统空闲状态
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

// }}}USR
// }}}RME

class SysIdleState : public SystemState
{
public:
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR
	    string GetStateName();

	static SysIdleState& GetStateInstance()
	{
		return _instance;
	}
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
	static SysIdleState _instance;
	// }}}USR
	// }}}RME

public:
	// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
	SysIdleState( void );
	// }}}RME
	// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
	virtual ~SysIdleState( void );
	// }}}RME
	// {{{RME operation 'Do(SystemCtrl*)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	    Function:       Do
	    Description:    转变到此状态后要处理的事
	    Calls:          
	    Called By:      
	    Input:          pContext：状态相关上下文
	                    
	    Output:         
	    Return:         
	    *************************************************
	   }}}USR */
	// }}}RME
	bool Do( SystemCtrl * pContext );
	// }}}RME
	// {{{RME operation 'TrigerEvent(SystemCtrl::SYSTEMEVENT,SystemCtrl*)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	    Function:       TrigerEvent
	    Description:    由SystemCtrl调用，具体工作在子类中完成，各子类根据不同的事件生成新的状态，并调用SystemCtrl的changeState函数改变当前状态
	    Calls:          
	    Called By:      
	    Input:          sysEvent：被触发的事件
	                    pContext：状态相关上下文
	    Output:         
	    Return:         
	    *************************************************
	   }}}USR */
	// }}}RME
	void TrigerEvent( SystemCtrl::SYSTEMEVENT sysEvent, SystemCtrl * pContext );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* SysIdleState_H */

// }}}RME
