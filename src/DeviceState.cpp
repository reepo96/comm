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
* 文件名称：DeviceState.cpp
* 文件标识：
* 摘    要：设备状态类，记录设备当前状态
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
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
