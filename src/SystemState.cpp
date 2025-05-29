// {{{RME classifier 'Logical View::SystemState'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "SystemState.h"
#endif

#include <RTSystem/Communication.h>
#include <SystemState.h>
#include <SystemCtrl.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* �ļ����ƣ�SystemCfger.cpp
* �ļ���ʶ��
* ժ    Ҫ��ϵͳ�����࣬���ڱ���ϵͳ��������Ϣ
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
*
**************************************************************************/

SystemState SystemState::_instance;

// }}}USR
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
SystemState::SystemState( void )
{
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
SystemState::~SystemState( void )
{
}
// }}}RME

// {{{RME operation 'Do(SystemCtrl*)'
bool SystemState::Do( SystemCtrl * pContext )
{
	// {{{USR
	    return true;
	// }}}USR
}
// }}}RME

// {{{RME operation 'TrigerEvent(SystemCtrl::SYSTEMEVENT,SystemCtrl*)'
void SystemState::TrigerEvent( SystemCtrl::SYSTEMEVENT sysEvent, SystemCtrl * pContext )
{
	// {{{USR

	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR
string SystemState::GetStateName()
{
    return string("SystemState");
}
// }}}USR
// }}}RME

// }}}RME
