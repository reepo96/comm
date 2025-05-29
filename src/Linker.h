// {{{RME classifier 'Logical View::Linker'

#ifndef Linker_H
#define Linker_H

#ifdef PRAGMA
#pragma interface "Linker.h"
#endif

#include <RTSystem/Communication.h>
class MsgQueue;
class QueRegister;

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR

/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* �ļ����ƣ�Linker.h
* �ļ���ʶ��
* ժ    Ҫ�����ӵĸ���
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
*
**************************************************************************/

#include "public.h"
#include "QueRegister.h"

#include <Protocol.h>

//ͨѶ�������
typedef enum linkErrorCode{LEC_NOERROR,LEC_LINKERR,LEC_LINK_TIMEOUT,LEC_USER_TERMINATE,LEC_LINK_UNKOWN_ERR,\
        LEC_PRO_UNKOWN_ERR,LEC_PRO_ERRCMD_NODATA,LEC_PRO_INTERRPUT,LEC_PRO_ERR_ANSWER,LEC_PRO_ERR_FRAME,\
		LEC_PRO_STARTDT_NOACT,LEC_PRO_RMT_STOP_TRANSFER,LEC_PRO_TIME_OUT,LEC_PRO_OPERAT_FILE_FAIL} LINKERRORCODE;

// }}}USR
// }}}RME

class Linker
{
public:
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR
#ifdef _MY_UNITTEST
	    //��Ԫ���Դ���
	    static bool m_bConnectFail; //�����ڵ�Ԫ���ԣ�ָʾlinker��connect�����ɹ���ʧ��

	    virtual string WhoAmI();
#endif

	    typedef enum LinkerType{LK_REMOTE,LK_ASDU,LK_UNKNOW} LINKERTYPE; //��������

		typedef enum CommType{CT_UNKNOW,CT_SOCKET,CT_COM,CT_MODEM} COMMTYPE; //ͨѶ����

	    /*************************************************
	    Function:       Listen
	    Description:    ������������
	    Calls:          
	    Called By:      
	    Input:          DisposeFun:������

	    Output:
	    Return:
	    *************************************************/	   
		virtual void Listen( DWORD (WINAPI DisposeFun)(void *pParm) = 0 );

		COMMTYPE GetCommType()
		{
			return m_CommType;
		}

		//��ȡ����������Ϣ
		void GetCmdDesc(struct Command & cmd);

	// }}}USR
	// }}}RME

protected:
	// {{{RME tool 'OT::Cpp' property 'ProtectedDeclarations'
	// {{{USR
	    PROTOCOL_TYPE m_ProtocolType;
	    LINKERTYPE   m_LinkerType;
		COMMTYPE	 m_CommType;

	    LINKERRORCODE m_errCode;
		int			m_iUserCount;

		unsigned int	m_nDevId;//���ӹ������豸ID
	// }}}USR
	// }}}RME

private:
	// {{{RME tool 'OT::Cpp' property 'PrivateDeclarations'
	// {{{USR

	// }}}USR
	// }}}RME

protected:
	// {{{RME classAttribute 'm_Link'
	// {{{RME general 'documentation'
	/* {{{USR
	ͨѶ���ӵľ��
	   }}}USR */
	// }}}RME
	HANDLE m_Link;
	// }}}RME
	// {{{RME classAttribute 'm_isConnect'
	// {{{RME general 'documentation'
	/* {{{USR
	�Ƿ������ӵ�
	   }}}USR */
	// }}}RME
	bool m_isConnect;
	// }}}RME
	// {{{RME classAttribute 'm_isInitComm'
	// {{{RME general 'documentation'
	/* {{{USR
	ͨѶ�Ƿ��Ѿ���ʼ������ͨѶ���Ӻ�Ҫ����ͨѶ��ʼ����
	   }}}USR */
	// }}}RME
	bool m_isInitComm;
	// }}}RME
	// {{{RME classAttribute 'm_isServerLink'
	// {{{RME general 'documentation'
	/* {{{USR
	�Ƿ��Ƿ������˵����ӣ�ͨ����ָsocketͨѶ�ķ���ˣ�
	   }}}USR */
	// }}}RME
	bool m_isServerLink;
	// }}}RME
	// {{{RME classAttribute 'm_isExitLink'
	// {{{RME general 'documentation'
	/* {{{USR
	�Ƿ�Ҫ�˳�����
	   }}}USR */
	// }}}RME
	bool m_isExitLink;
	// }}}RME
	// {{{RME associationEnd 'm_pProtocol'
	Protocol * m_pProtocol;
	// }}}RME
	// {{{RME associationEnd 'm_pMsgQue'
	MsgQueue * m_pMsgQue;
	// }}}RME

	bool m_isPreDisConnect;	//�Ƿ�ҪԤ�Ͽ�����

public:
		//     *************************************************
		//     Function:       PreClose
		//     Description:    Ԥ�ر����ӣ������߳���ͼ�ر���һ�̵߳�����ʱ���ô˽�ڣ�
		//     Calls:
		//     Called By:
		//     Input:
		// 
		//     Output:
		//     Return:
		// *************************************************
		void PreClose() { m_isPreDisConnect = true; }

		bool IsPreDisConnect() { return m_isPreDisConnect;}

		virtual int GetHandle() { return (int)m_Link;}

		void SetDevId(unsigned int nDevId) { m_nDevId = nDevId;}

		unsigned int GetDevId() { return m_nDevId;}

	// {{{RME operation 'Linker()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	Linker( void );
	// }}}RME
	// {{{RME operation '~Linker()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	virtual ~Linker( void );
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
	virtual void Init( void * parm, LINKERTYPE linkerType );
	// }}}RME
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
	virtual bool ExecuteCmd( struct Command & cmd );
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
	virtual bool Connect( void );
	// }}}RME
	// {{{RME operation 'BindUdpLink()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	virtual bool BindUdpLink( void );
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
	virtual int SendData( const char * pDataBuf, int iDataLen, unsigned int uiTimeOut );
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
	virtual bool CreatTask( void );
	// }}}RME
	// {{{RME operation 'GetUdpData(struct Command&)'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	virtual bool GetUdpData( struct Command & cmd );
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
	virtual int ReceiveData( char * pDataBuf, int iBufferLen, unsigned int uiTimeOut );
	// }}}RME
	// {{{RME operation 'ReceiveDataFrom(char*,int,unsigned int,struct sockaddr&)'
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
	virtual int ReceiveDataFrom( char * pDataBuf, int iBufferLen, unsigned int uiTimeOut, struct sockaddr & clientAddr );
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
	virtual bool Close( void );
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
	virtual string GetConnectAddr( void );
	// }}}RME
	// {{{RME operation 'IsConnect()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       IsConnect
	    Description:    �ж��Ƿ����ӵ�
	    Calls:          
	    Called By:      
	    Input:          

	    Output:         
	    Return:         �Ѿ����ӷ���true�����򷵻�false
	    *************************************************	
	   }}}USR */
	// }}}RME
	bool IsConnect( void );
	// }}}RME
	// {{{RME operation 'IsInitComm()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       IsInitComm
	    Description:    ͨѶ�Ƿ��Ѿ���ʼ������ͨѶ���Ӻ�Ҫ����ͨѶ��ʼ����
	    Calls:          
	    Called By:      
	    Input:          

	    Output:         
	    Return:         �Ѿ���ʼ������true�����򷵻�false
	    *************************************************		
	   }}}USR */
	// }}}RME
	bool IsInitComm( void );
	// }}}RME
	// {{{RME operation 'IsServerLink()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       IsServerLink
	    Description:    �Ƿ��Ƿ������˵����ӣ�ͨ����ָsocketͨѶ�ķ���ˣ�
	    Calls:          
	    Called By:      
	    Input:          

	    Output:         
	    Return:         �Ƿ���true�����򷵻�false
	    *************************************************	
	   }}}USR */
	// }}}RME
	bool IsServerLink( void );
	// }}}RME
	// {{{RME operation 'GetLinkType()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	LINKERTYPE GetLinkType( void );
	// }}}RME
	// {{{RME operation 'ExitLink()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       ExitLink
	    Description:    �˳�����
	    Calls:          
	    Called By:      
	    Input:          

	    Output:         
	    Return:         
	    *************************************************		
	   }}}USR */
	// }}}RME
	void ExitLink( void );
	// }}}RME
	// {{{RME operation 'GetLastErrCode()'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	      Function:       GetLastErrCode
	      Description:    ������������һ�δ������
	      Calls:          
	      Called By:      
	      Input:          
	                      
	      Output:         
	      Return:         �������
	    *************************************************
	   }}}USR */
	// }}}RME
	LINKERRORCODE GetLastErrCode( void );
	// }}}RME
	// {{{RME operation 'GetLastErrDesc()'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	      Function:       GetLastErrDesc
	      Description:    ������������һ�δ�������
	      Calls:          
	      Called By:      
	      Input:          
	                      
	                      
	      Output:         
	      Return:         ��������
	    *************************************************
	   }}}USR */
	// }}}RME
	const char * GetLastErrDesc( void );
	// }}}RME
	// {{{RME operation 'DoIdle()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       DoIdle
	    Description:    �������
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    
	    Output:         
	    Return:         
	    *************************************************	
	   }}}USR */
	// }}}RME
	void DoIdle( void );
	// }}}RME

	//���������ϴ�����
	void DealAutoSend()
	{
		if( m_pProtocol )
		{
			m_pProtocol->DealAutoSend(this);
		}
	}

	void AddRef(){ m_iUserCount++;}

	void SaftDelete()
	{
		m_iUserCount--;
		if( m_iUserCount <= 0 )
		{
			delete this;
		}
	}
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* Linker_H */

// }}}RME
