// {{{RME classifier 'Logical View::CmdState'

#ifndef CmdState_H
#define CmdState_H

#ifdef PRAGMA
#pragma interface "CmdState.h"
#endif

#include <RTSystem/Communication.h>

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
#include "public.h"

/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* �ļ����ƣ�CmdState.h
* �ļ���ʶ��
* ժ    Ҫ��ͨѶ����״̬��
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
*
**************************************************************************/

// }}}USR
// }}}RME

class CmdState
{
public:
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR
	    typedef enum State{CS_STEP1,CS_STEP2,CS_STEP3,CS_STEP4,CS_STEP5,CS_STEP6,CS_STEP7,CS_STEP8,CS_STEP9,\
	            CS_STEP10,CS_STEP11,CS_STEP12,CS_STEP13,CS_PAUSE,CS_SUCCESS,CS_FAIL} STATE;

	    STATE    _state;    //��ǰ״̬
	    STATE    preState;   //ǰһ״̬
	        void    *pStoreParm;  //����ĳһ״̬����ز���
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

	// }}}USR
	// }}}RME

public:
	// {{{RME operation 'CmdState()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	CmdState( void );
	// }}}RME
	// {{{RME operation '~CmdState()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	virtual ~CmdState( void );
	// }}}RME
	// {{{RME operation 'ToNextState()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	      Function:       ToNextState
	      Description:    �ı䵱ǰ״̬����һ״̬
	      Calls:          
	      Called By:      
	      Input:          
	                      
	      Output:         
	      Return:         
	    *************************************************
	   }}}USR */
	// }}}RME
	void ToNextState( void );
	// }}}RME
	// {{{RME operation 'ToPreState()'
	// {{{RME general 'documentation'
	/* {{{USR
	        *************************************************
	      Function:       ToPreState
	      Description:    �ı䵱ǰ״̬��ǰһ״̬
	      Calls:          
	      Called By:      
	      Input:          
	                      
	      Output:         
	      Return:         
	    *************************************************
	   }}}USR */
	// }}}RME
	void ToPreState( void );
	// }}}RME
	// {{{RME operation 'ChangeStateTo(STATE)'
	// {{{RME general 'documentation'
	/* {{{USR
	        *************************************************
	      Function:       ChangeStateTo
	      Description:    �ı䵱ǰ״̬��ĳһ��״̬
	      Calls:          
	      Called By:      
	      Input:          newState����״̬
	                      
	      Output:         
	      Return:         
	    *************************************************
	   }}}USR */
	// }}}RME
	void ChangeStateTo( STATE newState );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* CmdState_H */

// }}}RME
