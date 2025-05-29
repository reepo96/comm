// {{{RME classifier 'Logical View::DevWorkState'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "DevWorkState.h"
#endif

#include <RTSystem/Communication.h>
#include <DevWorkState.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* 文件名称：DevWorkState.cpp
* 文件标识：
* 摘    要：设备工作状态中，设备正在处理通讯任务，即处于工作装态中
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

#include "DevSuccessState.h"
#include "DevTimeOutState.h"
#include "DevFailState.h"
#include "DevIdleState.h"

DevWorkState DevWorkState::_instance;

// }}}USR
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
DevWorkState::DevWorkState( void )
{
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
DevWorkState::~DevWorkState( void )
{
}
// }}}RME

// {{{RME operation 'Do(Device*)'
bool DevWorkState::Do( Device * pContext )
{
	// {{{USR
	    return pContext->DoWork();
	// }}}USR
}
// }}}RME

// {{{RME operation 'TrigerEvent(Device::DEVEVENT,Device*)'
void DevWorkState::TrigerEvent( Device::DEVEVENT devEvent, Device * pContext )
{
	// {{{USR
		//EnterCriticalSection(&g_section4NewMem);

		//DeviceState *pNewDevState = NULL;
	    switch(devEvent)
	    {
	    case Device::DE_SUCCESS:      //成功事件被触发，则进入成功状态
			//pNewDevState = new DevSuccessState();
			pContext->ChangeState(dynamic_cast<DeviceState*>(&DevSuccessState::GetStateInstance()) );
	        break;
	    case Device::DE_FAIL:      //失败事件被触发，则进入失败状态
			//pNewDevState = new DevFailState();
			pContext->ChangeState(dynamic_cast<DeviceState*>(&DevFailState::GetStateInstance()) );
	        break;
	    case Device::DE_TIMEOUT:      //超时事件被触发，则进入超时状态
			//pNewDevState = new DevTimeOutState();
			pContext->ChangeState(dynamic_cast<DeviceState*>(&DevTimeOutState::GetStateInstance()) );
	        break;
	        case Device::DE_LEAVE:      //离开事件被触发，则进入Idle状态
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
DeviceState::DEVSTATEVAL DevWorkState::GetStateValue( void )
{
	// {{{USR
	        return DSV_WORK;
	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR
string DevWorkState::GetStateName()
{
    return string("DevWorkState");
}
// }}}USR
// }}}RME

// }}}RME
