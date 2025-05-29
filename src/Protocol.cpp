// {{{RME classifier 'Logical View::Protocol'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "Protocol.h"
#endif

#include <RTSystem/Communication.h>
#include <Protocol.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* 文件名称：Protocol.cpp
* 文件标识：
* 摘    要：所有协议的父类
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

#include "Linker.h"
#include "Task.h"

// }}}USR
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
Protocol::~Protocol( void )
{
}
// }}}RME

// {{{RME operation 'Protocol()'
Protocol::Protocol( void )
	// {{{RME tool 'OT::Cpp' property 'ConstructorInitializer'
	// {{{USR

	// }}}USR
	// }}}RME
{
	// {{{USR
	        m_errCode = PEC_NOERROR;
	// }}}USR
}
// }}}RME

// {{{RME operation 'Decode(Linker*)'
Task * Protocol::Decode( Linker * pLinker )
{
	// {{{USR
	    return NULL;
	// }}}USR
}
// }}}RME

// {{{RME operation 'ExecuteCmd(Linker *,struct Command&)'
bool Protocol::ExecuteCmd( Linker * pLinker, struct Command & cmd )
{
	// {{{USR
	    return true;

	// }}}USR
}
// }}}RME

// {{{RME operation 'InitConnect(Linker *)'
bool Protocol::InitConnect( Linker * pLinker )
{
	// {{{USR

	    return true;

	// }}}USR
}
// }}}RME

// {{{RME operation 'GetUdpData(Linker *,struct Command&)'
bool Protocol::GetUdpData( Linker * pLinker, struct Command & cmd )
{
	// {{{USR
	        return true;
	// }}}USR
}
// }}}RME

// {{{RME operation 'SendData(Linker *,const char*,int,unsigned int)'
int Protocol::SendData( Linker * pLinker, const char * pDataBuf, int iDataLen, unsigned int uiTimeOut )
{
	// {{{USR
	        int sendDataLen = pLinker->SendData(pDataBuf,iDataLen,uiTimeOut); 
	        /*if(sendDataLen != iDataLen)
	        {
	            LINKERRORCODE linkErrCode = pLinker->GetLastErrCode();
	            switch(linkErrCode)
	            {
	            case LEC_LINKERR:
	                m_errCode = PEC_LINKERR;
	                break;
	            case LEC_LINK_TIMEOUT:
	                m_errCode = PEC_LINK_TIMEOUT;
	                break;
	            case LEC_USER_TERMINATE:
	                m_errCode = PEC_USER_TERMINATE;
	                break;
	            default:
	                m_errCode = PEC_LINK_UNKOWN_ERR;
	                break;
	            }
	        }*/
	    return sendDataLen;
	// }}}USR
}
// }}}RME

// {{{RME operation 'ReceiveData(Linker *,char*,int,unsigned int)'
int Protocol::ReceiveData( Linker * pLinker, char * pDataBuf, int iBufferLen, unsigned int uiTimeOut )
{
	// {{{USR
	        int recvDataLen = pLinker->ReceiveData(pDataBuf,iBufferLen,uiTimeOut); 
	        /*if(recvDataLen != iBufferLen)
	        {
	            LINKERRORCODE linkErrCode = pLinker->GetLastErrCode();
	            switch(linkErrCode)
	            {
	            case LEC_LINKERR:
	                m_errCode = PEC_LINKERR;
	                break;
	            case LEC_LINK_TIMEOUT:
	                m_errCode = PEC_LINK_TIMEOUT;
	                break;
	            case LEC_USER_TERMINATE:
	                m_errCode = PEC_USER_TERMINATE;
	                break;
	            default:
	                m_errCode = PEC_LINK_UNKOWN_ERR;
	                break;
	            }
	        }*/
	    return recvDataLen;
	// }}}USR
}
// }}}RME

// {{{RME operation 'GetLastErrCode()'
PROERRORCODE Protocol::GetLastErrCode( void )
{
	// {{{USR
	        PROERRORCODE errCode = m_errCode;
	        m_errCode = PEC_NOERROR;
	        return errCode;
	// }}}USR
}
// }}}RME

// {{{RME operation 'DoIdle(Linker*)'
Task * Protocol::DoIdle( Linker * pLinker )
{
	// {{{USR
	return NULL;
	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR
void Protocol::CP56Time2aTotm(struct CP56Time2a& srcTime,struct tm& tmDestTime)
{
	if(srcTime.IV == false)
	{
		tmDestTime.tm_mday = srcTime.day;
		tmDestTime.tm_hour = srcTime.hours;
		tmDestTime.tm_sec = srcTime.milliseconds/1000;
		tmDestTime.tm_min = srcTime.minute;
		tmDestTime.tm_mon = srcTime.month-1;
		tmDestTime.tm_year = srcTime.year+2000-1900;
	}
	else
	{
		tmDestTime.tm_mday = 1;
		tmDestTime.tm_hour = 0;
		tmDestTime.tm_sec = 0;
		tmDestTime.tm_min = 0;
		tmDestTime.tm_mon = 1;
		tmDestTime.tm_year = 70;
	}
}

void Protocol::tmToCP56Time2a(struct tm& tmSrcTime,struct CP56Time2a& destTime)
{
	destTime.IV = true;
	destTime.day = tmSrcTime.tm_mday;
	destTime.hours = tmSrcTime.tm_hour;
	destTime.milliseconds = tmSrcTime.tm_sec*1000;
	destTime.minute = tmSrcTime.tm_min;
	destTime.month = tmSrcTime.tm_mon+1;
	destTime.year = tmSrcTime.tm_year+1900-2000;
}

// }}}USR
// }}}RME

// }}}RME
