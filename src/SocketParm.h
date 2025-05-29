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
* 文件名称：SocketParm.h
* 文件标识：
* 摘    要：tcp/ip通讯参数结构类
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
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
	本地IP Address
	   }}}USR */
	// }}}RME
	char loacalAddr[ 16 ];
	// }}}RME
	// {{{RME classAttribute 'loacalTcpPort'
	// {{{RME general 'documentation'
	/* {{{USR
	本地tcp连接端口
	   }}}USR */
	// }}}RME
	int loacalTcpPort;
	// }}}RME
	// {{{RME classAttribute 'loacalUdpPort'
	// {{{RME general 'documentation'
	/* {{{USR
	本地udp连接端口
	   }}}USR */
	// }}}RME
	int loacalUdpPort;
	// }}}RME
	// {{{RME classAttribute 'connetToAddr'
	// {{{RME general 'documentation'
	/* {{{USR
	链接的目的IP Address
	   }}}USR */
	// }}}RME
	char connetToAddr[ 16 ];
	// }}}RME
	// {{{RME classAttribute 'connetToTcpPort'
	// {{{RME general 'documentation'
	/* {{{USR
	链接目的tcp连接端口
	   }}}USR */
	// }}}RME
	int connetToTcpPort;
	// }}}RME
	// {{{RME classAttribute 'connetToUdpPort'
	// {{{RME general 'documentation'
	/* {{{USR
	链接udp连接端口
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
