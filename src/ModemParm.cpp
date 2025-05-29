// {{{RME classifier 'Logical View::ModemParm'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "ModemParm.h"
#endif

#include <RTSystem/Communication.h>
#include <ModemParm.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* 文件名称：ModenParm.cpp
* 文件标识：
* 摘    要：modem通讯参数结构类
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

// }}}USR
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
ModemParm::~ModemParm( void )
{
}
// }}}RME

// {{{RME operation 'ModemParm()'
ModemParm::ModemParm( void )
	// {{{RME tool 'OT::Cpp' property 'ConstructorInitializer'
	// {{{USR

	// }}}USR
	// }}}RME
{
	// {{{USR
	memset(phoneNum,0,sizeof(phoneNum));
	memset(comName,0,sizeof(comName));
	baudrate = 0;
	parity = 0;
	data = 0;
	stop =0;
	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR

// }}}USR
// }}}RME

// }}}RME
