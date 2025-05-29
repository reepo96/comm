// {{{RME classifier 'Logical View::FTPCFileFatcher'

#ifndef FTPCFileFatcher_H
#define FTPCFileFatcher_H

#ifdef PRAGMA
#pragma interface "FTPCFileFatcher.h"
#endif

#include <RTSystem/Communication.h>

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
#include "public.h"

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

class FTPCFileFatcher
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
	FTPCFileFatcher( void );
	// }}}RME
	// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
	virtual ~FTPCFileFatcher( void );
	// }}}RME
	// {{{RME operation 'GetFile()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	void GetFile( void );
	// }}}RME
	// {{{RME operation 'PutFile()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	void PutFile( void );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* FTPCFileFatcher_H */

// }}}RME
