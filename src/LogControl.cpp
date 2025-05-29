// {{{RME classifier 'Logical View::LogControl'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "LogControl.h"
#endif

#include <RTSystem/Communication.h>
#include <LogControl.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* 文件名称：LogControl.cpp
* 文件标识：
* 摘    要：日志记录类
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

#include <stdio.h>
#include "public.h"
#include "Command.h"
#include "FileControl.h"

const char *LOG_BAK_FILE_NAME = ".\\Communication_bak_%d.log";

void LogControl::FormatLogInf( LOGLEVEL logLevel, const char * msg...)
{
	if(m_hEvent != 0)
	{
		if(WAIT_OBJECT_0 != WaitForSingleObject((HANDLE)m_hEvent,5000))
		{
			return;
		}
	} 

	va_list	ap;
	va_start(ap,msg);
	_vsnprintf(m_szFormatInfo,sizeof(m_szFormatInfo)-1,msg,ap);
	va_end(ap);

	if(m_hEvent != 0)
	{
		::SetEvent((HANDLE)m_hEvent);
	}

	WriteLog2File(logLevel,m_szFormatInfo);
}

// }}}USR
// }}}RME

// {{{RME operation 'LogControl()'
LogControl::LogControl( void )
	// {{{RME tool 'OT::Cpp' property 'ConstructorInitializer'
	// {{{USR

	// }}}USR
	// }}}RME
{
	// {{{USR
			//更改本地目录为当前目录
			char szCurDir[_MAX_PATH];
			GetModuleFileName( GetModuleHandle(NULL), szCurDir, _MAX_PATH );
			char *pPostfix = strrchr(szCurDir, '\\');
			*pPostfix = '\0';
			_chdir(szCurDir);

	        m_pFile = fopen(".\\Communication.Log","a+");
			if(m_pFile != NULL)
			{
				fseek(m_pFile,0,SEEK_END);
			}

			m_hEvent = (unsigned int)CreateEvent(NULL,FALSE,TRUE,"CommLogEvent");
			memset(m_szLogInfo,0,sizeof(m_szLogInfo));
			memset(m_szFormatInfo,0,sizeof(m_szFormatInfo));

			m_uiCurrentBakFileNo = 0;

			//取最旧的一个日志备份文件作为下个覆盖目标
			char szFileName[64] = {0};
			FileControl fileControl;

			//取还没生成过的文件
			for(int i=1;i<=5;i++)
			{
				sprintf(szFileName,LOG_BAK_FILE_NAME,i);
				if( !fileControl.IsExist(szFileName) )
				{
					m_uiCurrentBakFileNo = i;
					break;
				}
			}

			//取时间最久的文件
			if(m_uiCurrentBakFileNo == 0)
			{
				m_uiCurrentBakFileNo = 1;

				unsigned int uiPreFileTime=0,uiCurrentFileTime=0;
				struct CP56Time2a fileTime;
				memset(&fileTime,0,sizeof(fileTime));
				for(int i=1;i<=5;i++)
				{
					sprintf(szFileName,LOG_BAK_FILE_NAME,i);
					if(fileControl.GetFileTime(szFileName,&fileTime))
					{
						if(uiPreFileTime == 0)
						{
							uiPreFileTime = fileTime.year*365*24*60 + fileTime.month*30*24*60 + fileTime.day*24*60+fileTime.minute;
						}
						else
						{
							uiCurrentFileTime = fileTime.year*365*24*60 + fileTime.month*30*24*60 + fileTime.day*24*60+fileTime.minute;
							if(uiCurrentFileTime < uiPreFileTime)
							{
								m_uiCurrentBakFileNo = i;
								uiPreFileTime = uiCurrentFileTime;
							}
						}
					}
				}
			}
	// }}}USR
}
// }}}RME

// {{{RME operation '~LogControl()'
LogControl::~LogControl( void )
{
	// {{{USR
	if(m_pFile != NULL)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}

	if( m_hEvent!=0 )
	{
		::CloseHandle((HANDLE)m_hEvent);
		m_hEvent = 0;
	}
	// }}}USR
}
// }}}RME

// {{{RME operation 'WriteLog(LogStruct*)'
void LogControl::WriteLog( LogStruct * LogInfo )
{
	// {{{USR
	    if(AddLog != NULL)
	    {
	        AddLog(LogInfo);
	    }

	// }}}USR
}
// }}}RME

// {{{RME operation 'WriteLog2File(LOGLEVEL,const char*)'
void LogControl::WriteLog2File( LOGLEVEL logLevel, const char * msg )
{
	// {{{USR
	   if(m_pFile == NULL || strlen(msg)>sizeof(m_szLogInfo)-60 )
			return;

		if(m_hEvent != 0)
		{
			if(WAIT_OBJECT_0 != WaitForSingleObject((HANDLE)m_hEvent,5000))
			{
				return;
			}
		}

		 SYSTEMTIME sysTime;
		char LevelInfo[30] = {0};
		bool    isCanWrite = false;    //是否可以写日志
		DWORD dwCurrentThreadId = GetCurrentThreadId();

		switch(logLevel)
		{
		case LDETAIL:
			if(SystemCfger::GetInstance().logLevel >= 5)
			{
				isCanWrite = true;
				sprintf(LevelInfo,"[TID:%d]%s",dwCurrentThreadId,"[DETAIL] ");
			}
			break;
		case LDEBUG:
			if(SystemCfger::GetInstance().logLevel >= 4)
			{
				isCanWrite = true;
				sprintf(LevelInfo,"[TID:%d]%s",dwCurrentThreadId,"[DEBUG] ");
			}
			break;
		case LINFO:
			if(SystemCfger::GetInstance().logLevel >= 3)
			{
				isCanWrite = true;
				sprintf(LevelInfo,"[TID:%d]%s",dwCurrentThreadId,"[INFO ] ");
			}
			break;
		case LWARNING:
			if(SystemCfger::GetInstance().logLevel >= 2)
			{
				isCanWrite = true;
				sprintf(LevelInfo,"[TID:%d]%s",dwCurrentThreadId,"[WARN ] ");
			}
			break;
		case LERROR:
			if(SystemCfger::GetInstance().logLevel >= 1)
			{
				isCanWrite = true;
				sprintf(LevelInfo,"[TID:%d]%s",dwCurrentThreadId,"[ERROR] ");
			}
			break;
		default:
			//sprintf(LevelInfo,"%s"," [Unkown] ");
			break;
		}

		if(isCanWrite)
		{
			//获得日志文件大小
			fpos_t pos;
			if(fgetpos(m_pFile,&pos) == 0)
			{
				if( (int)pos >= (1024 * 1024 * SystemCfger::GetInstance().logFileMaxSize))
				{
					//清空文件内容
					fclose(m_pFile);
					m_pFile = NULL;

					FileControl fileControl;
					char szFileName[64] = {0};
					char szCommand[72] = {0};
					sprintf(szFileName,LOG_BAK_FILE_NAME,m_uiCurrentBakFileNo);

					if(fileControl.IsExist(szFileName))
					{
						sprintf(szCommand,"del %s",szFileName);
						::system(szCommand);
					}

					sprintf(szCommand,"copy .\\Communication.Log %s",szFileName);
					::system(szCommand);
					m_uiCurrentBakFileNo++;
					if(m_uiCurrentBakFileNo > 5)
					{
						m_uiCurrentBakFileNo = 1;
					}

					m_pFile = fopen(".\\Communication.Log","w");
					if(m_pFile != NULL)
					{
						fclose(m_pFile);
					}

					m_pFile = fopen(".\\Communication.Log","a+");
					if(m_pFile == NULL)
					{
						if(m_hEvent != 0)
						{
							::SetEvent((HANDLE)m_hEvent);
						}
						return;
					}
				}
			}//end if(fgetpos(m_pFile,&pos) == 0)

			GetLocalTime(&sysTime);
			sprintf(m_szLogInfo,"%d-%02d-%02d %02d:%02d:%02d.%03d %s %s ",sysTime.wYear,sysTime.wMonth,sysTime.wDay,
				sysTime.wHour,sysTime.wMinute,sysTime.wSecond,sysTime.wMilliseconds,LevelInfo,msg);

			m_szLogInfo[1023]='\0';
			fwrite(&m_szLogInfo[0],sizeof(char),strlen(m_szLogInfo),m_pFile);
			fwrite("\r\n",sizeof(char),1,m_pFile);

			fflush(m_pFile);
		}

		if(m_hEvent != 0)
		{
			::SetEvent((HANDLE)m_hEvent);
		}

	// }}}USR
}
// }}}RME

void LogControl::WriteData2Log( LOGLEVEL logLevel,const char *datahead,const char *data,int datalen )
{
	if( !data )
		return;

	if(m_hEvent != 0)
	{
		if(WAIT_OBJECT_0 != WaitForSingleObject((HANDLE)m_hEvent,5000))
		{
			return;
		}
	}

	if( datahead)
	{
		fprintf(m_pFile,"%s",datahead);
	}

	for(int i=0;i<datalen;i++)
	{
		fprintf(m_pFile,"%02X ",(unsigned char)*data);
		data++;
	}
	fwrite("\r",sizeof(char),1,m_pFile);
	fflush(m_pFile);

	if(m_hEvent != 0)
	{
		::SetEvent((HANDLE)m_hEvent);
	}
}

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR

// }}}USR
// }}}RME

// }}}RME
