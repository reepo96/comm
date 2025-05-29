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
* �ļ����ƣ�Task.h
* �ļ���ʶ��
* ժ    Ҫ��������
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
*
**************************************************************************/

class Linker;

// }}}USR
// }}}RME

struct Task
{
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR

	//��¡����
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
	����
	   }}}USR */
	// }}}RME
	Linker * Linker;
	// }}}RME
	// {{{RME classAttribute 'cmd'
	// {{{RME general 'documentation'
	/* {{{USR
	����Ҫִ�е�����
	   }}}USR */
	// }}}RME
	struct Command cmd;
	// }}}RME
	// {{{RME classAttribute 'createTime'
	// {{{RME general 'documentation'
	/* {{{USR
	���񴴽�ʱ��
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
