// {{{RME classifier 'Logical View::SystemState'

#ifndef SystemState_H
#define SystemState_H

#ifdef PRAGMA
#pragma interface "SystemState.h"
#endif

#include <RTSystem/Communication.h>
class SystemCtrl;

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
#include "public.h"

/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* �ļ����ƣ�SystemState.h
* �ļ���ʶ��
* ժ    Ҫ��ϵͳ״̬
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
*
**************************************************************************/

#include "SystemCtrl.h"

#ifdef _DEBUG
#define new MYDEBUG_NEW
#endif

// }}}USR
// }}}RME

class SystemState
{
public:
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR
	    virtual string GetStateName();

	static SystemState& GetStateInstance()
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
	static SystemState _instance;
	// }}}USR
	// }}}RME

public:
	// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
	SystemState( void );
	// }}}RME
	// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
	virtual ~SystemState( void );
	// }}}RME
	// {{{RME operation 'Do(SystemCtrl*)'
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
	virtual bool Do( SystemCtrl * pContext );
	// }}}RME
	// {{{RME operation 'TrigerEvent(SystemCtrl::SYSTEMEVENT,SystemCtrl*)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	    Function:       TrigerEvent
	    Description:    ��SystemCtrl���ã����幤������������ɣ���������ݲ�ͬ���¼������µ�״̬��������SystemCtrl��changeState�����ı䵱ǰ״̬
	    Calls:          
	    Called By:      
	    Input:          sysEvent�����������¼�
	                    pContext��״̬���������
	    Output:         
	    Return:         
	    *************************************************
	   }}}USR */
	// }}}RME
	virtual void TrigerEvent( SystemCtrl::SYSTEMEVENT sysEvent, SystemCtrl * pContext );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* SystemState_H */

// }}}RME
