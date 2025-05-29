// {{{RME classifier 'Logical View::DevInvalidState'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "DevInvalidState.h"
#endif

#include <RTSystem/Communication.h>
#include <DevInvalidState.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* �ļ����ƣ�DevInvalidState.cpp
* �ļ���ʶ��
* ժ    Ҫ����ʾ�豸������Ч״̬���࣬�豸û�����Ӽ�Ϊ��Ч״̬
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
*
**************************************************************************/

#include "DevIdleState.h"

DevInvalidState DevInvalidState::_instance;

// }}}USR
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
DevInvalidState::DevInvalidState( void )
{
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
DevInvalidState::~DevInvalidState( void )
{
}
// }}}RME

// {{{RME operation 'Do(Device*)'
bool DevInvalidState::Do( Device * pContext )
{
	// {{{USR
	    return pContext->DoInvalid();
	// }}}USR
}
// }}}RME

// {{{RME operation 'TrigerEvent(Device::DEVEVENT,Device*)'
void DevInvalidState::TrigerEvent( Device::DEVEVENT devEvent, Device * pContext )
{
	// {{{USR
		//EnterCriticalSection(&g_section4NewMem);

		//DeviceState *pNewDevState = NULL;
	    switch(devEvent)
	    {
	    case Device::DE_LEAVE:
			//pNewDevState = new DevIdleState(); //����idle״̬
			pContext->ChangeState(dynamic_cast<DeviceState*>(&DevIdleState::GetStateInstance()) );
	        break;
	    case Device::DE_STILL:      //ͣ���¼������������ٴν����״̬
			//pNewDevState = new DevInvalidState();
			pContext->ChangeState(dynamic_cast<DeviceState*>(&DevInvalidState::GetStateInstance()) );
	        break;
	    default:
	        break;
	    }

		//LeaveCriticalSection(&g_section4NewMem);

	// }}}USR
}
// }}}RME

// {{{RME operation 'GetStateValue()'
DeviceState::DEVSTATEVAL DevInvalidState::GetStateValue( void )
{
	// {{{USR
	        return DSV_INVALID;
	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR
string DevInvalidState::GetStateName()
{
    return string("DevInvalidState");
}
// }}}USR
// }}}RME

// }}}RME
