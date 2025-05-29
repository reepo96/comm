// {{{RME classifier 'Logical View::MemMapMgr'

#ifndef MemMapMgr_H
#define MemMapMgr_H

#ifdef PRAGMA
#pragma interface "MemMapMgr.h"
#endif

#include <RTSystem/Communication.h>

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
#include "public.h"
#include "Task.h"

/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* 文件名称：MemMapMgr.h
* 文件标识：
* 摘    要：共享内存处理类
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

// }}}USR
// }}}RME

class MemMapMgr
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

protected:
	// {{{RME classAttribute 'm_hCmdMapFile'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	HANDLE m_hCmdMapFile;
	// }}}RME
	// {{{RME classAttribute 'm_pCmdMapData'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	unsigned char * m_pCmdMapData;
	// }}}RME
	// {{{RME classAttribute 'm_hResMapFile'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	HANDLE m_hResMapFile;
	// }}}RME
	// {{{RME classAttribute 'm_pResMapData'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	unsigned char * m_pResMapData;
	// }}}RME

public:
	// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
	MemMapMgr( void );
	// }}}RME
	// {{{RME operation '~MemMapMgr()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	virtual ~MemMapMgr( void );
	// }}}RME

protected:
	// {{{RME operation 'OpenCmdMapFile()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	void OpenCmdMapFile( void );
	// }}}RME
	// {{{RME operation 'CloseCmdMapFile()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	void CloseCmdMapFile( void );
	// }}}RME
	// {{{RME operation 'OpenResMapFile()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	void OpenResMapFile( void );
	// }}}RME
	// {{{RME operation 'CloseResMapFile()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	void CloseResMapFile( void );
	// }}}RME

public:
	// {{{RME operation 'GetTaskFromMem()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetTaskFromMem
	    Description:    从共享内存中读取通讯任务
	    Calls:          
	    Called By:      
	    Input:          
	                    
	    Output:         
	    Return:         成功返回任务，失败返回NULL
	    *************************************************	
	   }}}USR */
	// }}}RME
	Task * GetTaskFromMem( void );
	// }}}RME
	// {{{RME operation 'WriteTaskToMem(Task*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       WriteTaskToMem
	    Description:    将任务写入内存中
	    Calls:          
	    Called By:      
	    Input:          task：任务信息
	                    
	    Output:         
	    Return:         成功返回true，失败返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	bool WriteTaskToMem( Task * task );
	// }}}RME
	// {{{RME operation 'WriteTaskResultToMem(const struct Command&)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       WriteTaskResultToMem
	    Description:    将任务执行结果写入共享内存中
	    Calls:          
	    Called By:      
	    Input:          cmd：命令信息
	                    
	    Output:         
	    Return:         成功返回true，失败返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	bool WriteTaskResultToMem( const struct Command & cmd );
	// }}}RME

	void PrintShareMemoryInfo(unsigned char *  pMem);
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* MemMapMgr_H */

// }}}RME
