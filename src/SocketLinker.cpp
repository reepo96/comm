// {{{RME classifier 'Logical View::SocketLinker'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "SocketLinker.h"
#endif

#include <RTSystem/Communication.h>
#include <SocketLinker.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* �ļ����ƣ�SocketLinker.cpp
* �ļ���ʶ��
* ժ    Ҫ��tcp/ip������
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
*
**************************************************************************/

#include "Protocol103.h"
#include "Protocol60_103.h"

#ifdef _DEBUG
#define new MYDEBUG_NEW
#endif

#define SIO_KEEPALIVE_VALS _WSAIOW(IOC_VENDOR,4)

#define MTU_LEN			1448
#define	RECV_BUF_LEN	(1448*6)

#define MIN(a,b)	((a)<(b))?(a):(b)

// }}}USR
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
SocketLinker::SocketLinker( void )
	: m_isListenLinker( false )
{
	m_pRecvBuf = new char[RECV_BUF_LEN];
	m_RecvDataLen = 0;
	m_DataPos = 0;
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
SocketLinker::~SocketLinker( void )
{
	if( m_pRecvBuf )
		delete m_pRecvBuf;
}
// }}}RME

// {{{RME operation 'Init(void*,LINKERTYPE)'
void SocketLinker::Init( void * parm, LINKERTYPE linkerType )
{
	// {{{USR
	m_RecvDataLen = 0;
	m_DataPos = 0;
	m_isPreDisConnect = false;
	m_CommType = CT_SOCKET;  
	//��ʼ�����Ӳ���
	memset(&strLinkParm,0,sizeof(SocketParm));
	memcpy(&strLinkParm,parm,sizeof(SocketParm));
	
	if(linkerType == LK_ASDU)
	{
		const char * ipAddr = strLinkParm.connetToAddr;
		PDAUINFO pdauInfo = SystemCfger::GetInstance().GetDAUInfo(ipAddr);
		if(pdauInfo)
		{
				if(pdauInfo->ProtocalType==1)
				{
					m_ProtocolType = PRO_104_SH;
				}
		}
	}
	//���ø��ຯ��
	Linker::Init(parm,linkerType);
	   
	// }}}USR
}
// }}}RME

// {{{RME operation 'ExecuteCmd(struct Command&)'
bool SocketLinker::ExecuteCmd( struct Command & cmd )
{
	// {{{USR
	assert(m_pProtocol != NULL);
	return m_pProtocol->ExecuteCmd(this,cmd);
	// }}}USR
}
// }}}RME

// {{{RME operation 'Connect()'
bool SocketLinker::Connect( void )
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
	     
#ifndef _PILING
	m_isExitLink = false;
	m_isConnect = false;
	m_isInitComm = false;
	m_isPreDisConnect = false;

	if((m_socket =socket(PF_INET,SOCK_STREAM,0))==INVALID_SOCKET)
	{
		mylog.WriteLog2File(LogControl::LERROR,"create listen socket error");
		return false;
	}

	struct sockaddr_in localAddrIn,remotAddrIn;	
	memset(&localAddrIn,0,sizeof(localAddrIn));
	memset(&remotAddrIn,0,sizeof(remotAddrIn));

	/*//�󶨱��ص�ַ
	localAddrIn.sin_family = PF_INET;
	localAddrIn.sin_port = htons(strLinkParm.loacalTcpPort);
	localAddrIn.sin_addr.S_un.S_addr = inet_addr(strLinkParm.loacalAddr);

	if(bind(m_socket,(struct sockaddr *)&localAddrIn,sizeof(localAddrIn))==SOCKET_ERROR)
	{
	closesocket(m_socket);
	mylog.WriteLog2File(LogControl::LERROR,"bind socket error");
	return false;
	}*/
	char szLogInf[256] = {0};

	//����Զ�̵�ַ
	remotAddrIn.sin_family = PF_INET;
	remotAddrIn.sin_port = htons(strLinkParm.connetToTcpPort);
	remotAddrIn.sin_addr.S_un.S_addr = inet_addr(strLinkParm.connetToAddr);

	//���ö�д��ʱ
	int iRecvTimeOut =  5*1000;
	int len = sizeof(int);

	int ret = setsockopt(m_socket,SOL_SOCKET,SO_RCVTIMEO,(char*)&iRecvTimeOut,len);
	ret = setsockopt(m_socket,SOL_SOCKET,SO_SNDTIMEO,(char*)&iRecvTimeOut,len);

	//����Զ��
	mylog.FormatLogInf(LogControl::LDEBUG,"Begin connect to:%s,port:%d",strLinkParm.connetToAddr,strLinkParm.connetToTcpPort);

	if(connect(m_socket,(struct sockaddr *)&remotAddrIn,sizeof(remotAddrIn)) == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		closesocket(m_socket);
		sprintf(szLogInf,"socket connect error,connect to:%s,error code:%d",strLinkParm.connetToAddr,err);
		mylog.WriteLog2File(LogControl::LERROR,szLogInf);
		return false;
	}

	//����Ϊ��������ʽ
	u_long val=1;
	ret = ioctlsocket(m_socket,FIONBIO,&val);
	if(ret==SOCKET_ERROR)
		return false;

	//���ý��ջ�������С
	int recBufSize = 1024 * 32;
	len = sizeof(int);
	setsockopt(m_socket,SOL_SOCKET,SO_RCVBUF,(char*)&recBufSize,len);

	m_isConnect = true;

	/*
	//��������ǰ̨������keep-alive
	if(m_LinkerType == LK_ASDU)
	{
		struct TCP_KEEPALIVE
		{
			unsigned long onoff;
			unsigned long keepalivetime;
			unsigned long keepaliveinterval;
		};

		TCP_KEEPALIVE inKeepAlive = {0}; //�������
		unsigned long ulInLen = sizeof(TCP_KEEPALIVE);
		TCP_KEEPALIVE outKeepAlive = {0}; //�������
		unsigned long ulOutLen = sizeof(TCP_KEEPALIVE);
		unsigned long ulBytesReturn = 0; 

		//����socket��keep aliveΪ3�룬���ҷ��ʹ���Ϊ3��
		inKeepAlive.onoff = 1;
		inKeepAlive.keepaliveinterval = 500; //����KeepAlive̽����ʱ����
		inKeepAlive.keepalivetime = 3000; //��ʼ�״�KeepAlive̽��ǰ��TCP�ձ�ʱ��

		int iRT = WSAIoctl(m_socket, SIO_KEEPALIVE_VALS,
			(LPVOID)&inKeepAlive, ulInLen,
			(LPVOID)&outKeepAlive, ulOutLen,
			&ulBytesReturn, NULL, NULL);

	}
	*/

	//Э�����ӳ�ʼ��
	if(!m_pProtocol->InitConnect(this))
	{
		this->Close();
		mylog.WriteLog2File(LogControl::LERROR,"the protocol init connect with TCP/IP error");
		return false;
	}
	else
	{
		mylog.FormatLogInf(LogControl::LINFO,"Connect to ip:%s,port:%d,soketid:%d, success",
			strLinkParm.connetToAddr,strLinkParm.connetToTcpPort,(int)m_socket);
	}

	m_isInitComm = true;

#else
	mylog.WriteLog2File(LogControl::LDEBUG,"Enter SocketLinker::Connect() with piling");

	//��׮����
	m_isExitLink = false;
	m_isConnect = true;
	m_isInitComm = true;
#endif

	return true;
	// }}}USR
}
// }}}RME

// {{{RME operation 'BindUdpLink()'
bool SocketLinker::BindUdpLink( void )
{
	// {{{USR
	m_isExitLink = false;
	m_isConnect = false;
	m_isInitComm = false;

	if((m_socket =socket(PF_INET,SOCK_DGRAM,0))==INVALID_SOCKET)
	{
		mylog.WriteLog2File(LogControl::LERROR,"create udp socket error");
		return false;
	}

	struct sockaddr_in localAddrIn;	
	memset(&localAddrIn,0,sizeof(localAddrIn));

	//�󶨱��ص�ַ
	localAddrIn.sin_family = PF_INET;
	localAddrIn.sin_port = htons(strLinkParm.loacalUdpPort);
	//localAddrIn.sin_addr.S_un.S_addr = inet_addr(strLinkParm.loacalAddr);
	localAddrIn.sin_addr.S_un.S_addr = INADDR_ANY;

	char szInfo[256] = {0};
	sprintf(szInfo,"Begin bind udp link,addr:0.0.0.0,port:%d",strLinkParm.loacalUdpPort);
	mylog.WriteLog2File(LogControl::LDEBUG,szInfo);

	if(bind(m_socket,(struct sockaddr *)&localAddrIn,sizeof(localAddrIn))==SOCKET_ERROR)
	{
		closesocket(m_socket);
		int err = WSAGetLastError();
		sprintf(szInfo,"bind udp socket error,bind addr:%s,port:%d,error code:%d",strLinkParm.loacalAddr,strLinkParm.loacalUdpPort,err);
		mylog.WriteLog2File(LogControl::LERROR,szInfo);
		return false;
	}

	int recBufSize =  1024 * 320;
	int len = sizeof(int);	
	setsockopt(m_socket,SOL_SOCKET,SO_RCVBUF,(char*)&recBufSize,len);

	recBufSize = 0;
	getsockopt(m_socket,SOL_SOCKET,SO_RCVBUF,(char*)&recBufSize,&len);

	//����Ϊ��������ʽ
	u_long val=1;
	ioctlsocket(m_socket,FIONBIO,&val);

	m_isConnect = true;
	m_isInitComm = true;

	return true;

	// }}}USR
}
// }}}RME

// {{{RME operation 'SendData(const char*,int,unsigned int)'
int SocketLinker::SendData( const char * pDataBuf, int iDataLen, unsigned int uiTimeOut )
{
	// {{{USR
	assert(pDataBuf != NULL);

	int sendDataLen = 0;	//ʵ�ʷ��͵Ĵ�С
	int remainLen   = iDataLen ;	//���ж���������Ҫ����
	int sendLen = 1;
	int nTimeLeft = uiTimeOut;	//ʣ��ʱ��
	const int SELECTWAIT = 500;

	while( sendLen > 0 && sendDataLen < iDataLen)
	{
		if(m_isExitLink)    //�˳�����
		{
			m_errCode = LEC_USER_TERMINATE;
			this->Close();
			break;
		}

		if( m_isPreDisConnect )
		{
			mylog.FormatLogInf(LogControl::LERROR,"SocketLinker::SendData():m_isPreDisConnect is true,so close soket:%d",m_socket);
			this->Close();
			return -1;
		}

		fd_set writeSet;
		FD_ZERO(&writeSet);
		FD_SET(m_socket,&writeSet);

		struct timeval timeout;

		if(uiTimeOut> 0 && uiTimeOut < 1000 )
		{
			timeout.tv_sec = 0;
			timeout.tv_usec = (uiTimeOut%1000)*1000;
		}
		else
		{
			timeout.tv_sec = 0;
			timeout.tv_usec = SELECTWAIT*1000;
		}

		//����Ƿ���Է�������
		select(2,NULL,&writeSet,NULL,&timeout);
		if( FD_ISSET(m_socket,&writeSet) == 0)
		{
			FD_CLR(m_socket,&writeSet);

			//���Զ�������Ƿ��Ѿ��ر�
			send(m_socket,(char*)pDataBuf,0,0);
			int errorCode = WSAGetLastError();
			if(  errorCode == WSAEWOULDBLOCK || errorCode==0)
			{
				if(uiTimeOut == 0)
				{
					continue;
				}
				else
				{
					nTimeLeft -= SELECTWAIT;
					if( uiTimeOut >= 1000 && nTimeLeft > 0 && nTimeLeft < uiTimeOut )
					{
						continue;
					}

					m_errCode = LEC_LINK_TIMEOUT;
					break;
				}
			}
			else
			{
				m_errCode = LEC_LINKERR;
				this->Close();
				break;
			}
		}

		FD_CLR(m_socket,&writeSet);
		nTimeLeft = uiTimeOut;

		sendLen = ::send(m_socket,(char*)pDataBuf,remainLen,0);
		if( sendLen == SOCKET_ERROR)
		{
			m_errCode = LEC_LINKERR;
			int err = WSAGetLastError();
			char szInfo[256] = {0};
			sprintf(szInfo,"send data error when use socket linker,error code:%d",err);
			mylog.WriteLog2File(LogControl::LERROR,szInfo);
			return -1;
		}
		else if( sendLen == 0)
		{
			return sendDataLen;
		}
		else
		{
			sendDataLen += sendLen;
			pDataBuf += sendLen;
			remainLen -= sendLen;

			if( remainLen <= 0)
			{
				break;
			}
		}
	}

	return sendDataLen;
	// }}}USR
}
// }}}RME

// {{{RME operation 'CreatTask()'
bool SocketLinker::CreatTask( void )
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
				mylog.FormatLogInf(LogControl::LERROR,"send msg fail,cmd_code:%d ",pTask->cmd.funCode);
				delete pTask;
				pTask = NULL;
				return false;
			}
			return true;
		}

		mylog.FormatLogInf(LogControl::LERROR,"send msg fail,cmd_code:%d,m_pMsgQue=%x",pTask->cmd.funCode,m_pMsgQue);
	}

	return false;

	// }}}USR
}
// }}}RME

// {{{RME operation 'GetUdpData(struct Command&)'
bool SocketLinker::GetUdpData( struct Command & cmd )
{
	// {{{USR
	        return m_pProtocol->GetUdpData(this,cmd);
	// }}}USR
}
// }}}RME

// {{{RME operation 'ReceiveData(char*,int,unsigned int)'
int SocketLinker::ReceiveData( char * pDataBuf, int iBufferLen, unsigned int uiTimeOut )
{
	// {{{USR
	/*
	modify by libo 2012-9-8
	�õ���SHDFR_B�򻯰�ļ�ذ�ͨѶʱ���ֶ����ݵ����󣬾��������ֶ�����ʱ�����˰��ش�������������󣬶���������������һ
	������û�б���ȡ�ļ����ֽڣ��ֻ�������ʱ��û�ж�ȡ���������ݳ������������޸Ĳ��ԣ�����һ�ζ���ͨѶ������������
	*/

	assert(pDataBuf != NULL);

	//mylog.FormatLogInf(LogControl::LDEBUG,"Enter SocketLinker::ReceiveData()");

	int readDataLen = 0;    //ʵ�ʶ�ȡ�Ĵ�С
	int remainLen   = iBufferLen ;   //���ж���������Ҫ��ȡ
	int recvLen = 1;
	int copyLen = 0;
	char szInfo[256] = {0};
	int errorCode = 0;
	int nTimeLeft = uiTimeOut;	//ʣ��ʱ��
	const int SELECTWAIT = 500;

	if( m_RecvDataLen > 0 )
	{
		copyLen = MIN(m_RecvDataLen,iBufferLen);
		memcpy(pDataBuf,&m_pRecvBuf[m_DataPos],copyLen);
		m_RecvDataLen -= copyLen;
		pDataBuf += copyLen;
		remainLen -= copyLen;

		if( m_RecvDataLen <=0 )
		{
			m_RecvDataLen = 0;
			m_DataPos = 0;
		}
		else
		{
			m_DataPos += copyLen;
		}

		/*if( m_RecvDataLen > 0 )
			::memmove(m_pRecvBuf,&m_pRecvBuf[copyLen],m_RecvDataLen);*/

		/*mylog.FormatLogInf(LogControl::LDEBUG,
			">>ReceiveData()1:m_RecvDataLen=%d,m_DataPos=%d,iBufferLen=%d,remainLen=%d",
			m_RecvDataLen,m_DataPos,iBufferLen,remainLen);*/

		if( remainLen <=0 )
		{
			//mylog.FormatLogInf(LogControl::LDEBUG,"Leave SocketLinker::ReceiveData()");
			return copyLen;
		}

		//�ⲿ���������·��Ŀ�ģ�ͨ�����ó�ʱʱ��Ƚ϶̣�����ȷ�������ݵ�����£�Ӧ�ı�
		//��ʱʱ��
		if( uiTimeOut < 1000 && uiTimeOut !=0 )
		{
			uiTimeOut = 13000;
			nTimeLeft = uiTimeOut;
		}
	}

	//mylog.FormatLogInf(LogControl::LDEBUG,"recvLen=%d,readDataLen=%d,iBufferLen=%d",recvLen,readDataLen,iBufferLen);

	while( recvLen > 0 && readDataLen < iBufferLen)
	{
		if(m_isExitLink)    //�˳�����
		{
			m_errCode = LEC_USER_TERMINATE;
			mylog.FormatLogInf(LogControl::LWARNING,"Exit receive by other thread");
			this->Close();
			break;
		}

		if( m_isPreDisConnect )
		{
			mylog.FormatLogInf(LogControl::LERROR,"SocketLinker::ReceiveData():m_isPreDisConnect is true,so close soket:%d",m_socket);
			this->Close();
			return -1;
		}

		fd_set readSet;
		FD_ZERO(&readSet);
		FD_SET(m_socket,&readSet);

		struct timeval timeout;

		if(uiTimeOut> 0 && uiTimeOut < 1000 )
		{
			timeout.tv_sec = 0;
			timeout.tv_usec = (uiTimeOut%1000)*1000;
		}
		else
		{
			timeout.tv_sec = 0;
			timeout.tv_usec = SELECTWAIT*1000;
		}

		//����Ƿ������ݱ�����
		int iReturn = select(0,&readSet,NULL,NULL,&timeout);
		if(iReturn == SOCKET_ERROR)
		{
			errorCode = WSAGetLastError();
			sprintf(szInfo,"select error in SocketLinker::ReceiveData(),error code:%d",errorCode);
			mylog.WriteLog2File(LogControl::LERROR,szInfo);
			this->Close();
			return -1;
		}
		else if( iReturn == 0 || !FD_ISSET(m_socket,&readSet))
		{
			FD_CLR(m_socket,&readSet);

			//���Զ�������Ƿ��Ѿ��ر�
			send(m_socket,(char*)pDataBuf,0,0);
			errorCode = WSAGetLastError();
			if(  errorCode == WSAEWOULDBLOCK || errorCode==0)
			{
				if(uiTimeOut == 0)
				{
					continue;
				}
				else if(uiTimeOut < 1000 )
				{
					m_errCode = LEC_NOERROR;
					//mylog.FormatLogInf(LogControl::LWARNING,"break with timeout:%d",uiTimeOut);
					break;
				}
				else
				{
					nTimeLeft -= SELECTWAIT;
					//m_errCode = LEC_LINK_TIMEOUT;
					//break;
				}

				if( nTimeLeft >0 && nTimeLeft < uiTimeOut )
				{
					continue;
				}

				m_errCode = LEC_LINK_TIMEOUT;
				mylog.WriteLog2File(LogControl::LERROR,"select timeout in SocketLinker::ReceiveData()");
				break;
			}
			else
			{
				m_errCode = LEC_LINKERR;
				errorCode = WSAGetLastError();
				sprintf(szInfo,"select return 0 and send error in SocketLinker::ReceiveData(),error code:%d",errorCode);
				mylog.WriteLog2File(LogControl::LERROR,szInfo);

				this->Close();
				break;
			}
		}

		FD_CLR(m_socket,&readSet);
		nTimeLeft = uiTimeOut;

		int iNum = (RECV_BUF_LEN-m_RecvDataLen)/MTU_LEN;

		recvLen = ::recv(m_socket,(char*)(&m_pRecvBuf[m_RecvDataLen]),iNum*MTU_LEN,0);
		if( recvLen == SOCKET_ERROR )
		{
			int iErrorCode = WSAGetLastError();
			m_errCode = LEC_LINKERR;
			char szInfo[256] = {0};
			sprintf(szInfo,"receive data error when use socket linker,error code:%d,iBufferLen:%d,readDataLen:%d,remainLen:%d",
				iErrorCode,iBufferLen,readDataLen,remainLen);
			mylog.WriteLog2File(LogControl::LERROR,szInfo);
			this->Close();
			return -1;
		}
		else if( recvLen == 0)
		{
			errorCode = WSAGetLastError();
			if( errorCode !=0 && errorCode != WSAEWOULDBLOCK )
			{
				sprintf(szInfo,"recv return 0,has recv:%d,remain:%d in SocketLinker::ReceiveData(),error code:%d",
					readDataLen,remainLen,errorCode);
				mylog.WriteLog2File(LogControl::LERROR,szInfo);
				return readDataLen;
			}
			//mylog.FormatLogInf(LogControl::LWARNING,"recvLen=0 and errorcode=%d",errorCode);
		}
		else
		{
			//mylog.WriteData2Log(LogControl::LERROR,"recv:",(char*)(&m_pRecvBuf[m_RecvDataLen]),recvLen);

			readDataLen += recvLen;
			m_RecvDataLen += recvLen;

			//RECV_BUF_LEN�������iBufferLen
			if( m_RecvDataLen >= remainLen)
			{
				memcpy(pDataBuf,m_pRecvBuf,remainLen);
				m_RecvDataLen -= remainLen;

				if( m_RecvDataLen > 0 )
				{
					m_DataPos += remainLen;
				}
				else
				{
					m_DataPos = 0;
				}

				/*mylog.FormatLogInf(LogControl::LDEBUG,
					"**ReceiveData()2:recvLen=%d,m_RecvDataLen=%d,m_DataPos=%d,iBufferLen=%d,remainLen=%d",
					recvLen,m_RecvDataLen,m_DataPos,iBufferLen,remainLen);*/

				/*if( m_RecvDataLen > 0 )
					::memmove(m_pRecvBuf,&m_pRecvBuf[remainLen],m_RecvDataLen);*/

				readDataLen = iBufferLen;
				remainLen = 0;
				break;
			}
		}
	}

	//mylog.FormatLogInf(LogControl::LDEBUG,"Leave SocketLinker::ReceiveData() with len:%d",readDataLen);
	return readDataLen;
	// }}}USR
}
// }}}RME

// {{{RME operation 'ReceiveDataFrom(char*,int,unsigned int,struct sockaddr&)'
int SocketLinker::ReceiveDataFrom( char * pDataBuf, int iBufferLen, unsigned int uiTimeOut, struct sockaddr & clientAddr )
{
	// {{{USR
	assert(pDataBuf != NULL);

	while(1)
	{
		if(m_isExitLink)    //�˳�����
		{
			this->Close();
			break;
		}

		fd_set readSet;
		FD_ZERO(&readSet);
		FD_SET(m_socket,&readSet);

		struct timeval timeout;

		if(uiTimeOut == 0)
		{
			timeout.tv_sec = 1;
			timeout.tv_usec = 0;
		}
		else
		{
			timeout.tv_sec = uiTimeOut/1000;
			timeout.tv_usec = (uiTimeOut%1000)*1000;
		}

		//����Ƿ������ݱ�����
		int iReturn = select(2,&readSet,NULL,NULL,&timeout);
		if(iReturn < 0)
		{
			return -1;
		}
		else if( iReturn == 0 || FD_ISSET(m_socket,&readSet) == 0)
		{
			FD_CLR(m_socket,&readSet);
			break;
		}

		FD_CLR(m_socket,&readSet);

		int addrLen = sizeof(struct sockaddr);

		int recvLen = ::recvfrom(m_socket,(char*)pDataBuf,iBufferLen,0,&clientAddr,&addrLen);
		if( recvLen == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			if(  errorCode == WSAEWOULDBLOCK || errorCode==0)
			{
				return 0;
			}

			char szInfo[256] = {0};
			sprintf(szInfo,"receive data error when use socket linker in ReceiveDataFrom,error code:%d",errorCode);
			mylog.WriteLog2File(LogControl::LERROR,szInfo);
			this->Close();
			return -1;
		}
		else
		{
			return recvLen;
		}
	}

	return 0;

	// }}}USR
}
// }}}RME

// {{{RME operation 'Close()'
bool SocketLinker::Close( void )
{
	// {{{USR
	if( m_isConnect )
	{
		::shutdown(m_socket,SD_BOTH );
		::closesocket(m_socket);

		if( m_pProtocol )
			m_pProtocol->Init();
	}

	m_RecvDataLen = 0;
	m_DataPos = 0;
	m_isPreDisConnect = false;
	m_isConnect = false;
	m_isInitComm = false;	
	return true;
	// }}}USR
}
// }}}RME

// {{{RME operation 'SetSocket(SOCKET)'
void SocketLinker::SetSocket( SOCKET s )
{
	// {{{USR
	m_socket = s;
	m_isConnect = true;
	// }}}USR
}
// }}}RME

// {{{RME operation 'GetConnectAddr()'
string SocketLinker::GetConnectAddr( void )
{
	// {{{USR
	    return string(strLinkParm.connetToAddr);
	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR
void SocketLinker::Listen( DWORD (WINAPI DisposeFun)(void *pParm) )
{
	m_isExitLink = false;
	m_isListenLinker = true;

	SOCKET listenSock;
	if((listenSock =socket(PF_INET,SOCK_STREAM,0))==INVALID_SOCKET)
	{
		mylog.WriteLog2File(LogControl::LERROR,"create listen socket error");
		return;
	}

	struct sockaddr_in addrin;
	memset(&addrin,0,sizeof(addrin));
	//��
	addrin.sin_family = PF_INET;
	addrin.sin_port = htons(strLinkParm.loacalTcpPort);
	//addrin.sin_addr.S_un.S_addr = inet_addr(strLinkParm.loacalAddr);
	addrin.sin_addr.S_un.S_addr = INADDR_ANY;

	char szInfo[256] = {0};
	sprintf(szInfo,"Begin tcp listen on the addr:0.0.0.0,port:%d",strLinkParm.loacalTcpPort);
	mylog.WriteLog2File(LogControl::LDEBUG,szInfo);

	if(bind(listenSock,(struct sockaddr *)&addrin,sizeof(addrin))==SOCKET_ERROR)
	{
		printf("Bind IP error\n");
		int errorCode = WSAGetLastError();
		closesocket(listenSock);
		sprintf(szInfo,"bind socket error,bind addr:%s,port:%d,error code:%d",strLinkParm.loacalAddr,strLinkParm.loacalTcpPort,errorCode);
		mylog.WriteLog2File(LogControl::LERROR,szInfo);
		return;
	}
	else
	{
		printf("Bind IP Success!  Port = %d\n",strLinkParm.loacalTcpPort);
	}

	//����
	if(listen(listenSock,SystemCfger::GetInstance().listenNum)==SOCKET_ERROR)
	{
		printf("listen IP error \n");
		int errorCode = WSAGetLastError();
		closesocket(listenSock);
		sprintf(szInfo,"socket listen error,,error code:%d",errorCode);
		mylog.WriteLog2File(LogControl::LERROR,szInfo);
		return;
	}
	else
	{
		printf("listen IP Success\n");
	}

	//����
	struct sockaddr clientAddr;
	int addrLen = sizeof(struct sockaddr);
	memset(&clientAddr,0,sizeof(struct sockaddr));

	//���˳��������¼�
	HANDLE hExitListenEvent =OpenEvent(EVENT_ALL_ACCESS,FALSE,LISTEN_EXIT_EVENTNAME);

	//����Ϊ��������ʽ
	u_long val=1;
	ioctlsocket(listenSock,FIONBIO,&val);

	//vector<SocketLinker*> newSockLinkList;      //�����½�������

	while(1)
	{
		if(hExitListenEvent != NULL)
		{
			if(WAIT_OBJECT_0==::WaitForSingleObject(hExitListenEvent,20))
            {
				//�˳������¼��Ѿ�������,���˳�
				::SetEvent(hExitListenEvent);
				::CloseHandle(hExitListenEvent);
				hExitListenEvent = NULL;
				mylog.WriteLog2File(LogControl::LINFO,"Get exit_listen_event in SocketLinker::Listen()");
				break;
			}
		}

		SOCKET newSocket = accept(listenSock,&clientAddr,&addrLen);
		if(newSocket == INVALID_SOCKET )
		{
			if(WSAGetLastError() == WSAEWOULDBLOCK)
			{
				//û�пͻ����ӽ���������һ��ʱ����ٴμ��
				if(hExitListenEvent == NULL)
				{
					::Sleep(20);
				}
				continue;
			}
			else
			{
				int errorCode = WSAGetLastError();
				closesocket(listenSock);
				sprintf(szInfo,"socket accept error,error code:%d",errorCode);
				mylog.WriteLog2File(LogControl::LERROR,szInfo);
				break;
			}
		}
		else
		{
			printf("Accept Success!\n");
		}

		//����Ϊ��������ʽ
		val=1;
		ioctlsocket(newSocket,FIONBIO,&val);

		//���û�������С
		int recBufSize = 1024 * 256;
		int len = sizeof(int);
		setsockopt(newSocket,SOL_SOCKET,SO_RCVBUF,(char*)&recBufSize,len);

		//�����������ӵ��߳�
		SocketParm newParm;
		memset(&newParm,0,sizeof(SocketParm));

		struct sockaddr_in *newAddrin = (struct sockaddr_in*)&clientAddr;
		memcpy(newParm.connetToAddr,inet_ntoa(newAddrin->sin_addr),strlen(inet_ntoa(newAddrin->sin_addr)));
		newParm.connetToTcpPort = ntohs(newAddrin->sin_port);

		SocketLinker *pNewSocketLinker = new SocketLinker();
		pNewSocketLinker->Init(&newParm,LK_ASDU);
		pNewSocketLinker->SetSocket(newSocket);

		sprintf(szInfo,"Accept autoSend connecting addr:%s,port:%d,socketid:%d",
			newParm.connetToAddr,newParm.connetToTcpPort,(int)newSocket );
		mylog.WriteLog2File(LogControl::LDEBUG,szInfo);
		printf(szInfo);
		printf("\n");
		//newSockLinkList.push_back(pNewSocketLinker);

		if(DisposeFun == 0 )
		{
			HANDLE hThread = CreateThread(NULL,0,&DoWithClientThreadF,(void*)pNewSocketLinker,0,NULL);
			::CloseHandle(hThread);
		}
		else
		{
			HANDLE hThread = CreateThread(NULL,0,DisposeFun,(void*)pNewSocketLinker,0,NULL);
			CloseHandle(hThread);
		}
	}

	//�ر��½�����
	/*vector<SocketLinker*>::iterator it = newSockLinkList.begin();
	for(;it != newSockLinkList.end();it++)
	{
		(*it)->ExitLink();
	}

	//�ȵ����ӹرպ�������
	::Sleep(4000);
	it = newSockLinkList.begin();
	for(;it != newSockLinkList.end();it++)
	{
		SocketLinker *pTmpLinker = (*it);
		delete pTmpLinker;
		pTmpLinker = NULL;        
	}
	newSockLinkList.clear();*/

	//�ر�����
	::shutdown(listenSock,SD_BOTH);
	::closesocket(listenSock);

	::CloseHandle(hExitListenEvent);
	hExitListenEvent = NULL;
}

DWORD WINAPI DoWithClientThreadF(void *pParm)
{
	SocketLinker *pSocketLinker = (SocketLinker *)pParm;

	//���˳��������¼�
	HANDLE hExitListenEvent =OpenEvent(EVENT_ALL_ACCESS,FALSE,LISTEN_EXIT_EVENTNAME);

	if(!pSocketLinker->IsInitComm()) //ͨѶû�г�ʼ�������ȳ�ʼ��ͨѶ
	{
		if(pSocketLinker->m_pProtocol->InitConnect(pSocketLinker))
		{
			pSocketLinker->m_isInitComm = true;
		}
		else
		{
			if(hExitListenEvent != NULL)
			{
				::CloseHandle(hExitListenEvent);
				hExitListenEvent = NULL;
			}

			pSocketLinker->Close();
			delete pSocketLinker;
			pSocketLinker = NULL;
			return 0;
		}
	}

	while(1)
	{
		//ֱ������һ������ɹ����Ϳ����˳��̣߳��Ժ�����������������Ӧ���豸�����
		if(pSocketLinker->CreatTask())
		{
			pSocketLinker->m_isServerLink = true; //������Ϊ����˵�����
			break;
		}

		//�����Ѿ��Ͽ������˳��߳�
		if(!pSocketLinker->IsConnect())
		{
			delete pSocketLinker;
			pSocketLinker = NULL;
			break;
		}

		if(hExitListenEvent != NULL)
		{
			if(WAIT_OBJECT_0==::WaitForSingleObject(hExitListenEvent,300))
            {
				//�˳������¼��Ѿ�������,���˳�
				::SetEvent(hExitListenEvent);
				::CloseHandle(hExitListenEvent);
				hExitListenEvent = NULL;

				pSocketLinker->Close();
				delete pSocketLinker;
				pSocketLinker = NULL;

				break;
			}
		}
		else
		{
			::Sleep(300);
		}
	}

	if(hExitListenEvent != NULL)
	{
		::CloseHandle(hExitListenEvent);
		hExitListenEvent = NULL;
	}
	return 0;
}


#ifdef _MY_UNITTEST
string SocketLinker::WhoAmI()
{
    return string("SocketLinker");
}
#endif
// }}}USR
// }}}RME

// }}}RME
