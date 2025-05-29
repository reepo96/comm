// {{{RME classifier 'Logical View::Protocol'

#ifndef Protocol_H
#define Protocol_H

#ifdef PRAGMA
#pragma interface "Protocol.h"
#endif

#include <RTSystem/Communication.h>

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* �ļ����ƣ�Protocol.h
* �ļ���ʶ��
* ժ    Ҫ������Э��ĸ���
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
*
**************************************************************************/

struct Task;
class Linker;

#include "public.h"
#include "Command.h"
//#include "LogControl.h"

//Э�鴦��������
typedef enum proErrorCode{PEC_UNKOWN,PEC_NOERROR,PEC_ERRCMD_NODATA,PEC_INTERRUPT,PEC_ERR_ANSWER,
PEC_ERR_FRAME,PEC_STARTDT_NOACT,PEC_RMT_STOP_TRANSFER,PEC_TIME_OUT,PEC_OPERAT_FILE_FAIL} PROERRORCODE;

//����ASDU��Ϣ���
#define ASDU_INF_R_ALLGROUP_TITLE		240	//�����б�������ı���
#define ASDU_INF_R_ALLENTRY_VAL			241	//��һ�����ȫ����Ŀ��ֵ������
#define ASDU_INF_R_ONEENTRY_DIR			243	//��������Ŀ��Ŀ¼
#define ASDU_INF_R_ONEENTRY_VAL			244	//��������Ŀ��ֵ������
#define ASDU_INF_GEN_DATA				245	//ͨ�÷�������


//����ASDU���������
#define ASDU_KOD_ACT_VAL				1	//ʵ��ֵ
#define ASDU_KOD_DIMENSION				9	//����
#define ASDU_KOD_DESCRIPTION			10	//����

//����ASDU���������(����104��չ)
#define ASDU_KOD_RATING_104GB			101	//�ֵ
#define ASDU_KOD_OBJTYPE_104GB			101	//�����������

//����ͨ�÷�����������
typedef struct _GDD
{
    unsigned char dataType; //��������
    unsigned char dataLen;  //���ݳ���
    unsigned char dataNum;  //������Ŀ
}GDD,*PGDD;

typedef struct _GI
{
public:
	union
    {
        struct
        {
            unsigned char groupNo;//���
			unsigned char entryNo;//��Ŀ��
        }g;

        unsigned short GIN;//ͨ�÷����ʶ���
    };

    unsigned char KOD;      //�������
    GDD           gdd;      //ͨ�÷�����������
    unsigned short dataLen; //���ݳ���
    unsigned char *data;    //����

    _GI(unsigned short gin,unsigned char kod,unsigned char gddDataType,unsigned char gddDataLen,unsigned char gddDataNum,
        unsigned short giDataLen,unsigned char *pGiData)
    {
        this->GIN = gin;
        this->KOD = kod;
        this->gdd.dataType = gddDataType;
        this->gdd.dataLen = gddDataLen;
        this->gdd.dataNum = gddDataNum;
        this->dataLen = giDataLen;
        this->data = pGiData;
    }

	_GI(unsigned char groupNo,unsigned char entryNo,unsigned char kod,unsigned char gddDataType,unsigned char gddDataLen,unsigned char gddDataNum,
        unsigned short giDataLen,unsigned char *pGiData)
    {
		this->g.groupNo = groupNo;
		this->g.entryNo = entryNo;
        this->KOD = kod;
        this->gdd.dataType = gddDataType;
        this->gdd.dataLen = gddDataLen;
        this->gdd.dataNum = gddDataNum;
        this->dataLen = giDataLen;
        this->data = pGiData;
    }
}GI,*PGI;

//����Ԫ�ص���Ŀ
typedef struct _NDE
{
	unsigned char NO:6;		//��Ŀ
	unsigned char COUNT:1;	//������λ
	unsigned char CONT:1;	//����λ

	_NDE()
	{
		NO = 0;
		COUNT = 0;
		CONT = 0;
	}
}NDE,*PNDE;

//ͨ�÷������ݼ���Ŀ
typedef struct _NGD
{
	union
    {
		struct
		{
			unsigned char NO:6;		//��Ŀ
			unsigned char COUNT:1;	//������λ
			unsigned char CONT:1;	//����λ
		}s;
		unsigned char ngd;
	};

	_NGD()
	{
		s.NO = 0;
		s.COUNT = 0;
		s.CONT = 0;
	}
}NGD,*PNGD;


// }}}USR
// }}}RME

class Protocol
{
public:
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR

	/*********������������Ķ�Ӧ��ϵ-->***********************/
	typedef struct tagFunCode2Cmd
	{
		unsigned char cmdFunCode;	//��ϵͳ�ڲ�ʹ�õ��������
		bool (*exuteCmd)(Protocol * ,Linker * ,struct Command& );  //����ִ������ĺ���
		const char*	cmdName; //��Ϣ����
	}FUNCODE2CMD,*PFUNCODE2CMD;

	//��ȡ����������Ϣ
	virtual void GetCmdDesc(struct Command & cmd)
	{
	}

	virtual int DealAutoSend(Linker *pLinker)
	{
		return 0;
	}

	// }}}USR
	// }}}RME

protected:
	// {{{RME tool 'OT::Cpp' property 'ProtectedDeclarations'
	// {{{USR
	    //LogControl log;
	    PROERRORCODE    m_errCode;

		void CP56Time2aTotm(struct CP56Time2a& srcTime,struct tm& tmDestTime);

		void tmToCP56Time2a(struct tm& tmSrcTime,struct CP56Time2a& destTime);
	// }}}USR
	// }}}RME

private:
	// {{{RME tool 'OT::Cpp' property 'PrivateDeclarations'
	// {{{USR

	// }}}USR
	// }}}RME

public:
	// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
	virtual ~Protocol( void );
	// }}}RME
	// {{{RME operation 'Protocol()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	Protocol( void );
	// }}}RME

	virtual void Init() {}

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
	// {{{RME operation 'InitConnect(Linker *)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       InitConnect
	    Description:    ��ʼ������
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    
	    Output:         
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool InitConnect( Linker * pLinker );
	// }}}RME
	// {{{RME operation 'GetUdpData(Linker *,struct Command&)'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	virtual bool GetUdpData( Linker * pLinker, struct Command & cmd );
	// }}}RME

protected:
	// {{{RME operation 'SendData(Linker *,const char*,int,unsigned int)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	      Function:       SendData
	      Description:    ��������
	      Calls:          
	      Called By:      
	      Input:          pLinker����·����
	                          pDataBuf��ָ��Ҫ���͵�����
	                      iDataLen��Ҫ�������ݵĳ���
	                          uiTimeOut����ʱʱ�䣨��λ�����룩��Ϊ0��������ʱ
	      Output:         
	      Return:         ʵ�ʷ������ݵĳ���
	    *************************************************	
	   }}}USR */
	// }}}RME
	int SendData( Linker * pLinker, const char * pDataBuf, int iDataLen, unsigned int uiTimeOut );
	// }}}RME
	// {{{RME operation 'ReceiveData(Linker *,char*,int,unsigned int)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	      Function:       ReceiveData
	      Description:    ��������
	      Calls:          
	      Called By:      
	      Input:          pLinker����·����
	                          pDataBuf��������յ�������
	                      iBufferLen�������������ݵĳ���
	                      uiTimeOut����ʱʱ�䣨��λ�����룩��Ϊ0��������ʱ
	      Output:         
	      Return:         ʵ�ʽ��յ������ݳ���
	    *************************************************
	   }}}USR */
	// }}}RME
	int ReceiveData( Linker * pLinker, char * pDataBuf, int iBufferLen, unsigned int uiTimeOut );
	// }}}RME

public:
	// {{{RME operation 'GetLastErrCode()'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	      Function:       GetLastErrCode
	      Description:    ������������һ�δ������
	      Calls:          
	      Called By:      
	      Input:          
	                      	                      
	      Output:         
	      Return:         �������
	    *************************************************
	   }}}USR */
	// }}}RME
	PROERRORCODE GetLastErrCode( void );
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
	virtual Task * DoIdle( Linker * pLinker );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* Protocol_H */

// }}}RME
