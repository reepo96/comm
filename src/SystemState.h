// {{{RME classifier 'Logical View::SystemState'

#ifndef SystemState_H
#define SystemState_H

#ifdef PRAGMA
#pragma interface "SystemState.h"
#endif

#include <RTSystem/Communication.h>
class SystemCtrl;

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
#include "public.h"

/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* 文件名称：SystemState.h
* 文件标识：
* 摘    要：系统状态
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

#include "SystemCtrl.h"

#ifdef _DEBUG
#define new MYDEBUG_NEW
#endif

// }}}USR
// }}}RME

class SystemState
{
public:
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR
	    virtual string GetStateName();

	static SystemState& GetStateInstance()
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
	static SystemState _instance;
	// }}}USR
	// }}}RME

public:
	// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
	SystemState( void );
	// }}}RME
	// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
	virtual ~SystemState( void );
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
	virtual bool Do( SystemCtrl * pContext );
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
	virtual void TrigerEvent( SystemCtrl::SYSTEMEVENT sysEvent, SystemCtrl * pContext );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* SystemState_H */

// }}}RME
