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
* �ļ����ƣ�SystemCtrl.h
* �ļ���ʶ��
* ժ    Ҫ��ϵͳ������
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
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

			//����ϵͳ�¼�
			typedef enum SystemEvent{SE_LEAVE,SE_STILL,SE_INIT,SE_EXIT} SYSTEMEVENT;

			HANDLE      m_hExitListenEvent;         //�˳��������¼����
			HANDLE      m_hCmdResultMapEvent;       //�����������ڴ��¼����

			HANDLE      m_hRmtListenTread;          //����Զ�����ӵ��߳̾��
			HANDLE      m_hTimerTread;              //ʱ���߳̾��
			HANDLE      m_hUdpDataTread;              //udp�߳̾��
			HANDLE      m_hRltDataTread;              //����ʵʱ�����߳̾��
			
			HANDLE      m_hAutoSendDataTread;         //�����������������߳̾�� add by kh

			DeviceCtrl * GetDeviceCtrl() { return &m_devCtrl; }

	// }}}USR
	// }}}RME

protected:
	// {{{RME tool 'OT::Cpp' property 'ProtectedDeclarations'
	// {{{USR
	    //LogControl log;

	    SYSTEMEVENT m_nextEvent;   //��һ������Ҫ�������¼�
	    bool    m_isExit;          //�Ƿ�Ҫ�˳�

	    HANDLE m_hResultMapFile;      //����ִ�н�����ڴ�ӳ���ļ�
	    unsigned char *m_pResultMapData;  //ָ�����ڴ�ӳ������
		CWatchModule	m_WatchDog;	//���Ź�
		int		m_iWatchPoint;		//���Ź�����

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
	    Description:    ģ�����ڣ������к���
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
	    Description:    ϵͳ��ʼ����������SystemCtrl��run�����л���doInit�����е���
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
	    Description:    �����¼���ʹ״̬ת�ơ����������SystemState��trigerEvent����
	    Calls:
	    Called By:
	    Input:          sysEvent���������¼�

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
	    Description:    �ı�״̬������ԭ״̬������ǰ״̬��Ϊ��״̬��Ȼ�������״̬��do����
	    Calls:
	    Called By:
	    Input:          pNewState����״̬

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
	    Description:    ϵͳ��Idle״̬�µ��õĶ��������������getNextTask����
	    Calls:
	    Called By:
	    Input:

	    Output:
	    Return:         �ɹ�����true�����򷵻�false
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
	    Description:    ��ѯ������������е����񣬱�doIdle���ã����ȡ���������ʹϵͳ״̬�뿪Idle״̬���������ͣ����Idle״̬
	    Calls:
	    Called By:
	    Input:

	    Output:
	    Return:         �ɹ��������񣬷��򷵻�NULL
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
	    Description:    ϵͳ����working״̬��Ҫִ�еĶ���������������������ͣ�Ȼ��ַ��������Ҫ�ȴ�����ִ����ϡ������ɵ���������ͨ������ᴥ��leave�¼����˳�working״̬����Idle״̬�����������������ᴥ����Ӧ���¼������ʼ���¼���
	    Calls:
	    Called By:
	    Input:

	    Output:
	    Return:         �ɹ�����true�����򷵻�false
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
	    Description:    ��init�¼���������ϵͳ����init״̬ʱ�����Ķ��������������init�������³�ʼ��ϵͳ����
	    Calls: 
	    Called By:
	    Input:

	    Output:
	    Return:         �ɹ�����true�����򷵻�false
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
	    Description:    ���³�ʼ��ͨѶ���̵����л�������restart�¼���������ʱ�����
	    Calls:
	    Called By:
	    Input:

	    Output:
	    Return:         �ɹ�����true�����򷵻�false
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
	    Description:    �˳����̡���exit�¼�����ʱ����
	    Calls:
	    Called By:
	    Input:

	    Output:
	    Return:         �ɹ�����true�����򷵻�false
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
	    Description:    ��������ADUͨѶ���߳�
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
	    Description:    ����Զ�̼����߳�
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
	    Description:    ����ʱ���߳��߳�
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
	    Description:    ����߳�
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
	    Description:    ��������UDP���ݱ����߳�
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
