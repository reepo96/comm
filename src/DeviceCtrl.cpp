// {{{RME classifier 'Logical View::DeviceCtrl'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "DeviceCtrl.h"
#endif

#include <RTSystem/Communication.h>
#include <DeviceCtrl.h>
#include <Device.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* 文件名称：DeviceCtrl.cpp
* 文件标识：
* 摘    要：设备控制类，负责设备的初始化及管理
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

#include "Command.h"
#include "SocketParm.h"
#include "SocketLinker.h"
#include "ModemLinker.h"
#include "ComLinker.h"
#include <MemMapMgr.h>
#include "Protocol60_103.h"

#include "StringRes.h"

DWORD WINAPI CreateDevThread(void *pParm);//创建设备的线程

// }}}USR
// }}}RME

// {{{RME operation 'DeviceCtrl()'
DeviceCtrl::DeviceCtrl( void )
	// {{{RME tool 'OT::Cpp' property 'ConstructorInitializer'
	// {{{USR

	// }}}USR
	// }}}RME
{
	// {{{USR
	InitializeCriticalSection(&m_MapSection);
	    //Init();
	// }}}USR
}
// }}}RME

// {{{RME operation '~DeviceCtrl()'
DeviceCtrl::~DeviceCtrl( void )
{
	// {{{USR
	/*map< unsigned int, Device * >::iterator dauIt = ConstantDAUMap.begin();
	for(;dauIt != ConstantDAUMap.end();dauIt++)
	{
		Device *pDevice = (*dauIt).second;
		if(pDevice != NULL)
			delete pDevice;
		pDevice = NULL;
	}
	ConstantDAUMap.clear();

	dauIt = TransientDAUMap.begin();
	for(;dauIt != TransientDAUMap.end();dauIt++)
	{
		Device *pDevice = (*dauIt).second;
		if(pDevice != NULL)
			delete pDevice;
		pDevice = NULL;
	}
	TransientDAUMap.clear();
	*/
	//设备列表
	map< unsigned int, Device * >::iterator dauIt  = DAUMap.begin();
	for(;dauIt != DAUMap.end();dauIt++)
	{
		Device *pDevice = (*dauIt).second;
		if(pDevice != NULL)
			delete pDevice;
		pDevice = NULL;
	}
	DAUMap.clear();


	map< string, Device * >::iterator rmtIt = rmtDeviceMap.begin();
	for(;rmtIt != rmtDeviceMap.end();rmtIt++)
	{
		Device *pDevice = (*rmtIt).second;
		if(pDevice != NULL)
			delete pDevice;
		pDevice = NULL;
	}
	rmtDeviceMap.clear();
	// }}}USR
}
// }}}RME

// {{{RME operation 'Init()'
void DeviceCtrl::Init( void )
{
	// {{{USR

	int iMaxConnNum = SystemCfger::GetInstance().iMaxAutoConnDevNum;
	int i = 0;
	
	//初始化设备 该MAP key  为板ID 
	map<unsigned int, PDAUINFO>& dauMap = SystemCfger::GetInstance().DAUInfoMap;
	map<unsigned int, PDAUINFO>::iterator it = dauMap.begin();
	for(;it != dauMap.end() && i<iMaxConnNum; it++,i++)
	{
		PDAUINFO pDAUInfo = (*it).second;

		PCREATEDEVPARM pParm = new CREATEDEVPARM();
		pParm->pContrl = this;
		pParm->DevId = (*it).first;
		pParm->devType = pDAUInfo->BoardType;  //功能类型
		HANDLE hThread = CreateThread(NULL,0,&CreateDevThread,pParm,0,NULL);
		::CloseHandle(hThread);
	}

	//初始化暂态设备
/*	map<unsigned int, PDAUINFO>& dauMap = SystemCfger::GetInstance().TransientDAUInfoMap;
	map<unsigned int, PDAUINFO>::iterator it = dauMap.begin();
	for(;it != dauMap.end() && i<iMaxConnNum; it++,i++)
	{
		PDAUINFO pDAUInfo = (*it).second;

		PCREATEDEVPARM pParm = new CREATEDEVPARM();
		pParm->pContrl = this;
		pParm->DevId = (*it).first;
		pParm->devType = 1;
		HANDLE hThread = CreateThread(NULL,0,&CreateDevThread,pParm,0,NULL);
		::CloseHandle(hThread);
	}

	//初始化稳态设备
	map<unsigned int, PDAUINFO>& dauMap2 = SystemCfger::GetInstance().ConstantDAUInfoMap;
	map<unsigned int, PDAUINFO>::iterator it2 = dauMap2.begin();
	for(i=0;it2 != dauMap2.end() && i<iMaxConnNum; it2++,i++)
	{
		//如果稳态功能激活则连接稳态设备
		if( SystemCfger::GetInstance().m_IniMgr.IsEnabled( (*it2).first,1) )
		{
			PDAUINFO pDAUInfo = (*it2).second;

			PCREATEDEVPARM pParm = new CREATEDEVPARM();
			pParm->pContrl = this;
			pParm->DevId = (*it2).first;
			pParm->devType = 2;
			HANDLE hThread = CreateThread(NULL,0,&CreateDevThread,pParm,0,NULL);
			::CloseHandle(hThread);
		}
	}
*/
	::Sleep(500);

	// }}}USR
}
// }}}RME

// {{{RME operation 'DispatchTask(Task*)'
bool DeviceCtrl::DispatchTask( Task * task )
{
	// {{{USR
	int iErrorCode = 0; //0：无错，1：发送任务到队列失败， 2：其他错误

	//连接或断开设备的任务
	if(task->cmd.funCode == CMD_CONNECT_DEVICE)
	{
		PCONNDEVPARM pParm = new CONNDEVPARM();
		pParm->pContrl = this;
		pParm->pTask = task;

		HANDLE hThread = CreateThread(NULL,0,&ConnetDevThread,pParm,0,NULL);
		::CloseHandle(hThread);
		return true;
	}
	else if(task->cmd.funCode == CMD_DISCONNECT_DEVICE)
	{
		PCONNDEVPARM pParm = new CONNDEVPARM();
		pParm->pContrl = this;
		pParm->pTask = task;

		HANDLE hThread = CreateThread(NULL,0,&DisConnetDevThread,pParm,0,NULL);
		::CloseHandle(hThread);
		return true;
	}
	/*  任务直接发送到相关设备  这里不再分割
	else if(task->cmd.funCode == CMD_GETDAUINF && task->cmd.to == CD_ASDU )
	{
		//获得稳态连接状态
		int iLinkState = GetDevState(task->cmd.destDevId,2);
		if( iLinkState ==1 || iLinkState == 2)//稳态正常
		{
			//如果是发往暂态的自检命令，则还要发一份到稳态
			Task *pNewTask = task->Clone();
			pNewTask->cmd.to = CD_CONST_DAU;

			//获得暂态连接状态
			iLinkState = GetDevState(task->cmd.destDevId,1);
			if( iLinkState !=1 && iLinkState != 2) 
			{
				pNewTask->cmd.isSplitCmd = false;
			}
			else
			{
				pNewTask->cmd.isSplitCmd = true;
			}

			if(true == DispatchTask(pNewTask) )
			{
				task->cmd.isSplitCmd = true;
				::Sleep(100);
			}

			if( iLinkState !=1 && iLinkState != 2)
			{
				delete task;
				task = NULL;
				return true;
			}
		}
	}*/

	if(task->cmd.from == CD_REMOTE || task->cmd.to == CD_REMOTE) //与远程链接通讯
	{
		/*通讯程序暂不做服务端
		//如果是自动分析故障简报任务，则有远程连接时才执行此任务，且只执行一次
		if( task->cmd.funCode == CMD_CREATEREPORTFILE)
		{
			map<string, Device *>::iterator it = rmtDeviceMap.begin();
			if( it != rmtDeviceMap.end())
			{
				if(!(*it).second->SendTask(task))
				{
					iErrorCode = 1;
				}
			}
			else
			{
				delete task;
				task = NULL;
				return true;
			}
		}
		else if(task->Linker == NULL)
		{
			map<string, Device *>::iterator it = rmtDeviceMap.begin();
			for(;it != rmtDeviceMap.end();it++)
			{
				Task *pNewTask = task->Clone();
				if(!(*it).second->SendTask(pNewTask))
				{
					iErrorCode = 1;
					break;
				}
			}

			delete task;
			task = NULL;
		}
		else
		{
			string addr = task->Linker->GetConnectAddr();
			map<string, Device *>::iterator it = rmtDeviceMap.find(addr);

			if(it != rmtDeviceMap.end()) //设备已经存在
			{
				if(!(*it).second->SendTask(task))
				{
					iErrorCode = 1;
				}
			}
			else //设备不存在
			{
				//初始化将要发送到远程的故障简报列表
				SystemCfger::GetInstance().InitReportList(addr);

				Device *pDevice = new Device(0,this,task->Linker,Device::DT_REMOTE);
				rmtDeviceMap.insert(map<string,Device *>::value_type(addr,pDevice));

				if(!pDevice->SendTask(task))
				{
					iErrorCode = 1;
				}
				else
				{
					iErrorCode = 0;
					pDevice->runThread();
				}
			}//end else //设备不存在
		}*/
	}//end if(task->cmd.from == CD_REMOTE) //与远程链接通讯
	else //与ADU通讯   /** 现在用同一MAP管理设备***/
	{
		Device *pDevice = NULL;
		int iDevType =1;	//设备类型，1：暂态，2：稳态
		
		EnterCriticalSection(&m_MapSection);
		
		map< unsigned int, Device * >::iterator it = DAUMap.find(task->cmd.destDevId);
		if(it != DAUMap.end())
		{
			pDevice = (*it).second;
		}
		
		::LeaveCriticalSection(&m_MapSection);
	
		
		if(pDevice != NULL) //设备已经存在
		{
			if(!pDevice->SendTask(task))
			{
				iErrorCode = 1;
			}
		}
		else //设备不存在
		{
			//查看该监控板是否激活
			bool bActive = false ;
			PDAUINFO pDauInfo = SystemCfger::GetInstance().GetDAUInfo(task->cmd.destDevId);
			if(pDauInfo != NULL)
			{
				bActive = pDauInfo->IsActive;
			}
			if(bActive)
			{
				//创建线程重建设备
				PCREATEDEVPARM pParm = new CREATEDEVPARM();
				pParm->pContrl = this;
				pParm->DevId = task->cmd.destDevId;  //destDevId 为板ID 
				pParm->devType = iDevType;
				HANDLE hThread = CreateThread(NULL,0,&CreateDevThread,pParm,0,NULL);
				::CloseHandle(hThread);
			}
			else
			{
				printf("The device(BoardID:%d) is not active. \r\n",task->cmd.destDevId);
			}

			char info[64] ={0};
			sprintf(&info[0],"No find the DAU with boardNO:%d",task->cmd.destDevId);
			mylog.WriteLog2File(LogControl::LERROR,info);

			//记录日志
			LogInfo LogInfo;
			memset(&LogInfo,0,sizeof(LogInfo));

			//LogInfo.LogDAUID = (int)task->cmd.destDevId;
			if(pDauInfo != NULL)
			{
				LogInfo.LogDAUID = pDauInfo->DAUId;
				int minSize = min(sizeof(LogInfo.DAUName),sizeof(pDauInfo->DAUName));
				memcpy(LogInfo.DAUName,pDauInfo->DAUName,minSize);
				iDevType = pDauInfo->BoardType;
			}

			SYSTEMTIME sysTime;
			memset(&sysTime,0,sizeof(sysTime));
			::GetLocalTime(&sysTime);
			sprintf(LogInfo.SysLogTime,"%d-%02d-%02d %02d:%02d:%02d",sysTime.wYear,sysTime.wMonth,sysTime.wDay,
				sysTime.wHour,sysTime.wMinute,sysTime.wSecond);

			string sReason;
			if(task->cmd.to == CD_CONST_DAU||iDevType==2)
			{
				sReason.append( (CStringRes::GetInstance())->GetStrById(IDS_CONST_REC,"稳态：") );
			}
			sReason.append(Protocol60_103::GetCmdDesc(task->cmd.funCode));

			memcpy(LogInfo.SysLogReason,sReason.c_str(),sizeof(LogInfo.SysLogReason) -1);

			LogInfo.SysLogType = 0;
			LogInfo.SysLogLevel = 2;

			sprintf(LogInfo.SysLogDesc,
				(CStringRes::GetInstance())->GetStrById(IDS_LOG_DESC,"耗时:%d 毫秒, %s"),0,
				(CStringRes::GetInstance())->GetStrById(IDS_EXCFAIL_LINKFAIL,"执行失败，失败原因：前台设备连接失败") );
			mylog.WriteLog(&LogInfo);


			//写失败的返回结果
			task->cmd.resultNum = 2;
			PCMDRESULT pResult = new CMDRESULT();
			pResult->resultType = CDRT_TRUFALSE;
			pResult->resultLen = 1;
			pResult->pData = new char[1];
			pResult->pData[0] = 0 ; //false
			task->cmd.resultList.push_back(pResult);

			pResult = new CMDRESULT();
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

			iErrorCode = 2;
			return false;

		}//end else //设备不存在
	}//end else //与ADU通讯

	if(iErrorCode == 1 )
	{
		mylog.WriteLog2File(LogControl::LERROR,"The msg queue is full");

		//写失败的返回结果
		task->cmd.resultNum = 2;
		PCMDRESULT pResult = new CMDRESULT();
		pResult->resultType = CDRT_TRUFALSE;
		pResult->resultLen = 1;
		pResult->pData = new char[1];
		pResult->pData[0] = 0 ; //false
		task->cmd.resultList.push_back(pResult);

		pResult = new CMDRESULT();
		pResult->resultType = CDRT_DESC;
		pResult->resultLen = strlen( (CStringRes::GetInstance())->GetStrById(IDS_TASKQUE_FULL,"任务队列已满") );
		pResult->pData = new char[pResult->resultLen+1];
		strcpy(pResult->pData,(CStringRes::GetInstance())->GetStrById(IDS_TASKQUE_FULL,"任务队列已满") );
		pResult->pData[pResult->resultLen] = 0 ;
		task->cmd.resultList.push_back(pResult);

		//执行结果写入内存中
		MemMapMgr memMgr;
		memMgr.WriteTaskResultToMem(task->cmd);

		if(task != NULL)
		{
			task->Linker = NULL;
			delete task;
			task = NULL;
		}

		return false;
	}
	else
	{
		return true;
	}
	// }}}USR
}
// }}}RME

// {{{RME operation 'CreateDev(PDAUINFO)'
Device * DeviceCtrl::CreateDev( PDAUINFO pDAUInfo )
{
	// {{{USR
	return Device::Build(pDAUInfo,this); 
	// }}}USR
}
// }}}RME

// {{{RME operation 'DeleteDev(Device*)'
void DeviceCtrl::DeleteDev( Device * pDevice )
{
	// {{{USR
	    assert(pDevice != NULL);

		map< unsigned int, Device * >::iterator it;

	    switch(pDevice->GetDeviceType())
	    {
	    case Device::DT_TRNS:
		case Device::DT_CONST:
		case Device::DT_PQ:
		case Device::DT_PMU:
		case Device::DT_COMPLEX:
			EnterCriticalSection(&m_MapSection);

			it = DAUMap.find(pDevice->GetDevId());
			if(it != DAUMap.end())
			{
				DAUMap.erase(pDevice->GetDevId());
			}
	
			::LeaveCriticalSection(&m_MapSection);
	        break;
	    case Device::DT_REMOTE:
	        rmtDeviceMap.erase(pDevice->GetConnectAddr());
	        break;
	    default:
	        break;
	    }
	    
	    delete pDevice;
	    pDevice = NULL;

	// }}}USR
}
// }}}RME

// {{{RME operation 'Exit()'
bool DeviceCtrl::Exit( void )
{
	// {{{USR
	        map<string, Device *>::iterator rmtIt = rmtDeviceMap.begin();
	        for(;rmtIt!= rmtDeviceMap.end();)
	        {
	            (*rmtIt).second->Stop();    //stop and delete
	                ::Sleep(500);
	            rmtIt = rmtDeviceMap.begin();
	        }

			map<unsigned int, Device *>::iterator aduIt = DAUMap.begin();
	        for(;aduIt!= DAUMap.end();)
	        {
	            (*aduIt).second->Stop();    //stop and delete
	            ::Sleep(500);
	            aduIt = DAUMap.begin();
	        }


	      /*  aduIt = ConstantDAUMap.begin();
	        for(;aduIt!= ConstantDAUMap.end();)
	        {
	            (*aduIt).second->Stop();    //stop and delete
	            ::Sleep(500);
	            aduIt = ConstantDAUMap.begin();
	        }

			aduIt = TransientDAUMap.begin();
	        for(;aduIt!= TransientDAUMap.end();)
	        {
	            (*aduIt).second->Stop();    //stop and delete
	            ::Sleep(500);
	            aduIt = TransientDAUMap.begin();
	        }
			*/
		    return true;
	// }}}USR
}
// }}}RME

// {{{RME operation 'Rebuild(int)'
bool DeviceCtrl::Rebuild( int type )
{
	// {{{USR
	if(type == 1)   //重建远程通讯配置
	{        
		map<string, Device *>::iterator it = rmtDeviceMap.begin();
		for(;it!= rmtDeviceMap.end();)
		{
			(*it).second->Stop();   //stop and delete
			it = rmtDeviceMap.begin();
		}
		::Sleep(1000);
	}
	else if(type == 2) //重建ADU通讯配置
	{
		map<unsigned int, PDAUINFO>& dauInfoMap = SystemCfger::GetInstance().DAUInfoMap;
		bool bIsStopDev = false;

		//停止不存在的设备
		map<unsigned int, Device *>::iterator devTmpIt = DAUMap.begin();
		while( devTmpIt != DAUMap.end() )
		{
			unsigned int tmpDevId = (*devTmpIt).second->GetDevId();
			if(dauInfoMap.find( tmpDevId ) == dauInfoMap.end() )
			{
				//停止暂态设备
				map<unsigned int, Device *>::iterator devTmpIt2 = devTmpIt;
				devTmpIt2++;
				(*devTmpIt).second->Stop();
				devTmpIt = devTmpIt2;
				bIsStopDev = true;
			}
			else
			{
				devTmpIt++;
			}
		}

		if(bIsStopDev)
		{
			::Sleep(1000);
		}

		//重起新加或修改配置的设备
		map<unsigned int, PDAUINFO>::iterator it = dauInfoMap.begin();
		for(;it != dauInfoMap.end(); it++)
		{
			PDAUINFO pDAUInfo = (*it).second;

			map<unsigned int, Device *>::iterator devIt = DAUMap.find((*it).first);
			if(devIt != DAUMap.end()) //设备已经存在
			{
				if(pDAUInfo->isRebuild)     //相关配置已经重建
				{
					(*devIt).second->Stop();
					::Sleep(500);

					PCREATEDEVPARM pParm = new CREATEDEVPARM();
					pParm->pContrl = this;
					pParm->DevId = (*it).first;
					pParm->devType = pDAUInfo->BoardType;
					HANDLE hThread = CreateThread(NULL,0,&CreateDevThread,pParm,0,NULL);
					::CloseHandle(hThread);

				}//end if(pDAUInfo->isRebuild)
			}
			else    //设备不存在
			{
				if( (int)DAUMap.size() < SystemCfger::GetInstance().iMaxConnDevNum )
				{
					PCREATEDEVPARM pParm = new CREATEDEVPARM();
					pParm->pContrl = this;
					pParm->DevId = (*it).first;
					pParm->devType = pDAUInfo->BoardType;;
					HANDLE hThread = CreateThread(NULL,0,&CreateDevThread,pParm,0,NULL);
					::CloseHandle(hThread);
				}
			}//end if(devIt != aduDeviceMap.end())
		}//end for
	}
	return true;

	// }}}USR
}
// }}}RME

// {{{RME operation 'MonitorDev()'
void DeviceCtrl::MonitorDev( void )
{
	// {{{USR
			map<string, Device *>::iterator rmtIt = rmtDeviceMap.begin();
	        for(;rmtIt!= rmtDeviceMap.end();rmtIt++)
	        {
	            (*rmtIt).second->MonitorThread();
	        }

			EnterCriticalSection(&m_MapSection);

	        map<unsigned int, Device *>::iterator aduIt = DAUMap.begin();
	        for(;aduIt!= DAUMap.end();aduIt++)
	        {
	            (*aduIt).second->MonitorThread();
	        }

			::LeaveCriticalSection(&m_MapSection);
	// }}}USR
}
// }}}RME

// {{{RME operation 'GetAllDevState(vector<DAVICESTATE>&,int)'
bool DeviceCtrl::GetAllDevState( vector< DAVICESTATE > & StateList, int devType )
{
	if(DAUMap.size() == 0 )
	{
		printf("No find any device in GetAllDevState(),and try init it again\r\n");

		this->Init();
		::Sleep(100);
	}
	EnterCriticalSection(&m_MapSection);

	Device::DeviceType DevType = Device::DT_UNKOW;  /// 
	if(devType == 1)//暂态设备
	{
		DevType = Device::DT_TRNS;
	}
	else if(devType == 2)//稳态设备
	{
		DevType = Device::DT_CONST;
	}
	else if(devType == 4)//电能质量设备
	{
		DevType = Device::DT_PQ;
	}
	else if(devType == 8)//PMU设备
	{
		DevType = Device::DT_PMU;
	}
	else 
	{
		DevType = Device::DT_COMPLEX;
	}

	map<unsigned int, Device *>::iterator it = DAUMap.begin();
	for(;it != DAUMap.end();it++)
	{
		if((*it).second->GetDeviceType()==DevType)
		{
			DAVICESTATE devState;
			memset(&devState,0,sizeof(devState));
			devState.devId = (*it).first;
			devState.state = (*it).second->GetState();
			StateList.push_back(devState);
		}
	}	

	::LeaveCriticalSection(&m_MapSection);
	return true;
	// {{{USR
	/*
	if(TransientDAUMap.size() == 0 && ConstantDAUMap.size() == 0)
	{
		printf("No find any device in GetAllDevState(),and try init it again\r\n");

		this->Init();
		::Sleep(100);
	}

	EnterCriticalSection(&m_MapSection);

	if(devType == 1)
	{
		map<unsigned int, Device *>::iterator it = TransientDAUMap.begin();
		for(;it != TransientDAUMap.end();it++)
		{
			DAVICESTATE devState;
			memset(&devState,0,sizeof(devState));
			devState.devId = (*it).first;
			devState.state = (*it).second->GetState();

			StateList.push_back(devState);
		}		
	}
	else
	{
		map<unsigned int, Device *>::iterator it = ConstantDAUMap.begin();
		for(;it != ConstantDAUMap.end();it++)
		{
			DAVICESTATE devState;
			memset(&devState,0,sizeof(devState));
			devState.devId = (*it).first;
			devState.state = (*it).second->GetState();

			StateList.push_back(devState);
		}
	}

	::LeaveCriticalSection(&m_MapSection);


	return true;
	*/
	// }}}USR
}
// }}}RME

// {{{RME operation 'GetDevState(unsigned int,int)'
int DeviceCtrl::GetDevState( unsigned int DevId, int devType )
{
	// {{{USR
	/*if(TransientDAUMap.size() == 0 && ConstantDAUMap.size() == 0)
	{
		printf("No find any device in GetDevState(),and try init it again\r\n");
		this->Init();
		::Sleep(100);
	}*/
	EnterCriticalSection(&m_MapSection);

	Device *pDevice = NULL;
	
	//pDevice = TransientDAUMap[DevId];
	map< unsigned int, Device * >::iterator it = DAUMap.find(DevId);
	if(it != DAUMap.end())
	{
		pDevice = (*it).second;
	}
	else
	{
		printf("No find the device(BoardID:%d) in GetDevState(),and try create it again\r\n",DevId);
		//CreateDev(DevId,devType);
	}
	::LeaveCriticalSection(&m_MapSection);

	if(pDevice != NULL) //设备存在
	{
		return pDevice->GetState();
	}
	else
	{
		//查看该监控板是否激活
		bool bActive = false ;
		PDAUINFO pDauInfo = SystemCfger::GetInstance().GetDAUInfo(DevId);
		if(pDauInfo != NULL)
		{
			bActive = pDauInfo->IsActive;
		}
		if(bActive)
		{
			PCREATEDEVPARM pParm = new CREATEDEVPARM();
			pParm->pContrl = this;
			pParm->DevId = DevId;
			pParm->devType = devType;
			HANDLE hThread = CreateThread(NULL,0,&CreateDevThread,pParm,0,NULL);
			::CloseHandle(hThread);
		}
		else
		{
			printf("The device(BoardID:%d) is not active in GetDevState(),\r\n",DevId);
		}
		return 3;
	}
	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR

void DeviceCtrl::CreateDev( unsigned int devId ,int devType )
{
	
	EnterCriticalSection(&m_MapSection);
	
	map<unsigned int,unsigned int>::iterator buildingIt = BuildingDevMap.find(devId);
	if(buildingIt != BuildingDevMap.end())
	{
		//设备正在创建中，不需要再次创建
		LeaveCriticalSection(&m_MapSection);
		return;
	}
	
	//加入到创建列表中
	BuildingDevMap.insert(map<unsigned int,unsigned int>::value_type(devId,devId));
	LeaveCriticalSection(&m_MapSection);
	
	//初始化设备
	map<unsigned int, PDAUINFO>& dauInfoMap = SystemCfger::GetInstance().DAUInfoMap;
	map<unsigned int, PDAUINFO>::iterator it = dauInfoMap.find(devId);
	if(it != dauInfoMap.end())
	{
		PDAUINFO pDAUInfo = (*it).second;
		Device *pDevice = CreateDev(pDAUInfo);
		
		if(pDevice != NULL)
		{
			EnterCriticalSection(&m_MapSection);
			DAUMap.insert(map<unsigned int,Device *>::value_type(pDAUInfo->BoardId,pDevice)); //key 为boardID
			LeaveCriticalSection(&m_MapSection);
		}
	}
	
	//创建完毕，从列表中删除
	EnterCriticalSection(&m_MapSection);
	BuildingDevMap.erase(devId);
	LeaveCriticalSection(&m_MapSection);

	/*
	if(devType==1)
	{
		EnterCriticalSection(&m_MapSection);
		
		map<unsigned int,unsigned int>::iterator buildingIt = BuildingTransientDevMap.find(devId);
		if(buildingIt != BuildingTransientDevMap.end())
		{
			//设备正在创建中，不需要再次创建
			LeaveCriticalSection(&m_MapSection);
			return;
		}
		
		//加入到创建列表中
		BuildingTransientDevMap.insert(map<unsigned int,unsigned int>::value_type(devId,devId));
		LeaveCriticalSection(&m_MapSection);
		
		//初始化暂态设备
		map<unsigned int, PDAUINFO>& dauMap = SystemCfger::GetInstance().TransientDAUInfoMap;
		map<unsigned int, PDAUINFO>::iterator it = dauMap.find(devId);
		if(it != dauMap.end())
		{
			PDAUINFO pDAUInfo = (*it).second;
			Device *pDevice = CreateDev(pDAUInfo);
			
			if(pDevice != NULL)
			{
				EnterCriticalSection(&m_MapSection);
				TransientDAUMap.insert(map<unsigned int,Device *>::value_type(pDAUInfo->DAUId,pDevice));
				LeaveCriticalSection(&m_MapSection);
			}
		}
		
		//创建完毕，从列表中删除
		EnterCriticalSection(&m_MapSection);
		BuildingTransientDevMap.erase(devId);
		LeaveCriticalSection(&m_MapSection);
	}
	else
	{
		EnterCriticalSection(&m_MapSection);

		map<unsigned int,unsigned int>::iterator buildingIt = BuildingConstantDevMap.find(devId);
		if(buildingIt != BuildingConstantDevMap.end())
		{
			//设备正在创建中，不需要再次创建
			LeaveCriticalSection(&m_MapSection);
			return;
		}

		//加入到创建列表中
		BuildingConstantDevMap.insert(map<unsigned int,unsigned int>::value_type(devId,devId));
		LeaveCriticalSection(&m_MapSection);

		//初始化稳态设备
		map<unsigned int, PDAUINFO>& dauMap2 = SystemCfger::GetInstance().ConstantDAUInfoMap;
		map<unsigned int, PDAUINFO>::iterator it2 = dauMap2.find(devId);
		if(it2 != dauMap2.end())
		{
			PDAUINFO pDAUInfo = (*it2).second;
			Device *pDevice = CreateDev(pDAUInfo);

			if(pDevice != NULL)
			{
				EnterCriticalSection(&m_MapSection);
				ConstantDAUMap.insert(map<unsigned int,Device *>::value_type(pDAUInfo->DAUId,pDevice));
				LeaveCriticalSection(&m_MapSection);
			}
		}

		//创建完毕，从列表中删除
		EnterCriticalSection(&m_MapSection);
		BuildingConstantDevMap.erase(devId);
		LeaveCriticalSection(&m_MapSection);
	}
	*/
}

bool DeviceCtrl::SetAutoSendLinker(Linker *pLinker)
{
	if( !pLinker || pLinker->GetCommType() != Linker::CT_SOCKET )
		return false;

	//根据IP地址将主动上传的链接与已存在的设备关联起来
	map<unsigned int,Device *>::iterator it = DAUMap.begin();
	for(;it!=DAUMap.end();it++)
	{
		Device *pDev = (*it).second;
		if( !pDev )
			continue;
		if((pDev->GetConnectAddr()).compare(pLinker->GetConnectAddr()) == 0)
		{
			pDev->SetAutoLinker(pLinker);
			return true;
		}
	}
	return false;
}

DWORD WINAPI CreateDevThread(void *pParm)
{
	DeviceCtrl::PCREATEDEVPARM pCreateParm = (DeviceCtrl::PCREATEDEVPARM)pParm;
	if(pCreateParm == NULL)
		return 0;

	if(pCreateParm->pContrl != NULL)
	{
		pCreateParm->pContrl->CreateDev(pCreateParm->DevId,pCreateParm->devType);
	}

	delete pCreateParm;
	pCreateParm = NULL;

	return 0;
}

DWORD WINAPI ConnetDevThread(void *pParm)
{
	DeviceCtrl::PCONNDEVPARM pConnParm = (DeviceCtrl::PCONNDEVPARM)pParm;
	if(pConnParm == NULL)
		return 0;

	if(pConnParm->pContrl == NULL || pConnParm->pTask == NULL)
	{
		delete pConnParm;
		pConnParm = NULL;
		return 0;
	}

	EnterCriticalSection( &(pConnParm->pContrl->m_MapSection) );

	int iIsSuccess = 1;
	string sErrorStr;

	if( pConnParm->pContrl->DAUMap.find(pConnParm->pTask->cmd.destDevId) != 
			pConnParm->pContrl->DAUMap.end() )
	{//设备已创建
		LeaveCriticalSection( &(pConnParm->pContrl->m_MapSection) );

		iIsSuccess = 1;
	}
	else if( (int)pConnParm->pContrl->DAUMap.size() >= SystemCfger::GetInstance().iMaxConnDevNum )
	{//超过最大连接数
		LeaveCriticalSection( &(pConnParm->pContrl->m_MapSection) );

		iIsSuccess = 0;
		sErrorStr = string( (CStringRes::GetInstance())->GetStrById(IDS_OVER_MAXLINKNUM,"超过最大连接数") );
	}
	else//创建设备
	{
		LeaveCriticalSection( &(pConnParm->pContrl->m_MapSection) );

		unsigned int boardId  = pConnParm->pTask->cmd.destDevId;
		// //获取功能类型
		int DevTyp = 1;
		map<unsigned int, PDAUINFO>& dauInfoMap = SystemCfger::GetInstance().DAUInfoMap;
		map<unsigned int, PDAUINFO>::iterator it = dauInfoMap.find(boardId);
		if(it!= dauInfoMap.end())
		{
			PDAUINFO pDAUInfo = (*it).second;
			DevTyp = pDAUInfo->BoardType;
		}
		else if(pConnParm->pTask->cmd.pParm !=NULL && 
			pConnParm->pTask->cmd.parmLen== sizeof(unsigned int) )
		{
			//重新获得设备配置信息
			unsigned int devId;
			memcpy(&devId,pConnParm->pTask->cmd.pParm,sizeof(unsigned int));
			SystemCfger::GetInstance().InitDauInfo2(devId);

			it = dauInfoMap.find(boardId);
			if(it!= dauInfoMap.end())
			{
				PDAUINFO pDAUInfo = (*it).second;
				DevTyp = pDAUInfo->BoardType;
			}
		}

		//连接设备
		pConnParm->pContrl->CreateDev(pConnParm->pTask->cmd.destDevId,DevTyp);

		EnterCriticalSection( &(pConnParm->pContrl->m_MapSection) );

		if( pConnParm->pContrl->DAUMap.find(pConnParm->pTask->cmd.destDevId) != 
			pConnParm->pContrl->DAUMap.end() )
		{
			LeaveCriticalSection( &(pConnParm->pContrl->m_MapSection) );

			//创建成功
			iIsSuccess = 1;
		}
		else//创建失败
		{
			LeaveCriticalSection( &(pConnParm->pContrl->m_MapSection) );

			iIsSuccess = 0;
			sErrorStr = string( (CStringRes::GetInstance())->GetStrById(IDS_LINK_FAIL,"连接设备失败") );
		}
	}//end 创建设备

	pConnParm->pTask->cmd.resultNum = 1;

	PCMDRESULT pCmdResult = new CMDRESULT();
	pCmdResult->resultType = CDRT_TRUFALSE;	//返回的结果是真假类型
	pCmdResult->resultLen = 1;		//结果长度1字节
	pCmdResult->pData = new char[1];
	pCmdResult->pData[0] = iIsSuccess;
	pConnParm->pTask->cmd.resultList.push_back(pCmdResult);

	if( iIsSuccess == 0 )
	{
		pConnParm->pTask->cmd.resultNum = 2;

		pCmdResult = new CMDRESULT();
		pCmdResult->resultType = CDRT_DESC;	//错误描述
		pCmdResult->resultLen = sErrorStr.size() +1;		//结果长度1字节
		pCmdResult->pData = new char[pCmdResult->resultLen];
		memset(pCmdResult->pData,0,pCmdResult->resultLen);
		memcpy(pCmdResult->pData,sErrorStr.c_str(),pCmdResult->resultLen-1);
		pConnParm->pTask->cmd.resultList.push_back(pCmdResult);
	}

	MemMapMgr memMgr;
	memMgr.WriteTaskResultToMem(pConnParm->pTask->cmd);

	delete pConnParm->pTask;
	pConnParm->pTask = NULL;
	delete pConnParm;
	pConnParm = NULL;

	return 0;
}

DWORD WINAPI DisConnetDevThread(void *pParm)
{
	DeviceCtrl::PCONNDEVPARM pConnParm = (DeviceCtrl::PCONNDEVPARM)pParm;
	if(pConnParm == NULL)
		return 0;

	if(pConnParm->pContrl == NULL || pConnParm->pTask == NULL)
	{
		delete pConnParm;
		pConnParm = NULL;
		return 0;
	}

	//停止暂态设备
	EnterCriticalSection( &(pConnParm->pContrl->m_MapSection) );
	map<unsigned int, Device *>& DevMap = pConnParm->pContrl->DAUMap;

	map<unsigned int, Device *>::iterator it = DevMap.find(pConnParm->pTask->cmd.destDevId);
	if( it != DevMap.end() )
	{
		Device *pDevice = (*it).second;
		LeaveCriticalSection( &(pConnParm->pContrl->m_MapSection) );

		pDevice->Stop();//停止设备
	}
	else
	{
		LeaveCriticalSection( &(pConnParm->pContrl->m_MapSection) );
	}

	pConnParm->pTask->cmd.resultNum = 1;

	PCMDRESULT pCmdResult = new CMDRESULT();
	pCmdResult->resultType = CDRT_TRUFALSE;	//返回的结果是真假类型
	pCmdResult->resultLen = 1;		//结果长度1字节
	pCmdResult->pData = new char[1];
	pCmdResult->pData[0] = 1;
	pConnParm->pTask->cmd.resultList.push_back(pCmdResult);

	MemMapMgr memMgr;
	memMgr.WriteTaskResultToMem(pConnParm->pTask->cmd);

	delete pConnParm->pTask;
	pConnParm->pTask = NULL;
	delete pConnParm;
	pConnParm = NULL;

	return 0;
}

// }}}USR
// }}}RME

// }}}RME

