// {{{RME classifier 'Logical View::SysIdleState'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "SysIdleState.h"
#endif

#include <RTSystem/Communication.h>
#include <SysIdleState.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* 文件名称：SysIdleState.cpp
* 文件标识：
* 摘    要：通讯系统空闲状态
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

#include "SysWorkState.h"

SysIdleState SysIdleState::_instance;

// }}}USR
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
SysIdleState::SysIdleState( void )
{
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
SysIdleState::~SysIdleState( void )
{
}
// }}}RME

// {{{RME operation 'Do(SystemCtrl*)'
bool SysIdleState::Do( SystemCtrl * pContext )
{
	// {{{USR
	    return pContext->DoIdle();
	// }}}USR
}
// }}}RME

// {{{RME operation 'TrigerEvent(SystemCtrl::SYSTEMEVENT,SystemCtrl*)'
void SysIdleState::TrigerEvent( SystemCtrl::SYSTEMEVENT sysEvent, SystemCtrl * pContext )
{
	// {{{USR
		//EnterCriticalSection(&g_section4NewMem);
		
		//SystemState *pNewSysState = NULL;
	    switch(sysEvent)
	    {
	    case SystemCtrl::SE_LEAVE:      //离开事件被触发，则进入work状态
			//pNewSysState = new SysWorkState();			
			pContext->ChangeState(dynamic_cast<SystemState*>(&SysWorkState::GetStateInstance()) );
	        break;
	    case SystemCtrl::SE_STILL:      //停留事件被触发，则再次进入此状态
			//pNewSysState = new SysIdleState();
			pContext->ChangeState( dynamic_cast<SystemState*>(&SysIdleState::GetStateInstance()));
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
string SysIdleState::GetStateName()
{
    return string("SysIdleState");
}
// }}}USR
// }}}RME

// }}}RME
