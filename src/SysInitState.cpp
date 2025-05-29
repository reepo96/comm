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
* �ļ����ƣ�SysInitState.cpp
* �ļ���ʶ��
* ժ    Ҫ��ϵͳ��ʼ��״̬
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
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
	    case SystemCtrl::SE_LEAVE:      //�뿪�¼��������������Idle״̬
			//pNewSysState = new SysIdleState();
			pContext->ChangeState(dynamic_cast<SystemState*>(&SysIdleState::GetStateInstance()) );
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
string SysInitState::GetStateName()
{
    return string("SysInitState");
}
// }}}USR
// }}}RME

// }}}RME
