// {{{RME classifier 'Logical View::NormalComm'

#ifndef NormalComm_H
#define NormalComm_H

#ifdef PRAGMA
#pragma interface "NormalComm.h"
#endif

#include <RTSystem/Communication.h>

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
#include "public.h"
#include "Task.h"
#include "Linker.h"

/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* �ļ����ƣ�SystemCfger.h
* �ļ���ʶ��
* ժ    Ҫ��ϵͳ�����࣬���ڱ���ϵͳ��������Ϣ
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
*
**************************************************************************/

// }}}USR
// }}}RME

class NormalComm
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
	NormalComm( void );
	// }}}RME
	// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
	virtual ~NormalComm( void );
	// }}}RME
	// {{{RME operation 'RunTask(Task*)'
	// {{{RME general 'documentation'
	/* {{{USR
	ִ������
	   }}}USR */
	// }}}RME
	bool RunTask( Task * pTask );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* NormalComm_H */

// }}}RME
