// {{{RME classifier 'Logical View::FtpCtrl'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "FtpCtrl.h"
#endif

#include <RTSystem/Communication.h>
#include <FtpCtrl.h>
#include <FTPCFileFatcher.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* 文件名称：SystemCfger.cpp
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

// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
FtpCtrl::FtpCtrl( void )
{
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
FtpCtrl::~FtpCtrl( void )
{
}
// }}}RME

// {{{RME operation 'RunTask(Task*)'
bool FtpCtrl::RunTask( Task * task )
{
	// {{{USR
	    return true;
	// }}}USR
}
// }}}RME

// {{{RME operation 'GetFTPPassword()'
char * FtpCtrl::GetFTPPassword( void )
{
	// {{{USR
	    return NULL;
	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR

// }}}USR
// }}}RME

// }}}RME
