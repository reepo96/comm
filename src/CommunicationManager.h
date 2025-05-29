// {{{RME classifier 'Logical View::CommunicationManager'

#ifndef CommunicationManager_H
#define CommunicationManager_H

#ifdef PRAGMA
#pragma interface "CommunicationManager.h"
#endif

#include <RTSystem/Communication.h>
class FtpCtrl;
class NormalComm;

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
#include "public.h"

/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* �ļ����ƣ�CommunicationManager.h
* �ļ���ʶ��
* ժ    Ҫ��ͨѶ����
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
*
**************************************************************************/

struct Task;

// }}}USR
// }}}RME

class CommunicationManager
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
	CommunicationManager( void );
	// }}}RME
	// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
	virtual ~CommunicationManager( void );
	// }}}RME
	// {{{RME operation 'RunTask(Task*)'
	// {{{RME general 'documentation'
	/* {{{USR
	ִ������
	   }}}USR */
	// }}}RME
	bool RunTask( Task * task );
	// }}}RME

protected:
	// {{{RME operation 'AnalyseTask(Task*)'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	bool AnalyseTask( Task * task );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* CommunicationManager_H */

// }}}RME
