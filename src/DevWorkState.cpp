// {{{RME classifier 'Logical View::DevWorkState'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "DevWorkState.h"
#endif

#include <RTSystem/Communication.h>
#include <DevWorkState.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* �ļ����ƣ�DevWorkState.cpp
* �ļ���ʶ��
* ժ    Ҫ���豸����״̬�У��豸���ڴ���ͨѶ���񣬼����ڹ���װ̬��
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
*
**************************************************************************/

#include "DevSuccessState.h"
#include "DevTimeOutState.h"
#include "DevFailState.h"
#include "DevIdleState.h"

DevWorkState DevWorkState::_instance;

// }}}USR
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
DevWorkState::DevWorkState( void )
{
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
DevWorkState::~DevWorkState( void )
{
}
// }}}RME

// {{{RME operation 'Do(Device*)'
bool DevWorkState::Do( Device * pContext )
{
	// {{{USR
	    return pContext->DoWork();
	// }}}USR
}
// }}}RME

// {{{RME operation 'TrigerEvent(Device::DEVEVENT,Device*)'
void DevWorkState::TrigerEvent( Device::DEVEVENT devEvent, Device * pContext )
{
	// {{{USR
		//EnterCriticalSection(&g_section4NewMem);

		//DeviceState *pNewDevState = NULL;
	    switch(devEvent)
	    {
	    case Device::DE_SUCCESS:      //�ɹ��¼��������������ɹ�״̬
			//pNewDevState = new DevSuccessState();
			pContext->ChangeState(dynamic_cast<DeviceState*>(&DevSuccessState::GetStateInstance()) );
	        break;
	    case Device::DE_FAIL:      //ʧ���¼��������������ʧ��״̬
			//pNewDevState = new DevFailState();
			pContext->ChangeState(dynamic_cast<DeviceState*>(&DevFailState::GetStateInstance()) );
	        break;
	    case Device::DE_TIMEOUT:      //��ʱ�¼�������������볬ʱ״̬
			//pNewDevState = new DevTimeOutState();
			pContext->ChangeState(dynamic_cast<DeviceState*>(&DevTimeOutState::GetStateInstance()) );
	        break;
	        case Device::DE_LEAVE:      //�뿪�¼��������������Idle״̬
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
DeviceState::DEVSTATEVAL DevWorkState::GetStateValue( void )
{
	// {{{USR
	        return DSV_WORK;
	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR
string DevWorkState::GetStateName()
{
    return string("DevWorkState");
}
// }}}USR
// }}}RME

// }}}RME
