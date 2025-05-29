// {{{RME classifier 'Logical View::DevTimeOutState'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "DevTimeOutState.h"
#endif

#include <RTSystem/Communication.h>
#include <DevTimeOutState.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* 文件名称：TimeOuteState.cpp
* 文件标识：
* 摘    要：设备超时状态
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

#include "DevIdleState.h"

DevTimeOutState DevTimeOutState::_instance;

// }}}USR
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
DevTimeOutState::DevTimeOutState( void )
{
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
DevTimeOutState::~DevTimeOutState( void )
{
}
// }}}RME

// {{{RME operation 'Do(Device*)'
bool DevTimeOutState::Do( Device * pContext )
{
	// {{{USR
	    return pContext->DoTimout();
	// }}}USR
}
// }}}RME

// {{{RME operation 'TrigerEvent(Device::DEVEVENT,Device*)'
void DevTimeOutState::TrigerEvent( Device::DEVEVENT devEvent, Device * pContext )
{
	// {{{USR
		//EnterCriticalSection(&g_section4NewMem);

		//DeviceState *pNewDevState = NULL;
	    switch(devEvent)
	    {
	    case Device::DE_LEAVE:      //离开事件被触发，则进入idle状态
			//pNewDevState = new DevIdleState();
			pContext->ChangeState(dynamic_cast<DeviceState*>(&DevIdleState::GetStateInstance()) );
	        break;
	    default:                      //其它事件不改变状态
			break;
	    }

		//LeaveCriticalSection(&g_section4NewMem);

	// }}}USR
}
// }}}RME

// {{{RME operation 'GetStateValue()'
DeviceState::DEVSTATEVAL DevTimeOutState::GetStateValue( void )
{
	// {{{USR
	        return DSV_TIMEOUT;
	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR

// }}}USR
// }}}RME

// }}}RME
