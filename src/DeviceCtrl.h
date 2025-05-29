// {{{RME classifier 'Logical View::DeviceCtrl'

#ifndef DeviceCtrl_H
#define DeviceCtrl_H

#ifdef PRAGMA
#pragma interface "DeviceCtrl.h"
#endif

#include <RTSystem/Communication.h>
class Device;
class Linker;

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* 文件名称：DeviceCtrl.h
* 文件标识：
* 摘    要：设备控制类，负责设备的初始化及管理
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

#include "public.h"
#include "Task.h"
#include <map>
using namespace std;

#ifdef _MY_UNITTEST
    class DeviceCtrl_UT;
#endif

DWORD WINAPI ConnetDevThread(void *pParm);//连接设备的线程
DWORD WINAPI DisConnetDevThread(void *pParm);//断开设备的线程

// }}}USR
// }}}RME

class DeviceCtrl
{
public:
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR

	friend DWORD WINAPI ConnetDevThread(void *pParm);
	friend DWORD WINAPI DisConnetDevThread(void *pParm);

	typedef struct tagCreatDevParm
	{
		DeviceCtrl *pContrl;
		unsigned int DevId;
		int devType;
	}CREATEDEVPARM,*PCREATEDEVPARM;

	typedef struct tagConnDevParm
	{
		DeviceCtrl *pContrl;
		Task *pTask;
	}CONNDEVPARM,*PCONNDEVPARM;

#ifdef _MY_UNITTEST
	        friend class DeviceCtrl_UT;
#endif

	//创建设备
	void CreateDev( unsigned int devId ,int devType);

	bool SetAutoSendLinker(Linker *pLinker);

	// }}}USR
	// }}}RME

protected:
	// {{{RME tool 'OT::Cpp' property 'ProtectedDeclarations'
	// {{{USR
	//停用
//	map< unsigned int, Device * > ConstantDAUMap;//稳态设备对象

	CRITICAL_SECTION	m_MapSection;
	//停用
//	map<unsigned int,unsigned int> BuildingConstantDevMap;//正在创建中的稳态设备列表
	//停用
//	map<unsigned int,unsigned int> BuildingTransientDevMap;//正在创建中的暂态设备列表
	//统一使用面MAP 
	map<unsigned int,unsigned int> BuildingDevMap;//正在创建中的设备列表

	//设备列表
	map< unsigned int, Device * > DAUMap;

	// }}}USR
	// }}}RME

private:
	// {{{RME tool 'OT::Cpp' property 'PrivateDeclarations'
	// {{{USR
	    
	// }}}USR
	// }}}RME

protected:
	// {{{RME classAttribute 'TransientDAUMap'
	// {{{RME general 'documentation'
	/* {{{USR
	暂态设备对象
	   }}}USR */
	// }}}RME
	//停用
//	map< unsigned int, Device * > TransientDAUMap;
	// }}}RME
	// {{{RME classAttribute 'rmtDeviceMap'
	// {{{RME general 'documentation'
	/* {{{USR
	远程ip地址与远程设备对象的映射
	   }}}USR */
	// }}}RME
	map< string, Device * > rmtDeviceMap;
	// }}}RME

public:
	// {{{RME operation 'DeviceCtrl()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	DeviceCtrl( void );
	// }}}RME
	// {{{RME operation '~DeviceCtrl()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	virtual ~DeviceCtrl( void );
	// }}}RME
	// {{{RME operation 'Init()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       Init
	    Description:    初始化
	    Calls:          
	    Called By:      
	    Input:          
	                    
	    Output:         
	    Return:         
	    *************************************************		
	   }}}USR */
	// }}}RME
	void Init( void );
	// }}}RME
	// {{{RME operation 'DispatchTask(Task*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       RunTask
	    Description:    执行任务
	    Calls:          
	    Called By:      
	    Input:          task：任务
	                    
	    Output:         
	    Return:         执行成功返回true，否则返回false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool DispatchTask( Task * task );
	// }}}RME

protected:
	// {{{RME operation 'CreateDev(PDAUINFO)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       CreateDev
	    Description:    根据配置信息创建设备
	    Calls:          
	    Called By:      
	    Input:          pDAUInfo：保存DAU配置信息数据
	                    
	    Output:         
	    Return:         执行成功返回设备，否则返回NULL
	    *************************************************	
	   }}}USR */
	// }}}RME
	Device * CreateDev( PDAUINFO pDAUInfo );
	// }}}RME

public:
	// {{{RME operation 'DeleteDev(Device*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       DeleteDev
	    Description:    删除设备
	    Calls:          
	    Called By:      
	    Input:          pDevice：要删除的设备
	                    
	    Output:         
	    Return:         
	    *************************************************	
	   }}}USR */
	// }}}RME
	void DeleteDev( Device * pDevice );
	// }}}RME
	// {{{RME operation 'Exit()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       Exit
	    Description:    系统退出
	    Calls:          
	    Called By:      
	    Input:          
	                    
	    Output:         
	    Return:         执行成功返回true，否则返回false
	    *************************************************		
	   }}}USR */
	// }}}RME
	bool Exit( void );
	// }}}RME
	// {{{RME operation 'Rebuild(int)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       Rebuild
	    Description:    系统配置重建
	    Calls:          
	    Called By:      
	    Input:          type：重建类型
	                    
	    Output:         
	    Return:         执行成功返回true，否则返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	bool Rebuild( int type );
	// }}}RME
	// {{{RME operation 'MonitorDev()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       MonitorDev
	    Description:    监控设备线程
	    Calls:          
	    Called By:      
	    Input:          
	                    
	    Output:         
	    Return:         
	    *************************************************	
	   }}}USR */
	// }}}RME
	void MonitorDev( void );
	// }}}RME
	// {{{RME operation 'GetAllDevState(vector<DAVICESTATE>&,int)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetAllDevState
	    Description:    查询所有设备状态
	    Calls:          
	    Called By:      
	    Input:          devType:设备类型，1-暂态,2-稳态
	                    
	    Output:         StateList：设备状态列表
	    Return:         true:成功；false：失败    *************************************************
	   }}}USR */
	// }}}RME
	bool GetAllDevState( vector< DAVICESTATE > & StateList, int devType );
	// }}}RME
	// {{{RME operation 'GetDevState(unsigned int,int)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetDevState
	    Description:    查询设备状态
	    Calls:          
	    Called By:      
	    Input:          DevId：设备编号
							devType:设备类型，1-暂态,2-稳态
	                    
	    Output:         
	    Return:         0：未知状态， 1：空闲状态， 2：工作状态， 3：无效状态
	    *************************************************
	   }}}USR */
	// }}}RME
	int GetDevState( unsigned int DevId, int devType );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* DeviceCtrl_H */

// }}}RME
