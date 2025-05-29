// {{{RME classifier 'Logical View::QueRegister'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "QueRegister.h"
#endif

#include <RTSystem/Communication.h>
#include <QueRegister.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* 文件名称：QueRegister.cpp
* 文件标识：
* 摘    要：队列注册类
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

#pragma warning( disable : 4786 )

// }}}USR
// }}}RME

// {{{RME classAttribute '_instance'
QueRegister QueRegister::_instance;
// }}}RME

// {{{RME operation 'QueRegister()'
QueRegister::QueRegister( void )
	// {{{RME tool 'OT::Cpp' property 'ConstructorInitializer'
	// {{{USR

	// }}}USR
	// }}}RME
{
	// {{{USR
	m_hEvent = ::CreateEvent(NULL,FALSE,TRUE,NULL);
	// }}}USR
}
// }}}RME

// {{{RME operation '~QueRegister()'
QueRegister::~QueRegister( void )
{
	// {{{USR
	if(m_hEvent != NULL)
	{
		::SetEvent(m_hEvent);
		::CloseHandle(m_hEvent);
		m_hEvent = NULL;
	}

	map< string, MsgQueue * >::iterator it = queueMap.begin();
	for(;it != queueMap.end(); it++)
	{
		MsgQueue *pQue = (*it).second;
		delete pQue;
		pQue = NULL;
	}
	queueMap.clear();
	// }}}USR
}
// }}}RME

// {{{RME operation 'GetInstance()'
QueRegister & QueRegister::GetInstance( void )
{
	// {{{USR
	    return _instance;
	// }}}USR
}
// }}}RME

// {{{RME operation 'RegisterQue(const string)'
MsgQueue * QueRegister::RegisterQue( const string queName )
{
	// {{{USR
	if(m_hEvent != NULL)
	{
		::WaitForSingleObject(m_hEvent,INFINITE);
	}

	map<string, MsgQueue*>::iterator it = queueMap.find(queName);
	if(it == queueMap.end())  //queue还不存在
	{
		MsgQueue *pQue = new MsgQueue();
		queueMap.insert(map<string, MsgQueue*>::value_type(queName,pQue));

		if(m_hEvent != NULL)
		{
			::SetEvent(m_hEvent);
		}
		return pQue;
	}
	else    //queue已存在
	{
		if(m_hEvent != NULL)
		{
			::SetEvent(m_hEvent);
		}
		return (*it).second;
	}
	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR

// }}}USR
// }}}RME

// }}}RME
