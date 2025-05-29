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
* �ļ����ƣ�MemMapMgr.h
* �ļ���ʶ��
* ժ    Ҫ�������ڴ洦����
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
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
	    Description:    �ӹ����ڴ��ж�ȡͨѶ����
	    Calls:          
	    Called By:      
	    Input:          
	                    
	    Output:         
	    Return:         �ɹ���������ʧ�ܷ���NULL
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
	    Description:    ������д���ڴ���
	    Calls:          
	    Called By:      
	    Input:          task��������Ϣ
	                    
	    Output:         
	    Return:         �ɹ�����true��ʧ�ܷ���false
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
	    Description:    ������ִ�н��д�빲���ڴ���
	    Calls:          
	    Called By:      
	    Input:          cmd��������Ϣ
	                    
	    Output:         
	    Return:         �ɹ�����true��ʧ�ܷ���false
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
