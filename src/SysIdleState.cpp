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
* �ļ����ƣ�SysIdleState.cpp
* �ļ���ʶ��
* ժ    Ҫ��ͨѶϵͳ����״̬
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
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
	    case SystemCtrl::SE_LEAVE:      //�뿪�¼��������������work״̬
			//pNewSysState = new SysWorkState();			
			pContext->ChangeState(dynamic_cast<SystemState*>(&SysWorkState::GetStateInstance()) );
	        break;
	    case SystemCtrl::SE_STILL:      //ͣ���¼������������ٴν����״̬
			//pNewSysState = new SysIdleState();
			pContext->ChangeState( dynamic_cast<SystemState*>(&SysIdleState::GetStateInstance()));
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
string SysIdleState::GetStateName()
{
    return string("SysIdleState");
}
// }}}USR
// }}}RME

// }}}RME
