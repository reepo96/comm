// {{{RME classifier 'Logical View::SystemCtrl'

#ifndef SystemCtrl_H
#define SystemCtrl_H

#ifdef PRAGMA
#pragma interface "SystemCtrl.h"
#endif

#include <RTSystem/Communication.h>
#include <DeviceCtrl.h>
class MemMapMgr;
class MsgQueue;
class QueRegister;
class SystemState;

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
#include "public.h"
#include "Task.h"
#include "../WatchDog/WatchModule.h"

/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* 文件名称：SystemCtrl.h
* 文件标识：
* 摘    要：系统控制类
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

#ifdef _MY_UNITTEST
    class SystemCtrl_UT;
#endif

// }}}USR
// }}}RME

class SystemCtrl
{
public:
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR
#ifdef _MY_UNITTEST
	        friend class SystemCtrl_UT;
#endif

	        friend class SysIdleState;
	        friend class SysWorkState;
	        friend class SysExitState;
	        friend class SysInitState;

			friend DWORD WINAPI GetUdpDataThread(void *pParm);
			friend DWORD WINAPI GetRltDataThread(void *pParm);
			
			friend DWORD WINAPI AutoSendDataThread(void *pParm);

			//定义系统事件
			typedef enum SystemEvent{SE_LEAVE,SE_STILL,SE_INIT,SE_EXIT} SYSTEMEVENT;

			HANDLE      m_hExitListenEvent;         //退出监听的事件句柄
			HANDLE      m_hCmdResultMapEvent;       //命令结果共享内存事件句柄

			HANDLE      m_hRmtListenTread;          //监听远程连接的线程句柄
			HANDLE      m_hTimerTread;              //时间线程句柄
			HANDLE      m_hUdpDataTread;              //udp线程句柄
			HANDLE      m_hRltDataTread;              //接收实时数据线程句柄
			
			HANDLE      m_hAutoSendDataTread;         //接收主动上送数据线程句柄 add by kh

			DeviceCtrl * GetDeviceCtrl() { return &m_devCtrl; }

	// }}}USR
	// }}}RME

protected:
	// {{{RME tool 'OT::Cpp' property 'ProtectedDeclarations'
	// {{{USR
	    //LogControl log;

	    SYSTEMEVENT m_nextEvent;   //下一个即将要发生的事件
	    bool    m_isExit;          //是否要退出

	    HANDLE m_hResultMapFile;      //命令执行结果的内存映射文件
	    unsigned char *m_pResultMapData;  //指向结果内存映射数据
		CWatchModule	m_WatchDog;	//看门狗
		int		m_iWatchPoint;		//看门狗看点

	// }}}USR
	// }}}RME

private:
	// {{{RME tool 'OT::Cpp' property 'PrivateDeclarations'
	// {{{USR

	// }}}USR
	// }}}RME
	// {{{RME classAttribute 'm_pTask'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	Task * m_pTask;
	// }}}RME

protected:
	// {{{RME associationEnd '_state'
	SystemState * _state;
	// }}}RME
	// {{{RME associationEnd 'm_devCtrl'
	DeviceCtrl m_devCtrl;
	// }}}RME

public:
	// {{{RME operation 'SystemCtrl()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	SystemCtrl( void );
	// }}}RME
	// {{{RME operation '~SystemCtrl()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	virtual ~SystemCtrl( void );
	// }}}RME
	// {{{RME operation 'Run()'
	// {{{RME general 'documentation'
	/* {{{USR

	    *************************************************
	    Function:       Run
	    Description:    模块的入口，主运行函数
	    Calls:
	    Called By:
	    Input:


	    Output:
	    Return:
	    *************************************************
	   }}}USR */
	// }}}RME
	void Run( void );
	// }}}RME

protected:
	// {{{RME operation 'Init()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       Init
	    Description:    系统初始化工作，在SystemCtrl的run函数中或者doInit函数中调用
	    Calls:
	    Called By:
	    Input:


	    Output
	    Return:
	    *************************************************	
	   }}}USR */
	// }}}RME
	void Init( void );
	// }}}RME

public:
	// {{{RME operation 'TrigerEvent(SYSTEMEVENT)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       TrigerEvent
	    Description:    触发事件，使状态转移。在这里调用SystemState的trigerEvent函数
	    Calls:
	    Called By:
	    Input:          sysEvent：触发的事件

	    Output:
	    Return:
	    *************************************************	
	   }}}USR */
	// }}}RME
	void TrigerEvent( SYSTEMEVENT sysEvent );
	// }}}RME
	// {{{RME operation 'ChangeState(SystemState*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       ChangeState
	    Description:    改变状态。保存原状态，将当前状态改为新状态，然后调用新状态的do函数
	    Calls:
	    Called By:
	    Input:          pNewState：新状态

	    Output:
	    Return:
	    *************************************************	
	   }}}USR */
	// }}}RME
	void ChangeState( SystemState * pNewState );
	// }}}RME

protected:
	// {{{RME operation 'DoIdle()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       DoIdle
	    Description:    系统在Idle状态下调用的动作，在这里调用getNextTask函数
	    Calls:
	    Called By:
	    Input:

	    Output:
	    Return:         成功返回true，否则返回false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool DoIdle( void );
	// }}}RME
	// {{{RME operation 'GetNextTask()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetNextTask
	    Description:    轮询各个任务队列中的任务，被doIdle调用，如果取到任务，则促使系统状态离开Idle状态，否则继续停留在Idle状态
	    Calls:
	    Called By:
	    Input:

	    Output:
	    Return:         成功返回任务，否则返回NULL
	    *************************************************
	   }}}USR */
	// }}}RME
	Task * GetNextTask( void );
	// }}}RME
	// {{{RME operation 'DoWork()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       DoWork
	    Description:    系统进入working状态后要执行的动作，在这里分析任务类型，然后分发任务，最后要等待任务执行完毕。如果完成的任务是普通任务，则会触发leave事件，退出working状态进入Idle状态；如果是特殊任务，则会触发相应的事件（如初始化事件）
	    Calls:
	    Called By:
	    Input:

	    Output:
	    Return:         成功返回true，否则返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	bool DoWork( void );
	// }}}RME
	// {{{RME operation 'DoInit()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       DoInit
	    Description:    在init事件被触发，系统进入init状态时触发的动作，在这里调用init函数重新初始化系统配置
	    Calls: 
	    Called By:
	    Input:

	    Output:
	    Return:         成功返回true，否则返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	bool DoInit( void );
	// }}}RME
	// {{{RME operation 'DoRestart()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       DoRestart
	    Description:    重新初始化通讯进程的运行环境，在restart事件被触发的时候调用
	    Calls:
	    Called By:
	    Input:

	    Output:
	    Return:         成功返回true，否则返回false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool DoRestart( void );
	// }}}RME
	// {{{RME operation 'DoExit()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       DoExit
	    Description:    退出进程。在exit事件发生时调用
	    Calls:
	    Called By:
	    Input:

	    Output:
	    Return:         成功返回true，否则返回false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool DoExit( void );
	// }}}RME
	// {{{RME operation 'CreatADUListenThread()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       CreatADUListenThread
	    Description:    创建监听ADU通讯的线程
	    Calls:
	    Called By:
	    Input:

	    Output:
	    Return:
	    *************************************************	
	   }}}USR */
	// }}}RME
	void CreatADUListenThread( void );
	// }}}RME
	// {{{RME operation 'CreatRmtListenThread()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       CreatRmtListenThread
	    Description:    创建远程监听线程
	    Calls:
	    Called By
	    Input:

	    Output:
	    Return:
	    *************************************************	
	   }}}USR */
	// }}}RME
	void CreatRmtListenThread( void );
	// }}}RME
	// {{{RME operation 'CreateTimerThread()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       CreateTimerThread
	    Description:    创建时间线程线程
	    Calls:
	    Called By:
	    Input:

	    Output:
	    Return:
	    *************************************************	
	   }}}USR */
	// }}}RME
	void CreateTimerThread( void );
	// }}}RME
	// {{{RME operation 'MonitorThread()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       MonitorThread
	    Description:    监控线程
	    Calls:
	    Called By:
	    Input:

	    Output:
	    Return
	    *************************************************	
	   }}}USR */
	// }}}RME
	void MonitorThread( void );
	// }}}RME
	// {{{RME operation 'CreateUdpDataThread()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       CreateUdpDataThread
	    Description:    创建接收UDP数据报的线程
	    Calls:
	    Called By:
	    Input:

	    Output:
	    Return:
	    *************************************************	
	   }}}USR */
	// }}}RME
	void CreateUdpDataThread( void );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* SystemCtrl_H */

// }}}RME
