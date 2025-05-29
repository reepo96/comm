// {{{RME classifier 'Logical View::DevSuccessState'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "DevSuccessState.h"
#endif

#include <RTSystem/Communication.h>
#include <DevSuccessState.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* �ļ����ƣ�SuccessState.cpp
* �ļ���ʶ��
* ժ    Ҫ���豸�ɹ�����ͨѶ�����״̬
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
*
**************************************************************************/

#include "DevIdleState.h"

DevSuccessState DevSuccessState::_instance;

// }}}USR
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
DevSuccessState::DevSuccessState( void )
{
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
DevSuccessState::~DevSuccessState( void )
{
}
// }}}RME

// {{{RME operation 'Do(Device*)'
bool DevSuccessState::Do( Device * pContext )
{
	// {{{USR
	    return pContext->DoSuccess();
	// }}}USR
}
// }}}RME

// {{{RME operation 'TrigerEvent(Device::DEVEVENT,Device*)'
void DevSuccessState::TrigerEvent( Device::DEVEVENT devEvent, Device * pContext )
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
DeviceState::DEVSTATEVAL DevSuccessState::GetStateValue( void )
{
	// {{{USR
	        return DSV_SUCCESS;
	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR
string DevSuccessState::GetStateName()
{
    return string("DevSuccessState");
}
// }}}USR
// }}}RME

// }}}RME
