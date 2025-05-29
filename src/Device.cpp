// {{{RME classifier 'Logical View::Device'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "Device.h"
#endif

#include <RTSystem/Communication.h>
#include <Device.h>
#include <CommunicationManager.h>
#include <DeviceCtrl.h>
#include <DeviceState.h>
#include <Linker.h>
#include <LogControl.h>
#include <MemMapMgr.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* 文件名称：Device.cpp
* 文件标识：
* 摘    要：设备类，一个设备代表一条连接
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

#include "DevIdleState.h"
#include <CommDataType.h>
#include "SocketLinker.h"
#include "ModemLinker.h"
#include "ComLinker.h"
#include "P61850Linker.h"

#ifdef _DEBUG
#define new MYDEBUG_NEW
#endif

#define SHDFR_B_TYPE "SHDFR_B"
#include "StringRes.h"

// }}}USR
// }}}RME

// {{{RME operation 'Build(PDAUINFO,DeviceCtrl*)'
Device * Device::Build( PDAUINFO pDAUInfo, DeviceCtrl * pParent )
{
	// {{{USR
	if(pDAUInfo == NULL) //配置不存在
	{
		return NULL;
	}
	else //配置存在 
	{
		DeviceType DevType = DT_UNKOW;  /// 
		if(pDAUInfo->BoardType == 1)//暂态设备
		{
			DevType = DT_TRNS;
		}
		else if(pDAUInfo->BoardType == 2)//稳态设备
		{
			DevType = DT_CONST;
		}
		else if(pDAUInfo->BoardType == 4)//电能质量设备
		{
			DevType = DT_PQ;
		}
		else if(pDAUInfo->BoardType == 8)//PMU设备
		{
			DevType = DT_PMU;
		}
		else 
		{
			DevType = DT_COMPLEX;      //复合设备
		}

		void   *pParam = NULL;  //通讯需要的参数
		Linker *pLinker = NULL;            
		struct SocketParm strLinkParm;             //设置tcp/ip通讯所需的参数
		memset(&strLinkParm,0,sizeof(strLinkParm));

		struct ComParm comParm;                    //设置串口通讯参数
		memset(&comParm,0,sizeof(comParm));

		struct ModemParm modemParm;                //设置moden通讯参数
		memset(&modemParm,0,sizeof(modemParm));

		//if( strcmp(pDAUInfo->DAUType,SHDFR_B_TYPE) == 0||pDAUInfo->ProtocalType ==2)//设备类型是SHDFR_B设备
		if( pDAUInfo->ProtocalType ==2)//设备类型是SHDFR_B设备
		{
			pLinker = new P61850Linker();

			struct P61850Linker::tagLinkParm p61850Parm;
			pParam = (void*)&p61850Parm;

			GetPrivateProfileString("System","CfgFile","./osicfg.xml",p61850Parm.cfgFile,sizeof(p61850Parm.cfgFile),"./IEC61850.ini");
			
			::memcpy(p61850Parm.devName,pDAUInfo->DAUName,strlen(pDAUInfo->DAUName));
			::memcpy(p61850Parm.ARName,pDAUInfo->DAUName,strlen(pDAUInfo->DAUName));
			if(pDAUInfo->BoardType == 1)//暂态设备
			{
				::strcat(p61850Parm.ARName,"_trns");
			//	DevType = DT_TRNS;
			}
			else if(pDAUInfo->BoardType == 2)//稳态设备
			{
				::strcat(p61850Parm.ARName,"_cont");
				//DevType = DT_CONST;
			}
			else if(pDAUInfo->BoardType == 4)//电能质量设备
			{
				::strcat(p61850Parm.ARName,"_pq");
			//	DevType = DT_CONST;
			}
			else if(pDAUInfo->BoardType == 8)//PMU设备
			{
				::strcat(p61850Parm.ARName,"_pmu");
			//	DevType = DT_CONST;
			}
			else 
			{
				::strcat(p61850Parm.ARName,"_complex");  //复合设备
			//	DevType = DT_CONST;
			}

			/***转换成utf-8编码-->begin***/
			//ascII码转换转换成unicode
			int nLen = MultiByteToWideChar(CP_ACP, 0,p61850Parm.ARName, -1, NULL, NULL);
			LPWSTR pwsTmpDevName = new WCHAR[nLen];
			int WriteLen=MultiByteToWideChar(CP_ACP, 0,p61850Parm.ARName, -1, pwsTmpDevName, nLen);
			
			//unicode码转换转换成utf-8
			nLen = WideCharToMultiByte(CP_UTF8,0,pwsTmpDevName,-1,p61850Parm.ARName,0,NULL,NULL);
			WideCharToMultiByte(CP_UTF8,0,pwsTmpDevName,-1,p61850Parm.ARName,nLen,NULL,NULL);
			delete []pwsTmpDevName;
			pwsTmpDevName = NULL;
			/***转换成utf-8编码-->end***/
		}
		else
		{
			switch(pDAUInfo->linkType)
			{
			case CFG_SOCKET_LINK:      //使用tcp/ip通讯
				pLinker = new SocketLinker();
				pParam = (void*)&strLinkParm;

				//设置tcp/ip通讯所需的参数
				memcpy(&strLinkParm.loacalAddr[0],
					&(SystemCfger::GetInstance().addrForADU[0]),
					sizeof(SystemCfger::GetInstance().addrForADU)); //设置邦定本地地址
				strLinkParm.loacalTcpPort = SystemCfger::GetInstance().tcpPortForADU; //设置邦定本地端口

				memcpy(&strLinkParm.connetToAddr[0],
					&(pDAUInfo->addr[0]),
					sizeof(pDAUInfo->addr));                     //设置邦定的地址
				strLinkParm.connetToTcpPort = pDAUInfo->tcpPort; //设置邦定的端口
				strLinkParm.connetToUdpPort = pDAUInfo->udpPort; //设置udp端口
				break;

			case CFG_COM_LINK:         //使用串口通讯
				pLinker = new ComLinker();

				//设置串口通讯参数
				pParam = (void*)&comParm;
				memcpy(&comParm.comName[0],&(pDAUInfo->comName[0]),
					sizeof(comParm.comName));
				comParm.baudrate = pDAUInfo->baud;
				comParm.data = pDAUInfo->dataBit;
				comParm.parity = pDAUInfo->parity;
				comParm.stop = pDAUInfo->stop;

				break;

			case CFG_MODEN_LINK:       //使用moden通讯
				pLinker = new ModemLinker();

				//设置moden通讯参数
				pParam = (void*)&modemParm;
				memcpy(modemParm.phoneNum,pDAUInfo->phoneNum,sizeof(modemParm.phoneNum));

				break;

			default:
				mylog.WriteLog2File(LogControl::LERROR,"Unkown the link type for the DAU.");
				return NULL;
			}//end switch
		}//end if

		if(pLinker != NULL)
		{
			switch(DevType)
			{
			case DT_TRNS:
				mylog.FormatLogInf(LogControl::LDEBUG,"Begin create trans device,device name:%s.",pDAUInfo->DAUName);
				break;
			case DT_CONST:
				mylog.FormatLogInf(LogControl::LDEBUG,"Begin create const device,device name:%s.",pDAUInfo->DAUName);
				break;
			case DT_PQ:
				mylog.FormatLogInf(LogControl::LDEBUG,"Begin create PQ  device,device name:%s.",pDAUInfo->DAUName);
				break;
			case DT_PMU:
				mylog.FormatLogInf(LogControl::LDEBUG,"Begin create PMU device,device name:%s.",pDAUInfo->DAUName);
				break;
			case DT_COMPLEX:
				mylog.FormatLogInf(LogControl::LDEBUG,"Begin create COMPLEX device,device name:%s.",pDAUInfo->DAUName);
				break;
			}
/*
			if(pDAUInfo->BoardType == 1)//暂态设备
			{
				mylog.FormatLogInf(LogControl::LDEBUG,"Begin create trans device,device name:%s.",pDAUInfo->DAUName);
			}
			else
			{
				mylog.FormatLogInf(LogControl::LDEBUG,"Begin create const device,device name:%s.",pDAUInfo->DAUName);
			}
*/

			pLinker->Init(pParam,Linker::LK_ASDU);    //初始化
			pLinker->SetDevId(pDAUInfo->DAUId);

			if(!pLinker->Connect()) //连接失败
			{
				delete pLinker;
				pLinker = NULL;
				return NULL;
			}

			//Device *pDevice = new Device(pDAUInfo->DAUId,pParent,pLinker,DevType);
			Device *pDevice = new Device(pDAUInfo->BoardId,pParent,pLinker,DevType);
			return pDevice;
		}
		else
		{
			return NULL;
		}
	}
	// }}}USR
}
// }}}RME

bool Device::SetAutoLinker(Linker *pLinker)
{
	if( !pLinker )
		return false;

	if( m_pAutoLinker != NULL )
	{
		mylog.FormatLogInf(LogControl::LDEBUG,"m_pAutoLinker != NULL,delete it");
		m_pAutoLinker->SaftDelete();
	}
	m_pAutoLinker = pLinker;
	m_pAutoLinker->AddRef();
	m_pAutoLinker->DealAutoSend();
	m_pAutoLinker->SaftDelete();
	m_pAutoLinker = NULL;

	//处理主动上传的线程出错退出，则同时关闭主线程的链接
	if( m_pLinker )
	{
		mylog.FormatLogInf(LogControl::LINFO,"AutoSend thread has exit and preclose the main linker");
		m_pLinker->PreClose();
	}
	return true;
}

// {{{RME operation 'Device(unsigned int,DeviceCtrl*,Linker*)'
Device::Device( unsigned int deviceId, DeviceCtrl * pParent, Linker * pLinker,DeviceType DevType )
	// {{{RME tool 'OT::Cpp' property 'ConstructorInitializer'
	// {{{USR

	// }}}USR
	// }}}RME
{
	// {{{USR
	m_hDevTread = NULL;
	m_task = NULL;
	_preState = NULL;
	_devState = NULL;
	m_pLinker = pLinker;
	m_pParent = pParent;
	m_deviceId = deviceId;

	m_bIsStop = false;
	m_iWatchPoint = 0;
	m_iGetTimeContErrCount = 0;

	m_DevType = DevType;
	m_pAutoLinker = NULL;

	if( m_pLinker )
		m_pLinker->SetDevId(deviceId);

#ifndef _DEBUG
	//设置看门狗超时时间
	if( GetDeviceType() != DT_REMOTE )
	{
		m_iWatchPoint = m_WatchDog.addMonitorPoint(10*60);//10分钟超时
	}
	else
	{
		m_iWatchPoint = m_WatchDog.addMonitorPoint(60*60);//1小时超时
	}
#endif

	m_pTimerResultMsgQue = NULL;
	m_pTimerResultMsgQue = QueRegister::GetInstance().RegisterQue(COMM_TIMER_RESULTQUE_NAME);

	//初始状态为Idle状态
	_devState = dynamic_cast<DeviceState*>(&DevIdleState::GetStateInstance());

	//触发停留事件
	m_nextEvent = DE_STILL;

	//runThread(); //创建线程

	// }}}USR
}
// }}}RME

// {{{RME operation '~Device()'
Device::~Device( void )
{
	// {{{USR
#ifndef _DEBUG
	m_WatchDog.delMonitorPoint(m_iWatchPoint);
#endif

	if(m_task != NULL)
	{
		delete m_task;
		m_task = NULL;
	}

	/*if(_preState != NULL)
	{
	delete _preState;
	_preState = NULL;
	}

	if(_devState != NULL)
	{
	delete _devState;
	_devState = NULL;
	}*/

	if(m_pLinker != NULL)
	{
		m_pLinker->ExitLink();
		::Sleep(50);
		m_pLinker->Close();
		delete m_pLinker;
		m_pLinker = NULL;
	}

	Task * task=NULL;
	while( (task = m_rmtQue.ReceiveMsg()) != NULL)
	{
		delete task;
		task = NULL;
	}
	while( (task = m_aduQue.ReceiveMsg()) != NULL)
	{
		delete task;
		task = NULL;
	}
	while( (task = m_uiQue.ReceiveMsg()) != NULL)
	{
		delete task;
		task = NULL;
	}
	while( (task = m_timerQue.ReceiveMsg()) != NULL)
	{
		delete task;
		task = NULL;
	}

	DWORD dwExitCode = 0;
	if(m_hDevTread != NULL)
	{
		GetExitCodeThread(m_hDevTread,&dwExitCode);
		if( STILL_ACTIVE == dwExitCode)
		{
			TerminateThread(m_hDevTread,0);
		}
		::CloseHandle(m_hDevTread);
		m_hDevTread = NULL;
	}

	// }}}USR
}
// }}}RME

// {{{RME operation 'SendTask(Task*)'
bool Device::SendTask( Task * task )
{
	// {{{USR
	assert(task != NULL);

	//如果设备无效,则删除任务并记录日志
	if(_devState->GetStateValue() == DeviceState::DSV_INVALID || 
		_devState->GetStateValue() == DeviceState::DSV_UNKOWN)
	{
		if(m_pLinker != NULL)
		{
			m_pLinker->GetCmdDesc(task->cmd);
		}

		//记录日志
		LogInfo LogInfo;
		memset(&LogInfo,0,sizeof(LogInfo));

		PDAUINFO pDauInfo = SystemCfger::GetInstance().GetDAUInfo(task->cmd.destDevId);

		if(pDauInfo != NULL)
		{
			int minSize = min(sizeof(LogInfo.DAUName),sizeof(pDauInfo->DAUName));
			memcpy(LogInfo.DAUName,pDauInfo->DAUName,minSize);
			LogInfo.LogDAUID = pDauInfo->DAUId;
		}
	/*	PDAUINFO pDauInfo = SystemCfger::GetInstance().GetDAUInfo(task->cmd.destDevId,1);
		if(pDauInfo == NULL)
		{
			pDauInfo = SystemCfger::GetInstance().GetDAUInfo(task->cmd.destDevId,2);
		}

		LogInfo.LogDAUID = (int)task->cmd.destDevId;
		if(pDauInfo != NULL)
		{
			int minSize = min(sizeof(LogInfo.DAUName),sizeof(pDauInfo->DAUName));
			memcpy(LogInfo.DAUName,pDauInfo->DAUName,minSize);
		}
*/
		SYSTEMTIME sysTime;
		memset(&sysTime,0,sizeof(sysTime));
		::GetLocalTime(&sysTime);
		sprintf(LogInfo.SysLogTime,"%d-%02d-%02d %02d:%02d:%02d",sysTime.wYear,sysTime.wMonth,sysTime.wDay,
			sysTime.wHour,sysTime.wMinute,sysTime.wSecond);

		string sReason;
		if(task->cmd.to == CD_CONST_DAU)
		{
			sReason.append( (CStringRes::GetInstance())->GetStrById(IDS_CONST_REC,"稳态：") );
		}
		sReason.append(task->cmd.describe);
		memcpy(LogInfo.SysLogReason,sReason.c_str(),sizeof(LogInfo.SysLogReason) -1);

		LogInfo.SysLogType = 0;
		LogInfo.SysLogLevel = 2;

		sprintf(LogInfo.SysLogDesc,
			(CStringRes::GetInstance())->GetStrById(IDS_LOG_DESC,"耗时:%d 毫秒, %s"),0,
			(CStringRes::GetInstance())->GetStrById(IDS_EXCFAIL_LINKFAIL,"执行失败，失败原因：前台设备连接失败"));
		mylog.WriteLog(&LogInfo);

		//写失败的返回结果
		task->cmd.resultNum = 2;
		PCMDRESULT pResult = new CMDRESULT;
		pResult->resultType = CDRT_TRUFALSE;
		pResult->resultLen = 1;
		pResult->pData = new char[1];
		pResult->pData[0] = 0 ; //false
		task->cmd.resultList.push_back(pResult);

		pResult = new CMDRESULT;
		pResult->resultType = CDRT_DESC;
		pResult->resultLen = strlen( (CStringRes::GetInstance())->GetStrById(IDS_CONNDEV_FAIL,"前台设备不可连通") );
		pResult->pData = new char[pResult->resultLen+1];
		strcpy(pResult->pData,(CStringRes::GetInstance())->GetStrById(IDS_CONNDEV_FAIL,"前台设备不可连通") );
		pResult->pData[pResult->resultLen] = 0 ;
		task->cmd.resultList.push_back(pResult);

		//执行结果写入内存中
		MemMapMgr memMgr;
		memMgr.WriteTaskResultToMem(task->cmd);

		task->Linker = NULL;
		delete task;
		task = NULL;

		return true;
	}

	//m_task = task;

	//分析任务来源，以便放入不同的队列中
	switch(task->cmd.from)
	{
	case CD_REMOTE:
		return m_rmtQue.ReplaceOrAddMsg(task);
	case CD_ASDU:
	case CD_CONST_DAU:
		return m_aduQue.ReplaceOrAddMsg(task);
	case CD_UI:
	case CD_NEURON:
		return m_uiQue.ReplaceOrAddMsg(task);
	case CD_TIMER:
		return m_timerQue.ReplaceOrAddMsg(task);
	default:
		break;
	}

	return false;
	// }}}USR
}
// }}}RME

// {{{RME operation 'run()'
void Device::run( void )
{
	// {{{USR
	//设备线程的主运行程序
	mylog.WriteLog2File(LogControl::LINFO,"Enter Device::run()");
	assert(m_pLinker != NULL);

	//初始化通讯连接
	//m_pLinker->Connect();

	while(!m_bIsStop)
	{
		this->TrigerEvent(m_nextEvent);

#ifndef _DEBUG
		m_WatchDog.kickMonitorPoint(m_iWatchPoint);
#endif
	}

	if(m_pParent)
		m_pParent->DeleteDev(this); //删除设备

	// }}}USR
}
// }}}RME

// {{{RME operation 'runThread()'
void Device::runThread( void )
{
	// {{{USR
		if( m_hDevTread == NULL )
		{
			m_hDevTread = CreateThread(NULL,0,&DeviceThread,this,0,NULL);
		}
	// }}}USR
}
// }}}RME

// {{{RME operation 'TrigerEvent(DEVEVENT)'
void Device::TrigerEvent( DEVEVENT devEvent )
{
	// {{{USR
	char szLogInfo[200] ={0};
	try
	{
		if(_devState != NULL)
		{
			_devState->TrigerEvent(devEvent,this);
		}
	}
	catch(...)
	{
		mylog.WriteLog2File(LogControl::LWARNING,"Catch an exception when TrigerEvent in Device::TrigerEvent()");
	}

	// }}}USR
}
// }}}RME

// {{{RME operation 'ChangeState(DeviceState*)'
void Device::ChangeState( DeviceState * newState )
{
	// {{{USR
	char szLogInfo[200] ={0};
	try
	{
		_preState = _devState;
		_devState = newState;

		_devState->Do(this);//处理状态的动作
	}
	catch(...)
	{
		mylog.WriteLog2File(LogControl::LWARNING,"Catch an exception when triger the new state action in Device::ChangeState()");

		_preState = NULL;
		_devState = dynamic_cast<DeviceState*>(&DevIdleState::GetStateInstance());

		_devState->Do(this);
	}

	// }}}USR
}
// }}}RME

// {{{RME operation 'DoIdle()'
bool Device::DoIdle( void )
{
	// {{{USR
	//在idle状态下主要做两件事：1、有没有发送过来的任务请求。2、任务队列中有没有任务
	assert(m_pLinker != NULL);

	if(!m_pLinker->IsConnect() || m_pLinker->IsPreDisConnect() )  //如果连接已经被关闭
	{
		//如果连接是服务端的连接，则需销毁此设备。因为连接关闭后，
		//客户端会再次调用connect()连接到本地的服务器端，此时会产生新的连接设备
		if(m_pLinker->IsServerLink() && m_pLinker->GetCommType() == Linker::CT_SOCKET)
		{
			m_bIsStop = true;       //退出线程
			return true;
		}
		else
		{
			m_nextEvent = DE_INVALID; //设备进入无效状态
			return true;
		}
	}

	//先看看有没有发送过来的任务
	if(m_pLinker->CreatTask())
	{
		//已经成功创建新任务，则等待任务发送到设备的任务队列中
		::Sleep(10);
		m_nextEvent = DE_STILL; //重入idle状态
		return true;
	}

	//再察看队列中的任务
	if(m_task != NULL)
	{
		m_task->Linker = NULL;
		delete m_task;
		m_task = NULL;
	}

	m_task = m_aduQue.ReceiveMsg();     //先取ADU中的任务
	if(m_task == NULL)
	{
		m_task = m_rmtQue.ReceiveMsg(); //再取远程队列中的任务
	}
	if(m_task == NULL)
	{
		//从暂停队列获取任务
		vector<Task*>::iterator it = m_PauseTaskList.begin();
		if(it != m_PauseTaskList.end())
		{
			m_task = (*it);
			m_PauseTaskList.erase(it);
		}
	}
	if(m_task == NULL)
	{
		m_task = m_uiQue.ReceiveMsg(); //再取UI队列中的任务
	}
	if(m_task == NULL)
	{
		m_task = m_timerQue.ReceiveMsg(); //再取定时器队列中的任务
	}

	//队列中找到任务
	if(m_task != NULL)
	{
		//系统不设超时时间或任务是暂停的，则不考虑任务是否已经超时
		if(m_task->cmd._state._state != CmdState::CS_PAUSE &&
			SystemCfger::GetInstance().timeOut != 0)
		{
			DWORD currentTime = GetTickCount();
			if( (currentTime - m_task->createTime) >= (DWORD)SystemCfger::GetInstance().timeOut)
			{
				//任务已经超时，则删除它
				delete m_task;
				m_task = NULL;
				m_nextEvent =DE_STILL; //重入idle状态
				return true;
			}
		}

		m_nextEvent = DE_LEAVE; //离开idle进入work状态，处理此任务
		return true;
	}

	//执行链路的idle功能
	m_pLinker->DoIdle();

	//没有可执行的任务
	::Sleep(10);
	m_nextEvent =DE_STILL; //重入idle状态

	return true;

	// }}}USR
}
// }}}RME

// {{{RME operation 'DoWork()'
bool Device::DoWork( void )
{
	// {{{USR
	assert(m_task != NULL);

	CommunicationManager cmmMgr;
	::Sleep(20);

	//记录日志
	LogInfo LogInfo;
	memset(&LogInfo,0,sizeof(LogInfo));

	PDAUINFO pDauInfo = SystemCfger::GetInstance().GetDAUInfo(m_task->cmd.destDevId);
	if(pDauInfo != NULL)
	{
			int minSize = min(sizeof(LogInfo.DAUName),sizeof(pDauInfo->DAUName));
			memcpy(LogInfo.DAUName,pDauInfo->DAUName,minSize);
			LogInfo.LogDAUID = pDauInfo->DAUId;
	}
	/*
	PDAUINFO pDauInfo = SystemCfger::GetInstance().GetDAUInfo(m_task->cmd.destDevId,1);
	if(pDauInfo == NULL)
	{
		pDauInfo = SystemCfger::GetInstance().GetDAUInfo(m_task->cmd.destDevId,2);
	}
	
	LogInfo.LogDAUID = (int)m_task->cmd.destDevId;
	if(pDauInfo != NULL)
	{
		int minSize = min(sizeof(LogInfo.DAUName),sizeof(pDauInfo->DAUName));
		memcpy(LogInfo.DAUName,pDauInfo->DAUName,minSize);
	}
	*/

	SYSTEMTIME sysTime;
	memset(&sysTime,0,sizeof(sysTime));
	::GetLocalTime(&sysTime);
	sprintf(LogInfo.SysLogTime,"%d-%02d-%02d %02d:%02d:%02d",sysTime.wYear,sysTime.wMonth,sysTime.wDay,
		sysTime.wHour,sysTime.wMinute,sysTime.wSecond);

	string sLogReason("");

	LogInfo.SysLogType = 0;
	LogInfo.SysLogLevel = 0;

	char szInfo[255]={0};
	sprintf(szInfo,"Begin run a task:FunCode=0x%x,dest DAU id=%d,from=%d,to=%d",m_task->cmd.funCode,
		m_task->cmd.destDevId,m_task->cmd.from,m_task->cmd.to);
	mylog.WriteLog2File(LogControl::LDEBUG,szInfo);

	DWORD startTime = GetTickCount();

	string sResult("");

	//开始执行任务
	m_task->cmd._state.ToPreState();    //先将命令状态置为前一状态
	m_task->Linker = this->m_pLinker;
	
	if(cmmMgr.RunTask(m_task)) //任务执行成功
	{
		m_nextEvent = DE_SUCCESS;
		sResult.append( (CStringRes::GetInstance())->GetStrById(IDS_EXC_SUCCESS,"执行成功") );

		if(m_task->cmd.funCode == CMD_GETTIME)
		{
			m_iGetTimeContErrCount = 0;
		}
	}
	else    //任务执行失败
	{
		if(m_task->cmd._state._state == CmdState::CS_PAUSE)
		{
			//暂停
			m_PauseTaskList.push_back(m_task);
			m_nextEvent = DE_LEAVE;
			sResult.append( (CStringRes::GetInstance())->GetStrById(IDS_CMD_SUSPEND,"命令暂停") );
			Sleep(10);
		}
		else
		{
			LogInfo.SysLogLevel = 2;
			m_nextEvent = DE_FAIL;
			sResult.append( (CStringRes::GetInstance())->GetStrById(IDS_EXCFAIL_REASON,"执行失败,失败原因：") );

			int iResultNum = m_task->cmd.resultNum;
			if( iResultNum > 0 && (m_task->cmd.resultList[iResultNum-1])->resultType == CDRT_DESC)
			{
				sResult.append((m_task->cmd.resultList[iResultNum-1])->pData);
			}

			if(m_task->cmd.funCode == CMD_GETTIME)
			{
				m_iGetTimeContErrCount++;
			}
		}
	}
	sLogReason.append(m_task->cmd.describe);
	memcpy(LogInfo.SysLogReason,sLogReason.c_str(),sLogReason.size());

	DWORD endTime = GetTickCount();

	sprintf(LogInfo.SysLogDesc,
		(CStringRes::GetInstance())->GetStrById(IDS_LOG_DESC,"耗时:%d 毫秒, %s"),
		endTime-startTime,sResult.c_str());

	if(	m_nextEvent == DE_FAIL && m_task->cmd.funCode == CMD_GETTIME )
	{
		//如果是获取时间，则连续失败5次才记录日志
		if( m_iGetTimeContErrCount >=5 )
		{
			m_iGetTimeContErrCount = 0;
			mylog.WriteLog(&LogInfo);
		}
	}
	else
	{
		//CMD_104_CONFIRM_TESTFR == 0X26  。104 通信网络测试应答
		if(m_task->cmd.funCode !=0X26 && DE_SUCCESS == m_nextEvent)
		{
			mylog.WriteLog(&LogInfo);
		}
		if(m_task->cmd.funCode ==0X26 && DE_SUCCESS == m_nextEvent)
		{
			printf("Test frame \n");
		}

	}

	if(m_task->cmd._state._state == CmdState::CS_PAUSE)
	{
		//暂停的任务不写返回结果
		m_task = NULL;
		return true;
	}
	
	
	//执行结果写入内存中
	MemMapMgr memMgr;
	memMgr.WriteTaskResultToMem(m_task->cmd);

	//执行结果发给定时器
	if(m_task->cmd.from == CD_TIMER && m_pTimerResultMsgQue != NULL)
	{
		if(!m_pTimerResultMsgQue->SendMsg(m_task))
		{
			delete m_task;
		}
		m_task = NULL;
	}

	if(m_task != NULL)
	{
		m_task->Linker = NULL;
		delete m_task;
		m_task = NULL;
	}
	return true;

	// }}}USR
}
// }}}RME

// {{{RME operation 'DoInvalid()'
bool Device::DoInvalid( void )
{
	// {{{USR
	    assert(m_pLinker != NULL);
		printf("The device is invalid\r\n");

		//主链接失败，处理自动上传的链接要断开
		if( m_pAutoLinker && m_pAutoLinker->IsConnect() )
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Device into invalid stat,and close m_pAutoLinker");
			m_pAutoLinker->PreClose();
		}
		if( m_pLinker->IsPreDisConnect() && m_pLinker->IsConnect() )
		{
			m_pLinker->Close();
		}

		if(_preState->GetStateValue() != DeviceState::DSV_INVALID )//first into invalid
		{
			mylog.WriteLog2File(LogControl::LWARNING,"The device is invalid,maybe the linker has error.");
			Sleep(1000);
		}
		else
		{
			//再次进入，重新处理任务
			int iCount = 0;
			Task *pTask = m_uiQue.ReceiveMsg();
			while(pTask != NULL && iCount < 255 )
			{
				this->SendTask(pTask);
				pTask = m_uiQue.ReceiveMsg();
				iCount++;
			}
		}

		//等处理自动上传的链接断开之后才再次尝试连接
		if( m_pAutoLinker )
		{
			m_nextEvent = DE_STILL; 
			::Sleep(500);
			return true;
		}

		if(m_pLinker->IsServerLink())
		{
			if(!m_pLinker->IsConnect())
			{
				m_nextEvent = DE_STILL;  //连接失败，则再次进入无效状态
				::Sleep(500);
			}
			else
			{
				m_nextEvent = DE_LEAVE; //成功连接，退出无效状态
			}
		}
		else
		{
			printf("Try to link the device again\r\n");

			//不断尝试连接
			if(!m_pLinker->Connect())
			{
				m_nextEvent = DE_STILL;  //连接失败，则再次进入无效状态
				::Sleep(1000);
			}
			else
			{
				::Sleep(1000);
				m_nextEvent = DE_LEAVE; //成功连接，退出无效状态
			}
		}

		if(m_nextEvent == DE_LEAVE)
		{
			mylog.WriteLog2File(LogControl::LWARNING,"The device turn to work from invalid state.");
		}

		if(_preState->GetStateName() != string("DevInvalidState") || m_nextEvent == DE_LEAVE)
		{
			/*本模块不接受远程连接，所以不需主动发送状态报告
			//主动发送状态报告
			Task *pNewTask = new Task();
			pNewTask->Linker = NULL;
			pNewTask->createTime = ::GetTickCount();
			pNewTask->cmd.funCode = CMD_AUTOSEND_LINKSTATE;
			pNewTask->cmd.destDevId = m_deviceId;
			pNewTask->cmd.from = CD_NEURON;
			pNewTask->cmd.to = CD_REMOTE;
			strcpy(pNewTask->cmd.describe,"自动上传设备状态");
			pNewTask->cmd.parmLen = sizeof(DAVICESTATE);
			pNewTask->cmd.pParm = new char[pNewTask->cmd.parmLen];

			DAVICESTATE DevState;
			DevState.devId = m_deviceId;

			if(m_nextEvent == DE_LEAVE)
			{
				DevState.state = 2;
			}
			else
			{
				DevState.state = 11;
			}
			memcpy(pNewTask->cmd.pParm,&DevState,sizeof(DevState));

			(QueRegister::GetInstance().RegisterQue(COMM_REMOTEQUE_NAME))->SendMsg(pNewTask);*/
		}

		return true;
	// }}}USR
}
// }}}RME

// {{{RME operation 'DoTimout()'
bool Device::DoTimout( void )
{
	// {{{USR
	    return true;
	// }}}USR
}
// }}}RME

// {{{RME operation 'DoSuccess()'
bool Device::DoSuccess( void )
{
	// {{{USR

	    //TODO:
	    //记录日志
	    m_nextEvent = DE_LEAVE; //离开进入idle状态

	    return true;
	// }}}USR
}
// }}}RME

// {{{RME operation 'DoFail()'
bool Device::DoFail( void )
{
	// {{{USR
	    
	    //TODO:
	    //记录日志

	    m_nextEvent = DE_LEAVE; //离开进入idle状态
	    return true;

	// }}}USR
}
// }}}RME

// {{{RME operation 'GetDeviceType()'
Device::DEVUCETYPE Device::GetDeviceType( void )
{
	// {{{USR
	    return m_DevType;
	// }}}USR
}
// }}}RME

// {{{RME operation 'GetConnectAddr()'
string Device::GetConnectAddr( void )
{
	// {{{USR
	    assert(m_pLinker != NULL);

	    return m_pLinker->GetConnectAddr();

	// }}}USR
}
// }}}RME

// {{{RME operation 'GetDevId()'
unsigned int Device::GetDevId( void )
{
	// {{{USR
	    return m_deviceId;
	// }}}USR
}
// }}}RME

// {{{RME operation 'Stop()'
void Device::Stop( void )
{
	// {{{USR

	    m_bIsStop = true;
	    m_pLinker->ExitLink();

	// }}}USR
}
// }}}RME

// {{{RME operation 'MonitorThread()'
void Device::MonitorThread( void )
{
	// {{{USR
	DWORD  exitCode = 0;
	if(m_hDevTread == NULL)
	{
		this->runThread();
	}
	else
	{
		GetExitCodeThread(m_hDevTread,&exitCode);
		if(exitCode != STILL_ACTIVE)
		{
			mylog.WriteLog2File(LogControl::LWARNING,"The device thread isn't active,terminate it and create it again in Device::MonitorThread()");
			::TerminateThread(m_hDevTread,0);
			::CloseHandle(m_hDevTread);
			m_hDevTread = NULL;
			runThread();
		}
	}
	// }}}USR
}
// }}}RME

// {{{RME operation 'GetState()'
int Device::GetState( void )
{
	// {{{USR
	if(_devState != NULL)
	{
		switch(_devState->GetStateValue())
		{
		case DeviceState::DSV_IDLE:
			return 1;
		case DeviceState::DSV_INVALID:
			return 3;
		case DeviceState::DSV_UNKOWN:
			return 0;
		default:
			return 2;
		}
	}

	return 0;
	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR

DWORD WINAPI DeviceThread(LPVOID pParm)
{
	Device *pDev = (Device*)pParm;

	pDev->run();

	return 0;
}


// }}}USR
// }}}RME

// }}}RME
