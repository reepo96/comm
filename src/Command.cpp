// {{{RME classifier 'Logical View::Command'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "Command.h"
#endif

#include <RTSystem/Communication.h>
#include <Command.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* 文件名称：Command.cpp
* 文件标识：
* 摘    要：本系统内部使用的命令结构
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

#pragma warning( disable : 4786 )

// }}}USR
// }}}RME

// {{{RME operation 'Command()'
Command::Command( void )
	// {{{RME tool 'OT::Cpp' property 'ConstructorInitializer'
	// {{{USR

	// }}}USR
	// }}}RME
{
	// {{{USR
		processType = 0;
		processId = 0;
		isSplitCmd = false;
	    msgId = 0;
		from = (CMDDIR)0;
		to = (CMDDIR)0;
		destDevId = 0;
		funCode = 0;
		parmLen = 0;
		pParm = NULL;
		resultNum = 0;
		memset(describe,0,sizeof(describe));
	// }}}USR
}
// }}}RME

// {{{RME operation '~Command()'
Command::~Command( void )
{
	// {{{USR
		if(parmLen >0 && pParm != NULL)
		{
			delete []pParm;
			pParm = NULL;
		}

	    vector< PCMDRESULT >::iterator it = resultList.begin();
		for(;it != resultList.end();it++)
		{
			PCMDRESULT pResult = (*it);
			if(pResult != NULL)
			{
				if(pResult->pData != NULL)
				{
					delete []pResult->pData;
					pResult->pData = NULL;
				}
				delete pResult;
			}
			pResult = NULL;
		}
		resultList.clear();

	// }}}USR
}
// }}}RME

// {{{RME operation 'operator==(const Command &)'
bool Command::operator==( const Command & rtg_arg )
{
	// {{{USR
	    if( this->destDevId == rtg_arg.destDevId &&
	        this->from == rtg_arg.from &&
	        this->to == rtg_arg.to && 
	        this->funCode == rtg_arg.funCode )
	    {
	        return true;
	    }

	    return false;
	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR

void Command::Clone(struct Command *pNewCmd)
{
	pNewCmd->_state._state = this->_state._state;
	strcpy(pNewCmd->describe,this->describe);
	pNewCmd->destDevId = this->destDevId;
	pNewCmd->from = this->from;
	pNewCmd->funCode = this->funCode;
	pNewCmd->msgId = this->msgId;
	pNewCmd->parmLen = this->parmLen;
	if(pNewCmd->parmLen > 0)
	{
		pNewCmd->pParm = new char[pNewCmd->parmLen+1];
		memset(pNewCmd->pParm,0,pNewCmd->parmLen+1);
		memcpy(pNewCmd->pParm,this->pParm,pNewCmd->parmLen);
	}
	else
	{
		pNewCmd->pParm = NULL;
	}

	vector< PCMDRESULT >::iterator it = this->resultList.begin();
	for(;it != this->resultList.end();it++)
	{
		PCMDRESULT pCmdResult = new CMDRESULT();
		pCmdResult->resultType = (*it)->resultType;
		pCmdResult->resultLen = (*it)->resultLen;
		pCmdResult->pData = new char[pCmdResult->resultLen];
		memcpy(pCmdResult->pData,(*it)->pData,pCmdResult->resultLen);
		pNewCmd->resultList.push_back(pCmdResult);
	}

	pNewCmd->processId = this->processId;
	pNewCmd->processType = this->processType;
	pNewCmd->to = this->to;
}

// }}}USR
// }}}RME

// }}}RME
