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
* 文件名称：LogControl.h
* 文件标识：
* 摘    要：日志记录类
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
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
		Description:	将日志信息格式化并写日志到文件中
		Calls:
		Called By:
		Input:			logLevel：日志级别
						msg：可变参数
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
		unsigned int	m_uiCurrentBakFileNo;//当前备份文件号
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
		Description:	写日志到数据库中
		Calls:
		Called By:
		Input:			LogInfo：日志信息

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
		Description:	写日志到文件中
		Calls:
		Called By:
		Input:			logLevel：日志级别
						msg：日志描述
		Output:
		Return:
		*************************************************
	   }}}USR */
	// }}}RME
	void WriteLog2File( LOGLEVEL logLevel, const char * msg );
	// }}}RME

	/* *************************************************
		Function:		WriteData2Log
		Description:	二进制数据写入日志文件中
		Calls:
		Called By:
		Input:			logLevel：日志级别
						datahead：数据头
						data：二进制数据
						datalen：二进制数据长度
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
