// {{{RME classifier 'Logical View::DevTimeOutState'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "DevTimeOutState.h"
#endif

#include <RTSystem/Communication.h>
#include <DevTimeOutState.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* �ļ����ƣ�TimeOuteState.cpp
* �ļ���ʶ��
* ժ    Ҫ���豸��ʱ״̬
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
*
**************************************************************************/

#include "DevIdleState.h"

DevTimeOutState DevTimeOutState::_instance;

// }}}USR
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
DevTimeOutState::DevTimeOutState( void )
{
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
DevTimeOutState::~DevTimeOutState( void )
{
}
// }}}RME

// {{{RME operation 'Do(Device*)'
bool DevTimeOutState::Do( Device * pContext )
{
	// {{{USR
	    return pContext->DoTimout();
	// }}}USR
}
// }}}RME

// {{{RME operation 'TrigerEvent(Device::DEVEVENT,Device*)'
void DevTimeOutState::TrigerEvent( Device::DEVEVENT devEvent, Device * pContext )
{
	// {{{USR
		//EnterCriticalSection(&g_section4NewMem);

		//DeviceState *pNewDevState = NULL;
	    switch(devEvent)
	    {
	    case Device::DE_LEAVE:      //�뿪�¼��������������idle״̬
			//pNewDevState = new DevIdleState();
			pContext->ChangeState(dynamic_cast<DeviceState*>(&DevIdleState::GetStateInstance()) );
	        break;
	    default:                      //�����¼����ı�״̬
			break;
	    }

		//LeaveCriticalSection(&g_section4NewMem);

	// }}}USR
}
// }}}RME

// {{{RME operation 'GetStateValue()'
DeviceState::DEVSTATEVAL DevTimeOutState::GetStateValue( void )
{
	// {{{USR
	        return DSV_TIMEOUT;
	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR

// }}}USR
// }}}RME

// }}}RME
