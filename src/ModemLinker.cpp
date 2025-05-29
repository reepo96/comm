// {{{RME classifier 'Logical View::ModemLinker'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "ModemLinker.h"
#endif

#include <RTSystem/Communication.h>
#include <ModemLinker.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* 文件名称：ModenLinker.cpp
* 文件标识：
* 摘    要：处理modem连接的类
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

#include "Protocol103.h"
#include "Protocol60_103.h"

// }}}USR
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
ModemLinker::~ModemLinker( void )
{
}
// }}}RME

// {{{RME operation 'ModemLinker()'
ModemLinker::ModemLinker( void )
	// {{{RME tool 'OT::Cpp' property 'ConstructorInitializer'
	// {{{USR

	// }}}USR
	// }}}RME
{
	// {{{USR
	m_bIsOpen = false;
	m_hComDev = NULL;
	m_bIsCmdState = true;
	// }}}USR
}
// }}}RME

// {{{RME operation 'Init(void*,LINKERTYPE)'
void ModemLinker::Init( void * parm, LINKERTYPE linkerType )
{
	// {{{USR
	m_CommType = CT_MODEM;

	//初始化连接参数
	memset(&strLinkParm,0,sizeof(strLinkParm));
	memcpy(&strLinkParm,parm,sizeof(strLinkParm));

	//调用父类函数
	Linker::Init(parm,linkerType);
	// }}}USR
}
// }}}RME

// {{{RME operation 'OpenCom()'
bool ModemLinker::OpenCom( void )
{
	// {{{USR
	m_isConnect = false;
	m_isInitComm = false;
	char szAswer[255]={0};

	//打开串口
	m_hComDev = CreateFile(strLinkParm.comName,GENERIC_READ|GENERIC_WRITE,0,NULL,
		OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(m_hComDev == INVALID_HANDLE_VALUE)
	{
		mylog.WriteLog2File(LogControl::LERROR,"open com error in ModemLinker::OpenCom()");
		return false;
	}

	//初始化串口配置
	DCB dcb;
	memset(&dcb,0,sizeof(DCB));
	dcb.DCBlength = sizeof(DCB);

	char dcbInitString[50]={0};
	sprintf(&dcbInitString[0],"%s: baud=%d parity=%c data=%d stop=1",
		strLinkParm.comName,strLinkParm.baudrate,strLinkParm.parity,strLinkParm.data);
	if(!BuildCommDCB(dcbInitString,&dcb))
	{
		mylog.WriteLog2File(LogControl::LERROR,"build DCB error in ModemLinker::OpenCom()");
		Close();
		return false;
	}

	dcb.fOutxCtsFlow = TRUE;
	dcb.fOutxDsrFlow = TRUE;
	dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
	dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;

	if(!SetCommState(m_hComDev,&dcb) || !SetupComm(m_hComDev,1600,1600))
	{
		mylog.WriteLog2File(LogControl::LERROR,"set com error in ModemLinker::OpenCom()");
		Close();
		return false;
	}

	COMMTIMEOUTS tOut;
	if(!GetCommTimeouts(m_hComDev,&tOut))
	{
		mylog.WriteLog2File(LogControl::LERROR,"get com timeouts error in ModemLinker::OpenCom()");
		Close();
		return false;
	}

	tOut.ReadIntervalTimeout = 200;
	tOut.ReadTotalTimeoutMultiplier = 80;
	tOut.WriteTotalTimeoutMultiplier = 80;
	tOut.WriteTotalTimeoutConstant = 1000;
	tOut.ReadTotalTimeoutConstant = 1000;
	if(!SetCommTimeouts(m_hComDev,&tOut))
	{
		mylog.WriteLog2File(LogControl::LERROR,"set com timeouts error in ModemLinker::OpenCom()");
		Close();
		return false;
	}

	if(7 != SendData("ATS0=2\r",7,0))
	{
		this->Close();
		return false;
	}

	::Sleep(500);

	this->ReceiveData(szAswer,255,1000);
	if(::strstr(szAswer,"OK") == NULL && strstr(szAswer,"ok") == NULL)
	{
		this->Close();
		return false;
	}

	m_bIsOpen = true;
	return true;
	// }}}USR
}
// }}}RME

// {{{RME operation 'ExecuteCmd(struct Command&)'
bool ModemLinker::ExecuteCmd( struct Command & cmd )
{
	// {{{USR
	assert(m_pProtocol != NULL);
	return m_pProtocol->ExecuteCmd(this,cmd);
	// }}}USR
}
// }}}RME

// {{{RME operation 'Connect()'
bool ModemLinker::Connect( void )
{
	// {{{USR
#ifdef _MY_UNITTEST
	//单元测试代码
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
	char szCmd[64] ={0};

	if(!m_bIsOpen)
	{
		if(!OpenCom())
			return false;
	}

	sprintf(szCmd,"%s\r",strLinkParm.phoneNum);
	if(5 != SendData(szCmd,::strlen(szCmd),0))
		return false;

	::Sleep(1000);

	char szAnswer[256]={0};
	if(ReceiveData(szAnswer,255,20000) <= 0)
		return false;

	if( ::strstr(szAnswer,"CONNECT") == NULL)
		return false;

	m_isConnect = true;
	m_bIsCmdState = false;

	//协议连接初始化
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
int ModemLinker::SendData( const char * pDataBuf, int iDataLen, unsigned int uiTimeOut )
{
	// {{{USR
	assert(pDataBuf != NULL);

	DWORD writenLen = 0;    //实际写的大小
	DWORD remainLen   = iDataLen ;   //仍有多少数据需要写
	unsigned int waitTime = 0;      //等待时间
	DWORD iHasSendLen = 0;
	DWORD iSendLen = 0;

	uiTimeOut *= 2;

	while( remainLen > 0 )
	{
		if(uiTimeOut != 0 && waitTime>= uiTimeOut)
		{
			if(uiTimeOut > 2000)
				m_errCode = LEC_LINK_TIMEOUT;
			break;
		}

		DWORD state=0;
		GetCommModemStatus(m_hComDev,&state);

		if(!m_bIsCmdState && (state&MS_RLSD_ON) != MS_RLSD_ON) //非命令状态下已没有载波信号
		{
			m_errCode = LEC_LINKERR;			
			return -1;
		}

		if((state&MS_DSR_ON) != MS_DSR_ON || (state&MS_CTS_ON) != MS_CTS_ON) //modem没准备好
		{
			::Sleep(5);
			waitTime += 5;
			continue;
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
		if(comStat.cbOutQue >= 1600) //写缓冲区已满
		{
			::Sleep(10);
			waitTime += 10;
			continue;
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

		if(m_isExitLink)    //退出连接
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
bool ModemLinker::CreatTask( void )
{
	// {{{USR

	//用协议解析出任务
	Task* pTask = m_pProtocol->Decode(this);
	if(pTask != NULL)   
	{
		//设置任务来源
		if(m_LinkerType != LK_REMOTE)
		{
			pTask->cmd.from = CD_ASDU;
			pTask->cmd.to   = CD_NEURON;
		}

		if(m_pMsgQue != NULL)
		{
			if(!m_pMsgQue->SendMsg(pTask))  //将任务发送到任务队列中
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
int ModemLinker::ReceiveData( char * pDataBuf, int iBufferLen, unsigned int uiTimeOut )
{
	// {{{USR
	assert(pDataBuf != NULL);

	int readDataLen = 0;    //实际读取的大小
	int remainLen   = iBufferLen ;   //仍有多少数据需要读取
	DWORD recvLen = 1;

	unsigned int waitTime = 0;      //等待时间
	uiTimeOut *= 2;

	while( recvLen > 0 && readDataLen < iBufferLen)
	{
		if(m_isExitLink)    //退出连接
		{
			m_errCode = LEC_USER_TERMINATE;			
			break;
		}

		if(uiTimeOut != 0 && waitTime>= uiTimeOut)
		{
			if(uiTimeOut > 2000)
				m_errCode = LEC_LINK_TIMEOUT;
			break;
		}

		DWORD state=0;
		GetCommModemStatus(m_hComDev,&state);

		if(!m_bIsCmdState && (state&MS_RLSD_ON) != MS_RLSD_ON) //非命令状态下已没有载波信号
		{
			m_errCode = LEC_LINKERR;			
			return -1;
		}

		if((state&MS_DSR_ON) != MS_DSR_ON) //modem没准备好
		{
			::Sleep(5);
			waitTime += 5;
			continue;
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
		if(!comStat.cbInQue) //还没有数据发送过来
		{
			::Sleep(10);
			waitTime += 10;
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
bool ModemLinker::Close( void )
{
	// {{{USR
	SendData("+++",3,0);
	::Sleep(500);
	m_bIsCmdState = true; //回到命令状态

	SendData("ATH\r",4,0);
	Sleep(500);

	char szAnswer[256]={0};
	ReceiveData(szAnswer,255,1000);

	m_bIsOpen = false;

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
string ModemLinker::GetConnectAddr( void )
{
	// {{{USR
	return string(strLinkParm.comName);
	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR
void ModemLinker::Listen( DWORD (WINAPI DisposeFun)(void *pParm) )
{
    //打开退出监听的事件
    HANDLE hExitListenEvent =OpenEvent(EVENT_ALL_ACCESS,FALSE,LISTEN_EXIT_EVENTNAME);

	while(1)
	{
		//打开端口
		while(!m_bIsOpen)
		{
			if(OpenCom())
				break;

			if(WAIT_OBJECT_0==::WaitForSingleObject(hExitListenEvent,2000))
			{
				//退出监听事件已经被触发,则退出
				::ResetEvent(hExitListenEvent);
				CloseHandle(hExitListenEvent);
				hExitListenEvent = NULL;
				return;
			}
		}

		//等待连接
		char szInData[255]={0};
		DWORD recvLen = 254;
		while(1)
		{
			DWORD state=0;
			GetCommModemStatus(m_hComDev,&state);
			if((state&MS_RLSD_ON) == MS_RLSD_ON)
			{
				break;
			}

			if(WAIT_OBJECT_0==::WaitForSingleObject(hExitListenEvent,80))
			{
				//退出监听事件已经被触发,则退出
				::ResetEvent(hExitListenEvent);
				CloseHandle(hExitListenEvent);
				hExitListenEvent = NULL;
				return;
			}
		}

		::Sleep(200);
		m_bIsCmdState = false; //退出命令状态 

		//接收连接时产生的信息
		COMSTAT comStat;
		DWORD   flags;
		BOOL result = ClearCommError(m_hComDev,&flags,&comStat);
		if(!result)
		{
			m_errCode = LEC_LINKERR;
			mylog.WriteLog2File(LogControl::LERROR,"receive data error when use com linker");
			return ;
		}
		if(comStat.cbInQue >0)
		{
			ReadFile(m_hComDev,szInData,recvLen,&recvLen,NULL);
		}

		//初始化
		while(!IsInitComm()) //通讯没有初始化，则先初始化通讯
		{
			if(m_pProtocol->InitConnect(this))
			{
				m_isInitComm = true;
				m_isConnect = true;
				break;
			}

			if(WAIT_OBJECT_0==::WaitForSingleObject(hExitListenEvent,1000))
			{
				//退出监听事件已经被触发,则退出
				::ResetEvent(hExitListenEvent);
				CloseHandle(hExitListenEvent);
				hExitListenEvent = NULL;
				Close();
				return;
			}
		}

		//创建任务，直到创建一个任务成功，就可以退出线程，以后的任务监听工作在相应的设备中完成
		while(!CreatTask())
		{
			//如果串口已经被关闭，则再次打开
			while(!m_bIsOpen)
			{
				if(OpenCom())
					break;

				if(WAIT_OBJECT_0==::WaitForSingleObject(hExitListenEvent,2000))
				{
					//退出监听事件已经被触发,则退出
					::ResetEvent(hExitListenEvent);
					CloseHandle(hExitListenEvent);
					hExitListenEvent = NULL;
					return;
				}
			}

			if(WAIT_OBJECT_0==::WaitForSingleObject(hExitListenEvent,500))
			{
				//退出监听事件已经被触发,则退出
				::ResetEvent(hExitListenEvent);
				CloseHandle(hExitListenEvent);
				hExitListenEvent = NULL;
				Close();
				return;
			}
		}

		//保持住线程
		while(1)
		{
			if(WAIT_OBJECT_0==::WaitForSingleObject(hExitListenEvent,2000))
			{
				//退出监听事件已经被触发,则退出
				::ResetEvent(hExitListenEvent);
				CloseHandle(hExitListenEvent);
				hExitListenEvent = NULL;
				Close();
				return;
			}

			if(!IsConnect())
			{
				break;
			}
			else
			{
				continue;
			}
		}

	}

	CloseHandle(hExitListenEvent);
	hExitListenEvent = NULL;
}

#ifdef _MY_UNITTEST
string ModemLinker::WhoAmI()
{
    return string("ModemLinker");
}
#endif

// }}}USR
// }}}RME

// }}}RME
