// {{{RME classifier 'Logical View::Protocol104_ZJ'

#ifndef Protocol104_ZJ_H
#define Protocol104_ZJ_H

#ifdef PRAGMA
#pragma interface "Protocol104_ZJ.h"
#endif

#include <RTSystem/Communication.h>
#include <Protocol104.h>
class DataControl;

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
#include "public.h"

/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* �ļ����ƣ�Protocol104_ZJ.h
* �ļ���ʶ��
* ժ    Ҫ���㽭104Э��
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2008��3��4��
*
**************************************************************************/

#define		PRO104ZJ_FRAME_MAXSIZE	4090			//֡��󳤶�
#define		PRO104ZJ_ASDUDATA_MAXSIZE	4078		//֡ASDU DATA(��ϢԪ�ؼ�����)��󳤶�

#define		PRO104ZJ_FILE_LEN	40		//�ļ�������

// }}}USR
// }}}RME

class Protocol104_ZJ : public Protocol104
{
public:
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR
	typedef struct tagMarkEvnt
	{
		unsigned char groupNo;
		unsigned char entryNo;
		unsigned char devAddr;
		char desc[40];
		struct CP56Time2a startTime;
		unsigned short timeLen;
		unsigned int fileLen;
		char fileName[40];
	}MARKEVNT,*PMARKEVNT;

	/**************************************************
	    Function:       CreateReportFile
	    Description:    �������ϼ�
	    Calls:
	    Called By:
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����

	    Output:
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************/	
	bool CreateReportFile( Linker * pLinker, struct Command * pCmd );

	/**************************************************
	    Function:       AutoSendReportFile
	    Description:    �Զ��ϴ����ϼ�
	    Calls:
	    Called By:
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����

	    Output:
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************/	
	bool AutoSendReportFile( Linker * pLinker, struct Command * pCmd );
				
	// }}}USR
	// }}}RME

protected:
	// {{{RME tool 'OT::Cpp' property 'ProtectedDeclarations'
	// {{{USR

	DWORD	m_dwBeginIdleTime;

	/**************************************************
	    Function:       SendReportFile
	    Description:    ���͹��ϼ�
	    Calls:
	    Called By:
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����
						type������ 1-�����ٻ���2-�Զ��ϴ�
						pReportData��������
						DataLen�������ݳ���
						startPos����ʼ����λ��

	    Output:
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************/	
	bool SendReportFile( Linker * pLinker, struct Command * pCmd,int type,const char *pReportData,int DataLen,unsigned int startPos =0);

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
		int SendIFrame(Linker * pLinker,const char *pAsdu,int iAsduLen,struct Command * pCmd);

		/*************************************************
	    Function:       SendAsduFrame
	    Description:    ����ASDU֡
	    Calls:
	    Called By:
	    Input:          type�����ͱ�ʶ
	                    asduStructNo�����
						cot������ԭ��
						addr����ַ
						pAsduData��ASDU����
						iAsduDataLen�����ݳ���
	    Output:

	    Return:         �ɹ�����true��ʧ�ܷ���false
	    ************************************************/
	bool SendAsduFrame( Linker * pLinker,struct Command * pCmd,unsigned char type, int asduStructNo, unsigned char cot, unsigned short addr, const char * pAsduData, int iAsduDataLen );

	// }}}USR
	// }}}RME

private:
	// {{{RME tool 'OT::Cpp' property 'PrivateDeclarations'
	// {{{USR

	// }}}USR
	// }}}RME

public:
	// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
	virtual ~Protocol104_ZJ( void );
	// }}}RME
	// {{{RME operation 'Protocol104_ZJ()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	Protocol104_ZJ( void );
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
	// {{{RME operation 'AnalyseTask(PASDUID104,char*,int,Linker*)'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	Task * AnalyseTask( PASDUID104 pAsduId, char * pAsduData, int iAsduDataLen, Linker * pLinker );
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
	bool GetRecWaveFileList( Linker * pLinker, struct Command * pCmd );
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
	bool GetRecWaveFile( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'GetReportFile(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetReportFile
	    Description:    ��ȡ��Ҫ¼������
	    Calls:
	    Called By:
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����

	    Output:
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool GetReportFile( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'GetMarkEventList(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetMarkEventList
	    Description:    ���¼��б�
	    Calls:
	    Called By:
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����

	    Output:
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************	
	   }}}USR */
	// }}}RME
	bool GetMarkEventList( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'AutoSendDevState(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       AutoSendDevState
	    Description:    �Զ��ϴ��豸״̬
	    Calls:
	    Called By:
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����

	    Output:
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************	
	   }}}USR */
	// }}}RME
	bool AutoSendDevState( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'AutoSendMarkEvent(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       AutoSendMarkEvent
	    Description:    �Զ��ϴ�����¼�
	    Calls:
	    Called By:
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����

	    Output:
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool AutoSendMarkEvent( Linker * pLinker, struct Command * pCmd );
	// }}}RME
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
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

	//���¼��б�
	const   unsigned char   CMD_104ZJ_GET_MARKEVN_LIST = 0X30;

	/*********Э��֡������Ķ�Ӧ��ϵ-->***********************/
	typedef struct tagPro104ZJFrame2Cmd
	{
		unsigned char type;        //Ӧ�÷���Ԫ���ͱ�ʶ
		unsigned char cmdFunCode;    //��ϵͳ�ڲ�ʹ�õ��������
		bool (*exuteCmd)(Protocol104_ZJ*,Linker * ,struct Command&);  //����ִ������ĺ���
		const char *	cmdName; //��Ϣ����
	}PRO104ZJFRAME2CMD,*PPRO104ZJFRAME2CMD;

	extern PRO104ZJFRAME2CMD pro104ZJFR2CmdMap[8];

	//�ٻ�¼���ļ��б�
    bool GetRecWaveFileList(Protocol104_ZJ *pProtocol,Linker * pLinker,struct Command& cmd);

	//�ٻ�¼���ļ�
    bool GetRecWaveFile(Protocol104_ZJ *pProtocol,Linker * pLinker,struct Command& cmd);

    //��Ҫ¼������
    bool GetReportFile(Protocol104_ZJ *pProtocol,Linker * pLinker,struct Command& cmd);

	//���¼��б�
    bool GetMarkEventList(Protocol104_ZJ *pProtocol,Linker * pLinker,struct Command& cmd);

	//�Զ��ϴ��豸״̬
    bool AutoSendDevState(Protocol104_ZJ *pProtocol,Linker * pLinker,struct Command& cmd);

	//�Զ��ϴ�����¼�
    bool AutoSendMarkEvent(Protocol104_ZJ *pProtocol,Linker * pLinker,struct Command& cmd);

	//�������ϼ��ļ�
    bool CreateReportFile(Protocol104_ZJ *pProtocol,Linker * pLinker,struct Command& cmd);

	//�Զ��ϴ����ϼ��ļ�
    bool AutoSendReportFile(Protocol104_ZJ *pProtocol,Linker * pLinker,struct Command& cmd);

// }}}USR
// }}}RME

#endif /* Protocol104_ZJ_H */

// }}}RME
