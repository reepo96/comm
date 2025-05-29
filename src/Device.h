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
* �ļ����ƣ�Device.h
* �ļ���ʶ��
* ժ    Ҫ���豸�࣬һ���豸����һ������
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
*
**************************************************************************/

DWORD WINAPI DeviceThread(LPVOID pParm);     //�豸�߳�

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

	    //�����豸�¼�
	    typedef enum DevEvent{DE_STILL,DE_LEAVE,DE_SUCCESS,DE_FAIL,DE_TIMEOUT,DE_INVALID} DEVEVENT;

	    /*�豸����
		DT_UNKOW��δ֪
		DT_REMOTE��Զ���豸
		DT_TRNS����̬�豸
		DT_CONST����̬�豸
		DT_PQ�����������豸
		DT_PMU��PMU�豸
		DT_COMPLEX  �����豸 ��һ������ж��ҵ����
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

	    DEVEVENT m_nextEvent;   //��һ������Ҫ�������¼�

	    bool     m_bIsStop;     //�Ƿ�Ҫֹͣ�豸
	    HANDLE   m_hDevTread;   //�߳̾��

	    MsgQueue * m_pTimerResultMsgQue; //��Ŷ�ʱ����ִ�н������Ϣ����

	    vector<Task*>   m_PauseTaskList;    //����ͣ�������б�
		CWatchModule	m_WatchDog;	//���Ź�
		int		m_iWatchPoint;		//���Ź�����

		int		m_iGetTimeContErrCount;//��ȡʱ�������������

		DeviceType	m_DevType;

		Linker * m_pAutoLinker;	//�����Զ��ϴ����ݵ�����

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
	���������
	   }}}USR */
	// }}}RME
	Task * m_task;
	// }}}RME
	// {{{RME classAttribute '_preState'
	// {{{RME general 'documentation'
	/* {{{USR
	�豸ǰһ״̬
	   }}}USR */
	// }}}RME
	DeviceState * _preState;
	// }}}RME
	// {{{RME classAttribute 'm_deviceId'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	unsigned int m_deviceId;   // ʹ�ð�������ֵ, 2010-10-18
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
	    Description:    ��������queue��
	    Calls:          
	    Called By:      
	    Input:          task:ͨѶ����
	                    
	    Output:         
	    Return:         �ɹ�����true�����򷵻�false
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
	    Description:    �豸������̺߳���
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
	    Description:    �����̡߳�����Ĺ��캯����ִ�д˺��������߳���ִ�����run����
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
	    Description:    �����¼���ʹ״̬ת�ơ����������TaskState��trigerEvent����
	    Calls:          
	    Called By:      
	    Input:          devEvent���¼�
	                    
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
	    Description:    �ı�״̬������ԭ״̬������ǰ״̬��Ϊ��״̬��Ȼ�������״̬��do����
	    Calls:          
	    Called By:      
	    Input:          newState����״̬
	                    
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
	    Description:    �����豸����ʱ�Ĺ���
	    Calls:          
	    Called By:      
	    Input:          
	                    
	    Output:         
	    Return:         ����ɹ�����true�����򷵻�false
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
	    Description:    �����豸�ڹ���״̬�µĹ���
	    Calls:          
	    Called By:      
	    Input:          
	                    
	    Output:         
	    Return:         ����ɹ�����true�����򷵻�false
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
	    Description:    �����豸��Ч״̬�Ĺ���
	    Calls:          
	    Called By:      
	    Input:          
	                    
	    Output:         
	    Return:         ����ɹ�����true�����򷵻�false
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
	    Description:    �����豸����ʱ�Ĺ���
	    Calls:          
	    Called By:      
	    Input:          
	                    
	    Output:         
	    Return:         ����ɹ�����true�����򷵻�false
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
	    Description:    �豸����ִ�гɹ���Ĵ���
	    Calls:          
	    Called By:      
	    Input:          
	                    
	    Output:         
	    Return:         ����ɹ�����true�����򷵻�false
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
	    Description:    �豸����ʧ�ܺ�Ĵ���
	    Calls:          
	    Called By:      
	    Input:          
	                    
	    Output:         
	    Return:         ����ɹ�����true�����򷵻�false
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
	    Description:    �豸ֹͣ����
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
	    Description:    ����߳�
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
	    Description:    ��ѯ�豸״̬
	    Calls:          
	    Called By:      
	    Input:          
	                    
	    Output:         
	    Return:         0��δ֪״̬�� 1������״̬�� 2������״̬�� 3����Ч״̬
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
