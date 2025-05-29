// {{{RME classifier 'Logical View::DevIdleState'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "DevIdleState.h"
#endif

#include <RTSystem/Communication.h>
#include <DevIdleState.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* �ļ����ƣ�DeviceState.cpp
* �ļ���ʶ��
* ժ    Ҫ�������豸����״̬����
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
*
**************************************************************************/

#include "DevWorkState.h"
#include "DevTimeOutState.h"
#include "DevInvalidState.h"

DevIdleState DevIdleState::_instance;

// }}}USR
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
DevIdleState::DevIdleState( void )
{
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
DevIdleState::~DevIdleState( void )
{
}
// }}}RME

// {{{RME operation 'Do(Device*)'
bool DevIdleState::Do( Device * pContext )
{
	// {{{USR
	    return pContext->DoIdle();
	// }}}USR
}
// }}}RME

// {{{RME operation 'TrigerEvent(Device::DEVEVENT,Device*)'
void DevIdleState::TrigerEvent( Device::DEVEVENT devEvent, Device * pContext )
{
	// {{{USR
		//EnterCriticalSection(&g_section4NewMem);

		//DeviceState *pNewDevState = NULL;
	    switch(devEvent)
	    {
	    case Device::DE_STILL:      //ͣ���¼������������ٴν����״̬
			//pNewDevState = new DevIdleState();
			pContext->ChangeState(dynamic_cast<DeviceState*>(&DevIdleState::GetStateInstance()) );
	        break;
	    case Device::DE_LEAVE:      //�뿪�¼��������������work״̬
			//pNewDevState = new DevWorkState();	        
			pContext->ChangeState(dynamic_cast<DeviceState*>(&DevWorkState::GetStateInstance()) );
	        break;
	    case Device::DE_INVALID:      //��Ч�¼����������������Ч״̬
			//pNewDevState = new DevInvalidState();
			pContext->ChangeState(dynamic_cast<DeviceState*>(&DevInvalidState::GetStateInstance()) );
	        break;
	    default:                      //�����¼����ı�״̬
			break;
	    }

		//LeaveCriticalSection(&g_section4NewMem);		

	// }}}USR
}
// }}}RME

// {{{RME operation 'GetStateValue()'
DeviceState::DEVSTATEVAL DevIdleState::GetStateValue( void )
{
	// {{{USR
	        return DSV_IDLE;
	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR
string DevIdleState::GetStateName()
{
    return string("DevIdleState");
}
// }}}USR
// }}}RME

// }}}RME
