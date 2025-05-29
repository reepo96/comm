// {{{RME classifier 'Logical View::DevFailState'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "DevFailState.h"
#endif

#include <RTSystem/Communication.h>
#include <DevFailState.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* 文件名称：SystemCfger.cpp
* 文件标识：
* 摘    要：系统配置类，用于保存系统的配置信息
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

#include "DevIdleState.h"

DevFailState DevFailState::_instance;

// }}}USR
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
DevFailState::DevFailState( void )
{
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
DevFailState::~DevFailState( void )
{
}
// }}}RME

// {{{RME operation 'Do(Device*)'
bool DevFailState::Do( Device * pContext )
{
	// {{{USR
	    return pContext->DoFail();
	// }}}USR
}
// }}}RME

// {{{RME operation 'TrigerEvent(Device::DEVEVENT,Device*)'
void DevFailState::TrigerEvent( Device::DEVEVENT devEvent, Device * pContext )
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
DeviceState::DEVSTATEVAL DevFailState::GetStateValue( void )
{
	// {{{USR
	        return DSV_FAIL;
	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR

// }}}USR
// }}}RME

// }}}RME
