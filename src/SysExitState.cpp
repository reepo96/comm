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
* �ļ����ƣ�SysExitState.cpp
* �ļ���ʶ��
* ժ    Ҫ��ϵͳ�˳�״̬
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
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
