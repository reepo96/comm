// {{{RME classifier 'Logical View::APCI4Prot104'

#ifndef APCI4Prot104_H
#define APCI4Prot104_H

#ifdef PRAGMA
#pragma interface "APCI4Prot104.h"
#endif

#include <RTSystem/Communication.h>

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR

#include "public.h"

/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* 文件名称：APCI4Prot104.h
* 文件标识：
* 摘    要：104协议的APCI头
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2008年3月4日
*
**************************************************************************/

const unsigned char PRO104_STARTCHAR = 0X68;  //104协议启动字符

// }}}USR
// }}}RME

class APCI4Prot104
{
public:
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR

	//定义帧类型
	typedef enum frameType{FT_UNKNOW,FT_I,FT_S,FT_U} FRAMETYPE;

	//104协议控制域
	typedef struct tagControl
	{
		unsigned char control1;
		unsigned char control2;
		unsigned char control3;
		unsigned char control4;
	}CONTROL,*PCONTROL;

	unsigned char	startChar;		//启动字符
	int				iApduLen;		//APDU长度
	CONTROL			controlArea;	//控制域


	FRAMETYPE GetFrameType()
	{
		return m_FrameType;
	}

	void SetFrameType(FRAMETYPE type)
	{
		m_FrameType = type;
	}

	int GetSendNum()
	{
		return m_iSendNum;
	}

	int GetReceivNum()
	{
		return m_iReceivNum;
	}

	bool SetSendNum(int iSendNum);

	bool SetReceivNum(int iReceivNum);

	//获取APCI头的长度
	int GetApciLen();

	void NoAnalysCtrlArea()
	{
		m_bNoAnalysCtrlArea = true;
	}

	// }}}USR
	// }}}RME

protected:
	// {{{RME tool 'OT::Cpp' property 'ProtectedDeclarations'
	// {{{USR
	int		m_iLenOfApdulen;		//APDU长度所占字节数（标准为1字节）
	FRAMETYPE	m_FrameType;		//帧类型
	int			m_iSendNum;			//发送序列号
	int			m_iReceivNum;		//接收序列号

	bool		m_bNoAnalysCtrlArea;	//不分析控制域(为某些不规范协议)
	// }}}USR
	// }}}RME

private:
	// {{{RME tool 'OT::Cpp' property 'PrivateDeclarations'
	// {{{USR

	// }}}USR
	// }}}RME

public:
	// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
	virtual ~APCI4Prot104( void );
	// }}}RME
	// {{{RME operation 'APCI4Prot104(int)'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	APCI4Prot104( int iLenOfApdulen );
	// }}}RME
	// {{{RME operation 'Decode(const char*,int)'
	// {{{RME general 'documentation'
	/* {{{USR
		将字符流解析成APCI头
	   }}}USR */
	// }}}RME
	bool Decode( const char * pData, int iDataLen );
	// }}}RME
	// {{{RME operation 'Encode()'
	// {{{RME general 'documentation'
	/* {{{USR
		将APCI头编码成字符流
	   }}}USR */
	// }}}RME
	string Encode( void );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR
	const int P104_STARTCHAR_LEN = sizeof(unsigned char);
	const int P104_CONTROLAREA_LEN = sizeof(APCI4Prot104::tagControl);
// }}}USR
// }}}RME

#endif /* APCI4Prot104_H */

// }}}RME
