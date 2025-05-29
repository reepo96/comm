// {{{RME classifier 'Logical View::ModemLinker'

#ifndef ModemLinker_H
#define ModemLinker_H

#ifdef PRAGMA
#pragma interface "ModemLinker.h"
#endif

#include <RTSystem/Communication.h>
#include <Linker.h>

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
#include "public.h"
#include "ModemParm.h"

/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* �ļ����ƣ�ModenLinker.h
* �ļ���ʶ��
* ժ    Ҫ������modem���ӵ���
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
*
**************************************************************************/
class Protocol;

// }}}USR
// }}}RME

class ModemLinker : public Linker
{
public:
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR
#ifdef _MY_UNITTEST
	    //��Ԫ���Դ���
	    string WhoAmI();
#endif

	    typedef struct tagModemId
	    {
	        HANDLE hModem;
	        char modemName[1];
	    }MODEMID;

	    /*************************************************
	    Function:       Listen
	    Description:    ������������
	    Calls:
	    Called By:
	    Input:          DisposeFun��������

	    Output:
	    Return:
	    *************************************************/	   
	    void Listen( DWORD (WINAPI DisposeFun)(void *pParm) = 0 );

	// }}}USR
	// }}}RME

protected:
	// {{{RME tool 'OT::Cpp' property 'ProtectedDeclarations'
	// {{{USR

	    bool	m_bIsOpen;
		HANDLE	m_hComDev;

		bool	m_bIsCmdState;	//modem�Ƿ�������״̬��

	// }}}USR
	// }}}RME

private:
	// {{{RME tool 'OT::Cpp' property 'PrivateDeclarations'
	// {{{USR

	// }}}USR
	// }}}RME

protected:
	// {{{RME classAttribute 'strLinkParm'
	// {{{RME general 'documentation'
	/* {{{USR
	���Ӳ���
	   }}}USR */
	// }}}RME
	ModemParm strLinkParm;
	// }}}RME

public:
	// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
	virtual ~ModemLinker( void );
	// }}}RME
	// {{{RME operation 'ModemLinker()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	ModemLinker( void );
	// }}}RME
	// {{{RME operation 'Init(void*,LINKERTYPE)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	      Function:       Init
	      Description:    ��ʼ������
	      Calls:
	      Called By:
	      Input:          parm:ͨѶ�����������
	                      linkerType:��������(����Զ�̻�����DAU)
	      Output:
	      Return:
	    *************************************************	
	   }}}USR */
	// }}}RME
	void Init( void * parm, LINKERTYPE linkerType );
	// }}}RME

protected:
	// {{{RME operation 'OpenCom()'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	      Function:       OpenCom
	      Description:    �򿪴���
	      Calls:
	      Called By:
	      Input:
	                      
	      Output:
	      Return:         ִ�гɹ�����true�����򷵻�false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool OpenCom( void );
	// }}}RME

public:
	// {{{RME operation 'ExecuteCmd(struct Command&)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	      Function:       ExecuteCmd
	      Description:    ִ��ͨѶ����
	      Calls:
	      Called By:
	      Input:          cmd������ͨѶ������Ϣ

	      Output:
	      Return:         ִ�гɹ�����true�����򷵻�false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool ExecuteCmd( struct Command & cmd );
	// }}}RME
	// {{{RME operation 'Connect()'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	      Function:       Connect
	      Description:    ���ӵ�������
	      Calls:
	      Called By:
	      Input:

	      Output:
	      Return:         ���ӳɹ�����true�����򷵻�false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool Connect( void );
	// }}}RME
	// {{{RME operation 'SendData(const char*,int,unsigned int)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	      Function:       SendData
	      Description:    ��������
	      Calls:
	      Called By:
	      Input:          pDataBuf��ָ��Ҫ���͵�����
	                      iDataLen��Ҫ�������ݵĳ���
	      Output:
	      Return:         ʵ�ʷ������ݵĳ���
	    *************************************************
	   }}}USR */
	// }}}RME
	int SendData( const char * pDataBuf, int iDataLen, unsigned int uiTimeOut );
	// }}}RME
	// {{{RME operation 'CreatTask()'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	      Function:       CreatTask
	      Description:    ���������ݺ���ô˺�����������
	      Calls:
	      Called By:
	      Input:

	      Output:
	      Return:         �����ɹ�����true��ʧ�ܷ���false
	    *************************************************	
	   }}}USR */
	// }}}RME
	bool CreatTask( void );
	// }}}RME
	// {{{RME operation 'ReceiveData(char*,int,unsigned int)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	      Function:       ReceiveData
	      Description:    ��������
	      Calls:
	      Called By:
	      Input:          pDataBuf��������յ�������
	                      iBufferLen�������������ݵĳ���
	                      uiTimeOut����ʱʱ�䣨��λ�����룩��Ϊ0��������ʱ
	      Output:
	      Return:         ʵ�ʽ��յ������ݳ���
	    *************************************************
	   }}}USR */
	// }}}RME
	int ReceiveData( char * pDataBuf, int iBufferLen, unsigned int uiTimeOut );
	// }}}RME
	// {{{RME operation 'Close()'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	      Function:       Close
	      Description:    �ر�����
	      Calls:
	      Called By:
	      Input:

	      Output:
	      Return:         �ɹ��رշ���true�����򷵻�false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool Close( void );
	// }}}RME
	// {{{RME operation 'GetConnectAddr()'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	      Function:       GetConnectAddr
	      Description:    �������Ŀ�ĵĵ�ַ��IP��ַ���绰������ߴ�����
	      Calls:
	      Called By:
	      Input:

	      Output:
	      Return:         ��������Ŀ�ĵĵ�ַ
	    *************************************************
	   }}}USR */
	// }}}RME
	string GetConnectAddr( void );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* ModemLinker_H */

// }}}RME
