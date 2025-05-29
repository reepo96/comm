// {{{RME classifier 'Logical View::Task'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "Task.h"
#endif

#include <RTSystem/Communication.h>
#include <Task.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* �ļ����ƣ�Task.cpp
* �ļ���ʶ��
* ժ    Ҫ��������
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
*
**************************************************************************/

#include "Command.h"
#include "Linker.h"

// }}}USR
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
Task::Task( void )
	: Linker( NULL )
	, createTime( 0 )
{
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
Task::~Task( void )
{
	while(cmd.resultList.size() > 0)
	{
		PCMDRESULT pResult = cmd.resultList.back();
		if( pResult )
		{
			cmd.resultList.pop_back();
			delete pResult;
			pResult=NULL;
			cmd.resultNum--;
		}
		else
		{
			break;
		}
	}
	cmd.resultNum = 0;
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR

Task * Task::Clone()
{
	Task *pTask = new Task();
	pTask->Linker = this->Linker;
	pTask->createTime = this->createTime;
	this->cmd.Clone( &(pTask->cmd) );

	return pTask;
}

// }}}USR
// }}}RME

// }}}RME
