// {{{RME classifier 'Logical View::SocketParm'

#ifndef SocketParm_H
#define SocketParm_H

#ifdef PRAGMA
#pragma interface "SocketParm.h"
#endif

#include <RTSystem/Communication.h>

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
#include "public.h"

/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* �ļ����ƣ�SocketParm.h
* �ļ���ʶ��
* ժ    Ҫ��tcp/ipͨѶ�����ṹ��
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
*
**************************************************************************/

// }}}USR
// }}}RME

struct SocketParm
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
	// {{{RME classAttribute 'loacalAddr'
	// {{{RME general 'documentation'
	/* {{{USR
	����IP Address
	   }}}USR */
	// }}}RME
	char loacalAddr[ 16 ];
	// }}}RME
	// {{{RME classAttribute 'loacalTcpPort'
	// {{{RME general 'documentation'
	/* {{{USR
	����tcp���Ӷ˿�
	   }}}USR */
	// }}}RME
	int loacalTcpPort;
	// }}}RME
	// {{{RME classAttribute 'loacalUdpPort'
	// {{{RME general 'documentation'
	/* {{{USR
	����udp���Ӷ˿�
	   }}}USR */
	// }}}RME
	int loacalUdpPort;
	// }}}RME
	// {{{RME classAttribute 'connetToAddr'
	// {{{RME general 'documentation'
	/* {{{USR
	���ӵ�Ŀ��IP Address
	   }}}USR */
	// }}}RME
	char connetToAddr[ 16 ];
	// }}}RME
	// {{{RME classAttribute 'connetToTcpPort'
	// {{{RME general 'documentation'
	/* {{{USR
	����Ŀ��tcp���Ӷ˿�
	   }}}USR */
	// }}}RME
	int connetToTcpPort;
	// }}}RME
	// {{{RME classAttribute 'connetToUdpPort'
	// {{{RME general 'documentation'
	/* {{{USR
	����udp���Ӷ˿�
	   }}}USR */
	// }}}RME
	int connetToUdpPort;
	// }}}RME
	// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
	SocketParm( void );
	// }}}RME
	// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
	virtual ~SocketParm( void );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* SocketParm_H */

// }}}RME
