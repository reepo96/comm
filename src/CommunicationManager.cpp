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
* �ļ����ƣ�CommunicationManager.cpp
* �ļ���ʶ��
* ժ    Ҫ��ͨѶ����
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
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
	    return AnalyseTask(task);   //������ִ������
	// }}}USR
}
// }}}RME

// {{{RME operation 'AnalyseTask(Task*)'
bool CommunicationManager::AnalyseTask( Task * task )
{
	// {{{USR

	    //���������ǲ���FtpCtrl�����ǲ���NormalComm����ͨѶ����Ŀǰȫ������NormalComm
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
