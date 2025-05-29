// {{{RME classifier 'Logical View::Linker'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "Linker.h"
#endif

#include <RTSystem/Communication.h>
#include <Linker.h>
#include <MsgQueue.h>
#include <QueRegister.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* �ļ����ƣ�Linker.cpp
* �ļ���ʶ��
* ժ    Ҫ�����ӵĸ���
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
*
**************************************************************************/

#include "Protocol103.h"
#include "Protocol60_103.h"
#include "Protocol104_GB.h"
#include "Protocol104_ZJ.h"
#include "Protocal104_SH.h"

#include "StringRes.h"

//�����������(����������������ж�Ӧ��ϵ)
const char *LINKERRORDESC[] ={"",(CStringRes::GetInstance())->GetStrById(IDS_LINK_FAIL,"��·ͨѶʧ��"),
		(CStringRes::GetInstance())->GetStrById(IDS_LINK_TIMEOUT,"��·ͨѶ��ʱ"),
		(CStringRes::GetInstance())->GetStrById(IDS_USER_TERMIT_COMM,"�û���ֹͨѶ"),
		(CStringRes::GetInstance())->GetStrById(IDS_UNKOWN_LINKERR,"δ֪��ͨѶ��·����"),
		(CStringRes::GetInstance())->GetStrById(IDS_UNKOWN_ERR,"δ֪����"),
        (CStringRes::GetInstance())->GetStrById(IDS_CMDFAIL_NODATA,"����ִ��ʧ�ܣ�����û�����������"),
		(CStringRes::GetInstance())->GetStrById(IDS_TASK_TERMINATE,"�����ж�"),
		(CStringRes::GetInstance())->GetStrById(IDS_ERR_RESPONSE,"ͨѶ�˶Է������Ӧ��"),
		(CStringRes::GetInstance())->GetStrById(IDS_ERR_FRAME,"�����֡��ʽ"),
		(CStringRes::GetInstance())->GetStrById(IDS_NOACT_STARTTRANS,"���������䡱û�м���"),
		(CStringRes::GetInstance())->GetStrById(IDS_RMT_TERMIT_TRANS,"Զ����ֹ����"),
		(CStringRes::GetInstance())->GetStrById(IDS_RESPONSE_TIMEOUT,"�Է���ʱδ��Ӧ"),
		(CStringRes::GetInstance())->GetStrById(IDS_OPERFILE_FAIL,"�����ļ�ʧ��")};

#ifdef _MY_UNITTEST
    bool Linker::m_bConnectFail = false; //�����ڵ�Ԫ���ԣ�ָʾlinker��connect�����ɹ���ʧ��
#endif

// }}}USR
// }}}RME

// {{{RME operation 'Linker()'
Linker::Linker( void )
	// {{{RME tool 'OT::Cpp' property 'ConstructorInitializer'
	// {{{USR

	// }}}USR
	// }}}RME
{
	// {{{USR
	m_pProtocol = NULL;
	m_isServerLink = false;
	m_errCode = LEC_NOERROR;
	m_isExitLink = false;
	m_CommType = CT_UNKNOW;
	m_Link = 0;
	m_iUserCount = 0;
	m_isPreDisConnect = false;
	m_nDevId = 0;
	// }}}USR
}
// }}}RME

// {{{RME operation '~Linker()'
Linker::~Linker( void )
{
	// {{{USR
	if(m_pProtocol != NULL)
	{
		delete m_pProtocol;
		m_pProtocol = NULL;
	}

	// }}}USR
}
// }}}RME

// {{{RME operation 'Init(void*,LINKERTYPE)'
void Linker::Init( void * parm, LINKERTYPE linkerType )
{
	// {{{USR
	m_LinkerType = linkerType;
	m_isPreDisConnect = false;

	//����Э�����ʹ������ʵ�Э�����
	switch(linkerType)
	{
	case LK_REMOTE://Զ������
		m_pMsgQue = QueRegister::GetInstance().RegisterQue(COMM_REMOTEQUE_NAME);
		m_ProtocolType = SystemCfger::GetInstance().rmtProtocolType;

		switch(m_ProtocolType)
		{
		case PRO_103_HB:
			m_pProtocol = new Protocol103();
			break;
		case PRO_104_GB:
			m_pProtocol = new Protocol104_GB();
			break;
		case PRO_104_ZJ:
			m_pProtocol = new Protocol104_ZJ();
			break;
		default:
			m_pProtocol = new Protocol104_GB();
			break;
		}
		
		break;
	case LK_ASDU:
		
		m_pMsgQue = QueRegister::GetInstance().RegisterQue(COMM_ASDUQUE_NAME);

		if(PRO_104_SH==m_ProtocolType)
		{
			m_pProtocol = new Protocal104_SH();
		}
		else
		{
			m_ProtocolType = PRO_60_103;
			m_pProtocol = new Protocol60_103();
		}
		break;
	default:
		m_pProtocol = NULL;
		break;
	}

	// }}}USR
}
// }}}RME

// {{{RME operation 'ExecuteCmd(struct Command&)'
bool Linker::ExecuteCmd( struct Command & cmd )
{
	// {{{USR

	    //assert(m_pProtocol != NULL);
	        
	    //return m_pProtocol->exuteCmd(this,cmd);
	        m_errCode = LEC_LINKERR;
	        return false;

	// }}}USR
}
// }}}RME

// {{{RME operation 'Connect()'
bool Linker::Connect( void )
{
	// {{{USR
	    return true;
	// }}}USR
}
// }}}RME

// {{{RME operation 'BindUdpLink()'
bool Linker::BindUdpLink( void )
{
	// {{{USR
	    return true;
	// }}}USR
}
// }}}RME

// {{{RME operation 'SendData(const char*,int,unsigned int)'
int Linker::SendData( const char * pDataBuf, int iDataLen, unsigned int uiTimeOut )
{
	// {{{USR
	    return 0;
	// }}}USR
}
// }}}RME

// {{{RME operation 'CreatTask()'
bool Linker::CreatTask( void )
{
	// {{{USR

	    return false;

	// }}}USR
}
// }}}RME

// {{{RME operation 'GetUdpData(struct Command&)'
bool Linker::GetUdpData( struct Command & cmd )
{
	// {{{USR
	        return true;
	// }}}USR
}
// }}}RME

// {{{RME operation 'ReceiveData(char*,int,unsigned int)'
int Linker::ReceiveData( char * pDataBuf, int iBufferLen, unsigned int uiTimeOut )
{
	// {{{USR
	    return 0;
	// }}}USR
}
// }}}RME

// {{{RME operation 'ReceiveDataFrom(char*,int,unsigned int,struct sockaddr&)'
int Linker::ReceiveDataFrom( char * pDataBuf, int iBufferLen, unsigned int uiTimeOut, struct sockaddr & clientAddr )
{
	// {{{USR
	    return 0;
	// }}}USR
}
// }}}RME

// {{{RME operation 'Close()'
bool Linker::Close( void )
{
	// {{{USR
	m_isPreDisConnect = false;
	m_isConnect = false;
	m_isInitComm = false;
	if( m_pProtocol )
		m_pProtocol->Init();
	return true;
	// }}}USR
}
// }}}RME

// {{{RME operation 'GetConnectAddr()'
string Linker::GetConnectAddr( void )
{
	// {{{USR
	        return "";
	// }}}USR
}
// }}}RME

// {{{RME operation 'IsConnect()'
bool Linker::IsConnect( void )
{
	// {{{USR
	        return m_isConnect;
	// }}}USR
}
// }}}RME

// {{{RME operation 'IsInitComm()'
bool Linker::IsInitComm( void )
{
	// {{{USR
	        return m_isInitComm;
	// }}}USR
}
// }}}RME

// {{{RME operation 'IsServerLink()'
bool Linker::IsServerLink( void )
{
	// {{{USR
	    return m_isServerLink;
	// }}}USR
}
// }}}RME

// {{{RME operation 'GetLinkType()'
Linker::LINKERTYPE Linker::GetLinkType( void )
{
	// {{{USR
	        return m_LinkerType;
	// }}}USR
}
// }}}RME

// {{{RME operation 'ExitLink()'
void Linker::ExitLink( void )
{
	// {{{USR
	        m_isExitLink = true;
	// }}}USR
}
// }}}RME

// {{{RME operation 'GetLastErrCode()'
LINKERRORCODE Linker::GetLastErrCode( void )
{
	// {{{USR
	        LINKERRORCODE errCode = m_errCode;
	        if( errCode == LEC_NOERROR)
	        {
	            if(m_pProtocol == NULL)
	            {
	                return errCode;
	            }

	            PROERRORCODE proErr = m_pProtocol->GetLastErrCode();
	            switch(proErr)
	            {                
	            case PEC_NOERROR:
	                errCode = LEC_NOERROR;
	                break;
	            case PEC_ERRCMD_NODATA:
	                errCode = LEC_PRO_ERRCMD_NODATA;
	                break;
	            case PEC_INTERRUPT:
	                errCode = LEC_PRO_INTERRPUT;
	                break;
	            case PEC_ERR_ANSWER:
	                errCode = LEC_PRO_ERR_ANSWER;
	                break;
	            default:
	                errCode = LEC_PRO_UNKOWN_ERR;
	                break;
	            }
	        }

	        m_errCode = LEC_NOERROR;
	        return errCode;
	// }}}USR
}
// }}}RME

// {{{RME operation 'GetLastErrDesc()'
const char * Linker::GetLastErrDesc( void )
{
	// {{{USR
	        LINKERRORCODE errCode = GetLastErrCode();
	        int iIndex = (int)errCode;
	        if(iIndex >= sizeof(LINKERRORDESC))
	        {
	            iIndex = 0;
	        }	        
	        return LINKERRORDESC[iIndex];
	// }}}USR
}
// }}}RME

// {{{RME operation 'DoIdle()'
void Linker::DoIdle( void )
{
	// {{{USR
	Task* pTask = m_pProtocol->DoIdle(this);
	if(pTask != NULL && m_pMsgQue != NULL)
	{		
		if(!m_pMsgQue->SendMsg(pTask))  //�������͵����������
		{
			delete pTask;
			pTask = NULL;
			return;
		}
		Sleep(50);
	}
		
	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR
void Linker::Listen(  DWORD (WINAPI DisposeFun)(void *pParm) )
{
	
}

void Linker::GetCmdDesc(struct Command & cmd)
{
	if(m_pProtocol != NULL)
	{
		m_pProtocol->GetCmdDesc(cmd);
	}
}

#ifdef _MY_UNITTEST
string Linker::WhoAmI()
{
    return string("Linker");
}
#endif
// }}}USR
// }}}RME

// }}}RME
