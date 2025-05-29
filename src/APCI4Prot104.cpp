// {{{RME classifier 'Logical View::APCI4Prot104'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "APCI4Prot104.h"
#endif

#include <RTSystem/Communication.h>
#include <APCI4Prot104.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* 文件名称：APCI4Prot104.cpp
* 文件标识：
* 摘    要：104协议的APCI头
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2008年3月4日
*
**************************************************************************/

// }}}USR
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
APCI4Prot104::~APCI4Prot104( void )
{
}
// }}}RME

// {{{RME operation 'APCI4Prot104(int)'
APCI4Prot104::APCI4Prot104( int iLenOfApdulen )
	// {{{RME tool 'OT::Cpp' property 'ConstructorInitializer'
	// {{{USR

	// }}}USR
	// }}}RME
{
	// {{{USR
	m_FrameType = FT_UNKNOW;
	m_iLenOfApdulen = iLenOfApdulen;
	startChar = 0;
	iApduLen = 0;
	m_iSendNum = 0;
	m_iReceivNum = 0;
	m_bNoAnalysCtrlArea = false;
	memset(&controlArea,0,sizeof(controlArea));
	// }}}USR
}
// }}}RME

// {{{RME operation 'Decode(const char*,int)'
bool APCI4Prot104::Decode( const char * pData, int iDataLen )
{
	// {{{USR
	int iTemp = 0;

	if(iDataLen < (P104_STARTCHAR_LEN + m_iLenOfApdulen + P104_CONTROLAREA_LEN) )
	{
		m_FrameType = FT_UNKNOW;
		return false;
	}

	//启动字符
	startChar = (unsigned char)pData[0];
	if( startChar != PRO104_STARTCHAR)
	{
		m_FrameType = FT_UNKNOW;
		return false;
	}

	//分析长度
	iApduLen = (unsigned char)pData[1];
	if(m_iLenOfApdulen == 2)
	{
		int iHighApduLen = ( ((unsigned char)pData[2]) & 0x0F ) << 8;
		iApduLen += iHighApduLen;
	}

	if( iApduLen < P104_CONTROLAREA_LEN)
	{
		m_FrameType = FT_UNKNOW;
		return false;
	}

	//某些协议不规范，所以不分析控制域
	if(m_bNoAnalysCtrlArea)
	{
		if(iApduLen > 4)
			m_FrameType = FT_I;
		else
			m_FrameType = FT_U;
		return true;
	}

	//分析控制域
	memcpy(&controlArea,&pData[P104_STARTCHAR_LEN + m_iLenOfApdulen],P104_CONTROLAREA_LEN);

	if( (controlArea.control1 & 0x01) == 0 && (controlArea.control3 & 0x01) == 0)
	{
		m_FrameType = FT_I;	//I格式帧		

		//计算发送序列号
		m_iSendNum = (controlArea.control1 & 0xFE) >> 1;
		iTemp = controlArea.control2 << 7;
		m_iSendNum += iTemp;

		//计算接收序列号
		m_iReceivNum = (controlArea.control3 & 0xFE) >> 1;
		iTemp = controlArea.control4 << 7;
		m_iReceivNum += iTemp;
	}
	else if( iApduLen != P104_CONTROLAREA_LEN)
	{
		m_FrameType = FT_UNKNOW;
		return false;
	}
	else if( (controlArea.control1 & 0x03) ==1 && (controlArea.control3 & 0x01) == 0 )
	{
		m_FrameType = FT_S;	//S格式帧

		m_iSendNum = 0;

		//计算接收序列号
		m_iReceivNum = (controlArea.control3 & 0xFE) >> 1;
		iTemp = controlArea.control4 << 7;
		m_iReceivNum += iTemp;
	}
	else if( (controlArea.control1 & 0x03) ==3 && (controlArea.control3 & 0x01) == 0 )
	{
		m_iSendNum = 0;
		m_iReceivNum = 0;
		m_FrameType = FT_U;	//U格式帧
	}
	else
	{
		m_FrameType = FT_UNKNOW;
		return false;
	}

	return true;
	// }}}USR
}
// }}}RME

// {{{RME operation 'Encode()'
string APCI4Prot104::Encode( void )
{
	// {{{USR
	char szHeadData[8] = {0};

	szHeadData[0] = startChar;

	if(m_iLenOfApdulen == 1)
	{
		szHeadData[1] = (char)iApduLen;
	}
	else
	{
		szHeadData[1] = (char)(iApduLen & 0x00FF);
		szHeadData[2] = (char)( (iApduLen & 0x0F00) >>8 );
	}

	memcpy(&szHeadData[P104_STARTCHAR_LEN + m_iLenOfApdulen],&controlArea,P104_CONTROLAREA_LEN);

	string sResult("");
	sResult.resize(P104_STARTCHAR_LEN+m_iLenOfApdulen+P104_CONTROLAREA_LEN);
	memcpy(&(sResult.operator [](0)),szHeadData,P104_STARTCHAR_LEN+m_iLenOfApdulen+P104_CONTROLAREA_LEN);

	return sResult;
	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR
bool APCI4Prot104::SetSendNum(int iSendNum)
{
	if(m_FrameType != FT_I)
		return false;

	m_iSendNum = iSendNum;
	controlArea.control1 = (m_iSendNum&0x7F)<<1;
	controlArea.control2 = (m_iSendNum&0x7F80)>>7;

	return true;
}

bool APCI4Prot104::SetReceivNum(int iReceivNum)
{
	if(m_FrameType != FT_I && m_FrameType != FT_S)
		return false;

	m_iReceivNum = iReceivNum;
	controlArea.control3 = (m_iReceivNum&0x7F)<<1;
	controlArea.control4 = (m_iReceivNum&0x7F80)>>7;

	return true;
}

int APCI4Prot104::GetApciLen()
{
	return (P104_STARTCHAR_LEN + m_iLenOfApdulen + P104_CONTROLAREA_LEN);
}

// }}}USR
// }}}RME

// }}}RME
