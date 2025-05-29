// {{{RME classifier 'Logical View::DevInvalidState'

#ifndef DevInvalidState_H
#define DevInvalidState_H

#ifdef PRAGMA
#pragma interface "DevInvalidState.h"
#endif

#include <RTSystem/Communication.h>
#include <DeviceState.h>

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
#include "public.h"

/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* 文件名称：DevInvalidState.h
* 文件标识：
* 摘    要：表示设备处于无效状态的类，设备没有连接即为无效状态
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

// }}}USR
// }}}RME

class DevInvalidState : public DeviceState
{
public:
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR
	    string GetStateName();

	static DevInvalidState& GetStateInstance()
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
	static DevInvalidState _instance;
	// }}}USR
	// }}}RME

public:
	// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
	DevInvalidState( void );
	// }}}RME
	// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
	virtual ~DevInvalidState( void );
	// }}}RME
	// {{{RME operation 'Do(Device*)'
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
	bool Do( Device * pContext );
	// }}}RME
	// {{{RME operation 'TrigerEvent(Device::DEVEVENT,Device*)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	    Function:       TrigerEvent
	    Description:    由Device调用，具体工作在子类中完成，各子类根据不同的事件生成新的状态，并调用Device的changeState函数改变当前状态
	    Calls:          
	    Called By:      
	    Input:          devEvent：被触发的事件
	                    pContext：状态相关上下文
	    Output:         
	    Return:         
	    *************************************************
	   }}}USR */
	// }}}RME
	void TrigerEvent( Device::DEVEVENT devEvent, Device * pContext );
	// }}}RME
	// {{{RME operation 'GetStateValue()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetStateValue
	    Description:    查询状态对应的数值
	    Calls:          
	    Called By:      
	    Input:          
	                    
	    Output:         
	    Return:         状态对应的数值
	    *************************************************	
	   }}}USR */
	// }}}RME
	DeviceState::DEVSTATEVAL GetStateValue( void );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* DevInvalidState_H */

// }}}RME
