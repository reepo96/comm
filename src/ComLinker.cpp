// {{{RME classifier 'Logical View::ComLinker'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "ComLinker.h"
#endif

#include <RTSystem/Communication.h>
#include <ComLinker.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* �ļ����ƣ�ComLinker.cpp
* �ļ���ʶ��
* ժ    Ҫ�������������ʵ���ļ�
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
*
**************************************************************************/
#pragma warning( disable : 4786 )

#include "Protocol103.h"
#include "Protocol60_103.h"

// }}}USR
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
ComLinker::ComLinker( void )
{
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
ComLinker::~ComLinker( void )
{
}
// }}}RME

// {{{RME operation 'Init(void*,LINKERTYPE)'
void ComLinker::Init( void * parm, LINKERTYPE linkerType )
{
	// {{{USR
	m_CommType = CT_COM;
	//��ʼ�����Ӳ���
	memset(&strLinkParm,0,sizeof(ComParm));
	memcpy(&strLinkParm,parm,sizeof(ComParm));

	//���ø��ຯ��
	Linker::Init(parm,linkerType);

	// }}}USR
}
// }}}RME

// {{{RME operation 'ExecuteCmd(struct Command&)'
bool ComLinker::ExecuteCmd( struct Command & cmd )
{
	// {{{USR

	    assert(m_pProtocol != NULL);

	    return m_pProtocol->ExecuteCmd(this,cmd);

	// }}}USR
}
// }}}RME

// {{{RME operation 'Connect()'
bool ComLinker::Connect( void )
{
	// {{{USR
#ifdef _MY_UNITTEST
	        //��Ԫ���Դ���
	        if(m_bConnectFail)
	        {
	            m_isExitLink = false;
	            m_isConnect = false;
	            m_isInitComm = false;
	            return false;
	        }
	        else
	        {
	            m_isExitLink = false;
	        m_isConnect = true;
	        m_isInitComm = true;
	            return true;
	        }
#endif

	        m_isConnect = false;
	        m_isInitComm = false;

	        //�򿪴���
	        m_hComDev = CreateFile(strLinkParm.comName,GENERIC_READ|GENERIC_WRITE,0,NULL,
	            OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	        if(m_hComDev == INVALID_HANDLE_VALUE)
	        {
	            mylog.WriteLog2File(LogControl::LERROR,"open com error");
	            return false;
	        }

	        //��ʼ����������
	        DCB dcb;
	        memset(&dcb,0,sizeof(DCB));
	        dcb.DCBlength = sizeof(DCB);

	        char dcbInitString[50]={0};
	        sprintf(&dcbInitString[0],"%s: baud=%d parity=%c data=%d stop=1",
	            strLinkParm.comName,strLinkParm.baudrate,strLinkParm.parity,strLinkParm.data);
	        if(!BuildCommDCB(dcbInitString,&dcb))
	        {
	            mylog.WriteLog2File(LogControl::LERROR,"build DCB error");
	            Close();
	            return false;
	        }

	        dcb.fRtsControl = RTS_CONTROL_ENABLE;
			dcb.fDtrControl = DTR_CONTROL_ENABLE;
			dcb.fOutxCtsFlow = TRUE;

	        if(!SetCommState(m_hComDev,&dcb) || !SetupComm(m_hComDev,1600,1600))
	        {
	            mylog.WriteLog2File(LogControl::LERROR,"set com error");
	            Close();
	            return false;
	        }

	        COMMTIMEOUTS tOut;
	        if(!GetCommTimeouts(m_hComDev,&tOut))
	        {
	            mylog.WriteLog2File(LogControl::LERROR,"get com timeouts error");
	            Close();
	            return false;
	        }
	        tOut.WriteTotalTimeoutConstant = 5000;
	        tOut.ReadTotalTimeoutConstant = 5000;
	        if(!SetCommTimeouts(m_hComDev,&tOut))
	        {
	            mylog.WriteLog2File(LogControl::LERROR,"set com timeouts error");
	            Close();
	            return false;
	        }

	        m_isConnect = true;


	        //Э�����ӳ�ʼ��
	        if(!m_pProtocol->InitConnect(this))
	        {
	            this->Close();
	            mylog.WriteLog2File(LogControl::LERROR,"the protocol init connect with com error");
	            return false;
	        }

	        m_isInitComm = true;
	        return true;
	// }}}USR
}
// }}}RME

// {{{RME operation 'SendData(const char*,int,unsigned int)'
int ComLinker::SendData( const char * pDataBuf, int iDataLen, unsigned int uiTimeOut )
{
	// {{{USR
	assert(pDataBuf != NULL);

	DWORD writenLen = 0;    //ʵ��д�Ĵ�С
	DWORD remainLen   = iDataLen ;   //���ж���������Ҫд
	unsigned int waitTime = 0;      //�ȴ�ʱ��
	DWORD iHasSendLen = 0;

	uiTimeOut *= 2;

	while( remainLen > 0 )
	{
		if(uiTimeOut != 0 && waitTime>= uiTimeOut)
		{
			if(uiTimeOut > 2000)
				m_errCode = LEC_LINK_TIMEOUT;
			break;
		}

		COMSTAT comStat;
		DWORD   flags;
		BOOL result = ClearCommError(m_hComDev,&flags,&comStat);
		if(!result)
		{
			m_errCode = LEC_LINKERR;
			mylog.WriteLog2File(LogControl::LERROR,"send data error when use com linker");
			return -1;
		}

		if(!WriteFile(m_hComDev,pDataBuf,remainLen,&writenLen,NULL))
		{
			m_errCode = LEC_LINKERR;
			mylog.WriteLog2File(LogControl::LERROR,"send data error when use com linker");
			return -1;
		}

		waitTime = 0;
		iHasSendLen += writenLen;
		remainLen -= writenLen;
		pDataBuf += writenLen;

		if(m_isExitLink)    //�˳�����
		{
			m_errCode = LEC_USER_TERMINATE;
			this->Close();
			break;
		}
	}

	return iHasSendLen;
	// }}}USR
}
// }}}RME

// {{{RME operation 'CreatTask()'
bool ComLinker::CreatTask( void )
{
	// {{{USR

	//��Э�����������
	Task* pTask = m_pProtocol->Decode(this);
	if(pTask != NULL)   
	{
		//����������Դ
		if(m_LinkerType != LK_REMOTE)
		{
			pTask->cmd.from = CD_ASDU;
			pTask->cmd.to   = CD_NEURON;
		}

		if(m_pMsgQue != NULL)
		{
			if(!m_pMsgQue->SendMsg(pTask))  //�������͵����������
			{
				delete pTask;
				pTask = NULL;
				return false;
			}
			return true;
		}
	}

	return false;

	// }}}USR
}
// }}}RME

// {{{RME operation 'ReceiveData(char*,int,unsigned int)'
int ComLinker::ReceiveData( char * pDataBuf, int iBufferLen, unsigned int uiTimeOut )
{
	// {{{USR
	assert(pDataBuf != NULL);

	int readDataLen = 0;    //ʵ�ʶ�ȡ�Ĵ�С
	int remainLen   = iBufferLen ;   //���ж���������Ҫ��ȡ
	DWORD recvLen = 1;

	unsigned int waitTime = 0;      //�ȴ�ʱ��
	uiTimeOut *= 2;

	while( recvLen > 0 && readDataLen < iBufferLen)
	{
		if(m_isExitLink)    //�˳�����
		{
			m_errCode = LEC_USER_TERMINATE;
			this->Close();
			break;
		}

		if(uiTimeOut != 0 && waitTime>= uiTimeOut)
		{
			if(uiTimeOut > 2000)
				m_errCode = LEC_LINK_TIMEOUT;
			break;
		}

		COMSTAT comStat;
		DWORD   flags;
		BOOL result = ClearCommError(m_hComDev,&flags,&comStat);
		if(!result)
		{
			m_errCode = LEC_LINKERR;
			mylog.WriteLog2File(LogControl::LERROR,"receive data error when use com linker");
			return -1;
		}
		if(!comStat.cbInQue) //��û�����ݷ��͹���
		{
			::Sleep(80);
			waitTime += 80;
			continue;
		}

		waitTime = 0;

		recvLen = min((unsigned int)remainLen,comStat.cbInQue);
		if(!ReadFile(m_hComDev,pDataBuf,recvLen,&recvLen,NULL))
		{
			m_errCode = LEC_LINKERR;
			mylog.WriteLog2File(LogControl::LERROR,"receive data error when use com linker");
			return -1;
		}
		else if( recvLen == 0)
		{
			return readDataLen;
		}
		else
		{
			readDataLen += recvLen;
			pDataBuf += recvLen;
			remainLen -= recvLen;
			if( remainLen <= 0)
			{
				break;
			}
		}
	}

	return readDataLen;
	// }}}USR
}
// }}}RME

// {{{RME operation 'Close()'
bool ComLinker::Close( void )
{
	// {{{USR
	if(m_hComDev != NULL)
	{
		CloseHandle(m_hComDev);
		m_hComDev = NULL;
	}

	m_isConnect = false;
	m_isInitComm = false;
	return true;
	// }}}USR
}
// }}}RME

// {{{RME operation 'GetConnectAddr()'
string ComLinker::GetConnectAddr( void )
{
	// {{{USR
	    return string(strLinkParm.comName);
	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR
void ComLinker::Listen( DWORD (WINAPI DisposeFun)(void *pParm) )
{
    //���˳��������¼�
    HANDLE hExitListenEvent =OpenEvent(EVENT_ALL_ACCESS,FALSE,LISTEN_EXIT_EVENTNAME);

    //�����û�д򿪴��ڣ����ȴ�
    while(!m_isConnect)
    {
        if(Connect())
            break;

        if(WAIT_OBJECT_0==::WaitForSingleObject(hExitListenEvent,2000))
        {
            //�˳������¼��Ѿ�������,���˳�
            ::ResetEvent(hExitListenEvent);
			CloseHandle(hExitListenEvent);
			hExitListenEvent = NULL;
            return;
        }
    }

    //��������ֱ������һ������ɹ����Ϳ����˳��̣߳��Ժ�����������������Ӧ���豸�����
    while(!CreatTask())
    {
        //��������Ѿ����رգ����ٴδ�
        while(!m_isConnect)
        {
            if(Connect())
                break;

            if(WAIT_OBJECT_0==::WaitForSingleObject(hExitListenEvent,2000))
            {
                //�˳������¼��Ѿ�������,���˳�
                ::ResetEvent(hExitListenEvent);
				CloseHandle(hExitListenEvent);
				hExitListenEvent = NULL;
                return;
            }
        }

        if(WAIT_OBJECT_0==::WaitForSingleObject(hExitListenEvent,500))
        {
            //�˳������¼��Ѿ�������,���˳�
            ::ResetEvent(hExitListenEvent);
			CloseHandle(hExitListenEvent);
			hExitListenEvent = NULL;
            return;
        }
    }

	CloseHandle(hExitListenEvent);
	hExitListenEvent = NULL;
}

#ifdef _MY_UNITTEST
string ComLinker::WhoAmI()
{
    return string("ComLinker");
}
#endif
// }}}USR
// }}}RME

// }}}RME
