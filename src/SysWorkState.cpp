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
* �ļ����ƣ�SysWorkState.cpp
* �ļ���ʶ��
* ժ    Ҫ��ϵͳ����״̬
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
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
	    case SystemCtrl::SE_LEAVE:      //�뿪�¼��������������idle״̬
			//pNewSysState = new SysIdleState();
			pContext->ChangeState(dynamic_cast<SystemState*>(&SysIdleState::GetStateInstance()) );
	        break;
	    case SystemCtrl::SE_INIT:      //��ʼ���¼��������������init״̬
			//pNewSysState = new SysInitState();
			pContext->ChangeState(dynamic_cast<SystemState*>(&SysInitState::GetStateInstance()) );
	        break;
	    case SystemCtrl::SE_EXIT:      //�˳��¼��������������exit״̬
			//pNewSysState = new SysExitState();
			pContext->ChangeState(dynamic_cast<SystemState*>(&SysExitState::GetStateInstance()) );
	        break;
	    default:                        //�����¼����ı�״̬
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
