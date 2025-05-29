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
* �ļ����ƣ�Device.cpp
* �ļ���ʶ��
* ժ    Ҫ���豸�࣬һ���豸����һ������
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
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
	if(pDAUInfo == NULL) //���ò�����
	{
		return NULL;
	}
	else //���ô��� 
	{
		DeviceType DevType = DT_UNKOW;  /// 
		if(pDAUInfo->BoardType == 1)//��̬�豸
		{
			DevType = DT_TRNS;
		}
		else if(pDAUInfo->BoardType == 2)//��̬�豸
		{
			DevType = DT_CONST;
		}
		else if(pDAUInfo->BoardType == 4)//���������豸
		{
			DevType = DT_PQ;
		}
		else if(pDAUInfo->BoardType == 8)//PMU�豸
		{
			DevType = DT_PMU;
		}
		else 
		{
			DevType = DT_COMPLEX;      //�����豸
		}

		void   *pParam = NULL;  //ͨѶ��Ҫ�Ĳ���
		Linker *pLinker = NULL;            
		struct SocketParm strLinkParm;             //����tcp/ipͨѶ����Ĳ���
		memset(&strLinkParm,0,sizeof(strLinkParm));

		struct ComParm comParm;                    //���ô���ͨѶ����
		memset(&comParm,0,sizeof(comParm));

		struct ModemParm modemParm;                //����modenͨѶ����
		memset(&modemParm,0,sizeof(modemParm));

		//if( strcmp(pDAUInfo->DAUType,SHDFR_B_TYPE) == 0||pDAUInfo->ProtocalType ==2)//�豸������SHDFR_B�豸
		if( pDAUInfo->ProtocalType ==2)//�豸������SHDFR_B�豸
		{
			pLinker = new P61850Linker();

			struct P61850Linker::tagLinkParm p61850Parm;
			pParam = (void*)&p61850Parm;

			GetPrivateProfileString("System","CfgFile","./osicfg.xml",p61850Parm.cfgFile,sizeof(p61850Parm.cfgFile),"./IEC61850.ini");
			
			::memcpy(p61850Parm.devName,pDAUInfo->DAUName,strlen(pDAUInfo->DAUName));
			::memcpy(p61850Parm.ARName,pDAUInfo->DAUName,strlen(pDAUInfo->DAUName));
			if(pDAUInfo->BoardType == 1)//��̬�豸
			{
				::strcat(p61850Parm.ARName,"_trns");
			//	DevType = DT_TRNS;
			}
			else if(pDAUInfo->BoardType == 2)//��̬�豸
			{
				::strcat(p61850Parm.ARName,"_cont");
				//DevType = DT_CONST;
			}
			else if(pDAUInfo->BoardType == 4)//���������豸
			{
				::strcat(p61850Parm.ARName,"_pq");
			//	DevType = DT_CONST;
			}
			else if(pDAUInfo->BoardType == 8)//PMU�豸
			{
				::strcat(p61850Parm.ARName,"_pmu");
			//	DevType = DT_CONST;
			}
			else 
			{
				::strcat(p61850Parm.ARName,"_complex");  //�����豸
			//	DevType = DT_CONST;
			}

			/***ת����utf-8����-->begin***/
			//ascII��ת��ת����unicode
			int nLen = MultiByteToWideChar(CP_ACP, 0,p61850Parm.ARName, -1, NULL, NULL);
			LPWSTR pwsTmpDevName = new WCHAR[nLen];
			int WriteLen=MultiByteToWideChar(CP_ACP, 0,p61850Parm.ARName, -1, pwsTmpDevName, nLen);
			
			//unicode��ת��ת����utf-8
			nLen = WideCharToMultiByte(CP_UTF8,0,pwsTmpDevName,-1,p61850Parm.ARName,0,NULL,NULL);
			WideCharToMultiByte(CP_UTF8,0,pwsTmpDevName,-1,p61850Parm.ARName,nLen,NULL,NULL);
			delete []pwsTmpDevName;
			pwsTmpDevName = NULL;
			/***ת����utf-8����-->end***/
		}
		else
		{
			switch(pDAUInfo->linkType)
			{
			case CFG_SOCKET_LINK:      //ʹ��tcp/ipͨѶ
				pLinker = new SocketLinker();
				pParam = (void*)&strLinkParm;

				//����tcp/ipͨѶ����Ĳ���
				memcpy(&strLinkParm.loacalAddr[0],
					&(SystemCfger::GetInstance().addrForADU[0]),
					sizeof(SystemCfger::GetInstance().addrForADU)); //���ð���ص�ַ
				strLinkParm.loacalTcpPort = SystemCfger::GetInstance().tcpPortForADU; //���ð���ض˿�

				memcpy(&strLinkParm.connetToAddr[0],
					&(pDAUInfo->addr[0]),
					sizeof(pDAUInfo->addr));                     //���ð�ĵ�ַ
				strLinkParm.connetToTcpPort = pDAUInfo->tcpPort; //���ð�Ķ˿�
				strLinkParm.connetToUdpPort = pDAUInfo->udpPort; //����udp�˿�
				break;

			case CFG_COM_LINK:         //ʹ�ô���ͨѶ
				pLinker = new ComLinker();

				//���ô���ͨѶ����
				pParam = (void*)&comParm;
				memcpy(&comParm.comName[0],&(pDAUInfo->comName[0]),
					sizeof(comParm.comName));
				comParm.baudrate = pDAUInfo->baud;
				comParm.data = pDAUInfo->dataBit;
				comParm.parity = pDAUInfo->parity;
				comParm.stop = pDAUInfo->stop;

				break;

			case CFG_MODEN_LINK:       //ʹ��modenͨѶ
				pLinker = new ModemLinker();

				//����modenͨѶ����
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
			if(pDAUInfo->BoardType == 1)//��̬�豸
			{
				mylog.FormatLogInf(LogControl::LDEBUG,"Begin create trans device,device name:%s.",pDAUInfo->DAUName);
			}
			else
			{
				mylog.FormatLogInf(LogControl::LDEBUG,"Begin create const device,device name:%s.",pDAUInfo->DAUName);
			}
*/

			pLinker->Init(pParam,Linker::LK_ASDU);    //��ʼ��
			pLinker->SetDevId(pDAUInfo->DAUId);

			if(!pLinker->Connect()) //����ʧ��
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

	//���������ϴ����̳߳����˳�����ͬʱ�ر����̵߳�����
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
	//���ÿ��Ź���ʱʱ��
	if( GetDeviceType() != DT_REMOTE )
	{
		m_iWatchPoint = m_WatchDog.addMonitorPoint(10*60);//10���ӳ�ʱ
	}
	else
	{
		m_iWatchPoint = m_WatchDog.addMonitorPoint(60*60);//1Сʱ��ʱ
	}
#endif

	m_pTimerResultMsgQue = NULL;
	m_pTimerResultMsgQue = QueRegister::GetInstance().RegisterQue(COMM_TIMER_RESULTQUE_NAME);

	//��ʼ״̬ΪIdle״̬
	_devState = dynamic_cast<DeviceState*>(&DevIdleState::GetStateInstance());

	//����ͣ���¼�
	m_nextEvent = DE_STILL;

	//runThread(); //�����߳�

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

	//����豸��Ч,��ɾ�����񲢼�¼��־
	if(_devState->GetStateValue() == DeviceState::DSV_INVALID || 
		_devState->GetStateValue() == DeviceState::DSV_UNKOWN)
	{
		if(m_pLinker != NULL)
		{
			m_pLinker->GetCmdDesc(task->cmd);
		}

		//��¼��־
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
			sReason.append( (CStringRes::GetInstance())->GetStrById(IDS_CONST_REC,"��̬��") );
		}
		sReason.append(task->cmd.describe);
		memcpy(LogInfo.SysLogReason,sReason.c_str(),sizeof(LogInfo.SysLogReason) -1);

		LogInfo.SysLogType = 0;
		LogInfo.SysLogLevel = 2;

		sprintf(LogInfo.SysLogDesc,
			(CStringRes::GetInstance())->GetStrById(IDS_LOG_DESC,"��ʱ:%d ����, %s"),0,
			(CStringRes::GetInstance())->GetStrById(IDS_EXCFAIL_LINKFAIL,"ִ��ʧ�ܣ�ʧ��ԭ��ǰ̨�豸����ʧ��"));
		mylog.WriteLog(&LogInfo);

		//дʧ�ܵķ��ؽ��
		task->cmd.resultNum = 2;
		PCMDRESULT pResult = new CMDRESULT;
		pResult->resultType = CDRT_TRUFALSE;
		pResult->resultLen = 1;
		pResult->pData = new char[1];
		pResult->pData[0] = 0 ; //false
		task->cmd.resultList.push_back(pResult);

		pResult = new CMDRESULT;
		pResult->resultType = CDRT_DESC;
		pResult->resultLen = strlen( (CStringRes::GetInstance())->GetStrById(IDS_CONNDEV_FAIL,"ǰ̨�豸������ͨ") );
		pResult->pData = new char[pResult->resultLen+1];
		strcpy(pResult->pData,(CStringRes::GetInstance())->GetStrById(IDS_CONNDEV_FAIL,"ǰ̨�豸������ͨ") );
		pResult->pData[pResult->resultLen] = 0 ;
		task->cmd.resultList.push_back(pResult);

		//ִ�н��д���ڴ���
		MemMapMgr memMgr;
		memMgr.WriteTaskResultToMem(task->cmd);

		task->Linker = NULL;
		delete task;
		task = NULL;

		return true;
	}

	//m_task = task;

	//����������Դ���Ա���벻ͬ�Ķ�����
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
	//�豸�̵߳������г���
	mylog.WriteLog2File(LogControl::LINFO,"Enter Device::run()");
	assert(m_pLinker != NULL);

	//��ʼ��ͨѶ����
	//m_pLinker->Connect();

	while(!m_bIsStop)
	{
		this->TrigerEvent(m_nextEvent);

#ifndef _DEBUG
		m_WatchDog.kickMonitorPoint(m_iWatchPoint);
#endif
	}

	if(m_pParent)
		m_pParent->DeleteDev(this); //ɾ���豸

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

		_devState->Do(this);//����״̬�Ķ���
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
	//��idle״̬����Ҫ�������£�1����û�з��͹�������������2�������������û������
	assert(m_pLinker != NULL);

	if(!m_pLinker->IsConnect() || m_pLinker->IsPreDisConnect() )  //��������Ѿ����ر�
	{
		//��������Ƿ���˵����ӣ��������ٴ��豸����Ϊ���ӹرպ�
		//�ͻ��˻��ٴε���connect()���ӵ����صķ������ˣ���ʱ������µ������豸
		if(m_pLinker->IsServerLink() && m_pLinker->GetCommType() == Linker::CT_SOCKET)
		{
			m_bIsStop = true;       //�˳��߳�
			return true;
		}
		else
		{
			m_nextEvent = DE_INVALID; //�豸������Ч״̬
			return true;
		}
	}

	//�ȿ�����û�з��͹���������
	if(m_pLinker->CreatTask())
	{
		//�Ѿ��ɹ�������������ȴ������͵��豸�����������
		::Sleep(10);
		m_nextEvent = DE_STILL; //����idle״̬
		return true;
	}

	//�ٲ쿴�����е�����
	if(m_task != NULL)
	{
		m_task->Linker = NULL;
		delete m_task;
		m_task = NULL;
	}

	m_task = m_aduQue.ReceiveMsg();     //��ȡADU�е�����
	if(m_task == NULL)
	{
		m_task = m_rmtQue.ReceiveMsg(); //��ȡԶ�̶����е�����
	}
	if(m_task == NULL)
	{
		//����ͣ���л�ȡ����
		vector<Task*>::iterator it = m_PauseTaskList.begin();
		if(it != m_PauseTaskList.end())
		{
			m_task = (*it);
			m_PauseTaskList.erase(it);
		}
	}
	if(m_task == NULL)
	{
		m_task = m_uiQue.ReceiveMsg(); //��ȡUI�����е�����
	}
	if(m_task == NULL)
	{
		m_task = m_timerQue.ReceiveMsg(); //��ȡ��ʱ�������е�����
	}

	//�������ҵ�����
	if(m_task != NULL)
	{
		//ϵͳ���賬ʱʱ�����������ͣ�ģ��򲻿��������Ƿ��Ѿ���ʱ
		if(m_task->cmd._state._state != CmdState::CS_PAUSE &&
			SystemCfger::GetInstance().timeOut != 0)
		{
			DWORD currentTime = GetTickCount();
			if( (currentTime - m_task->createTime) >= (DWORD)SystemCfger::GetInstance().timeOut)
			{
				//�����Ѿ���ʱ����ɾ����
				delete m_task;
				m_task = NULL;
				m_nextEvent =DE_STILL; //����idle״̬
				return true;
			}
		}

		m_nextEvent = DE_LEAVE; //�뿪idle����work״̬�����������
		return true;
	}

	//ִ����·��idle����
	m_pLinker->DoIdle();

	//û�п�ִ�е�����
	::Sleep(10);
	m_nextEvent =DE_STILL; //����idle״̬

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

	//��¼��־
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

	//��ʼִ������
	m_task->cmd._state.ToPreState();    //�Ƚ�����״̬��Ϊǰһ״̬
	m_task->Linker = this->m_pLinker;
	
	if(cmmMgr.RunTask(m_task)) //����ִ�гɹ�
	{
		m_nextEvent = DE_SUCCESS;
		sResult.append( (CStringRes::GetInstance())->GetStrById(IDS_EXC_SUCCESS,"ִ�гɹ�") );

		if(m_task->cmd.funCode == CMD_GETTIME)
		{
			m_iGetTimeContErrCount = 0;
		}
	}
	else    //����ִ��ʧ��
	{
		if(m_task->cmd._state._state == CmdState::CS_PAUSE)
		{
			//��ͣ
			m_PauseTaskList.push_back(m_task);
			m_nextEvent = DE_LEAVE;
			sResult.append( (CStringRes::GetInstance())->GetStrById(IDS_CMD_SUSPEND,"������ͣ") );
			Sleep(10);
		}
		else
		{
			LogInfo.SysLogLevel = 2;
			m_nextEvent = DE_FAIL;
			sResult.append( (CStringRes::GetInstance())->GetStrById(IDS_EXCFAIL_REASON,"ִ��ʧ��,ʧ��ԭ��") );

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
		(CStringRes::GetInstance())->GetStrById(IDS_LOG_DESC,"��ʱ:%d ����, %s"),
		endTime-startTime,sResult.c_str());

	if(	m_nextEvent == DE_FAIL && m_task->cmd.funCode == CMD_GETTIME )
	{
		//����ǻ�ȡʱ�䣬������ʧ��5�βż�¼��־
		if( m_iGetTimeContErrCount >=5 )
		{
			m_iGetTimeContErrCount = 0;
			mylog.WriteLog(&LogInfo);
		}
	}
	else
	{
		//CMD_104_CONFIRM_TESTFR == 0X26  ��104 ͨ���������Ӧ��
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
		//��ͣ������д���ؽ��
		m_task = NULL;
		return true;
	}
	
	
	//ִ�н��д���ڴ���
	MemMapMgr memMgr;
	memMgr.WriteTaskResultToMem(m_task->cmd);

	//ִ�н��������ʱ��
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

		//������ʧ�ܣ������Զ��ϴ�������Ҫ�Ͽ�
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
			//�ٴν��룬���´�������
			int iCount = 0;
			Task *pTask = m_uiQue.ReceiveMsg();
			while(pTask != NULL && iCount < 255 )
			{
				this->SendTask(pTask);
				pTask = m_uiQue.ReceiveMsg();
				iCount++;
			}
		}

		//�ȴ����Զ��ϴ������ӶϿ�֮����ٴγ�������
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
				m_nextEvent = DE_STILL;  //����ʧ�ܣ����ٴν�����Ч״̬
				::Sleep(500);
			}
			else
			{
				m_nextEvent = DE_LEAVE; //�ɹ����ӣ��˳���Ч״̬
			}
		}
		else
		{
			printf("Try to link the device again\r\n");

			//���ϳ�������
			if(!m_pLinker->Connect())
			{
				m_nextEvent = DE_STILL;  //����ʧ�ܣ����ٴν�����Ч״̬
				::Sleep(1000);
			}
			else
			{
				::Sleep(1000);
				m_nextEvent = DE_LEAVE; //�ɹ����ӣ��˳���Ч״̬
			}
		}

		if(m_nextEvent == DE_LEAVE)
		{
			mylog.WriteLog2File(LogControl::LWARNING,"The device turn to work from invalid state.");
		}

		if(_preState->GetStateName() != string("DevInvalidState") || m_nextEvent == DE_LEAVE)
		{
			/*��ģ�鲻����Զ�����ӣ����Բ�����������״̬����
			//��������״̬����
			Task *pNewTask = new Task();
			pNewTask->Linker = NULL;
			pNewTask->createTime = ::GetTickCount();
			pNewTask->cmd.funCode = CMD_AUTOSEND_LINKSTATE;
			pNewTask->cmd.destDevId = m_deviceId;
			pNewTask->cmd.from = CD_NEURON;
			pNewTask->cmd.to = CD_REMOTE;
			strcpy(pNewTask->cmd.describe,"�Զ��ϴ��豸״̬");
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
	    //��¼��־
	    m_nextEvent = DE_LEAVE; //�뿪����idle״̬

	    return true;
	// }}}USR
}
// }}}RME

// {{{RME operation 'DoFail()'
bool Device::DoFail( void )
{
	// {{{USR
	    
	    //TODO:
	    //��¼��־

	    m_nextEvent = DE_LEAVE; //�뿪����idle״̬
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
