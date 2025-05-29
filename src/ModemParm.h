// {{{RME classifier 'Logical View::ModemParm'

#ifndef ModemParm_H
#define ModemParm_H

#ifdef PRAGMA
#pragma interface "ModemParm.h"
#endif

#include <RTSystem/Communication.h>

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
#include "public.h"

/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* �ļ����ƣ�ModenParm.h
* �ļ���ʶ��
* ժ    Ҫ��modemͨѶ�����ṹ��
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
*
**************************************************************************/

// }}}USR
// }}}RME

struct ModemParm
{
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
	// {{{RME classAttribute 'phoneNum'
	// {{{RME general 'documentation'
	/* {{{USR
	�绰����
	   }}}USR */
	// }}}RME
	char phoneNum[ 20 ];
	// }}}RME
	// {{{RME classAttribute 'comName'
	// {{{RME general 'documentation'
	/* {{{USR
	�˿�����
	   }}}USR */
	// }}}RME
	char comName[ 10 ];
	// }}}RME
	// {{{RME classAttribute 'baudrate'
	// {{{RME general 'documentation'
	/* {{{USR
	������
	   }}}USR */
	// }}}RME
	int baudrate;
	// }}}RME
	// {{{RME classAttribute 'parity'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	char parity;
	// }}}RME
	// {{{RME classAttribute 'data'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	int data;
	// }}}RME
	// {{{RME classAttribute 'stop'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	int stop;
	// }}}RME
	// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
	virtual ~ModemParm( void );
	// }}}RME
	// {{{RME operation 'ModemParm()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	ModemParm( void );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* ModemParm_H */

// }}}RME
