// {{{RME classifier 'Logical View::CommunicationManager'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "CommunicationManager.h"
#endif

#include <RTSystem/Communication.h>
#include <CommunicationManager.h>
#include <FtpCtrl.h>
#include <NormalComm.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* 文件名称：CommunicationManager.cpp
* 文件标识：
* 摘    要：通讯处理
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

#include "Task.h"

// }}}USR
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
CommunicationManager::CommunicationManager( void )
{
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
CommunicationManager::~CommunicationManager( void )
{
}
// }}}RME

// {{{RME operation 'RunTask(Task*)'
bool CommunicationManager::RunTask( Task * task )
{
	// {{{USR
	    return AnalyseTask(task);   //分析并执行任务
	// }}}USR
}
// }}}RME

// {{{RME operation 'AnalyseTask(Task*)'
bool CommunicationManager::AnalyseTask( Task * task )
{
	// {{{USR

	    //分析任务，是采用FtpCtrl，还是采用NormalComm进行通讯处理。目前全部采用NormalComm
	    NormalComm normal;
	    return normal.RunTask(task);

	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR

// }}}USR
// }}}RME

// }}}RME
