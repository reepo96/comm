// {{{RME classifier 'Logical View::Timer'

#ifndef Timer_H
#define Timer_H

#ifdef PRAGMA
#pragma interface "Timer.h"
#endif

#include <RTSystem/Communication.h>
class MsgQueue;
class QueRegister;

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
#include "public.h"

/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* �ļ����ƣ�Timer.h
* �ļ���ʶ��
* ժ    Ҫ����ʱ��
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
*
**************************************************************************/

DWORD WINAPI GetTimerTaskResult(void *pParm);     //���ն�ʱ����ִ�н�����߳�

// }}}USR
// }}}RME

class Timer
{
public:
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR

	    friend DWORD WINAPI GetTimerTaskResult(void *pParm);

	// }}}USR
	// }}}RME

protected:
	// {{{RME tool 'OT::Cpp' property 'ProtectedDeclarations'
	// {{{USR
	        int m_iMsgId;
			MsgQueue * m_pTimerResultMsgQue; //��Ŷ�ʱ����ִ�н������Ϣ����

			int GetRecWaveFileList( unsigned char devId);
			int GetRecWaveFile(const unsigned char devId,const char* fileName,int startPos = 0);
	// }}}USR
	// }}}RME

private:
	// {{{RME tool 'OT::Cpp' property 'PrivateDeclarations'
	// {{{USR

	// }}}USR
	// }}}RME

protected:
	// {{{RME associationEnd 'm_pMsgQue'
	MsgQueue * m_pMsgQue;
	// }}}RME

public:
	// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
	virtual ~Timer( void );
	// }}}RME
	// {{{RME operation 'Timer()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	Timer( void );
	// }}}RME
	// {{{RME operation 'Run()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	void Run( void );
	// }}}RME

protected:
	// {{{RME operation 'SendTask(PCFGTIMERCALL)'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	void SendTask( PCFGTIMERCALL pTimerCall );
	// }}}RME
	// {{{RME operation 'GetFileList(unsigned char,unsigned char)'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	int GetFileList( unsigned char devId, unsigned char dirType );
	// }}}RME
	// {{{RME operation 'PullFile(unsigned char,unsigned char,int,int,char *,char *)'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	int PullFile( unsigned char devId, unsigned char fileType, int fileNo, int fileLen, char * fileName, char * fileTime );
	// }}}RME
	// {{{RME operation 'GetTaskResult()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	void GetTaskResult( void );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* Timer_H */

// }}}RME
