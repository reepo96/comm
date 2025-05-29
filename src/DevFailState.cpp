// {{{RME classifier 'Logical View::DevFailState'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "DevFailState.h"
#endif

#include <RTSystem/Communication.h>
#include <DevFailState.h>

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

#include "DevIdleState.h"

DevFailState DevFailState::_instance;

// }}}USR
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
DevFailState::DevFailState( void )
{
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
DevFailState::~DevFailState( void )
{
}
// }}}RME

// {{{RME operation 'Do(Device*)'
bool DevFailState::Do( Device * pContext )
{
	// {{{USR
	    return pContext->DoFail();
	// }}}USR
}
// }}}RME

// {{{RME operation 'TrigerEvent(Device::DEVEVENT,Device*)'
void DevFailState::TrigerEvent( Device::DEVEVENT devEvent, Device * pContext )
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
DeviceState::DEVSTATEVAL DevFailState::GetStateValue( void )
{
	// {{{USR
	        return DSV_FAIL;
	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR

// }}}USR
// }}}RME

// }}}RME
