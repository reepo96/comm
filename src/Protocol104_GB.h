// {{{RME classifier 'Logical View::Protocol104_GB'

#ifndef Protocol104_GB_H
#define Protocol104_GB_H

#ifdef PRAGMA
#pragma interface "Protocol104_GB.h"
#endif

#include <RTSystem/Communication.h>
#include <Protocol104.h>
class FileControl;

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
#include "public.h"

/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* �ļ����ƣ�Protocol104_GB.h
* �ļ���ʶ��
* ժ    Ҫ������104Э��
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2008��3��4��
*
**************************************************************************/

#define		PRO104GB_FRAME_MAXSIZE	4090			//֡��󳤶�
#define		PRO104GB_ASDUDATA_MAXSIZE	4078		//֡ASDU DATA(��ϢԪ�ؼ�����)��󳤶�

#define		PRO104GB_FILEPATH_LEN	100		//�ļ�(Ŀ¼)������

// }}}USR
// }}}RME

class Protocol104_GB : public Protocol104
{
public:
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR

	/*************************************************
	    Function:       StopDataTransfer
	    Description:    ��ֹ���ݴ���
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����
	                    
	    Output:                             
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************/	   
	bool StopDataTransfer( Linker * pLinker, struct Command * pCmd );

	/*************************************************
	    Function:       CpyGI2SendBuf
	    Description:    ��GI�ṹ����������buffer��
	    Calls:          
	    Called By:      
	    Input:          ppDestBuf��Ŀ��buffer
	                    iBufLen��Ŀ��buffer����
						gi:GI�ṹ
	                    
	    Output:                             
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************/	   
	bool CpyGI2SendBuf( char **ppDestBuf, int iBufLen,GI& gi);

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

protected:
	// {{{RME tool 'OT::Cpp' property 'ProtectedDeclarations'
	// {{{USR

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

	// }}}USR
	// }}}RME

private:
	// {{{RME tool 'OT::Cpp' property 'PrivateDeclarations'
	// {{{USR

	// }}}USR
	// }}}RME

public:
	// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
	virtual ~Protocol104_GB( void );
	// }}}RME
	// {{{RME operation 'Protocol104_GB()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	Protocol104_GB( void );
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
	// {{{RME operation 'AnalyseTask(PASDUID104,PASDUINFID104,char*,int,Linker*)'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	Task * AnalyseTask( PASDUID104 pAsduId, PASDUINFID104 pInfId, char * pAsduData, int iAsduDataLen, Linker * pLinker );
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
	// {{{RME operation 'CommParamArrange(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	    Function:       CommParamArrange
	    Description:    ͨѶ����Э��
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����
	                    
	    Output:                             
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool CommParamArrange( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'GenCommand(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	    Function:       GenCommand
	    Description:    ͨ�÷�������
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����
	                    
	    Output:                             
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool GenCommand( Linker * pLinker, struct Command * pCmd );
	// }}}RME

protected:
	// {{{RME operation 'GetMarkEvn(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	    Function:       GetMarkEvn
	    Description:    ����ʷ�¼�
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����
	                    
	    Output:                             
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool GetMarkEvn( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'GetSearchType(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	    Function:       GetSearchType
	    Description:    ��ѯ���ݼ�������
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����
	                    
	    Output:                             
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool GetSearchType( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'GetAllDataObjDesc(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	    Function:       GetAllDataObjDesc
	    Description:    ��ѯ�������ݼ�����������
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����
	                    
	    Output:                             
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool GetAllDataObjDesc( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'GetDataObjAttr(Linker *,struct Command*,unsigned char,unsigned char)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	    Function:       GetDataObjAttr
	    Description:    ��ѯһ�����ݼ������������
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����
							groupNo:���
						entryNo:��Ŀ��
	                    
	    Output:                             
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool GetDataObjAttr( Linker * pLinker, struct Command * pCmd, unsigned char groupNo, unsigned char entryNo );
	// }}}RME

public:
	// {{{RME operation 'GetSerialRecorder(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	    Function:       GetSerialRecorder
	    Description:    ��ȡ������¼����
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����
	                    
	    Output:                             
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool GetSerialRecorder( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'QuerySerialData(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	    Function:       QuerySerialData
	    Description:    ������¼����
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����
	                    
	    Output:                             
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool QuerySerialData( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'GeneralFileList(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	    Function:       GeneralFileList
	    Description:    ͨ���ļ��б�
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����
	                    
	    Output:                             
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool GeneralFileList( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'ReadGeneralFile(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	    Function:       ReadGeneralFile
	    Description:    ͨ���ļ���
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����
	                    
	    Output:                             
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool ReadGeneralFile( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'WriteGeneralFile(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	    Function:       WriteGeneralFile
	    Description:    ͨ���ļ�д
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����
	                    
	    Output:                             
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool WriteGeneralFile( Linker * pLinker, struct Command * pCmd );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR
	
	//ͨ�÷�������
	const   unsigned char   CMD_104GB_GEN_COMMAND = 0X30;

	//��ȡ������¼����
	const   unsigned char   CMD_104GB_GET_SERIALRECORDER = 0X31;

	//������¼����
	const   unsigned char   CMD_104GB_QUERY_SERIALDATA = 0X32;

	//ͨ���ļ��б�
	const   unsigned char   CMD_104GB_GENERAL_FILE_LIST = 0X33;

	//ͨ���ļ���
	const   unsigned char   CMD_104GB_GENERAL_READ_FILE = 0X34;

	//ͨ���ļ�д
	const   unsigned char   CMD_104GB_GENERAL_WRITE_FILE = 0X35;

	//ͨѶ����Э��
	const   unsigned char   CMD_104GB_COMM_PARAM_ARRANGE = 0X36;

	//��ֹ���ݴ���
	const   unsigned char   CMD_104GB_COMM_STOP_TRANSFER = 0X37;

	/*********Э��֡������Ķ�Ӧ��ϵ-->***********************/
	typedef struct tagPro104GBFrame2Cmd
	{
		unsigned char type;        //Ӧ�÷���Ԫ���ͱ�ʶ		
		unsigned char cmdFunCode;    //��ϵͳ�ڲ�ʹ�õ��������
		bool (*exuteCmd)(Protocol104_GB*,Linker * ,struct Command&);  //����ִ������ĺ���
		const char *	cmdName; //��Ϣ����
	}PRO104GBFRAME2CMD,*PPRO104GBFRAME2CMD;

	extern PRO104GBFRAME2CMD pro104GBFR2CmdMap[8];

	//ͨ�÷�������
	bool GenCommand(Protocol104_GB *pProtocol,Linker *pLinker ,struct Command& cmd);

	//��ȡ������¼����
	bool GetSerialRecorder(Protocol104_GB *pProtocol,Linker *pLinker ,struct Command& cmd);

	//������¼����
	bool QuerySerialData(Protocol104_GB *pProtocol,Linker *pLinker ,struct Command& cmd);

	//ͨ���ļ��б�
	bool GeneralFileList(Protocol104_GB *pProtocol,Linker *pLinker ,struct Command& cmd);

	//ͨ���ļ���
	bool ReadGeneralFile(Protocol104_GB *pProtocol,Linker *pLinker ,struct Command& cmd);

	//ͨ���ļ�д
	bool WriteGeneralFile(Protocol104_GB *pProtocol,Linker *pLinker ,struct Command& cmd);

	//ͨѶ����Э��
	bool CommParamArrange(Protocol104_GB *pProtocol,Linker *pLinker ,struct Command& cmd);

	//��ֹ���ݴ���
	bool StopDataTransfer(Protocol104_GB *pProtocol,Linker *pLinker ,struct Command& cmd);

// }}}USR
// }}}RME

#endif /* Protocol104_GB_H */

// }}}RME
