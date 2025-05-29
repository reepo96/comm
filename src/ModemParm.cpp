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
* �ļ����ƣ�ModenParm.cpp
* �ļ���ʶ��
* ժ    Ҫ��modemͨѶ�����ṹ��
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
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
