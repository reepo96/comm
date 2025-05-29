// {{{RME classifier 'Logical View::SysExitState'

#ifndef SysExitState_H
#define SysExitState_H

#ifdef PRAGMA
#pragma interface "SysExitState.h"
#endif

#include <RTSystem/Communication.h>
#include <SystemState.h>

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
#include "public.h"

/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* �ļ����ƣ�SysExitState.h
* �ļ���ʶ��
* ժ    Ҫ��ϵͳ�˳�״̬
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
*
**************************************************************************/

// }}}USR
// }}}RME

class SysExitState : public SystemState
{
public:
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR
	    string GetStateName();

	static SysExitState& GetStateInstance()
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
	static SysExitState _instance;
	// }}}USR
	// }}}RME

public:
	// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
	SysExitState( void );
	// }}}RME
	// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
	virtual ~SysExitState( void );
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
	bool Do( SystemCtrl * pContext );
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
	void TrigerEvent( SystemCtrl::SYSTEMEVENT sysEvent, SystemCtrl * pContext );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* SysExitState_H */

// }}}RME
