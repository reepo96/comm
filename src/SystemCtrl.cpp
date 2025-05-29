#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "SystemCtrl.h"
#endif

#include <RTSystem/Communication.h>
#include <SystemCtrl.h>
#include <MemMapMgr.h>
#include <MsgQueue.h>
#include <QueRegister.h>
#include <SystemState.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,

* All rights reserved.
*
* 文件名称：SystemCtrl.cpp
* 文件标识
* 摘    要：系统控制类

*
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日

*
**************************************************************************/

#include "Linker.h"
#include "SocketLinker.h"
#include "ModemLinker.h"
#include "ComLinker.h"
#include "SysInitState.h"
#include "Timer.h"
#include "MemMapMgr.h"
#include "SysIdleState.h"
#include "FileDB.h"
#include "FileControl.h"
#include "acsi.h"
#include "Encrypt.h"

#ifdef _DEBUG
//调试内存泄露
#define new MYDEBUG_NEW
#endif

extern bool	g_bIsExitSys;
extern SystemCtrl *g_pSystemCtrlObj;

//定义队列名称
const string COMM_REMOTEQUE_NAME("CommRemoteQueName");//远程任务队列名
const string COMM_ASDUQUE_NAME("CommAsduQueName");//ASDU任务队列名
const string COMM_TIMERQUE_NAME("CommTimerQueName");//定时任务队列名
const string COMM_TIMER_RESULTQUE_NAME("CommTimerResultQueName");//定时任务执行结果队列名
const string COMM_INSIDE_RESULTQUE_NAME("CommInsideResultQueName");//通讯内部执行结果队列名
//定义共享内存文件名
const char* COMM_COMMAND_FILEMAP = "SHNeuronCmdFileMap";  //命令映射文件名
const char* COMM_RESULT_FILEMAP = "SHNeuronResultFileMap"; //命令返回映射文件名

//数据库操作相关
HINSTANCE g_hDBInstance = NULL;//DB动态库实例
bool LoadDBDll();

DBINIT DbInit;
ACCESSDBINIT AccessDBInit;
ADDLOG AddLog;
GETALLDAU GetAllDau;
GETONEDAU GetOneDau;
GETALLCFG GetAllCfg;
RELEASEMEMERY releaseMemery;
GETTOBESENDRPT2RMT getToBeSendRpt2Rmt;
ADDSENDFAULTINF addSendFaultInf;
GETRPTBYSQL getRptBySql;
GETRPTBYID getRptById;
GETALLTIMERCALLINF getAllTimerCallInf;
GETMAXFILENO getMaxFileNo;
ADDCALLFILEINF addCallFileInf;
GETNOCALLFILEBYTYPE getNoCallFileByType;
UPDATECALLFILEINFBYID updateCallFileInfById;
GETMARKEDEVTBYSQL getMarkedEvtBySql;

LogControl mylog;     //日志
LPCRITICAL_SECTION 	g_pSection4NewMem = NULL; //用于分配内存同步的临界区
DWORD WINAPI RmtListenThreadF(void *pParm);		//监听远程通讯的线程
DWORD WINAPI ADUListenThreadF(void *pParm);		//监听ADU通讯的线程
DWORD WINAPI TimerThreadF(void *pParm);			//时间线程
DWORD WINAPI GetUdpDataThread(void *pParm);		//接收UDP数据报的线程
DWORD WINAPI GetRltDataThread(void *pParm);		//接收UDP实时数据报的线程
DWORD WINAPI DisposeADUConn(void *pParm);		//处理adu的连接
DWORD WINAPI AutoSendDataThread(void *pParm);//监听主动上送数据的线程
DWORD WINAPI AutoSendDealThread(void *pParm);//处理主动上送数据的线程

//将采样数据写入共享内存中
bool WriteSampleData2Mem(PORIGPOINTDATAHEAD pOriPoint,char *pData,unsigned int iDataLen,unsigned int devId,unsigned char funCode);

// }}}USR
// }}}RME

// {{{RME operation 'SystemCtrl()'
SystemCtrl::SystemCtrl( void )
// {{{RME tool 'OT::Cpp' property 'ConstructorInitializer'
// {{{USR

// }}}USR
// }}}RME
{
	// {{{USR
	mylog.WriteLog2File(LogControl::LINFO, "Communication module start--------->>>>>>");

	g_pSystemCtrlObj = this;

	m_pTask = NULL;
	m_pResultMapData = NULL;
	m_hResultMapFile = NULL;
	m_hExitListenEvent = NULL;
	m_hCmdResultMapEvent = NULL;
	_state = NULL;

	m_hTimerTread = NULL;
	m_hRmtListenTread = NULL;
	m_hUdpDataTread = NULL;
	m_hRltDataTread = NULL;
	m_hAutoSendDataTread = NULL;

	g_pSection4NewMem = new CRITICAL_SECTION;
	InitializeCriticalSection(g_pSection4NewMem);
	m_iWatchPoint = 0;

	Init();//初始化

#ifndef _DEBUG
	m_WatchDog.createWatchDog();
	m_iWatchPoint = m_WatchDog.addMonitorPoint(30);
#endif

	// }}}USR
}
// }}}RME

// {{{RME operation '~SystemCtrl()'
SystemCtrl::~SystemCtrl( void )
{
	// {{{USR
	mylog.WriteLog2File(LogControl::LINFO,"Enter SystemCtrl::~SystemCtrl( void )");
	WSACleanup( );

#ifndef _DEBUG
	m_WatchDog.delMonitorPoint(m_iWatchPoint);
	m_WatchDog.quitWatchDog();
	::Sleep(1000);
#endif

	if(g_hDBInstance != NULL)
	{
		FreeLibrary(g_hDBInstance);
		g_hDBInstance = NULL;

		DbInit = 0;
		AddLog = 0;
		GetAllDau = 0;
		GetOneDau = 0;
		GetAllCfg = 0;
		releaseMemery = 0;
		getToBeSendRpt2Rmt = 0;
		addSendFaultInf = 0;
		getRptBySql = 0;
		getRptById = 0;
		getAllTimerCallInf = 0;
		getMaxFileNo = 0;
		addCallFileInf = 0;
		getNoCallFileByType = 0;
		updateCallFileInfById = 0;
	}

	//释放资源
	if(m_pResultMapData != NULL)
	{
		::UnmapViewOfFile((LPCVOID)m_pResultMapData);
		m_pResultMapData = NULL;
	}
	if(m_hResultMapFile != NULL)
	{
		::CloseHandle(m_hResultMapFile);
		m_hResultMapFile = NULL;
	}

	if(m_hExitListenEvent != NULL)
	{
		::CloseHandle(m_hExitListenEvent);
		m_hExitListenEvent = NULL;
	}

	if(m_hCmdResultMapEvent != NULL)
	{
		::CloseHandle(m_hCmdResultMapEvent);
		m_hCmdResultMapEvent = NULL;
	}

	if(g_pSection4NewMem != NULL)
	{
		DeleteCriticalSection(g_pSection4NewMem);
		g_pSection4NewMem = NULL;
	}
	// }}}USR
}
// }}}RME

// {{{RME operation 'Run()'
void SystemCtrl::Run( void )
{
	// {{{USR
	FileControl FileCtrl;
	struct CP56Time2a FileTime;
	memset(&FileTime,0,sizeof(struct CP56Time2a));

	FileCtrl.GetFileTime("./Communication.ini",&FileTime,3);
	unsigned int uiOldFileTime = (FileTime.year*365 +FileTime.month*30+FileTime.day)*24*3600
		+FileTime.hours*3600 +FileTime.minute*60 +FileTime.milliseconds/1000;
	unsigned int uiNewFileTime = uiOldFileTime;

	DWORD dwBeginTime = ::GetTickCount();
	DWORD dwEndTime = dwBeginTime;
	while(!m_isExit && !g_bIsExitSys)
	{
		this->MonitorThread();	//监控线程
		m_devCtrl.MonitorDev();	//监控设备

		this->TrigerEvent(m_nextEvent);	//触发事件，离开当前状态
		//每隔一定时间去检查定值文件是否被修改
		dwEndTime = ::GetTickCount();
		if( (dwEndTime - dwBeginTime) >= 2000)
		{
			dwBeginTime = dwEndTime;
			FileCtrl.GetFileTime("./Communication.ini",&FileTime,3);
			uiNewFileTime = (FileTime.year*365 +FileTime.month*30+FileTime.day)*24*3600
				+FileTime.hours*3600 +FileTime.minute*60 +FileTime.milliseconds/1000;

			if(uiNewFileTime != uiOldFileTime)
			{
				uiOldFileTime = uiNewFileTime;
				//重新读取ini配置
				SystemCfger::GetInstance().InitIniFileCfg();
			}
		}

#ifndef _DEBUG
		m_WatchDog.kickMonitorPoint(m_iWatchPoint);
#endif
	}

	m_isExit = true;
	g_bIsExitSys = true;

	mylog.WriteLog2File(LogControl::LINFO, "Communication module exit");
	// }}}USR
}
// }}}RME

// {{{RME operation 'Init()'
void SystemCtrl::Init( void )
{
	// {{{USR

	WSADATA wsaData;
	WORD    wVersion = MAKEWORD( 2, 2 );
	int result = WSAStartup( wVersion, &wsaData );

	//加载数据库接口动态库
	if(!LoadDBDll())
	{
		m_isExit = true;
		::Sleep(5000);
		return;
	}
	else
	{
		printf("Load DBAdapterDll Success\n");
	}

	//初始化配置
	SystemCfger::GetInstance().Init();

	//创建事件

	//创建自动reset，初始状态为无信号的事件
	m_hExitListenEvent =CreateEvent(NULL,FALSE,FALSE,LISTEN_EXIT_EVENTNAME);

	m_hCmdResultMapEvent = CreateEvent(NULL,FALSE,TRUE,CMDRESULT_FILEMAP_EVENT);

	int ret = 0;
	char szCfgFile[64] ={0};
	GetPrivateProfileString("System","CfgFile","./osicfg.xml",szCfgFile,sizeof(szCfgFile),"./IEC61850.ini");
	//初始化ACSI环境
	/*	ret = initACSIEnvironment( szCfgFile);
	if ( ret != 0 )
	{
	mylog.WriteLog2File(LogControl::LERROR, "initACSIEnvironment return fail");
	}

	//启动ACSI通讯线程
	ret = startACSICommThread();
	if ( ret != 0 )
	{
	mylog.WriteLog2File(LogControl::LERROR, "startACSICommThread return fail");
	}*/

	//创建线程
#ifdef SHDFR_PQ
	{
		m_hAutoSendDataTread = CreateThread(NULL,0,&AutoSendDataThread,0,0,NULL);
	}
#else
	{
		this->CreatADUListenThread();
		this->CreateTimerThread();
		this->CreatRmtListenThread();
		this->CreateUdpDataThread();
		m_hRltDataTread = CreateThread(NULL,0,&GetRltDataThread,this,0,NULL);
	}
#endif

	m_devCtrl.Init();

	/*********创建内存映射文件——>********************/
	const DWORD fileSize = COMM_RESULT_FILEMAP_SIZE;    //共享文件大小

	//打开内存文件
	m_hResultMapFile = CreateFileMapping(INVALID_HANDLE_VALUE,NULL,
		PAGE_READWRITE|SEC_COMMIT,0,fileSize,COMM_RESULT_FILEMAP);
	if( m_hResultMapFile != NULL)
	{
		//映射视图
		m_pResultMapData = (unsigned char*)MapViewOfFile(m_hResultMapFile,FILE_MAP_ALL_ACCESS,0,0,0);
		if( m_pResultMapData == NULL)
		{
			::CloseHandle(m_hResultMapFile);
			m_hResultMapFile = NULL;
		}
		else
		{
			memset(m_pResultMapData,0,fileSize);
		}
	}
	/*********创建内存映射文件<——********************/

	//初始状态为init状态
	_state = dynamic_cast<SystemState*>(&SysInitState::GetStateInstance());

	m_isExit = false;
	m_nextEvent = SE_LEAVE; //离开初始化状态

	// }}}USR
}
// }}}RME

// {{{RME operation 'TrigerEvent(SYSTEMEVENT)'
void SystemCtrl::TrigerEvent( SYSTEMEVENT sysEvent )
{
	// {{{USR
	char szLogInfo[200] ={0};
	try
	{
		_state->TrigerEvent(sysEvent,this);
	}
	catch(...)
	{
		mylog.WriteLog2File(LogControl::LWARNING,"Catch an exception when TrigerEvent in SystemCtrl::TrigerEvent()");
	}
	// }}}USR
}
// }}}RME

// {{{RME operation 'ChangeState(SystemState*)'
void SystemCtrl::ChangeState( SystemState * pNewState )
{
	// {{{USR
	char szLogInfo[200] ={0};
	try
	{
		_state = pNewState;
		_state->Do(this);	//进入新的状态后，处理此新状态的action
	}
	catch(...)
	{
		mylog.WriteLog2File(LogControl::LWARNING,"Catch an exception when triger the new state action in SystemCtrl::ChangeState()");

		_state = dynamic_cast<SystemState*>(&SysIdleState::GetStateInstance());
		_state->Do(this);
	}
	// }}}USR
}
// }}}RME

// {{{RME operation 'DoIdle()'
bool SystemCtrl::DoIdle( void )
{
	// {{{USR
	//获取下一个任务

	m_pTask = GetNextTask();

	if( m_pTask == NULL)    //没有新任务

	{
		::Sleep(100);
		m_nextEvent = SE_STILL;     //触发停留事件，使状态再次进入idle状态

	}
	else
	{
		m_pTask->createTime = GetTickCount();
		m_nextEvent = SE_LEAVE;      //触发离开事件，使状态进入work状态
	}

	return true;

	// }}}USR
}
// }}}RME

// {{{RME operation 'GetNextTask()'
Task * SystemCtrl::GetNextTask( void )
{
	// {{{USR
	MsgQueue *pMsgQue = NULL;
	Task     *pTask   = NULL;

	//先从adu的queue中查找任务
	pMsgQue = QueRegister::GetInstance().RegisterQue(COMM_ASDUQUE_NAME);
	if( pMsgQue != NULL)
	{
		pTask = pMsgQue->ReceiveMsg();
	}

	//再从远程queue中查找任务
	if(pTask == NULL)
	{
		pMsgQue = QueRegister::GetInstance().RegisterQue(COMM_REMOTEQUE_NAME);
		if( pMsgQue != NULL)
		{
			pTask = pMsgQue->ReceiveMsg();
		}
	}

	//再从共享内存中获取UI发过来的任务
	if(pTask == NULL)
	{
		MemMapMgr memMgr;
		pTask = memMgr.GetTaskFromMem();
	}

	//再从timer queue中查找任务
	if(pTask == NULL)
	{
		pMsgQue = QueRegister::GetInstance().RegisterQue(COMM_TIMERQUE_NAME);
		if( pMsgQue != NULL)
		{
			pTask = pMsgQue->ReceiveMsg();
		}
	}

	return pTask;
	// }}}USR
}
// }}}RME

// {{{RME operation 'DoWork()'
bool SystemCtrl::DoWork( void )
{
	// {{{USR
	char szInfo[255]={0};

	if(m_pTask->cmd.funCode != CMD_QUERY_LINKSTATE)
	{
		string sLogReason("");
		sprintf(szInfo,
			"System get a task:FunCode=0x%x, dest DAU id=%d, from=%d, to=%d",
			m_pTask->cmd.funCode,m_pTask->cmd.destDevId,m_pTask->cmd.from,m_pTask->cmd.to);
		mylog.WriteLog2File(LogControl::LDEBUG,szInfo);
	}

	//printf("Get task,fun-code:%X,and the time=%d\r\n",(int)m_pTask->cmd.funCode,::GetTickCount());

	//分析任务
	switch(m_pTask->cmd.funCode)
	{
	case CMD_EXIT:
		m_nextEvent = SE_EXIT;      //触发退出事件
		return true;
	case CMD_REBUILD_RMT:
	case CMD_REBUILD_DAU:
	case CMD_REBUILD_OTHER:
	case CMD_NEW_FAULTREPORT:
		m_nextEvent = SE_INIT;      //触发重建事件，系统重新初始化
		return true;
	case CMD_QUERY_LINKSTATE:       //查询链路状态
		if(m_pTask->cmd.to ==CD_REMOTE)
		{
			vector< DAVICESTATE > StateList;
			m_devCtrl.GetAllDevState(StateList,1);

			Task *pNewTask = new Task();
			pNewTask->Linker = m_pTask->Linker;
			pNewTask->createTime = ::GetTickCount();
			pNewTask->cmd.funCode = CMD_AUTOSEND_LINKSTATE;
			pNewTask->cmd.destDevId = m_pTask->cmd.destDevId;
			pNewTask->cmd.from = CD_NEURON;
			pNewTask->cmd.to = CD_REMOTE;
			strcpy(pNewTask->cmd.describe,"自动上传设备状态");
			pNewTask->cmd.parmLen = sizeof(DAVICESTATE)*StateList.size();
			pNewTask->cmd.pParm = new char[pNewTask->cmd.parmLen];

			for(unsigned int i=0;i<StateList.size();i++)
			{
				DAVICESTATE DevState = StateList[i];
				memcpy(&pNewTask->cmd.pParm[i*sizeof(DAVICESTATE)],&DevState,sizeof(DevState));
			}

			delete m_pTask;
			m_pTask = NULL;
			m_pTask = pNewTask;

			bool result = m_devCtrl.DispatchTask(m_pTask);
			m_nextEvent = SE_LEAVE;      //触发离开事件，使状态进入idle状态

			return result;
		}
		else
		{
			//写返回结果
			m_pTask->cmd.resultNum = 1;
			PCMDRESULT pResult = new CMDRESULT;
			pResult->resultType = CDRT_CHAR;
			pResult->resultLen = 1;
			pResult->pData = new char[1];

			if(m_pTask->cmd.pParm != NULL)
			{
				pResult->pData[0] = m_devCtrl.GetDevState(m_pTask->cmd.destDevId,m_pTask->cmd.pParm[0]);
			}
			else
			{
				pResult->pData[0] = m_devCtrl.GetDevState(m_pTask->cmd.destDevId,1);
			}

			m_pTask->cmd.resultList.push_back(pResult);

			//执行结果写入内存中
			MemMapMgr memMgr;
			memMgr.WriteTaskResultToMem(m_pTask->cmd);

			m_pTask->Linker = NULL;
			delete m_pTask;
			m_pTask = NULL;

			m_nextEvent = SE_LEAVE;	//触发离开事件，使状态进入idle状态

		}
		return true;
	default:

		//printf(">>>>Begin dispatch task,fun-code:%X\r\n",(int)m_pTask->cmd.funCode);
		//执行任务
		bool result = m_devCtrl.DispatchTask(m_pTask);

		m_nextEvent = SE_LEAVE;	//触发离开事件，使状态进入idle状态


		return result;
	}
	return true;
	// }}}USR
}
// }}}RME

// {{{RME operation 'DoInit()'
bool SystemCtrl::DoInit( void )
{
	// {{{USR

	//重新初始化配置
	switch(m_pTask->cmd.funCode)
	{
	case CMD_REBUILD_RMT:   //重建远程通讯相关配置
		SystemCfger::GetInstance().Rebuild(1);
		SetEvent(m_hExitListenEvent);   //设置退出监听的事件
		m_devCtrl.Rebuild(1);
		::Sleep(4000);
		break;
	case CMD_REBUILD_DAU:
		SystemCfger::GetInstance().Rebuild(2);
		m_devCtrl.Rebuild(2);
		break;
	case CMD_REBUILD_OTHER:
		SystemCfger::GetInstance().Rebuild(3);
		break;
	case CMD_NEW_FAULTREPORT:  //后台生成新的故障简报

		if(m_pTask->cmd.parmLen == 4)
		{
			int fileId =0;
			memcpy(&fileId,m_pTask->cmd.pParm,4);

			//将新故障简报检索出来并添加到所有的故障简报列表中
			SystemCfger::GetInstance().AppendReportList(m_pTask->cmd.destDevId,fileId);
		}
		break;
	default:
		break;
	}

	//写返回结果

	if(m_pTask->cmd.from == CD_UI)
	{
		PCMDRESULT pCmdResult = new CMDRESULT;
		pCmdResult->resultType = CDRT_TRUFALSE;	//返回的结果是真假类型
		pCmdResult->resultLen = 1;			//结果长度1字节
		pCmdResult->pData = new char[1];
		pCmdResult->pData[0] = 1;
		m_pTask->cmd.resultList.push_back(pCmdResult);

		MemMapMgr memMgr;
		memMgr.WriteTaskResultToMem(m_pTask->cmd);
	}

	delete m_pTask;
	m_pTask = NULL;

	m_nextEvent = SE_LEAVE;	//触发离开事件，使状态进入idle状态
	return true;

	// }}}USR
}
// }}}RME

// {{{RME operation 'DoRestart()'
bool SystemCtrl::DoRestart( void )
{
	// {{{USR
	m_nextEvent = SE_LEAVE;	//触发离开事件，使状态进入idle状态
	return true;
	// }}}USR
}
// }}}RME

// {{{RME operation 'DoExit()'
bool SystemCtrl::DoExit( void )
{
	// {{{USR
	m_isExit = true;
	m_devCtrl.Exit();

	//写返回结果
	if(m_pTask->cmd.from == CD_UI)
	{
		PCMDRESULT pCmdResult = new CMDRESULT;
		pCmdResult->resultType = CDRT_TRUFALSE;	//返回的结果是真假类型
		pCmdResult->resultLen = 1;		//结果长度1字节
		pCmdResult->pData = new char[1];
		pCmdResult->pData[0] = 1;
		m_pTask->cmd.resultList.push_back(pCmdResult);

		MemMapMgr memMgr;
		memMgr.WriteTaskResultToMem(m_pTask->cmd);
	}

	delete m_pTask;
	m_pTask = NULL;

	//清除队列
	Task * task=NULL;
	MsgQueue *pMsgQue = NULL;

	pMsgQue = QueRegister::GetInstance().RegisterQue(COMM_ASDUQUE_NAME);
	if( pMsgQue != NULL)
	{
		while( (task = pMsgQue->ReceiveMsg()) != NULL)
		{
			delete task;
			task = NULL;
		}
	}

	pMsgQue = QueRegister::GetInstance().RegisterQue(COMM_REMOTEQUE_NAME);
	if( pMsgQue != NULL)
	{
		while( (task = pMsgQue->ReceiveMsg()) != NULL)
		{
			delete task;
			task = NULL;
		}
	}

	pMsgQue = QueRegister::GetInstance().RegisterQue(COMM_TIMERQUE_NAME);
	if( pMsgQue != NULL)
	{
		while( (task = pMsgQue->ReceiveMsg()) != NULL)
		{
			delete task;
			task = NULL;
		}
	}

	return true;

	// }}}USR
}
// }}}RME

// {{{RME operation 'CreatADUListenThread()'
void SystemCtrl::CreatADUListenThread( void )
{
	// {{{USR
	HANDLE hThread = CreateThread(NULL,0,&ADUListenThreadF,NULL,0,NULL);
	::CloseHandle(hThread);
	// }}}USR
}

void SystemCtrl::CreatRmtListenThread( void )
{
	m_hRmtListenTread = CreateThread(NULL,0,&RmtListenThreadF,NULL,0,NULL);
}

void SystemCtrl::CreateTimerThread( void )
{
}

void SystemCtrl::MonitorThread( void )
{
	DWORD  exitCode = 0;
#ifdef SHDFR_PQ
	{
		exitCode = 0;
		if(m_hAutoSendDataTread == NULL)
		{
			m_hAutoSendDataTread = CreateThread(NULL,0,&AutoSendDataThread,0,0,NULL);
		}
		else
		{
			GetExitCodeThread(m_hAutoSendDataTread,&exitCode);
			if(exitCode != STILL_ACTIVE)
			{
				mylog.WriteLog2File(LogControl::LWARNING,"The AutoSendData thread isn't active,terminate it and create it again in SystemCtrl::MonitorThread()");
				::TerminateThread(m_hAutoSendDataTread,0);
				::CloseHandle(m_hAutoSendDataTread);
				m_hAutoSendDataTread = NULL;
				m_hAutoSendDataTread = CreateThread(NULL,0,&AutoSendDataThread,0,0,NULL);
			}
		}
	}
#else
	{
		if(m_hRmtListenTread == NULL)
		{
			this->CreatRmtListenThread();
		}
		else
		{
			GetExitCodeThread(m_hRmtListenTread,&exitCode);
			if(exitCode != STILL_ACTIVE)
			{
				mylog.WriteLog2File(LogControl::LWARNING,"The remote thread isn't active,terminate it and create it again in SystemCtrl::MonitorThread()");
				::TerminateThread(m_hRmtListenTread,0);
				::CloseHandle(m_hRmtListenTread);
				m_hRmtListenTread = NULL;
				CreatRmtListenThread();
			}
		}

		exitCode = 0;
		if(m_hTimerTread == NULL)
		{
			this->CreateTimerThread();
		}
		else
		{
			GetExitCodeThread(m_hTimerTread,&exitCode);
			if(exitCode != STILL_ACTIVE)
			{
				mylog.WriteLog2File(LogControl::LWARNING,"The timer thread isn't active,terminate it and create it again in SystemCtrl::MonitorThread()");
				::TerminateThread(m_hTimerTread,0);
				::CloseHandle(m_hTimerTread);
				m_hTimerTread = NULL;
				CreateTimerThread();
			}
		}

		exitCode = 0;
		if(m_hUdpDataTread == NULL)
		{
			this->CreateUdpDataThread();
		}
		else
		{
			GetExitCodeThread(m_hUdpDataTread,&exitCode);
			if(exitCode != STILL_ACTIVE)
			{
				mylog.WriteLog2File(LogControl::LWARNING,"The UDP thread isn't active,terminate it and create it again in SystemCtrl::MonitorThread()");
				::TerminateThread(m_hUdpDataTread,0);
				::CloseHandle(m_hUdpDataTread);
				m_hUdpDataTread = NULL;
				CreateUdpDataThread();
			}
		}

		exitCode = 0;
		if(m_hRltDataTread == NULL)
		{
			m_hRltDataTread = CreateThread(NULL,0,&GetRltDataThread,this,0,NULL);
		}
		else
		{
			GetExitCodeThread(m_hRltDataTread,&exitCode);
			if(exitCode != STILL_ACTIVE)
			{
				mylog.WriteLog2File(LogControl::LWARNING,"The real-time thread isn't active,terminate it and create it again in SystemCtrl::MonitorThread()");
				::TerminateThread(m_hRltDataTread,0);
				::CloseHandle(m_hRltDataTread);
				m_hRltDataTread = NULL;
				m_hRltDataTread = CreateThread(NULL,0,&GetRltDataThread,this,0,NULL);
			}
		}
	}
#endif
}

void SystemCtrl::CreateUdpDataThread( void )
{
	m_hUdpDataTread = CreateThread(NULL,0,&GetUdpDataThread,this,0,NULL);
}

DWORD WINAPI RmtListenThreadF(void *pParm)
{
	Linker *pLinker = NULL;
	void   *pParam = NULL;  //通讯需要的参数
	struct SocketParm strLinkParm;	//设置tcp/ip通讯所需的参数
	memset(&strLinkParm,0,sizeof(strLinkParm));
	struct ComParm comParm;			//设置串口通讯参数
	memset(&comParm,0,sizeof(comParm));
	struct ModemParm modemParm;		//设置moden通讯参数
	memset(&modemParm,0,sizeof(modemParm));

	switch(SystemCfger::GetInstance().rmtLinkType)
	{
	case CFG_SOCKET_LINK:	//使用tcp/ip通讯
		pLinker = new SocketLinker();

		//设置tcp/ip通讯所需的参数
		pParam = (void*)&strLinkParm;
		memcpy(&strLinkParm.loacalAddr[0],
			&(SystemCfger::GetInstance().addrForRemote[0]),
			sizeof(SystemCfger::GetInstance().addrForRemote)); //设置邦定的监听地址
		strLinkParm.loacalTcpPort = SystemCfger::GetInstance().tcpPortForRemote; //设置邦定的监听端口
		strLinkParm.loacalUdpPort = SystemCfger::GetInstance().udpPortForRemote; //设置udp端口
		break;

	case CFG_COM_LINK:	//使用串口通讯
		pLinker = new ComLinker();

		//设置串口通讯参数
		pParam = (void*)&comParm;
		memcpy(&comParm.comName[0],&(SystemCfger::GetInstance().comNameForRmt[0]),
			sizeof(comParm.comName));
		comParm.baudrate = SystemCfger::GetInstance().baud;
		comParm.data = SystemCfger::GetInstance().dataBit;
		comParm.parity = SystemCfger::GetInstance().parity;
		comParm.stop = SystemCfger::GetInstance().stopBit;

		break;

	case CFG_MODEN_LINK:       //使用moden通讯
		pLinker = new ModemLinker();

		//设置moden通讯参数
		pParam = (void*)&modemParm;
		memcpy(&modemParm.comName[0],&(SystemCfger::GetInstance().modemComName[0]),
			sizeof(modemParm.comName));
		modemParm.baudrate = SystemCfger::GetInstance().modemBaud;
		modemParm.data = SystemCfger::GetInstance().modemDataBit;
		modemParm.parity = SystemCfger::GetInstance().modemParity;
		modemParm.stop = SystemCfger::GetInstance().modemStopBit;

		memcpy(modemParm.phoneNum,SystemCfger::GetInstance().phoneNumForRmt,sizeof(modemParm.phoneNum));
		break;

	default:
		mylog.WriteLog2File(LogControl::LERROR,"Unkown the link type for remote listen.");
		return -1;

	}

	pLinker->Init(pParam,Linker::LK_REMOTE);	//初始化

	pLinker->Listen();	//监听

	if(SystemCfger::GetInstance().rmtLinkType == CFG_SOCKET_LINK)
	{
		delete pLinker;
		pLinker = NULL;
	}

	return 0;
}

DWORD WINAPI ADUListenThreadF(void *pParm)
{
	Linker *pLinker = NULL;
	void   *pParam = NULL;  //通讯需要的参数

	struct SocketParm strLinkParm;	//设置tcp/ip通讯所需的参数
	memset(&strLinkParm,0,sizeof(strLinkParm));
	pLinker = new SocketLinker();

	//设置tcp/ip通讯所需的参数
	pParam = (void*)&strLinkParm;

	memcpy(&strLinkParm.loacalAddr[0],
		&(SystemCfger::GetInstance().addrForADU[0]),
		sizeof(SystemCfger::GetInstance().addrForADU)); //设置邦定的监听地址
	strLinkParm.loacalTcpPort = SystemCfger::GetInstance().tcpPortForADU; //设置邦定的监听端口

	pLinker->Init(pParam,Linker::LK_ASDU);  //初始化
	pLinker->Listen(DisposeADUConn);        //监听

	delete pLinker;
	pLinker = NULL;

	return 0;
}

//处理adu的连接

DWORD WINAPI DisposeADUConn(void *pParm)
{
	mylog.WriteLog2File(LogControl::LINFO, "Enter DWORD WINAPI DisposeADUConn() in SystemCtrl.cpp");

	SocketLinker *pSocketLinker = (SocketLinker *)pParm;
	int iReceiveLen = 0;

	char szLogInf[256] = {0};
	PACKINFO msg;
	memset(&msg,0,sizeof(msg));

	ORIGPOINTDATAHEAD OrigPointDataHead;
	memset(&OrigPointDataHead,0,sizeof(OrigPointDataHead));

	//分析DAU编号
	char szIPAddr[16] = {0};
	::strcpy(szIPAddr,pSocketLinker->GetConnectAddr().c_str());
	//PDAUINFO pDauInfo = SystemCfger::GetInstance().GetDAUInfo(szIPAddr,1);
	PDAUINFO pDauInfo = SystemCfger::GetInstance().GetDAUInfo(szIPAddr);
	if(pDauInfo == NULL)
	{
		sprintf(szLogInf,"The client addr is %s,no find any DAU with this ip addr in DisposeADUConn().",szIPAddr);
		mylog.WriteLog2File(LogControl::LWARNING, szLogInf);

		pSocketLinker->Close();
		delete pSocketLinker;
		pSocketLinker = NULL;
		return 0;
	}

	while(1)
	{
		//取消息头
		iReceiveLen = pSocketLinker->ReceiveData((char *)&msg,sizeof(msg),0);
		if(sizeof(msg) != iReceiveLen ||
			msg.flag[0] != 'S' || msg.flag[1] != 'H' ||
			msg.msgType <= 0 || msg.msgType > 5 )
		{
			mylog.WriteLog2File(LogControl::LWARNING, "The message head is error in DisposeADUConn()");
			break;
		}

		//接收数据头
		iReceiveLen = pSocketLinker->ReceiveData((char *)&OrigPointDataHead,sizeof(OrigPointDataHead),0);

		if(sizeof(OrigPointDataHead) != iReceiveLen )
		{
			mylog.WriteLog2File(LogControl::LWARNING, "Receive point head is error in DisposeADUConn()");
			break;
		}

		//计算数据长度
		unsigned int iDataLen = (OrigPointDataHead.ucPerCyclePoint * OrigPointDataHead.wPerSampleSize)
			* OrigPointDataHead.ucSendCountCycle;
		char *pData = NULL;
		if(iDataLen > 0)
		{
			pData = new char[iDataLen+1];
			if(pData == NULL)
				break;

			memset(pData,0,iDataLen+1);

			//接收数据
			int iReadLen = pSocketLinker->ReceiveData(pData,iDataLen,0);
			if(iReadLen != iDataLen)
			{
				mylog.WriteLog2File(LogControl::LWARNING, "Receive point data error in DisposeADUConn()");
				delete pData;
				pData = NULL;
				pSocketLinker->Close();
				break;
			}
		}

		switch(msg.msgType)
		{
		case 1://发送实时显示数据

			WriteSampleData2Mem(&OrigPointDataHead,pData,iDataLen,pDauInfo->BoardId,CMD_READ_RLT_DATA);
			break;
		case 2://发送零漂刻度数据

			WriteSampleData2Mem(&OrigPointDataHead,pData,iDataLen,pDauInfo->BoardId,CMD_READ_ZEROOFFSET);
			break;
		case 3://发送实时显示、零漂刻度数据

			WriteSampleData2Mem(&OrigPointDataHead,pData,iDataLen,pDauInfo->BoardId,CMD_READ_RLT_DATA);
			WriteSampleData2Mem(&OrigPointDataHead,pData,iDataLen,pDauInfo->BoardId,CMD_READ_ZEROOFFSET);
			break;
		case 5://发送发电机实验数据
			WriteSampleData2Mem(&OrigPointDataHead,pData,iDataLen,pDauInfo->BoardId,CMD_STARTGNEXPERIMENT);
		default:
			break;
		}

		delete []pData;
		pData = NULL;

		if(msg.msgType == 2)//只要零漂刻度数据，则关闭连接
		{
			break;
		}
	}//end while(1)

	pSocketLinker->Close();
	delete pSocketLinker;
	pSocketLinker = NULL;
	return 0;
}

//将采样数据写入共享内存中
bool WriteSampleData2Mem(PORIGPOINTDATAHEAD pOriPoint,char *pData,unsigned int iDataLen,unsigned int devId,unsigned char funCode)
{
	unsigned int iStructLen = sizeof(DWORD) + sizeof(SHTimeStructDAU); //by luozy 2008.11.19***SHFR_A_1采样点的序号和时间


	if(pOriPoint == NULL || pData == NULL)
		return false;

	CDeviceIniMgr *pDevImiMgr = &(SystemCfger::GetInstance().m_IniMgr);
	CDeviceIni *pDevIni = pDevImiMgr->GetDevice(devId);

	if(pDevIni == NULL)//配置文件不存在

	{
		mylog.WriteLog2File(LogControl::LWARNING, "No find ini file in WriteSampleData2Mem()");
		return false;
	}

	char *pOrgDataHead = pData;

	vector<SH_ANCH_PARM> anaChList = pDevIni->GetChannelInfo();
	std::vector<SH_BINCH_PARM> swList = pDevIni->GetSwitchInfo();

	//将收到的数据放到结果集中
	struct Command cmd;

	POINTDATAHEAD point;
	memset(&point,0,sizeof(point));

	///////////by luozy 2009.1.19///发电机实验数据抽通道处理
	if (funCode == CMD_STARTGNEXPERIMENT)
	{
		point.ChannelNumber = pOriPoint->ucTotalAn;
		point.SwitchNumber = pOriPoint->ucTotalSw;
	}
	else
	{
		point.ChannelNumber = anaChList.size();
		point.SwitchNumber = (swList.size()+15)/16;
	}
	point.Chan[0] = 0xffffffff;
	point.Chan[1] = 0xffffffff;
	point.Chan[2] = 0xffffffff;
	point.point = pOriPoint->ucSendCountCycle * pOriPoint->ucPerCyclePoint;
	point.samplrate = pOriPoint->ucPerCyclePoint*50/1000;
	unsigned int iNewDataLen = (point.ChannelNumber + point.SwitchNumber) * 2*point.point;
	//////////////////////////////////////////////////////

	cmd.from = CD_ASDU;
	cmd.msgId = -1;
	cmd.destDevId = devId;

	PCMDRESULT pResult = new CMDRESULT;
	cmd.funCode = funCode;
	cmd.resultNum = 1 ;
	pResult->resultType = CDRT_POINTDATA;
	pResult->resultLen = iNewDataLen + sizeof(POINTDATAHEAD);
	pResult->pData = new char[iNewDataLen + sizeof(POINTDATAHEAD) +1];
	memset(pResult->pData,0,pResult->resultLen+1);

	memcpy(pResult->pData,&point,sizeof(point));
	char *pDestData = &pResult->pData[sizeof(POINTDATAHEAD)];

	short maxValue = 0;
	float fMaxMultiplier = 0.00;
	short nValue = 0;
	SH_ANCH_PARM AnaCh;
	memset(&AnaCh,0,sizeof(AnaCh));
	//*********************************BY LUOZY 2008.11.18******************************************
	//如果是SHFR_A_1的实时数据，则按如下过程拷贝数据
	if (pOriPoint->wReserve == 4)
	{
		//按发送通道数取开关量 2009.1.19
		char *pSwitchData = &pData[(pOriPoint->ucTotalAn*2+iStructLen)*point.point+iStructLen];
		for(unsigned int i=0;i<(pOriPoint->ucSendCountCycle*pOriPoint->ucPerCyclePoint);i++) //采样点总数
		{
			//定值通道数不能大于接收的通道数
			if (pOriPoint->ucTotalAn < point.ChannelNumber)
			{
				mylog.WriteLog2File(LogControl::LDETAIL, "ChannelNumber Error!");
			}
			//由于SHFR_A_1的实时数据已经是顺序排列的，所以只要去掉不需要的字节，然后拷贝
			memcpy(pDestData, &pData[iStructLen], point.ChannelNumber*2);
			memcpy(&pDestData[point.ChannelNumber*2], pSwitchData, point.SwitchNumber*2);
			pDestData += point.ChannelNumber*2 + point.SwitchNumber*2;
			pData += pOriPoint->wPerSampleSize;
		}//处理完所有采样点
	}
	else if (funCode == CMD_STARTGNEXPERIMENT)
	{
		for(unsigned int i=0;i<(pOriPoint->ucSendCountCycle*pOriPoint->ucPerCyclePoint);i++) //采样点总数
		{
			//拷贝发电机实验数据
			memcpy(pDestData, pData, point.ChannelNumber*2);
			pDestData += point.ChannelNumber*2;
			pData += pOriPoint->wPerSampleSize;
		}//处理完所有采样点
	}
	else
	{
		for(unsigned int i=0;i<(pOriPoint->ucSendCountCycle*pOriPoint->ucPerCyclePoint);i++) //采样点总数
		{
			//拷贝数字通道数据
			memcpy(&pDestData[point.ChannelNumber*2],pData,point.SwitchNumber*2);
			char *pChannelData = &pData[(pOriPoint->ucTotalSw/16)*2];//模拟通道数据地址

			int iHasCpyAnNum = 0;	//已经拷贝的模拟通道数

			//拷贝模拟通道数据
			int iBoardNum = pOriPoint->ucTotalAn/32; //板数
			for(int m=0;m<iBoardNum;m++)
			{
				char *pTmpData = &pChannelData[m*2];//板第一个通道数据地址
				for(int n=0;n<32;n++)
				{
					//只取定值中设定的通道的数据
					if(iHasCpyAnNum >= point.ChannelNumber)
					{
						break;
					}

					memcpy(pDestData,pTmpData,2);//拷贝一个通道的数据

					iHasCpyAnNum++;
					pDestData += 2;
					pTmpData += iBoardNum *2;//跨过其它板找到本板的下一个通道数据地址
				}//本板32个通道处理完后再处理下一个板
			}//处理完所有板

			pDestData += point.SwitchNumber*2;
			pData += pOriPoint->wPerSampleSize;
		}//处理完所有采样点
	}
	/****************************END****************************************************************/

	cmd.resultList.push_back(pResult);

	mylog.WriteLog2File(LogControl::LDETAIL, "Begin write RealTime/ZeroOfset data into memery");

	MemMapMgr memMgr;

	memMgr.WriteTaskResultToMem(cmd);

	return true;
}


DWORD WINAPI TimerThreadF(void *pParm)
{
	Timer timer;
	timer.Run();

	return 0;
}

//接收UDP数据报的线程
DWORD WINAPI GetUdpDataThread(void *pParm)
{
	mylog.WriteLog2File(LogControl::LINFO,"Enter GetUdpDataThread()");

	SystemCtrl *pSysCtrl = (SystemCtrl*)pParm;

	Linker *pLinker = NULL;
	void   *pParam = NULL;  //通讯需要的参数

	SocketParm strLinkParm;	//设置tcp/ip通讯所需的参数

	memset(&strLinkParm,0,sizeof(strLinkParm));

	if(SystemCfger::GetInstance().ADULinkType == CFG_SOCKET_LINK)
	{
		pLinker = new SocketLinker();

		//设置tcp/ip通讯所需的参数
		pParam = (void*)&strLinkParm;

		memcpy(&strLinkParm.loacalAddr[0],
			&(SystemCfger::GetInstance().addrForADU[0]),
			sizeof(SystemCfger::GetInstance().addrForADU)); //设置邦定的监听地址
		strLinkParm.loacalTcpPort = SystemCfger::GetInstance().tcpPortForADU; //设置邦定的监听端口
		strLinkParm.loacalUdpPort = SystemCfger::GetInstance().udpPortForADU ; //设置udp端口
		pLinker->Init(pParam,Linker::LK_ASDU);	//初始化

		if(!pLinker->BindUdpLink())
		{
			delete pLinker;
			pLinker = NULL;
			return 0;
		}

		MemMapMgr memMgr;
		while(1)
		{
			struct Command cmd;
			memset(&cmd,0,sizeof(cmd));
			if(pLinker->GetUdpData(cmd))
			{
				//执行结果写入内存中
				memMgr.WriteTaskResultToMem(cmd);
			}

			if(pSysCtrl != NULL && pSysCtrl->m_isExit)
			{
				break;
			}
		}

		delete pLinker;
		pLinker = NULL;
	}

	return 0;
}

//接收UDP数据报的线程
DWORD WINAPI GetRltDataThread(void *pParm)
{
	mylog.WriteLog2File(LogControl::LINFO,"Enter GetRltDataThread()");

	SystemCtrl *pSysCtrl = (SystemCtrl*)pParm;

	Linker *pLinker = NULL;
	void   *pParam = NULL;	//通讯需要的参数

	SocketParm strLinkParm;	//设置tcp/ip通讯所需的参数

	memset(&strLinkParm,0,sizeof(strLinkParm));

	unsigned int iStructLen = sizeof(DWORD) + sizeof(SHTimeStructDAU);  //by luozy 2008.11.19

	DWORD dwBeginTime =0,dwEndTime =0,dwBeginErrTime =0;
	DWORD dwWarnCount = 0;
	dwBeginErrTime = GetTickCount();
	bool bIsFirstErrPack = true;

	if(SystemCfger::GetInstance().ADULinkType == CFG_SOCKET_LINK)
	{
		pLinker = new SocketLinker();

		//设置tcp/ip通讯所需的参数

		pParam = (void*)&strLinkParm;

		memcpy(&strLinkParm.loacalAddr[0],
			&(SystemCfger::GetInstance().addrForADU[0]),
			sizeof(SystemCfger::GetInstance().addrForADU)); //设置邦定的监听地址
		strLinkParm.loacalTcpPort = SystemCfger::GetInstance().tcpPortForADU; //设置邦定的监听端口

		strLinkParm.loacalUdpPort = SystemCfger::GetInstance().udpPortForRltData ; //设置udp端口

		pLinker->Init(pParam,Linker::LK_ASDU);    //初始化

		if(!pLinker->BindUdpLink())
		{
			delete pLinker;
			pLinker = NULL;
			::Sleep(1000);
			return 0;
		}

		bool bHasReadFirstPack = false;	//是否已经接收第一个包
		int iEveryDataPackLen = 0;		//每个数据包的固定长度
		unsigned short maxFrameNo = 0;	//每包最大帧号

		unsigned short preFrameNo = 0;	//前帧的序号

		int iPacketLen=0;	//收到包的长度
		char szLogInfo[250]={0};
		int iReceivTotalLen = 0;

#ifndef _DEBUG
		CWatchModule	WatchDog;	//看门狗

		int		iWatchPoint;		//看门狗看点

		iWatchPoint = WatchDog.addMonitorPoint(10);//10秒超时

#endif

		struct sockaddr clientAddr;
		memset(&clientAddr,0,sizeof(struct sockaddr));

		char szReadData[64*1024] ={0};
		MemMapMgr memMgr;
		while(1)
		{
#ifndef _DEBUG
			WatchDog.kickMonitorPoint(iWatchPoint);
#endif

			struct Command cmd;

			if(!bHasReadFirstPack)
			{
				iPacketLen = pLinker->ReceiveDataFrom(szReadData,sizeof(szReadData),500,clientAddr);
			}
			bHasReadFirstPack = false;

			if(iPacketLen >= sizeof(PACKINFO) + sizeof(ORIGPOINTDATAHEAD) )
			{
				//mylog.WriteLog2File(LogControl::LDEBUG,"receive rlt data in GetRltDataThread()");

				dwBeginTime = GetTickCount();

				//sprintf(szLogInfo,"Get the first udp data len: iPacketLen=%x",iPacketLen);
				//mylog.WriteLog2File(LogControl::LDEBUG,szLogInfo);

				//分析包头信息
				PPACKINFO pPackInf = (PPACKINFO)szReadData;
				if(pPackInf->flag[0] != 'S' || pPackInf->flag[1] != 'H' || (pPackInf->msgType != 4 && pPackInf->msgType !=1))
				{
					if(dwWarnCount%20 == 0)
					{
						struct sockaddr_in *newClientAddrin = (struct sockaddr_in*)&clientAddr;
						sprintf(szLogInfo,"Get an unkown type UDP pack from %s in GetRltDataThread()",inet_ntoa(newClientAddrin->sin_addr));
						mylog.WriteLog2File(LogControl::LWARNING,szLogInfo);
					}
					dwWarnCount++;
					::Sleep(40);

					if(bIsFirstErrPack)
					{
						dwBeginErrTime = GetTickCount();
						bIsFirstErrPack = false;
					}

					dwEndTime = GetTickCount();
					if( dwEndTime - dwBeginErrTime > 8000)
					{
						//长时间接收到错误数据，关闭线程，重新创建
#ifndef _DEBUG
						WatchDog.delMonitorPoint(iWatchPoint);
#endif
						mylog.WriteLog2File(LogControl::LWARNING,"Too many unkown type UDP data in GetRltDataThread(),so close the thread and rebuild it");
						break;
					}
					else
					{
						continue;
					}
				}
				bIsFirstErrPack = true;

				//第一次接收必须是帧号为1的包
				if(pPackInf->frameNo != 1)
				{
					mylog.WriteLog2File(LogControl::LDETAIL,"It is not the first UDP pack in GetRltDataThread()");
					continue;
				}
				preFrameNo = pPackInf->frameNo;

				dwEndTime = GetTickCount();
				//printf("Get packet no = %d using time=%d \r\n",pPackInf->frameNo,(dwEndTime - dwBeginTime));

				//分析客户端的IP地址，从而得到发送UDP信息的DAU编号
				char ipAddr[16]={0};
				struct sockaddr_in *newAddrin = (struct sockaddr_in*)&clientAddr;
				memcpy(ipAddr,inet_ntoa(newAddrin->sin_addr),strlen(inet_ntoa(newAddrin->sin_addr)));

				PDAUINFO pDauInfo = SystemCfger::GetInstance().GetDAUInfo(ipAddr);
				//#endif

				if(pDauInfo == NULL)
				{
					if(dwWarnCount%20 == 0)
					{
						sprintf(szLogInfo,"Get real-time data from %s,but no find the device's config",ipAddr);
						mylog.WriteLog2File(LogControl::LWARNING,szLogInfo);
					}
					dwWarnCount++;
					continue;
				}

				cmd.destDevId = pDauInfo->BoardId;
				cmd.from = CD_ASDU;
				cmd.msgId = -1;

				//********************************************by luozy 2008.11.18********************************
				unsigned int iDataLen;
				int iReReadLen;
				unsigned int dwPerSampleSize; //每个采样点占的字节数
				unsigned int iDataHeadSize = 0; //数据头的大小，如果是2000D的头，则是ORIGPOINTDATAHEAD的大小。如果是SHFR_A_1的头，则是RTDATAHEAD的大小

				PRTDATAHEAD pRtDataHead;
				PORIGPOINTDATAHEAD pPointDataHead;
				if (pPackInf->msgType == 4)
				{
					pRtDataHead = (PRTDATAHEAD)(&szReadData[sizeof(PACKINFO)]);

					//转换成2000D实时数据的头
					pPointDataHead = new ORIGPOINTDATAHEAD;
					pPointDataHead->ucSendCountCycle = pRtDataHead->dwSendCountCycle;
					pPointDataHead->ucPerCyclePoint = pRtDataHead->dwPerCyclePoint;
					pPointDataHead->ucTotalAn = pRtDataHead->dwTotalAn;
					pPointDataHead->ucTotalSw = pRtDataHead->dwTotalSw;
					pPointDataHead->wPerSampleSize = pRtDataHead->dwTotalAn * 2 + iStructLen;//这里只是模拟量采样点的大小。

					pPointDataHead->wReserve = 4;//原来的备用字节，现在用做区分是2000D的原始头还是由SHFR_A_1转换成2000D的头。0为原始头，4为转换后的头

					pRtDataHead = (PRTDATAHEAD)(&szReadData[sizeof(PACKINFO)]);
					dwPerSampleSize = pPointDataHead->wPerSampleSize;//模拟量采样点的大小

					iDataLen = (pRtDataHead->dwPerCyclePoint * pRtDataHead->dwSendCountCycle)
						* dwPerSampleSize + iStructLen + (pRtDataHead->dwTotalSw+15)/16*2;

					if(iDataLen > 1024*1024*5 || pRtDataHead->dwSendCountCycle==0 || pRtDataHead->dwPerCyclePoint==0
						|| pRtDataHead->dwTotalSw==0 || pRtDataHead->dwTotalSw > 192
						|| pRtDataHead->dwTotalAn==0 || pRtDataHead->dwTotalAn > 96 )
					{
						if(dwWarnCount%20 == 0)
							mylog.WriteLog2File(LogControl::LWARNING,"Data head has error");
						dwWarnCount++;
						continue;
					}
					iDataHeadSize = sizeof(RTDATAHEAD);
				}/*******************end*************************************************************************/
				else
				{
					pPointDataHead = (PORIGPOINTDATAHEAD)(&szReadData[sizeof(PACKINFO)]);

					iDataLen = (pPointDataHead->ucPerCyclePoint * pPointDataHead->wPerSampleSize)
						* pPointDataHead->ucSendCountCycle;

					//数据头中的信息不合理，则不处理这些数据
					if(iDataLen > 1024*1024*5 || pPointDataHead->ucSendCountCycle==0 || pPointDataHead->ucPerCyclePoint==0
						|| pPointDataHead->wPerSampleSize==0 || pPointDataHead->ucTotalSw==0 || pPointDataHead->ucTotalSw > 192
						|| pPointDataHead->ucTotalAn==0 || pPointDataHead->ucTotalAn > 96 )
					{
						if(dwWarnCount%20 == 0)
							mylog.WriteLog2File(LogControl::LWARNING,"Data head has error");
						dwWarnCount++;
						continue;
					}
					iDataHeadSize = sizeof(ORIGPOINTDATAHEAD);
				}

				//******************by luozy 2008.11.19
				iReReadLen = iDataLen + iDataHeadSize - (iPacketLen - sizeof(PACKINFO));//还需读入的长度

				char *pData = new char[iDataLen + iDataHeadSize +1];
				/****************************end*******/
				if(pData == NULL)
				{
					sprintf(szLogInfo,"no enough memery when new pData in Protocol60_103::GetRltData,want memery size:%x",iDataLen + sizeof(POINTDATAHEAD) +1);
					mylog.WriteLog2File(LogControl::LWARNING,szLogInfo );
					continue;
				}

				//拷贝原始采样点的头信息
				memset(pData,0,iDataLen + iDataHeadSize +1); //by luozy 2008.11.19
				memcpy(pData,&szReadData[sizeof(PACKINFO)],iPacketLen - sizeof(PACKINFO) );
				char *pResultData = pData;
				pResultData += ( iPacketLen - sizeof(PACKINFO) );

				iReceivTotalLen = 0;
				iReceivTotalLen += ( iPacketLen - sizeof(PACKINFO) );

				int count=0;

				//开始读采样点数据
				while(iReReadLen > 0)
				{
					DWORD dwBeginGetPackTime = GetTickCount();

					iPacketLen = pLinker->ReceiveDataFrom(szReadData,sizeof(szReadData),100,clientAddr);

					if(iPacketLen == 0)
					{
						dwEndTime = GetTickCount();
						if(dwEndTime - dwBeginTime > 4000)
						{
							//过长时间还没接收完1秒的数据，则丢弃此秒的数据

							delete []pData;
							pData = NULL;
							mylog.WriteLog2File(LogControl::LWARNING,"in GetRltDataThread(),Wait for one second data too long time,so stop waitting");
							break;
						}
						else
						{
							continue;
						}
					}
					else if(iPacketLen <= sizeof(PACKINFO) || iPacketLen < 0 )
					{
						delete []pData;
						pData = NULL;
						mylog.WriteLog2File(LogControl::LWARNING,"The UDP pack lenght too short in GetRltDataThread()");
						break;
					}

					DWORD dwEndGetPackTime = GetTickCount();

					//分析包头信息
					pPackInf = (PPACKINFO)szReadData;
					if(pPackInf->flag[0] != 'S' || pPackInf->flag[1] != 'H' || (pPackInf->msgType != 1 && pPackInf->msgType != 4))// by luozy 2008.11.19
					{
						printf("Flag:%c%c,MsgType:%d\r\n",pPackInf->flag[0],pPackInf->flag[1],pPackInf->msgType);
						delete []pData;
						pData = NULL;
						if(dwWarnCount%20 == 0)
						{
							struct sockaddr_in *newClientAddrin = (struct sockaddr_in*)&clientAddr;
							sprintf(szLogInfo,"Get an unkown type UDP pack from %s in GetRltDataThread()",inet_ntoa(newClientAddrin->sin_addr));
							mylog.WriteLog2File(LogControl::LWARNING,szLogInfo);
						}
						dwWarnCount = 0;
						::Sleep(80);
						break;
					}

					if( iEveryDataPackLen == 0)
					{
						//计算每包数据长度
						iEveryDataPackLen = iPacketLen - sizeof(PACKINFO);
					}

					if(maxFrameNo < pPackInf->frameNo)
					{
						maxFrameNo = pPackInf->frameNo;
					}

					//计算丢掉的包数
					short lostPack = 0;
					if(pPackInf->frameNo == 0) //last frame
					{
						lostPack = (short)maxFrameNo - (short)preFrameNo;
					}
					else if(pPackInf->frameNo == 1)//下一秒的第一帧数据

					{
						bHasReadFirstPack = true;
						lostPack = (short)maxFrameNo - (short)preFrameNo +1;
					}
					else
					{
						lostPack = (short)pPackInf->frameNo - (short)preFrameNo -1;
					}

					if( lostPack != 0)
					{
						//有丢帧，则丢弃全部数据

						delete []pData;
						pData = NULL;

						sprintf(szLogInfo,"Lost some UDP pack in GetRltDataThread(),prePack no is:%d,now pack no is:%d",
							preFrameNo,pPackInf->frameNo);
						mylog.WriteLog2File(LogControl::LWARNING,szLogInfo);
						//printf("%s \r\n",szLogInfo);

						break;
					}

					//dwBeginGetPackTime = GetTickCount();
					preFrameNo = pPackInf->frameNo;
					int iReadLen = iPacketLen - sizeof(PACKINFO);
					iReadLen = (iReadLen <iReReadLen)? iReadLen:iReReadLen;
					memcpy(pResultData,&szReadData[sizeof(PACKINFO)],iReadLen);
					pResultData += iReadLen;
					iReReadLen -= iReadLen;
					iReceivTotalLen += iReadLen;

					dwEndGetPackTime = GetTickCount();
					//printf("one while use time =%d \r\n",dwEndGetPackTime - dwBeginGetPackTime);

					//已经是最后一帧

					if(pPackInf->frameNo == 0)
					{
						break;
					}
				}//end while(iReReadLen > 0)

				//将收到的数据写入共享内存中
				if(pData != NULL)
				{
					if( iReceivTotalLen  < (iDataLen + iDataHeadSize))
					{
						sprintf(szLogInfo,"realtime data is too few,receiv total len:%d,need receiv len:%d,PerCyclePoint:%d,PerSampleSize:%d,SendCountCycle:%d",
							iReceivTotalLen,iDataLen + sizeof(ORIGPOINTDATAHEAD),
							pPointDataHead->ucPerCyclePoint,pPointDataHead->wPerSampleSize,pPointDataHead->ucSendCountCycle);
						mylog.WriteLog2File(LogControl::LDEBUG,szLogInfo);
					}
					//**************************by luozy 2008.11.19*****************************
					PORIGPOINTDATAHEAD pOriPoint;
					if (pPackInf->msgType == 4)
					{
						pOriPoint = pPointDataHead;
					}
					else
					{
						pOriPoint = (PORIGPOINTDATAHEAD)pData;
					}
					char *pPointData = &pData[iDataHeadSize];
					/************************************end*************************************/
					dwEndTime = GetTickCount();

					WriteSampleData2Mem(pOriPoint,pPointData,iDataLen,pDauInfo->DAUId,CMD_READ_RLT_DATA);

					//by luozy 2008.11.19
					if (pPackInf->msgType == 4)
					{
						delete pPointDataHead;
						pPointDataHead = NULL;
					}
					////////////////////////////
					delete []pData;
					pData = NULL;
				}

			}//end (iPacketLen >= sizeof(PACKINFO) + sizeof(ORIGPOINTDATAHEAD) )

			if(pSysCtrl != NULL && pSysCtrl->m_isExit)
			{
#ifndef _DEBUG
				WatchDog.delMonitorPoint(iWatchPoint);
#endif

				break;
			}
		}//end while(1)

		if(pLinker != NULL)
		{
			pLinker->Close();
			delete pLinker;
			pLinker = NULL;
		}
	}

	return 0;
}


bool LoadDBDll()
{
	//使用数据库类型
	char DBType[30]={0};
	int iDBType = 1;//数据库类型,0：不使用数据库； 1：sql server； 2：Access数据库

	GetPrivateProfileString("System","DBType","SQL_SERVER",DBType,sizeof(DBType)-1,"./Communication.ini");

	if(strcmp(DBType,"NONE") == 0)
	{
		iDBType = 0;
	}
	else if(strcmp(DBType,"SQL_SERVER") == 0)
	{
		iDBType = 1;
	}
	else if(strcmp(DBType,"ACCESS") == 0)
	{
		iDBType = 2;
	}

	if(iDBType == 1 || iDBType == 2)//使用sql server或Access数据库
	{
		if(g_hDBInstance != NULL)
		{
			return true;
		}

		g_hDBInstance = LoadLibrary("DBAdapterDll.dll");
		if(g_hDBInstance == NULL)
		{
			printf("******************\r\n");
			printf("Load the dll : DBAdapterDll.dll Error.\r\n");
			printf("******************\r\n");
			mylog.WriteLog2File(LogControl::LERROR,"Load the dll : DBAdapterDll.dll Error");
			return false;
		}

		DbInit = (DBINIT)GetProcAddress(g_hDBInstance,"dbInit");
		AccessDBInit = (ACCESSDBINIT)GetProcAddress(g_hDBInstance,"accessDBInit");
		AddLog = (ADDLOG)GetProcAddress(g_hDBInstance,"addLog");
		GetAllDau = (GETALLDAU)GetProcAddress(g_hDBInstance,"getAllDAU");
		GetOneDau = (GETONEDAU)GetProcAddress(g_hDBInstance,"getOneDAU");
		GetAllCfg = (GETALLCFG)GetProcAddress(g_hDBInstance,"getAllCfg");
		releaseMemery = (RELEASEMEMERY)GetProcAddress(g_hDBInstance,"releaseMemery");
		getToBeSendRpt2Rmt = (GETTOBESENDRPT2RMT)GetProcAddress(g_hDBInstance,"getToBeSendRpt2Rmt");
		addSendFaultInf = (ADDSENDFAULTINF)GetProcAddress(g_hDBInstance,"addSendFaultInf");
		getRptBySql = (GETRPTBYSQL)GetProcAddress(g_hDBInstance,"getRptBySql");
		getRptById = (GETRPTBYID)GetProcAddress(g_hDBInstance,"getRptById");
		getAllTimerCallInf = (GETALLTIMERCALLINF)GetProcAddress(g_hDBInstance,"getAllTimerCallInf");
		getMaxFileNo = (GETMAXFILENO)GetProcAddress(g_hDBInstance,"getMaxFileNo");
		addCallFileInf = (ADDCALLFILEINF)GetProcAddress(g_hDBInstance,"addCallFileInf");
		getNoCallFileByType = (GETNOCALLFILEBYTYPE)GetProcAddress(g_hDBInstance,"getNoCallFileByType");
		updateCallFileInfById = (UPDATECALLFILEINFBYID)GetProcAddress(g_hDBInstance,"updateCallFileInfById");
		getMarkedEvtBySql = (GETMARKEDEVTBYSQL)GetProcAddress(g_hDBInstance,"getMarkedEvtBySql");

		char dbServer[50]={0};
		char dbName[20]={0};
		char dbUser[20]={0};
		char dbPassword[20]={0};
		char szVal[256];

		GetPrivateProfileString("DB_SQL_SERVER","Server","",szVal,sizeof(szVal)-1,"./Communication.ini");
		SHDecrypt(szVal,dbServer);
		GetPrivateProfileString("DB_SQL_SERVER","DbName","",szVal,sizeof(szVal)-1,"./Communication.ini");
		SHDecrypt(szVal,dbName);
		GetPrivateProfileString("DB_SQL_SERVER","user","",szVal,sizeof(szVal)-1,"./Communication.ini");
		SHDecrypt(szVal,dbUser);
		GetPrivateProfileString("DB_SQL_SERVER","password","",szVal,sizeof(szVal)-1,"./Communication.ini");
		SHDecrypt(szVal,dbPassword);

		char DBFileName[300]={0};
		char AccessUser[30]={0};
		char AccessPsw[30]={0};
		char SecurityFileName[300]={0};
		GetPrivateProfileString("DB_Access","DBFileName","SHDB.mdb",DBFileName,sizeof(DBFileName)-1,"./Communication.ini");
		GetPrivateProfileString("DB_Access","user","sa",AccessUser,sizeof(AccessUser)-1,"./Communication.ini");
		GetPrivateProfileString("DB_Access","password","",AccessPsw,sizeof(AccessPsw)-1,"./Communication.ini");
		GetPrivateProfileString("DB_Access","SecurityFileName","",SecurityFileName,sizeof(SecurityFileName)-1,"./Communication.ini");

		int iRt = -1;
		int iCount = 0;
		while(iRt != 0 && iCount < 3)
		{
			if(iDBType == 1)
			{
				iRt = DbInit(dbServer,dbName,dbUser,dbPassword);
			}
			else
			{
				iRt = AccessDBInit(DBFileName,AccessUser,AccessPsw,SecurityFileName);
			}

			if(iRt != 0)
			{
				printf("Connect database fail,try to connect again...\r\n");
				mylog.WriteLog2File(LogControl::LWARNING,"Connect database fail,try to connect again");
			}

			iCount++;
			::Sleep(1000);
		}

		if(iRt != 0)
		{
			printf("\r\n");
			printf("*****************************************************\r\n");
			printf("**** Connect database fail,the system will exit *****\r\n");
			printf("*****************************************************\r\n");
			mylog.WriteLog2File(LogControl::LERROR,"Connect database error");
			return false;
		}
		else
		{
			return true;
		}
	}
	else//不使用数据库
	{
		DbInit = (DBINIT)CFileDB::DbInit;
		AddLog = (ADDLOG)CFileDB::AddLog;
		GetAllDau = (GETALLDAU)CFileDB::GetAllDau;
		GetAllCfg = (GETALLCFG)CFileDB::GetAllCfg;
		releaseMemery = (RELEASEMEMERY)CFileDB::ReleaseMemery;
		getToBeSendRpt2Rmt = (GETTOBESENDRPT2RMT)CFileDB::GetToBeSendRpt2Rmt;
		addSendFaultInf = (ADDSENDFAULTINF)CFileDB::AddSendFaultInf;
		getRptBySql = (GETRPTBYSQL)CFileDB::GetRptBySql;
		getRptById = (GETRPTBYID)CFileDB::GetRptById;
		getAllTimerCallInf = (GETALLTIMERCALLINF)CFileDB::GetAllTimerCallInf;
		getMaxFileNo = (GETMAXFILENO)CFileDB::GetMaxFileNo;
		addCallFileInf = (ADDCALLFILEINF)CFileDB::AddCallFileInf;
		getNoCallFileByType = (GETNOCALLFILEBYTYPE)CFileDB::GetNoCallFileByType;
		updateCallFileInfById = (UPDATECALLFILEINFBYID)CFileDB::UpdateCallFileInfById;
		getMarkedEvtBySql = (GETMARKEDEVTBYSQL)CFileDB::GetMarkedEvtBySql;

		return true;
	}
}

//ADD BY KH
DWORD WINAPI AutoSendDataThread(void *pParm)  //接收主动上送的数据
{
	printf("AutoSendDataThread Running!\n");

	SocketParm sockParm1;
	memset(&sockParm1, 0,sizeof(SocketParm));
	sockParm1.loacalTcpPort = SystemCfger::GetInstance().tcpPortForADUAutoSend;
	Linker* pListenLinker = NULL;
	pListenLinker = new SocketLinker();
	if(pListenLinker)
	{
		//这里不需要创建协议  故第二个参数设置为非枚举元素
		pListenLinker->Init(&sockParm1, Linker::LK_UNKNOW);
		pListenLinker->Listen(AutoSendDealThread);
		delete pListenLinker;
		pListenLinker = NULL;
	}
	return 0;
}

//线程
DWORD WINAPI  AutoSendDealThread(void *pParm)
{
	printf("处理主动上送数据的线程!\n");
	if( !g_pSystemCtrlObj || !pParm)
		return 0;

	Linker *pLinker=(Linker*)pParm;
	pLinker->AddRef();

	DeviceCtrl *pDeviceCtrlObj = g_pSystemCtrlObj->GetDeviceCtrl();
	if( pDeviceCtrlObj )
	{
		pDeviceCtrlObj->SetAutoSendLinker(pLinker);
	}

	pLinker->SaftDelete();
	pLinker = NULL;
	return 0;
}
