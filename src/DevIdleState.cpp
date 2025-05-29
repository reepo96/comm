// {{{RME classifier 'Logical View::DevIdleState'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "DevIdleState.h"
#endif

#include <RTSystem/Communication.h>
#include <DevIdleState.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* 文件名称：DeviceState.cpp
* 文件标识：
* 摘    要：代表设备空闲状态的类
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

#include "DevWorkState.h"
#include "DevTimeOutState.h"
#include "DevInvalidState.h"

DevIdleState DevIdleState::_instance;

// }}}USR
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
DevIdleState::DevIdleState( void )
{
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
DevIdleState::~DevIdleState( void )
{
}
// }}}RME

// {{{RME operation 'Do(Device*)'
bool DevIdleState::Do( Device * pContext )
{
	// {{{USR
	    return pContext->DoIdle();
	// }}}USR
}
// }}}RME

// {{{RME operation 'TrigerEvent(Device::DEVEVENT,Device*)'
void DevIdleState::TrigerEvent( Device::DEVEVENT devEvent, Device * pContext )
{
	// {{{USR
		//EnterCriticalSection(&g_section4NewMem);

		//DeviceState *pNewDevState = NULL;
	    switch(devEvent)
	    {
	    case Device::DE_STILL:      //停留事件被触发，则再次进入此状态
			//pNewDevState = new DevIdleState();
			pContext->ChangeState(dynamic_cast<DeviceState*>(&DevIdleState::GetStateInstance()) );
	        break;
	    case Device::DE_LEAVE:      //离开事件被触发，则进入work状态
			//pNewDevState = new DevWorkState();	        
			pContext->ChangeState(dynamic_cast<DeviceState*>(&DevWorkState::GetStateInstance()) );
	        break;
	    case Device::DE_INVALID:      //无效事件被触发，则进入无效状态
			//pNewDevState = new DevInvalidState();
			pContext->ChangeState(dynamic_cast<DeviceState*>(&DevInvalidState::GetStateInstance()) );
	        break;
	    default:                      //其它事件不改变状态
			break;
	    }

		//LeaveCriticalSection(&g_section4NewMem);		

	// }}}USR
}
// }}}RME

// {{{RME operation 'GetStateValue()'
DeviceState::DEVSTATEVAL DevIdleState::GetStateValue( void )
{
	// {{{USR
	        return DSV_IDLE;
	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR
string DevIdleState::GetStateName()
{
    return string("DevIdleState");
}
// }}}USR
// }}}RME

// }}}RME
