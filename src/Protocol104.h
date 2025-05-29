// {{{RME classifier 'Logical View::Protocol104'

#ifndef Protocol104_H
#define Protocol104_H

#ifdef PRAGMA
#pragma interface "Protocol104.h"
#endif

#include <RTSystem/Communication.h>
#include <Protocol.h>

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* �ļ����ƣ�Protocol104.h
* �ļ���ʶ��
* ժ    Ҫ��104Э��
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2008��3��4��
*
**************************************************************************/

#include "public.h"
#include "APCI4Prot104.h"
#include "Command.h"

//104���ͱ�ʾ
#define	PRO104_ASDU6	6
#define	PRO104_ASDU7	7
#define	PRO104_ASDU8	8
#define	PRO104_ASDU20	20
#define	PRO104_ASDU21	21
#define	PRO104_ASDU120	120
#define	PRO104_ASDU122	122
#define	PRO104_ASDU124	124
#define	PRO104_ASDU101	101
#define	PRO104_ASDU103	103
#define	PRO104_ASDU104	104
#define	PRO104_ASDU105	105
#define	PRO104_ASDU221	221

#define	PRO104_ASDU100	100
#define	PRO104_ASDU10	10
#define	PRO104_ASDU11	11
#define	PRO104_ASDU121	121
#define	PRO104_ASDU123	123
#define	PRO104_ASDU102	102
#define	PRO104_ASDU104	104
#define	PRO104_ASDU106	106
#define	PRO104_ASDU108	108
#define	PRO104_ASDU220	220

#define	PRO104_ASDU13	13
#define	PRO104_ASDU15	15
#define	PRO104_ASDU17	17
#define	PRO104_ASDU113	113
#define	PRO104_ASDU1	1
#define	PRO104_ASDU14	14
#define	PRO104_ASDU16	16
#define	PRO104_ASDU18	18
#define	PRO104_ASDU114	114

//����104Э��ͨѶ����
/*const unsigned short P104_K = 256;		//����k
const unsigned short P104_W = 168;		//����w
const unsigned int P104_T1 = 15;		//����t1
const unsigned int P104_T2 = 5;			//����t2
const unsigned int P104_T3 = 25;		//����t3*/

//104Ӧ�÷������ݵ�Ԫ��ʶ��
typedef struct tagASDUID104
{
    unsigned char TYP;      //���ͱ�ʶ
    unsigned char VSQ;    //��ϢԪ�ص���Ŀ    
    unsigned char COT;      //����ԭ��
    unsigned short COMADDR;  //ASDU������ַ
}ASDUID104,*PASDUID104;

//Ӧ�÷������ݵ�Ԫ��Ϣ���ʶ��
typedef struct tagASDUInfID104
{
    unsigned char FUN;      //��������
    unsigned char INF;      //��Ϣ���
}ASDUINFID104,*PASDUINFID104;

const int ASDUID104_LEN = sizeof(ASDUID104);
const int ASDUINFID104_LEN = sizeof(ASDUINFID104);

#define	STRNO_GLIB 1
#define	STRNO_GEN 2
#define	STRNO_ALL_GROUP_TITLE 3
#define	STRNO_GROUP_ALL_ENTRY_VAL 4
#define	STRNO_ONE_ENTRY_DIR 5
#define	STRNO_ONE_ENTRY_VAL 6

typedef struct tagAsduStruct104
{
	int no;

	unsigned char VSQ;
	unsigned char FUN;
	unsigned char INF;
	char		  desc[64];
}ASDUSTRUCT104;

extern ASDUSTRUCT104 asduStruct104[6];

// }}}USR
// }}}RME

class Protocol104 : public Protocol
{
public:
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR

	// }}}USR
	// }}}RME

protected:
	// {{{RME tool 'OT::Cpp' property 'ProtectedDeclarations'
	// {{{USR
		int		m_iLenOfApdulen;		//APDU������ռ�ֽ�������׼Ϊ1�ֽڣ�

		const unsigned short P104_K ;		//����k
		const unsigned short P104_W;		//����w
		const unsigned int P104_T1 ;		//����t1
		const unsigned int P104_T2;			//����t2
		const unsigned int P104_T3 ;		//����t3

		unsigned short	m_iHasSendNum:15;		//�ѷ��͵����к�
		unsigned short	m_iHasReceivNum:15;		//�ѽ��յ����к�
		unsigned short	m_iHasAckNum:15;		//�ѵõ�Ӧ������к�
		unsigned short	m_iNoAckNum;			//û�лش�İ���

		DWORD	m_dwBeginT1Time;		//t1ʱ�俪ʼ��ʱ
		DWORD	m_dwBeginT2Time;		//t2ʱ�俪ʼ��ʱ
		DWORD	m_dwBeginT3Time;		//t3ʱ�俪ʼ��ʱ

		bool	m_bCanSendData;			//�Ƿ���Դ�������
				
	// }}}USR
	// }}}RME

private:
	// {{{RME tool 'OT::Cpp' property 'PrivateDeclarations'
	// {{{USR

	// }}}USR
	// }}}RME

public:
	// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
	virtual ~Protocol104( void );
	// }}}RME
	// {{{RME operation 'Protocol104()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	Protocol104( void );
	
		/*************************************************
	    Function:       SendIFrame
	    Description:    ����I��ʽ֡
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
						pAsdu��ָ��ASDU����
						iAsduLen��ASDU���ֵĳ���
						pCmd����ǰִ�е�����
	                    
	    Output:         
	    Return:         -1����������0��ֹͣ���ͣ�1:���ͳɹ�
	    *************************************************/
	virtual int SendIFrame(Linker * pLinker,const char *pAsdu,int iAsduLen,struct Command * pCmd);

	void Init();
	
	// }}}RME
	// {{{RME operation 'Decode(Linker*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       Decode
	    Description:    ����ͨ��ͨѶ��������ݣ�����һ���������
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    
	    Output:         
	    Return:         �ɹ���������ʧ�ܷ���NULL
	    *************************************************
	   }}}USR */
	// }}}RME
	virtual Task * Decode( Linker * pLinker );
	// }}}RME

protected:
	// {{{RME operation 'AnalyseTask(APCI4Prot104*,Linker*)'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	Task * AnalyseTask( APCI4Prot104 * pAPCI, Linker * pLinker );
	// }}}RME

public:
	// {{{RME operation 'ExecuteCmd(Linker *,struct Command&)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       ExecuteCmd
	    Description:    ִ������
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    cmd��Ҫִ�е�����
	    Output:         
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool ExecuteCmd( Linker * pLinker, struct Command & cmd );
	// }}}RME

protected:
	// {{{RME operation 'AnalyseFrame(Linker *,char*,int*,APCI4Prot104*,PASDUID104*,PASDUINFID104*,char**,int*,unsigned int)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       AnalyseFrame
	    Description:    ������������������һ��֡
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    uiTimeOut����ʱʱ�䣬��λ���룬0��ʾ������ʱ
	    Output:         framePacket�����淵��֡�����ݰ�
	                    frameLen�����ݰ�����
	                    pAPCI��APCI���ֵ�����
	                    pOutASDUID��ASDUID���ֵ�����
						pOutASDUInfID:ASDUInfID���ֵ�����
	                    pOutAsduData����Ϣ�岿�ֵ�����
	                    pAsduDataLen����Ϣ�峤��
	    Return:         -1:��������Э��֡����;  0:��·û������   1:�ɹ�������Э��֡
	    *************************************************
	   }}}USR */
	// }}}RME
	int AnalyseFrame( Linker * pLinker, char * framePacket, int * frameLen, APCI4Prot104 * pAPCI, PASDUID104 * pOutASDUID, PASDUINFID104 * pOutASDUInfID, char * * pOutAsduData, int * pAsduDataLen, unsigned int uiTimeOut = 0 );
	// }}}RME

public:
	// {{{RME operation 'GetRecWaveFileList(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetRecWaveFileList
	    Description:    �ٻ�¼���ļ��б�
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����                    
	                    
	    Output:                             
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool GetRecWaveFileList( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'GetRecWaveFile(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetRecWaveFile
	    Description:    �ٻ�¼���ļ�
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����                    
	                    
	    Output:                             
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool GetRecWaveFile( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'StartDT(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       StartDT
	    Description:    ���յ���ʼ������������
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����                    
	                    
	    Output:                             
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool StartDT( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'StopDT(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       StopDT
	    Description:    ���յ�ֹͣ������������
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����                    
	                    
	    Output:                             
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool StopDT( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'SendTestFR(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       SendTestFR
	    Description:    ���Ͳ���֡
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����                    
	                    
	    Output:                             
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool SendTestFR( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'ConfirmTestFR(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       ConfirmTestFR
	    Description:    �յ�����֡��Ӧ����
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����                    
	                    
	    Output:                             
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool ConfirmTestFR( Linker * pLinker, struct Command * pCmd );
	// }}}RME

protected:
	// {{{RME operation 'CreateIFrame(const char*,int)'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	string CreateIFrame( const char * pAsdu, int iAsduLen );
	// }}}RME
	// {{{RME operation 'CreateSFrame()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	string CreateSFrame( void );
	// }}}RME
	// {{{RME operation 'CReateUFrame(int)'
	// {{{RME general 'documentation'
	/* {{{USR
	����U��ʽ֡
	iType��0-STARTDT��Ч��1-STARTDTȷ�ϣ�2-STOPDT��Ч��3-STOPDTȷ�ϣ�4-TESTFR��Ч��5-TESTFRȷ��
	   }}}USR */
	// }}}RME
	string CReateUFrame( int iType );
	// }}}RME

public:
	// {{{RME operation 'DoIdle(Linker*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       DoIdle
	    Description:    �������
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    
	    Output:         
	    Return:         ������е�����
	    *************************************************	
	   }}}USR */
	// }}}RME
	Task * DoIdle( Linker * pLinker );
	// }}}RME

protected:
	// {{{RME operation 'EncodeAsdu(unsigned char,int,unsigned char,unsigned short,const char*,int)'
	// {{{RME general 'documentation'
	/* {{{USR
		*************************************************
	    Function:       EncodeAsdu
	    Description:    ��ASDU������ַ���
	    Calls:          
	    Called By:      
	    Input:          type�����ͱ�ʶ
	                    asduStructNo�����
						cot������ԭ��
						addr����ַ
						pAsduData��ASDU����
						iAsduDataLen�����ݳ���
	    Output:         
	                    
	    Return:         �ַ���
	    *************************************************
	   }}}USR */
	// }}}RME
	string EncodeAsdu( unsigned char type, int asduStructNo, unsigned char cot, unsigned short addr, const char * pAsduData, int iAsduDataLen );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

	#define	PRO104_TESTFR_ACT	0X43	//���Լ���
	#define	PRO104_STOPDT_ACT	0X13	//ֹͣ���ݴ��伤��
	#define	PRO104_STARTDT_ACT	0X07	//�������ݴ��伤��
		
	/****-------->����ϵͳ�ڲ�ʹ�õ��������******/
	//���Ͳ���֡
	const   unsigned char   CMD_104_SEND_TESTFR = 0X25;

	//Ӧ�����֡
	const   unsigned char   CMD_104_CONFIRM_TESTFR = 0X26;

	//�������ݴ���
	const   unsigned char   CMD_104_STARTDT = 0X27;

	//ֹͣ���ݴ���
	const   unsigned char   CMD_104_STOPDT = 0X28;

	/*********Э��U��ʽ֡������Ķ�Ӧ��ϵ-->***********************/
	typedef struct tagPro104UFrame2Cmd
	{
		unsigned char control1;      //U��ʽ֡��һ������
		unsigned char cmdFunCode;    //��ϵͳ�ڲ�ʹ�õ��������
		bool (*exuteCmd)(Protocol104*,Linker * ,struct Command&);  //����ִ������ĺ���
		const char*	cmdName; //��Ϣ����
	}PRO104UFRAME2CMD,*PPRO104UFRAME2CMD;

	extern PRO104UFRAME2CMD pro104UFrame2Cmd[4];

	//���Ͳ���֡
	bool SendTestFR(Protocol104 *pProtocol,Linker *pLinker ,struct Command& cmd);

	//Ӧ�����֡
	bool ConfirmTestFR(Protocol104 *pProtocol,Linker *pLinker ,struct Command& cmd);

	//�������ݴ���
	bool StartDT(Protocol104 *pProtocol,Linker *pLinker ,struct Command& cmd);

	//ֹͣ���ݴ���
	bool StopDT(Protocol104 *pProtocol,Linker *pLinker ,struct Command& cmd);

// }}}USR
// }}}RME

#endif /* Protocol104_H */

// }}}RME
