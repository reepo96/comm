// {{{RME classifier 'Logical View::Task'

#ifndef Task_H
#define Task_H

#ifdef PRAGMA
#pragma interface "Task.h"
#endif

#include <RTSystem/Communication.h>

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
#include "public.h"
#include "Command.h"

/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* 文件名称：Task.h
* 文件标识：
* 摘    要：任务类
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

class Linker;

// }}}USR
// }}}RME

struct Task
{
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR

	//克隆任务
	Task * Clone();
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
	// {{{RME classAttribute 'Linker'
	// {{{RME general 'documentation'
	/* {{{USR
	连接
	   }}}USR */
	// }}}RME
	Linker * Linker;
	// }}}RME
	// {{{RME classAttribute 'cmd'
	// {{{RME general 'documentation'
	/* {{{USR
	任务要执行的命令
	   }}}USR */
	// }}}RME
	struct Command cmd;
	// }}}RME
	// {{{RME classAttribute 'createTime'
	// {{{RME general 'documentation'
	/* {{{USR
	任务创建时间
	   }}}USR */
	// }}}RME
	DWORD createTime;
	// }}}RME
	// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
	Task( void );
	// }}}RME
	// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
	virtual ~Task( void );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* Task_H */

// }}}RME
