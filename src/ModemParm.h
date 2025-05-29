// {{{RME classifier 'Logical View::ModemParm'

#ifndef ModemParm_H
#define ModemParm_H

#ifdef PRAGMA
#pragma interface "ModemParm.h"
#endif

#include <RTSystem/Communication.h>

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
#include "public.h"

/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* 文件名称：ModenParm.h
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

struct ModemParm
{
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR

	// }}}USR
	// }}}RME

protected:
	// {{{RME tool 'OT::Cpp' property 'ProtectedDeclarations'
	// {{{USR

	// }}}USR
	// }}}RME

private:
	// {{{RME tool 'OT::Cpp' property 'PrivateDeclarations'
	// {{{USR

	// }}}USR
	// }}}RME

public:
	// {{{RME classAttribute 'phoneNum'
	// {{{RME general 'documentation'
	/* {{{USR
	电话号码
	   }}}USR */
	// }}}RME
	char phoneNum[ 20 ];
	// }}}RME
	// {{{RME classAttribute 'comName'
	// {{{RME general 'documentation'
	/* {{{USR
	端口名称
	   }}}USR */
	// }}}RME
	char comName[ 10 ];
	// }}}RME
	// {{{RME classAttribute 'baudrate'
	// {{{RME general 'documentation'
	/* {{{USR
	波特率
	   }}}USR */
	// }}}RME
	int baudrate;
	// }}}RME
	// {{{RME classAttribute 'parity'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	char parity;
	// }}}RME
	// {{{RME classAttribute 'data'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	int data;
	// }}}RME
	// {{{RME classAttribute 'stop'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	int stop;
	// }}}RME
	// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
	virtual ~ModemParm( void );
	// }}}RME
	// {{{RME operation 'ModemParm()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	ModemParm( void );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* ModemParm_H */

// }}}RME
