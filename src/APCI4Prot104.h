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
* �ļ����ƣ�APCI4Prot104.h
* �ļ���ʶ��
* ժ    Ҫ��104Э���APCIͷ
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2008��3��4��
*
**************************************************************************/

const unsigned char PRO104_STARTCHAR = 0X68;  //104Э�������ַ�

// }}}USR
// }}}RME

class APCI4Prot104
{
public:
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR

	//����֡����
	typedef enum frameType{FT_UNKNOW,FT_I,FT_S,FT_U} FRAMETYPE;

	//104Э�������
	typedef struct tagControl
	{
		unsigned char control1;
		unsigned char control2;
		unsigned char control3;
		unsigned char control4;
	}CONTROL,*PCONTROL;

	unsigned char	startChar;		//�����ַ�
	int				iApduLen;		//APDU����
	CONTROL			controlArea;	//������


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

	//��ȡAPCIͷ�ĳ���
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
	int		m_iLenOfApdulen;		//APDU������ռ�ֽ�������׼Ϊ1�ֽڣ�
	FRAMETYPE	m_FrameType;		//֡����
	int			m_iSendNum;			//�������к�
	int			m_iReceivNum;		//�������к�

	bool		m_bNoAnalysCtrlArea;	//������������(ΪĳЩ���淶Э��)
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
		���ַ���������APCIͷ
	   }}}USR */
	// }}}RME
	bool Decode( const char * pData, int iDataLen );
	// }}}RME
	// {{{RME operation 'Encode()'
	// {{{RME general 'documentation'
	/* {{{USR
		��APCIͷ������ַ���
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
