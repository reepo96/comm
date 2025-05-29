// {{{RME classifier 'Logical View::DeviceState'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "DeviceState.h"
#endif

#include <RTSystem/Communication.h>
#include <DeviceState.h>
#include <Device.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* �ļ����ƣ�DeviceState.cpp
* �ļ���ʶ��
* ժ    Ҫ���豸״̬�࣬��¼�豸��ǰ״̬
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
*
**************************************************************************/

DeviceState DeviceState::_instance;

// }}}USR
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
DeviceState::DeviceState( void )
{
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
DeviceState::~DeviceState( void )
{
}
// }}}RME

// {{{RME operation 'Do(Device*)'
bool DeviceState::Do( Device * pContext )
{
	// {{{USR
	    return true;
	// }}}USR
}
// }}}RME

// {{{RME operation 'TrigerEvent(Device::DEVEVENT,Device*)'
void DeviceState::TrigerEvent( Device::DEVEVENT devEvent, Device * pContext )
{
	// {{{USR

	// }}}USR
}
// }}}RME

// {{{RME operation 'GetStateValue()'
DeviceState::DEVSTATEVAL DeviceState::GetStateValue( void )
{
	// {{{USR
	        return DSV_UNKOWN;
	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR
string DeviceState::GetStateName()
{
    return string("DeviceState");
}
// }}}USR
// }}}RME

// }}}RME
