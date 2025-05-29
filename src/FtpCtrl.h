// {{{RME classifier 'Logical View::FtpCtrl'

#ifndef FtpCtrl_H
#define FtpCtrl_H

#ifdef PRAGMA
#pragma interface "FtpCtrl.h"
#endif

#include <RTSystem/Communication.h>
class FTPCFileFatcher;

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
#include "public.h"
#include "Task.h"

/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* 文件名称：SystemCfger.h
* 文件标识：
* 摘    要：系统配置类，用于保存系统的配置信息
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

// }}}USR
// }}}RME

class FtpCtrl
{
public:
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
	// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
	FtpCtrl( void );
	// }}}RME
	// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
	virtual ~FtpCtrl( void );
	// }}}RME
	// {{{RME operation 'RunTask(Task*)'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	bool RunTask( Task * task );
	// }}}RME

protected:
	// {{{RME operation 'GetFTPPassword()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	char * GetFTPPassword( void );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* FtpCtrl_H */

// }}}RME
