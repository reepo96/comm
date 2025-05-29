// {{{RME classifier 'Logical View::LogControl'

#ifndef LogControl_H
#define LogControl_H

#ifdef PRAGMA
#pragma interface "LogControl.h"
#endif

#include <RTSystem/Communication.h>

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* �ļ����ƣ�LogControl.h
* �ļ���ʶ��
* ժ    Ҫ����־��¼��
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
*
**************************************************************************/

#include <CommDataType.h>
#include <stdio.h>

struct Command;

// }}}USR
// }}}RME

class LogControl
{
public:
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR
		typedef enum LogLevel{LDEBUG,LINFO,LWARNING,LERROR,LDETAIL} LOGLEVEL;

		/**************************************************
		Function:		FormatLogInf
		Description:	����־��Ϣ��ʽ����д��־���ļ���
		Calls:
		Called By:
		Input:			logLevel����־����
						msg���ɱ����
		Output:
		Return:
		*************************************************/
		void FormatLogInf( LOGLEVEL logLevel, const char * msg...);

	// }}}USR
	// }}}RME

protected:
	// {{{RME tool 'OT::Cpp' property 'ProtectedDeclarations'
	// {{{USR
		FILE * m_pFile;
		unsigned int m_hEvent;
		unsigned int	m_uiCurrentBakFileNo;//��ǰ�����ļ���
		char			m_szLogInfo[1024];
		char			m_szFormatInfo[1024];
	// }}}USR
	// }}}RME

private:
	// {{{RME tool 'OT::Cpp' property 'PrivateDeclarations'
	// {{{USR

	// }}}USR
	// }}}RME

public:
	// {{{RME operation 'LogControl()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	LogControl( void );
	// }}}RME
	// {{{RME operation '~LogControl()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	virtual ~LogControl( void );
	// }}}RME
	// {{{RME operation 'WriteLog(LogStruct*)'
	// {{{RME general 'documentation'
	/* {{{USR
		*************************************************
		Function:		WriteLog
		Description:	д��־�����ݿ���
		Calls:
		Called By:
		Input:			LogInfo����־��Ϣ

		Output:
		Return:
		*************************************************
	   }}}USR */
	// }}}RME
	void WriteLog( LogStruct * LogInfo );
	// }}}RME
	// {{{RME operation 'WriteLog2File(LOGLEVEL,const char*)'
	// {{{RME general 'documentation'
	/* {{{USR
		*************************************************
		Function:		WriteLog2File
		Description:	д��־���ļ���
		Calls:
		Called By:
		Input:			logLevel����־����
						msg����־����
		Output:
		Return:
		*************************************************
	   }}}USR */
	// }}}RME
	void WriteLog2File( LOGLEVEL logLevel, const char * msg );
	// }}}RME

	/* *************************************************
		Function:		WriteData2Log
		Description:	����������д����־�ļ���
		Calls:
		Called By:
		Input:			logLevel����־����
						datahead������ͷ
						data������������
						datalen�����������ݳ���
		Output:
		Return:
		**************************************************/
	void WriteData2Log( LOGLEVEL logLevel,const char *datahead,const char *data,int datalen );
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* LogControl_H */

// }}}RME
