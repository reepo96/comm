// {{{RME classifier 'Logical View::DevTimeOutState'

#ifndef DevTimeOutState_H
#define DevTimeOutState_H

#ifdef PRAGMA
#pragma interface "DevTimeOutState.h"
#endif

#include <RTSystem/Communication.h>
#include <DeviceState.h>

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
#include "public.h"

/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* �ļ����ƣ�TimeOuteState.h
* �ļ���ʶ��
* ժ    Ҫ���豸��ʱ״̬
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
*
**************************************************************************/

// }}}USR
// }}}RME

class DevTimeOutState : public DeviceState
{
public:
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR
	static DevTimeOutState& GetStateInstance()
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
	static DevTimeOutState _instance;
	// }}}USR
	// }}}RME

public:
	// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
	DevTimeOutState( void );
	// }}}RME
	// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
	virtual ~DevTimeOutState( void );
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
	bool Do( Device * pContext );
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
	void TrigerEvent( Device::DEVEVENT devEvent, Device * pContext );
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
	DeviceState::DEVSTATEVAL GetStateValue( void );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* DevTimeOutState_H */

// }}}RME
