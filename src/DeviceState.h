// {{{RME classifier 'Logical View::DeviceState'

#ifndef DeviceState_H
#define DeviceState_H

#ifdef PRAGMA
#pragma interface "DeviceState.h"
#endif

#include <RTSystem/Communication.h>
class Device;

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* �ļ����ƣ�DeviceState.h
* �ļ���ʶ��
* ժ    Ҫ���豸״̬�࣬��¼�豸��ǰ״̬
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
*
**************************************************************************/

#include "public.h"

#include "Device.h"

#ifdef _DEBUG
#define new MYDEBUG_NEW
#endif

// }}}USR
// }}}RME

class DeviceState
{
public:
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR
	//�����豸����״̬��Ӧ��ֵ
	typedef enum DevStateVal{DSV_UNKOWN,DSV_IDLE,DSV_WORK,DSV_INVALID,DSV_SUCCESS,DSV_FAIL,DSV_TIMEOUT}  DEVSTATEVAL;

	virtual string GetStateName();

	static DeviceState& GetStateInstance()
	{
		return _instance;
	}
	// }}}USR
	// }}}RME

protected:
	// {{{RME tool 'OT::Cpp' property 'ProtectedDeclarations'
	// {{{USR

	// }}}USR
	// }}}RME

private:
	// {{{RME tool 'OT::Cpp' property 'PrivateDeclarations'
	// {{{USR
	static DeviceState _instance;
	// }}}USR
	// }}}RME

public:
	// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
	DeviceState( void );
	// }}}RME
	// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
	virtual ~DeviceState( void );
	// }}}RME
	// {{{RME operation 'Do(Device*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       Do
	    Description:    ת�䵽��״̬��Ҫ�������
	    Calls:          
	    Called By:      
	    Input:          pContext��״̬���������
	                    
	    Output:         
	    Return:         
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool Do( Device * pContext );
	// }}}RME
	// {{{RME operation 'TrigerEvent(Device::DEVEVENT,Device*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       TrigerEvent
	    Description:    ��Device���ã����幤������������ɣ���������ݲ�ͬ���¼������µ�״̬��������Device��changeState�����ı䵱ǰ״̬
	    Calls:          
	    Called By:      
	    Input:          devEvent�����������¼�
	                    pContext��״̬���������
	    Output:         
	    Return:         
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual void TrigerEvent( Device::DEVEVENT devEvent, Device * pContext );
	// }}}RME
	// {{{RME operation 'GetStateValue()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetStateValue
	    Description:    ��ѯ״̬��Ӧ����ֵ
	    Calls:          
	    Called By:      
	    Input:          
	                    
	    Output:         
	    Return:         ״̬��Ӧ����ֵ
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual DEVSTATEVAL GetStateValue( void );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* DeviceState_H */

// }}}RME
