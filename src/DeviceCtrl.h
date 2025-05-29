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
* �ļ����ƣ�DeviceCtrl.h
* �ļ���ʶ��
* ժ    Ҫ���豸�����࣬�����豸�ĳ�ʼ��������
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
*
**************************************************************************/

#include "public.h"
#include "Task.h"
#include <map>
using namespace std;

#ifdef _MY_UNITTEST
    class DeviceCtrl_UT;
#endif

DWORD WINAPI ConnetDevThread(void *pParm);//�����豸���߳�
DWORD WINAPI DisConnetDevThread(void *pParm);//�Ͽ��豸���߳�

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

	//�����豸
	void CreateDev( unsigned int devId ,int devType);

	bool SetAutoSendLinker(Linker *pLinker);

	// }}}USR
	// }}}RME

protected:
	// {{{RME tool 'OT::Cpp' property 'ProtectedDeclarations'
	// {{{USR
	//ͣ��
//	map< unsigned int, Device * > ConstantDAUMap;//��̬�豸����

	CRITICAL_SECTION	m_MapSection;
	//ͣ��
//	map<unsigned int,unsigned int> BuildingConstantDevMap;//���ڴ����е���̬�豸�б�
	//ͣ��
//	map<unsigned int,unsigned int> BuildingTransientDevMap;//���ڴ����е���̬�豸�б�
	//ͳһʹ����MAP 
	map<unsigned int,unsigned int> BuildingDevMap;//���ڴ����е��豸�б�

	//�豸�б�
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
	��̬�豸����
	   }}}USR */
	// }}}RME
	//ͣ��
//	map< unsigned int, Device * > TransientDAUMap;
	// }}}RME
	// {{{RME classAttribute 'rmtDeviceMap'
	// {{{RME general 'documentation'
	/* {{{USR
	Զ��ip��ַ��Զ���豸�����ӳ��
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
	    Description:    ��ʼ��
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
	    Description:    ִ������
	    Calls:          
	    Called By:      
	    Input:          task������
	                    
	    Output:         
	    Return:         ִ�гɹ�����true�����򷵻�false
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
	    Description:    ����������Ϣ�����豸
	    Calls:          
	    Called By:      
	    Input:          pDAUInfo������DAU������Ϣ����
	                    
	    Output:         
	    Return:         ִ�гɹ������豸�����򷵻�NULL
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
	    Description:    ɾ���豸
	    Calls:          
	    Called By:      
	    Input:          pDevice��Ҫɾ�����豸
	                    
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
	    Description:    ϵͳ�˳�
	    Calls:          
	    Called By:      
	    Input:          
	                    
	    Output:         
	    Return:         ִ�гɹ�����true�����򷵻�false
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
	    Description:    ϵͳ�����ؽ�
	    Calls:          
	    Called By:      
	    Input:          type���ؽ�����
	                    
	    Output:         
	    Return:         ִ�гɹ�����true�����򷵻�false
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
	    Description:    ����豸�߳�
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
	    Description:    ��ѯ�����豸״̬
	    Calls:          
	    Called By:      
	    Input:          devType:�豸���ͣ�1-��̬,2-��̬
	                    
	    Output:         StateList���豸״̬�б�
	    Return:         true:�ɹ���false��ʧ��    *************************************************
	   }}}USR */
	// }}}RME
	bool GetAllDevState( vector< DAVICESTATE > & StateList, int devType );
	// }}}RME
	// {{{RME operation 'GetDevState(unsigned int,int)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetDevState
	    Description:    ��ѯ�豸״̬
	    Calls:          
	    Called By:      
	    Input:          DevId���豸���
							devType:�豸���ͣ�1-��̬,2-��̬
	                    
	    Output:         
	    Return:         0��δ֪״̬�� 1������״̬�� 2������״̬�� 3����Ч״̬
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
