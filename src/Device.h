// {{{RME classifier 'Logical View::Device'

#ifndef Device_H
#define Device_H

#ifdef PRAGMA
#pragma interface "Device.h"
#endif

#include <RTSystem/Communication.h>
#include <MsgQueue.h>
class CommunicationManager;
class DeviceCtrl;
class DeviceState;
class Linker;
class LogControl;
class MemMapMgr;

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
#include "public.h"
#include "Task.h"
#include "../WatchDog/WatchModule.h"

/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* 文件名称：Device.h
* 文件标识：
* 摘    要：设备类，一个设备代表一条连接
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

DWORD WINAPI DeviceThread(LPVOID pParm);     //设备线程

#ifdef _MY_UNITTEST
    class Device_UT;
    class DeviceCtrl_UT;
#endif

// }}}USR
// }}}RME

class Device
{
public:
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR
#ifdef _MY_UNITTEST
	    friend class Device_UT;
	    friend class DeviceCtrl_UT;
#endif

	    friend  DWORD WINAPI DeviceThread(void *pParm);	    

	    friend class DevIdleState;
	    friend class DevWorkState;	    
	    friend class DevSuccessState;
	    friend class DevTimeOutState;
	    friend class DevFailState;
	    friend class DevInvalidState;

	    //定义设备事件
	    typedef enum DevEvent{DE_STILL,DE_LEAVE,DE_SUCCESS,DE_FAIL,DE_TIMEOUT,DE_INVALID} DEVEVENT;

	    /*设备类型
		DT_UNKOW：未知
		DT_REMOTE：远程设备
		DT_TRNS：暂态设备
		DT_CONST：稳态设备
		DT_PQ：电能质量设备
		DT_PMU：PMU设备
		DT_COMPLEX  复合设备 即一块板上有多个业务功能
		*/
	    typedef enum DeviceType{DT_UNKOW,DT_REMOTE,DT_TRNS,DT_CONST,DT_PQ,DT_PMU,DT_COMPLEX} DEVUCETYPE;

		
		Linker *GetLinker()
		{
			return m_pLinker;
		}

		bool SetAutoLinker(Linker *pLinker);
	    
	// }}}USR
	// }}}RME

protected:
	// {{{RME tool 'OT::Cpp' property 'ProtectedDeclarations'
	// {{{USR

	    DEVEVENT m_nextEvent;   //下一个即将要发生的事件

	    bool     m_bIsStop;     //是否要停止设备
	    HANDLE   m_hDevTread;   //线程句柄

	    MsgQueue * m_pTimerResultMsgQue; //存放定时任务执行结果的消息队列

	    vector<Task*>   m_PauseTaskList;    //被暂停的任务列表
		CWatchModule	m_WatchDog;	//看门狗
		int		m_iWatchPoint;		//看门狗看点

		int		m_iGetTimeContErrCount;//获取时间连续出错次数

		DeviceType	m_DevType;

		Linker * m_pAutoLinker;	//处理自动上传数据的链接

	// }}}USR
	// }}}RME

private:
	// {{{RME tool 'OT::Cpp' property 'PrivateDeclarations'
	// {{{USR
	   
	// }}}USR
	// }}}RME

protected:
	// {{{RME classAttribute 'm_task'
	// {{{RME general 'documentation'
	/* {{{USR
	处理的任务
	   }}}USR */
	// }}}RME
	Task * m_task;
	// }}}RME
	// {{{RME classAttribute '_preState'
	// {{{RME general 'documentation'
	/* {{{USR
	设备前一状态
	   }}}USR */
	// }}}RME
	DeviceState * _preState;
	// }}}RME
	// {{{RME classAttribute 'm_deviceId'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	unsigned int m_deviceId;   // 使用板编号填充该值, 2010-10-18
	// }}}RME
	// {{{RME associationEnd '_devState'
	DeviceState * _devState;
	// }}}RME
	// {{{RME associationEnd 'm_timerQue'
	MsgQueue m_timerQue;
	// }}}RME
	// {{{RME associationEnd 'm_uiQue'
	MsgQueue m_uiQue;
	// }}}RME

public:
	// {{{RME associationEnd 'm_rmtQue'
	MsgQueue m_rmtQue;
	// }}}RME

protected:
	// {{{RME associationEnd 'm_aduQue'
	MsgQueue m_aduQue;
	// }}}RME
	// {{{RME associationEnd 'm_pLinker'
	Linker * m_pLinker;
	// }}}RME
	// {{{RME associationEnd 'm_pParent'
	DeviceCtrl * m_pParent;
	// }}}RME

public:
	// {{{RME operation 'Build(PDAUINFO,DeviceCtrl*)'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	static Device * Build( PDAUINFO pDAUInfo, DeviceCtrl * pParent );
	// }}}RME
	// {{{RME operation 'Device(unsigned int,DeviceCtrl*,Linker*)'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	Device( unsigned int deviceId, DeviceCtrl * pParent, Linker * pLinker,DeviceType DevType );
	// }}}RME
	// {{{RME operation '~Device()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	virtual ~Device( void );
	// }}}RME
	// {{{RME operation 'SendTask(Task*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       SendTask
	    Description:    发送任务到queue中
	    Calls:          
	    Called By:      
	    Input:          task:通讯任务
	                    
	    Output:         
	    Return:         成功返回true，否则返回false
	    *************************************************		
	   }}}USR */
	// }}}RME
	bool SendTask( Task * task );
	// }}}RME

protected:
	// {{{RME operation 'run()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       run
	    Description:    设备类的主线程函数
	    Calls:          
	    Called By:      
	    Input:          
	                    
	    Output:         
	    Return:         
	    *************************************************	
	   }}}USR */
	// }}}RME
	void run( void );
	// }}}RME

public:
	// {{{RME operation 'runThread()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       runThread
	    Description:    创建线程。在类的构造函数中执行此函数。在线程中执行类的run函数
	    Calls:          
	    Called By:      
	    Input:          
	                    
	    Output:         
	    Return:         
	    *************************************************		
	   }}}USR */
	// }}}RME
	void runThread( void );
	// }}}RME
	// {{{RME operation 'TrigerEvent(DEVEVENT)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       TrigerEvent
	    Description:    触发事件，使状态转移。在这里调用TaskState的trigerEvent函数
	    Calls:          
	    Called By:      
	    Input:          devEvent：事件
	                    
	    Output:         
	    Return:         
	    *************************************************		
	   }}}USR */
	// }}}RME
	void TrigerEvent( DEVEVENT devEvent );
	// }}}RME
	// {{{RME operation 'ChangeState(DeviceState*)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	    Function:       ChangeState
	    Description:    改变状态。保存原状态，将当前状态改为新状态，然后调用新状态的do函数
	    Calls:          
	    Called By:      
	    Input:          newState：新状态
	                    
	    Output:         
	    Return:         
	    *************************************************
	   }}}USR */
	// }}}RME
	void ChangeState( DeviceState * newState );
	// }}}RME

protected:
	// {{{RME operation 'DoIdle()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       DoIdle
	    Description:    处理设备空闲时的工作
	    Calls:          
	    Called By:      
	    Input:          
	                    
	    Output:         
	    Return:         处理成功返回true，否则返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	bool DoIdle( void );
	// }}}RME
	// {{{RME operation 'DoWork()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       DoWork
	    Description:    处理设备在工作状态下的工作
	    Calls:          
	    Called By:      
	    Input:          
	                    
	    Output:         
	    Return:         处理成功返回true，否则返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	bool DoWork( void );
	// }}}RME
	// {{{RME operation 'DoInvalid()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       DoWork
	    Description:    处理设备无效状态的工作
	    Calls:          
	    Called By:      
	    Input:          
	                    
	    Output:         
	    Return:         处理成功返回true，否则返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	bool DoInvalid( void );
	// }}}RME
	// {{{RME operation 'DoTimout()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       DoWork
	    Description:    处理设备任务超时的工作
	    Calls:          
	    Called By:      
	    Input:          
	                    
	    Output:         
	    Return:         处理成功返回true，否则返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	bool DoTimout( void );
	// }}}RME
	// {{{RME operation 'DoSuccess()'
	// {{{RME general 'documentation'
	/* {{{USR
	     *************************************************
	    Function:       DoSuccess
	    Description:    设备任务执行成功后的处理
	    Calls:          
	    Called By:      
	    Input:          
	                    
	    Output:         
	    Return:         处理成功返回true，否则返回false
	    *************************************************		
	   }}}USR */
	// }}}RME
	bool DoSuccess( void );
	// }}}RME
	// {{{RME operation 'DoFail()'
	// {{{RME general 'documentation'
	/* {{{USR
	     *************************************************
	    Function:       DoFail
	    Description:    设备任务失败后的处理
	    Calls:          
	    Called By:      
	    Input:          
	                    
	    Output:         
	    Return:         处理成功返回true，否则返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	bool DoFail( void );
	// }}}RME

public:
	// {{{RME operation 'GetDeviceType()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	DEVUCETYPE GetDeviceType( void );
	// }}}RME
	// {{{RME operation 'GetConnectAddr()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	string GetConnectAddr( void );
	// }}}RME
	// {{{RME operation 'GetDevId()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	unsigned int GetDevId( void );
	// }}}RME
	// {{{RME operation 'Stop()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       Stop
	    Description:    设备停止工作
	    Calls:          
	    Called By:      
	    Input:          
	                    
	    Output:         
	    Return:         
	    *************************************************	
	   }}}USR */
	// }}}RME
	void Stop( void );
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
	    Return:         
	    *************************************************	
	   }}}USR */
	// }}}RME
	void MonitorThread( void );
	// }}}RME
	// {{{RME operation 'GetState()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetState
	    Description:    查询设备状态
	    Calls:          
	    Called By:      
	    Input:          
	                    
	    Output:         
	    Return:         0：未知状态， 1：空闲状态， 2：工作状态， 3：无效状态
	    *************************************************
	   }}}USR */
	// }}}RME
	int GetState( void );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* Device_H */

// }}}RME
