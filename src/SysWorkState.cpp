// {{{RME classifier 'Logical View::SysWorkState'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "SysWorkState.h"
#endif

#include <RTSystem/Communication.h>
#include <SysWorkState.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* 文件名称：SysWorkState.cpp
* 文件标识：
* 摘    要：系统工作状态
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

#include "SysIdleState.h"
#include "SysInitState.h"
#include "SysExitState.h"

SysWorkState SysWorkState::_instance;

// }}}USR
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
SysWorkState::SysWorkState( void )
{
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
SysWorkState::~SysWorkState( void )
{
}
// }}}RME

// {{{RME operation 'Do(SystemCtrl*)'
bool SysWorkState::Do( SystemCtrl * pContext )
{
	// {{{USR
	    return pContext->DoWork();
	// }}}USR
}
// }}}RME

// {{{RME operation 'TrigerEvent(SystemCtrl::SYSTEMEVENT,SystemCtrl*)'
void SysWorkState::TrigerEvent( SystemCtrl::SYSTEMEVENT sysEvent, SystemCtrl * pContext )
{
	// {{{USR
		//EnterCriticalSection(&g_section4NewMem);

		//SystemState *pNewSysState = NULL;
	    switch(sysEvent)
	    {
	    case SystemCtrl::SE_LEAVE:      //离开事件被触发，则进入idle状态
			//pNewSysState = new SysIdleState();
			pContext->ChangeState(dynamic_cast<SystemState*>(&SysIdleState::GetStateInstance()) );
	        break;
	    case SystemCtrl::SE_INIT:      //初始化事件被触发，则进入init状态
			//pNewSysState = new SysInitState();
			pContext->ChangeState(dynamic_cast<SystemState*>(&SysInitState::GetStateInstance()) );
	        break;
	    case SystemCtrl::SE_EXIT:      //退出事件被触发，则进入exit状态
			//pNewSysState = new SysExitState();
			pContext->ChangeState(dynamic_cast<SystemState*>(&SysExitState::GetStateInstance()) );
	        break;
	    default:                        //其它事件不改变状态
	        break;
	    }

		//LeaveCriticalSection(&g_section4NewMem);		

	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR
string SysWorkState::GetStateName()
{
    return string("SysWorkState");
}
// }}}USR
// }}}RME

// }}}RME
