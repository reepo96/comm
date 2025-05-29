// {{{RME classifier 'Logical View::ComParm'

#ifndef ComParm_H
#define ComParm_H

#ifdef PRAGMA
#pragma interface "ComParm.h"
#endif

#include <RTSystem/Communication.h>

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
#include "public.h"

/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* 文件名称：ComParm.h
* 文件标识：
* 摘    要：串口通讯参数结构类
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

// }}}USR
// }}}RME

struct ComParm
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
	// {{{RME classAttribute 'comName'
	// {{{RME general 'documentation'
	/* {{{USR
	串口名
	   }}}USR */
	// }}}RME
	char comName[ 5 ];
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
	// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
	ComParm( void );
	// }}}RME
	// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
	virtual ~ComParm( void );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* ComParm_H */

// }}}RME
