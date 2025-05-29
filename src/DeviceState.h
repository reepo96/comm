// {{{RME classifier 'Logical View::DeviceState'

#ifndef DeviceState_H
#define DeviceState_H

#ifdef PRAGMA
#pragma interface "DeviceState.h"
#endif

#include <RTSystem/Communication.h>
class Device;

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* 文件名称：DeviceState.h
* 文件标识：
* 摘    要：设备状态类，记录设备当前状态
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

#include "public.h"

#include "Device.h"

#ifdef _DEBUG
#define new MYDEBUG_NEW
#endif

// }}}USR
// }}}RME

class DeviceState
{
public:
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR
	//定义设备各个状态对应的值
	typedef enum DevStateVal{DSV_UNKOWN,DSV_IDLE,DSV_WORK,DSV_INVALID,DSV_SUCCESS,DSV_FAIL,DSV_TIMEOUT}  DEVSTATEVAL;

	virtual string GetStateName();

	static DeviceState& GetStateInstance()
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
	static DeviceState _instance;
	// }}}USR
	// }}}RME

public:
	// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
	DeviceState( void );
	// }}}RME
	// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
	virtual ~DeviceState( void );
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
	virtual bool Do( Device * pContext );
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
	virtual void TrigerEvent( Device::DEVEVENT devEvent, Device * pContext );
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
	virtual DEVSTATEVAL GetStateValue( void );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* DeviceState_H */

// }}}RME
