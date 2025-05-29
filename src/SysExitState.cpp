// {{{RME classifier 'Logical View::SysExitState'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "SysExitState.h"
#endif

#include <RTSystem/Communication.h>
#include <SysExitState.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* 文件名称：SysExitState.cpp
* 文件标识：
* 摘    要：系统退出状态
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

SysExitState SysExitState::_instance;

// }}}USR
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
SysExitState::SysExitState( void )
{
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
SysExitState::~SysExitState( void )
{
}
// }}}RME

// {{{RME operation 'Do(SystemCtrl*)'
bool SysExitState::Do( SystemCtrl * pContext )
{
	// {{{USR
	    return pContext->DoExit();
	// }}}USR
}
// }}}RME

// {{{RME operation 'TrigerEvent(SystemCtrl::SYSTEMEVENT,SystemCtrl*)'
void SysExitState::TrigerEvent( SystemCtrl::SYSTEMEVENT sysEvent, SystemCtrl * pContext )
{
	// {{{USR
	    

	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR
string SysExitState::GetStateName()
{
    return string("SysExitState");
}
// }}}USR
// }}}RME

// }}}RME
