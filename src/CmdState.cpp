// {{{RME classifier 'Logical View::CmdState'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "CmdState.h"
#endif

#include <RTSystem/Communication.h>
#include <CmdState.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* �ļ����ƣ�CmdState.cpp
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

// {{{RME operation 'CmdState()'
CmdState::CmdState( void )
	// {{{RME tool 'OT::Cpp' property 'ConstructorInitializer'
	// {{{USR

	// }}}USR
	// }}}RME
{
	// {{{USR
	        preState = CS_STEP1;
	            pStoreParm = NULL;
	// }}}USR
}
// }}}RME

// {{{RME operation '~CmdState()'
CmdState::~CmdState( void )
{
	// {{{USR
	        if(pStoreParm != NULL)
	        {
	        delete pStoreParm;
	            pStoreParm = NULL;
	        }
	// }}}USR
}
// }}}RME

// {{{RME operation 'ToNextState()'
void CmdState::ToNextState( void )
{
	// {{{USR
	        assert( (int)_state < ((int)CS_PAUSE -1) );

	        preState = _state;
	        _state = (STATE)((int)_state + 1);
	// }}}USR
}
// }}}RME

// {{{RME operation 'ToPreState()'
void CmdState::ToPreState( void )
{
	// {{{USR
	        _state = preState;
	// }}}USR
}
// }}}RME

// {{{RME operation 'ChangeStateTo(STATE)'
void CmdState::ChangeStateTo( STATE newState )
{
	// {{{USR
	        preState = _state;
	        _state = newState;
	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR

// }}}USR
// }}}RME

// }}}RME
