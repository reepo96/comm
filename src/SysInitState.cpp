// {{{RME classifier 'Logical View::SysInitState'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "SysInitState.h"
#endif

#include <RTSystem/Communication.h>
#include <SysInitState.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* 文件名称：SysInitState.cpp
* 文件标识：
* 摘    要：系统初始化状态
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

#include "SysIdleState.h"

SysInitState SysInitState::_instance;

// }}}USR
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
SysInitState::SysInitState( void )
{
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
SysInitState::~SysInitState( void )
{
}
// }}}RME

// {{{RME operation 'Do(SystemCtrl*)'
bool SysInitState::Do( SystemCtrl * pContext )
{
	// {{{USR
	    return pContext->DoInit();
	// }}}USR
}
// }}}RME

// {{{RME operation 'TrigerEvent(SystemCtrl::SYSTEMEVENT,SystemCtrl*)'
void SysInitState::TrigerEvent( SystemCtrl::SYSTEMEVENT sysEvent, SystemCtrl * pContext )
{
	// {{{USR
		//EnterCriticalSection(&g_section4NewMem);

		//SystemState *pNewSysState = NULL;
	    switch(sysEvent)
	    {
	    case SystemCtrl::SE_LEAVE:      //离开事件被触发，则进入Idle状态
			//pNewSysState = new SysIdleState();
			pContext->ChangeState(dynamic_cast<SystemState*>(&SysIdleState::GetStateInstance()) );
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
string SysInitState::GetStateName()
{
    return string("SysInitState");
}
// }}}USR
// }}}RME

// }}}RME
